
/************************************************************************
 * window.cpp								*
 * Copyright (C) 2008,2009  Psyjo					*
 *									*
 * This program is free software; you can redistribute it and/or modify	*
 * it under the terms of the GNU General Public License as published by	*
 * the Free Software Foundation; either version 3 of the License,	*
 * or (at your option) any later version.				*
 *									*
 * This program is distributed in the hope that it will be useful, but	*
 * WITHOUT ANY WARRANTY; without even the implied warranty of		*
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.			*
 * See the GNU General Public License for more details.			*
 *									*
 * You should have received a copy of the GNU General Public License	*
 * along with this program; if not, see <http://www.gnu.org/licenses/>. *
 ************************************************************************/

#include "window.h"

MainWindow::MainWindow( QWidget * parent, Qt::WFlags f)
        : QMainWindow(parent, f)
{
	dummy = new QWidget(this);
	setCentralWidget(dummy); // to be able to group the dockwidgets around nothing ...

	config = new ConfigHandler(QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/esw.xml"), "esw_configuration");
	config->loadIsVisible() ? show() : hide();
	// set win title "ESW 1.2.3"
	setWindowTitle(QApplication::applicationName() +" "+ QApplication::applicationVersion());

	setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::VerticalTabs);
	// DockWidgets should be placeable in any constellation

	setStyle(QStyleFactory::create(config->loadStyle()));
	if(! config->loadStyleSheet().isEmpty())
		setStyleSheet(config->loadStyleSheet());
	else	setStyleSheet("* {}"); // without any stylesheet, windowstyles won't apply

	hTimer = new QTimer(this);
	hTimer->setInterval(3600000); // 1h
	connect(hTimer, SIGNAL(timeout()), this, SLOT(onHTimer()));

	statusBar = new QStatusBar(this);
	setStatusBar(statusBar);

	mFile = menuBar()->addMenu(tr("&file"));
	mFile->addAction(tr("exit"), this, SLOT(handleExitAction()), Qt::CTRL + Qt::Key_Q);

	mAction = menuBar()->addMenu(tr("A&ction"));
	mAction->addAction(tr("input API"), this, SLOT(handleInputApiAction()), Qt::Key_F8);
	mAction->addAction(tr("update"), this, SLOT(onHTimer()), Qt::Key_F5);

	mStyle = new QMenu(tr("&Style"), this);
	foreach(QString s, QStyleFactory::keys())	// fill in all available Styles
		mStyle->addAction(s);			//
	connect(mStyle, SIGNAL(triggered(QAction*)), this, SLOT(onStyleMenu(QAction*)));

	mOption = menuBar()->addMenu(tr("&Options"));
	ontopAction = new QAction(tr("always on &top"), this);
	showTrayAction = new QAction(tr("show tray &icon"), this);
	autoSyncAction = new QAction(tr("autosync"), this);
	showProgressBarAction = new QAction(tr("show progressbar"), this);
	closeToTrayAction = new QAction(tr("close to &tray"), this);

	ontopAction->setCheckable(true);
	showTrayAction->setCheckable(true);
	autoSyncAction->setCheckable(true);
	showProgressBarAction->setCheckable(true);
	closeToTrayAction->setCheckable(true);

	showTrayAction->setChecked(config->loadShowTray());
	ontopAction->setChecked(config->loadOntop());
	autoSyncAction->setChecked(config->loadAutoSync());
	showProgressBarAction->setChecked(config->loadProgressBar());
	closeToTrayAction->setChecked(config->loadCloseToTray());
	
	mOption->addAction(ontopAction);
	mOption->addAction(showTrayAction);
	mOption->addAction(autoSyncAction);
	mOption->addAction(showProgressBarAction);
	mOption->addAction(closeToTrayAction);
	mOption->addSeparator();
	mOption->addMenu(mStyle);
	connect(ontopAction, SIGNAL(toggled(bool)), this, SLOT(onOntopAction(bool)));
	connect(showTrayAction, SIGNAL(toggled(bool)), this, SLOT(onShowTrayAction(bool)));
	connect(autoSyncAction, SIGNAL(toggled(bool)), this, SLOT(onAutoSyncAction(bool)));
	connect(showProgressBarAction, SIGNAL(toggled(bool)), this, SLOT(onShowProgressBarAction(bool)));
	connect(closeToTrayAction, SIGNAL(toggled(bool)), this, SLOT(onCloseToTrayAction(bool)));

	about = menuBar()->addMenu(tr("&about"));
	about->addAction("ESW", this,  SLOT(handleAboutEswAction()));
	about->addAction("Qt", this,  SLOT(handleAboutQtAction()));

	trayIcon = new QSystemTrayIcon(QIcon(":/appicon"), this);
	trayMgr = new TrayManager (config, trayIcon);

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleTrayIcon(QSystemTrayIcon::ActivationReason)));
	if(config->loadShowTray()) trayIcon->show(); // only show when configured to show

	trayIconMenu = new QMenu;
	trayIconMenu->addAction(tr("update"), this, SLOT(onHTimer()));
	trayIconMenu->addAction(tr("exit"), this, SLOT(handleExitAction()));
	trayIcon->setContextMenu(trayIconMenu);

        skillTree = new WebDoc("http://api.eve-online.com/eve/SkillTree.xml.aspx", false, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/SkillTree.xml.aspx"));

//CharacterWidget, SkillTraining, SkillQueue for each char ////////////////////////////////////////////
	characterWidget = new QList<CharacterWidget*>;
	trainingWidget = new QList<SkillTraining*>;
	queueWidget = new QList<SkillQueue*>;

	accs = config->loadAccounts();

	for(int i = 0; i < accs.count(); i++)
		setupDockWidgets(i);
///////////////////////////////////////////////////////////////////////////////////////////////////////

	syncWidget = new SyncWidget(tr("next sync in"), "mm:ss", this);
	addDockWidget(Qt::TopDockWidgetArea, syncWidget);
	config->loadAutoSync() ? syncWidget->set(hTimer->interval()/1000) : syncWidget->disable();
	syncWidget->setObjectName("toolbar_sync");

	serverStat = new ServerStatWidget(tr("server status"), trayMgr, this);
	serverStat->setObjectName("toolbar_serverstats");
	addDockWidget(Qt::TopDockWidgetArea, serverStat);

	restoreState(config->loadState());
	setVisible(config->loadIsVisible());
	if(config->loadOntop())setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

	if(config->loadAutoSync())
	{
		serverStat->reload();
		for(int i = 0; i < accs.count(); i++)
			characterWidget->at(i)->reload();
		hTimer->start();
	}
	adjustSize();
}

void MainWindow::handleExitAction()
{
	QApplication::exit();
}

void MainWindow::handleAboutEswAction()
{
	QMessageBox::about( this, tr("about"), tr("<html>%1 %2<br>ESW<br><br>Copyright (C) 2008,2009 Psyjo<br><br><a href=\"http://www.code.google.com/p/eveskillwatcher/\">Project site</a><br><br>This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.<br><br>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br>See the GNU General Public License for more details.<br><br>You should have received a copy of the GNU General Public License along with this program; if not, see <a href=\"http://www.gnu.org/licenses/\">this link</a>.</html>").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()));
}

void MainWindow::handleAboutQtAction()
{
	QMessageBox::aboutQt ( this, tr("about"));
}

void MainWindow::onOntopAction(bool b)
{
	b ? setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint) : setWindowFlags(Qt::Dialog);
	setWindowIcon(QIcon(":/appicon"));
	show();
	config->saveOntop(b);
}

void MainWindow::onShowTrayAction(bool b)
{
	b ? trayIcon->show() : trayIcon->hide();
	config->saveShowTray(b);
}
void MainWindow::onAutoSyncAction(bool b)
{
	b ? hTimer->start() : hTimer->stop();
	b ? syncWidget->enable() : syncWidget->disable();
	config->saveAutoSync(b);
}

void MainWindow::onShowProgressBarAction(bool b)
{
	for(int i = 0; i < accs.count(); i++)
	{
		trainingWidget->at(i)->reload();
		trainingWidget->at(i)->showProgressBar(b);
	}
	config->saveProgressBar(b);
}

void MainWindow::onCloseToTrayAction(bool b)
{
	config->saveCloseToTray(b);
}

void MainWindow::handleTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason)
	{
		case QSystemTrayIcon::Trigger:
			if(isVisible())
			{
				hide();
				config->saveIsVisible(false);
			}
			else
			{
				show();
				config->saveIsVisible(true);
			}
			break;

		case QSystemTrayIcon::Context:
			break;
		case QSystemTrayIcon::DoubleClick:
			break;
		case QSystemTrayIcon::MiddleClick:
			break;
		case QSystemTrayIcon::Unknown:
			break;
	}

}

template <typename T> void MainWindow::delDockWidget(QList<T*>* w, int i)
{
	removeDockWidget(w->at(i));
	w->at(i)->deleteLater();
	w->removeAt(i);
}

template <typename T> void MainWindow::setupDockWidget(QList<T*>* w, int i)
{
	addDockWidget(Qt::TopDockWidgetArea, w->at(i));
	w->at(i)->setObjectName(config->loadAccounts().at(i));
}

void MainWindow::delDockWidgets(int i)
{
	//qDebug() << "del " << i;
	delDockWidget(characterWidget, i);
	delDockWidget(trainingWidget, i);
	delDockWidget(queueWidget, i);
}
void MainWindow::setupDockWidgets(int i)
{
	//qDebug() << "create " << i;
	characterWidget->insert(i, new CharacterWidget(tr("Character"), config->loadAccounts().at(i), config, this));
	setupDockWidget(characterWidget, i);

	trainingWidget->insert(i, new SkillTraining(config, trayMgr, skillTree, tr("skilltraining"), config->loadAccounts().at(i), this));
	setupDockWidget(trainingWidget, i);
//	qDebug() << "";
//	trainingWidget->at(i)->setObjectName(config->loadAccounts().at(i));

	queueWidget->insert(i, new SkillQueue(config, trayMgr, skillTree, tr("skilltqueue"), config->loadAccounts().at(i), this));
	setupDockWidget(queueWidget, i);
}

/*
void MainWindow::handleInputApiAction()
{
	QStringList l = config->loadAccounts();
	ApiInput input(tr("API"), config, this);
	if(input.exec())
	{
		bool found;
		if( accs.count() > config->loadAccounts().count() )
		{
			for(int i = accs.count()-1; i >= config->loadAccounts().count(); i--)
				delDockWidgets(i);
		}
		else
		{		
			for(int i = accs.count(); i < config->loadAccounts().count(); i++)
				setupDockWidgets(i);
		}
		
		accs = config->loadAccounts();

		// search for accs[i] matches all of the widgets accounts
		// if applicable recreate them
		for(int i = 0; i < accs.count(); i++)
		{
			found = false;
			foreach(QString s, accs)
				if(characterWidget->at(i)->getAccount() == s &&
				    trainingWidget->at(i)->getAccount() == s && 
				       queueWidget->at(i)->getAccount() == s )
				{
					found = true;
					break;
				}
			if(!found)
			{	
				delDockWidgets(i);
				setupDockWidgets(accs.count()-1);
			}
		}
		onHTimer();
	}
	trayMgr->flush();
}*/

void MainWindow::handleInputApiAction()
{
	ApiInput input(tr("API"), config, this);
	if(input.exec())
	{	
		QStringList l = config->loadAccounts();
		bool found;
		while (characterWidget->count() != l.count())
		{
			for (int i = 0; i < characterWidget->count(); i++)
			{
				found = false;
				for (int j = 0; j < l.count(); j++)
				{
					if( l.at(j) == characterWidget->at(i)->objectName() )
					{
						found = true;
						break;
					}
				}
				if (!found) delDockWidgets(i);
			}
			for (int i = 0; i < l.count(); i++)
			{
				found = false;
				for (int j = 0; j < characterWidget->count(); j++)
				{
					if( l.at(i) == characterWidget->at(j)->objectName() )
					{
						found = true;
						break;
					}
				}	
				if (!found) setupDockWidgets(i);
			}
		} // while
		accs = config->loadAccounts();
		onHTimer();
		trayMgr->flush();
	} // if
}

void MainWindow::onHTimer()
{
	serverStat->reload();
	for(int i = 0; i < accs.count(); i++)
	{
		characterWidget->at(i)->reload();
		trainingWidget->at(i)->reload();
		queueWidget->at(i)->reload();
	}
	syncWidget->set(hTimer->interval()/1000);
	hTimer->start();
	statusBar->showMessage(tr("last update @ %1.").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")), 0);
}

void MainWindow::onStyleMenu(QAction* a)
{
	setStyle(QStyleFactory::create(a->text()));
	config->saveStyle(a->text());
}

void MainWindow::closeEvent ( QCloseEvent *event )
{
	// tray is visible -> close to tray
	// else close app
	trayIcon->isVisible() ? event->accept() : QApplication::exit();
	// if close-to-tray is set, do so
	config->loadCloseToTray() ? event->accept() : QApplication::exit();
	// save this state
	trayIcon->isVisible() ? config->saveIsVisible(false) : config->saveIsVisible(true);

	if(config->loadCloseToTray() && trayIcon->isVisible())
	{
		if(config->loadCloseToTrayTip())
		{
			connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(handleMinimizedTip()));
			// watch for click on message
			QTimer::singleShot(10000, this, SLOT(disconnectMinimizedTip()));
			// disconnect watch after 10 secs
			trayMgr->showMessage("", tr("%1 minimized.").arg(QApplication::applicationName()));
		}
	}
}

void MainWindow::disconnectMinimizedTip() // is needed as slot, see top function
{
//	disconnect(trayIcon, SIGNAL(messageClicked()), 0, 0);
	disconnect(trayIcon, 0, this, 0);
}

void MainWindow::handleMinimizedTip()
{
	// buttons need to be localized
	QMessageBox(	QMessageBox::Question,
			tr("tip"),
			tr("show this tip further ?"),
			QMessageBox::Yes | QMessageBox::No,
			this)
	.exec() == QMessageBox::No ? config->saveCloseToTrayTip(0) : config->saveCloseToTrayTip(1);
	disconnectMinimizedTip();
}

MainWindow::~MainWindow()
{
	config->saveState(saveState());
	delete config;
}

