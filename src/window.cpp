
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
	setWindowTitle(tr("appName"));

	config = new ConfigHandler(QDir::toNativeSeparators(QDir::homePath ()  + "/.esw.xml"), "esw_configuration");
	setStyle(QStyleFactory::create(config->loadStyle()));
	setStyleSheet(config->loadStyleSheet());

	hTimer = new QTimer(this);
	hTimer->setInterval(3600000); // 1h
	connect(hTimer, SIGNAL(timeout()), this, SLOT(onHTimer()));

	statusBar = new QStatusBar(this);
	setStatusBar(statusBar);

	mFile = menuBar()->addMenu(tr("&file"));
	mFile->addAction(tr("exit"));
	connect(mFile, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));

	mAction = menuBar()->addMenu(tr("A&ction"));
	mAction->addAction(tr("input API"));
	updateAction = new QAction(tr("update"), this);
	updateAction->setShortcut(QKeySequence("F5")); 
	mAction->addAction(tr("update"));
	connect(mAction, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));

	mStyle = new QMenu(tr("&Style"), this);
	foreach(QString s, QStyleFactory::keys())	// fill in all available Styles
		mStyle->addAction(s);			//
	connect(mStyle, SIGNAL(triggered(QAction*)), this, SLOT(onStyleMenu(QAction*)));

	mOption = menuBar()->addMenu(tr("&Options"));
	ontopAction = new QAction(tr("always on &top"), this);
	showTrayAction = new QAction(tr("show tray &icon"), this);
	autoSyncAction = new QAction(tr("autosync"), this);
	showProgressBarAction = new QAction(tr("show progressbar"), this);
	ontopAction->setCheckable(true);
	showTrayAction->setCheckable(true);
	autoSyncAction->setCheckable(true);
	showProgressBarAction->setCheckable(true);
	showTrayAction->setChecked(config->loadShowTray());
	ontopAction->setChecked(config->loadOntop());
	autoSyncAction->setChecked(config->loadAutoSync());
	showProgressBarAction->setChecked(config->loadProgressBar());
	mOption->addAction(ontopAction);
	mOption->addAction(showTrayAction);
	mOption->addAction(autoSyncAction);
	mOption->addAction(showProgressBarAction);
	mOption->addSeparator();
	mOption->addMenu(mStyle);
	connect(ontopAction, SIGNAL(toggled(bool)), this, SLOT(onOntopAction(bool)));
	connect(showTrayAction, SIGNAL(toggled(bool)), this, SLOT(onShowTrayAction(bool)));
	connect(autoSyncAction, SIGNAL(toggled(bool)), this, SLOT(onAutoSyncAction(bool)));
	connect(showProgressBarAction, SIGNAL(toggled(bool)), this, SLOT(onShowProgressBarAction(bool)));

	trayIcon = new QSystemTrayIcon(QIcon(":/appicon"), this);
	about = menuBar()->addMenu(tr("&about"));
	about->addAction("ESW");
	about->addAction("Qt");
	connect(about, SIGNAL(triggered(QAction*)), this, SLOT(handleAboutAction(QAction*)));

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleTrayIcon(QSystemTrayIcon::ActivationReason)));
	if(config->loadShowTray()) trayIcon->show(); // only show when configured to show

	trayIconMenu = new QMenu;
	trayIconMenu->addAction(tr("update"));
	trayIconMenu->addAction(tr("exit"));
	connect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));
	trayIcon->setContextMenu(trayIconMenu);

	trainingWidget = new SkillTraining(config, trayIcon, tr("skilltraining"), this);
	addDockWidget(Qt::TopDockWidgetArea, trainingWidget);
	trainingWidget->showProgressBar(config->loadProgressBar());
	trainingWidget->setObjectName("toolbar_training");

	syncWidget = new SyncWidget(tr("next sync in"), "mm:ss", this);
	addToolBar(syncWidget);
	config->loadAutoSync() ? syncWidget->set(hTimer->interval()/1000) : syncWidget->disable();
	syncWidget->setObjectName("toolbar_sync");

	serverStat = new ServerStatWidget(tr("server status"), trayIcon, this);
	serverStat->setObjectName("toolbar_serverstats");
	addToolBar(serverStat);

	characterWidget = new CharacterWidget(tr("Character"), config, this);
	addToolBar(characterWidget);
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

void MainWindow::handleAboutAction(QAction* a)
{
	if (a->text() == "ESW")
	QMessageBox::about( this, tr("about"), tr("<html>%1<br>ESW<br><br>Copyright (C) 2008,2009 Psyjo<br><br><a href=\"http://www.code.google.com/p/eveskillwatcher/\">Project site</a><br><br>This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.<br><br>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br>See the GNU General Public License for more details.<br><br>You should have received a copy of the GNU General Public License along with this program; if not, see <a href=\"http://www.gnu.org/licenses/\">this link</a>.</html>").arg(tr("appName")));
	if (a->text() == "Qt") QMessageBox::aboutQt ( this, tr("about"));
}

void MainWindow::handleFileAction(QAction* a)
{
	if (a->text() == tr("exit")) close();
	if (a->text() == tr("input API")) onApiInput();
	if (a->text() == tr("update")) onHTimer();
}

void MainWindow::onOntopAction(bool b)
{
	b ? setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint) : setWindowFlags(Qt::Window);
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

void MainWindow::onApiInput()
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

MainWindow::~MainWindow()
{
	config->saveState(saveState());
	delete config;
}

