
/************************************************************************
 * window.cpp								*
 * Copyright (C) 2008  Psyjo						*
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

	mOption = menuBar()->addMenu(tr("&Options"));
	ontopAction = new QAction(tr("always on &top"), this);
	showTrayAction = new QAction(tr("show tray &icon"), this);
	ontopAction->setCheckable(true);
	showTrayAction->setCheckable(true);
	showTrayAction->setChecked(config->loadShowTray());
	ontopAction->setChecked(config->loadOntop());
	mOption->addAction(ontopAction);
	mOption->addAction(showTrayAction);
	connect(mOption, SIGNAL(triggered(QAction*)), this, SLOT(handleOptionAction(QAction*)));

	trayIcon = new QSystemTrayIcon(QIcon(":appicon"), this);
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
	trainingWidget->setObjectName("toolbar_training");

	syncWidget = new SyncWidget(tr("next sync in"), "mm:ss", this);
	addToolBar(syncWidget);
	syncWidget->set(hTimer->interval()/1000);
	syncWidget->setObjectName("toolbar_sync");

	serverStat = new ServerStatWidget(tr("server status"), trayIcon, this);
	serverStat->reload();
	serverStat->setObjectName("toolbar_serverstats");
	addToolBar(serverStat);

	characterWidget = new CharacterWidget(tr("Character"), config, this);
	characterWidget->reload();
	addToolBar(characterWidget);
	characterWidget->setObjectName("toolbar_character");

	restoreState(config->loadState());
	adjustSize();
	setVisible(config->loadIsVisible());

	hTimer->start();
}

void MainWindow::handleAboutAction(QAction* a)
{
	if (a->text() == "ESW") QMessageBox::about ( this, tr("about"), tr("%1\nESW\n\nCopyright (C) 2008  Psyjo\n\n This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n See the GNU General Public License for more details.\n\n You should have received a copy of the GNU General Public License along with this program; if not, see http://www.gnu.org/licenses/.").arg(tr("appname")));
	if (a->text() == "Qt") QMessageBox::aboutQt ( this, tr("about"));
}

void MainWindow::handleFileAction(QAction* a)
{
	if (a->text() == tr("exit")) close();
	if (a->text() == tr("input API")) onApiInput();
	if (a->text() == tr("update")) onHTimer();
}

void MainWindow::handleOptionAction(QAction* a)
{
	if (a->text() == tr("always on &top"))
		if(a->isChecked())
		{
			setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
			show();
			config->saveOntop(true);
		}
		else
		{
			setWindowFlags(Qt::Window);
			show();
			config->saveOntop(false);
		}

	if (a->text() == tr("show tray &icon"))
		if(a->isChecked())
		{
			trayIcon->show();
			config->saveShowTray(true);
		}
		else
		{
			trayIcon->hide();
			config->saveShowTray(false);
		}
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

MainWindow::~MainWindow()
{
	config->saveState(saveState());
	delete config;
}

