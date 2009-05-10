
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

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleTrayIcon(QSystemTrayIcon::ActivationReason)));
	if(config->loadShowTray()) trayIcon->show(); // only show when configured to show

	trayIconMenu = new QMenu;
	trayIconMenu->addAction(tr("update"), this, SLOT(onHTimer()));
	trayIconMenu->addAction(tr("exit"), this, SLOT(handleExitAction()));
	trayIcon->setContextMenu(trayIconMenu);

        skillTree = new WebDoc("http://api.eve-online.com/eve/SkillTree.xml.aspx", false, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/SkillTree.xml.aspx"));
	skillQueue = new WebDoc("http://api.eve-online.com/char/skillqueue.xml.aspx", true, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/skillqueue.xml.aspx"));

	trainingWidget = new SkillTraining(config, trayIcon, skillTree, skillQueue, tr("skilltraining"), this);
	addDockWidget(Qt::TopDockWidgetArea, trainingWidget);
	trainingWidget->showProgressBar(config->loadProgressBar());
	trainingWidget->setObjectName("toolbar_training");

	queueWidget = new SkillQueue(config, trayIcon, skillTree, skillQueue, tr("skilltqueue"), this);
	addDockWidget(Qt::TopDockWidgetArea, queueWidget);
	queueWidget->setObjectName("toolbar_skillqueue");

	syncWidget = new SyncWidget(tr("next sync in"), "mm:ss", this);
	addDockWidget(Qt::TopDockWidgetArea, syncWidget);
	config->loadAutoSync() ? syncWidget->set(hTimer->interval()/1000) : syncWidget->disable();
	syncWidget->setObjectName("toolbar_sync");

	serverStat = new ServerStatWidget(tr("server status"), trayIcon, this);
	serverStat->setObjectName("toolbar_serverstats");
	addDockWidget(Qt::TopDockWidgetArea, serverStat);

	characterWidget = new CharacterWidget(tr("Character"), config, this);
	addDockWidget(Qt::TopDockWidgetArea, characterWidget);
	characterWidget->setObjectName("toolbar_character");

	restoreState(config->loadState());
	setVisible(config->loadIsVisible());
	if(config->loadOntop())setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

	if(config->loadAutoSync())
	{
		serverStat->reload();
		characterWidget->reload();
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
	trainingWidget->showProgressBar(b);
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

void MainWindow::handleInputApiAction()
{
	ApiInput input(tr("API"), config, this);
	input.show();
	if(input.exec())
		onHTimer();
}

void MainWindow::onHTimer()
{
	trainingWidget->reload();
	serverStat->reload();
	characterWidget->reload();
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
			trayIcon->showMessage("", tr("%1 minimized.").arg(QApplication::applicationName()));
		}
	}
}

void MainWindow::disconnectMinimizedTip() // is needed as slot, see top function
{
	disconnect(trayIcon, SIGNAL(messageClicked()), 0, 0);
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

