
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
	
	mFile = menuBar()->addMenu(tr("&file"));
	mFile->addAction(tr("exit"));
	connect(mFile, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));

	mAction = menuBar()->addMenu(tr("A&ction"));
	mAction->addAction(tr("input API"));
	mAction->addAction(tr("update"));
	connect(mAction, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));

	about = menuBar()->addMenu(tr("&about"));
	about->addAction("ESW");
	about->addAction("Qt");
	connect(about, SIGNAL(triggered(QAction*)), this, SLOT(handleAboutAction(QAction*)));

	trayIcon = new QSystemTrayIcon(QIcon(":appicon"), this);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleTrayIcon(QSystemTrayIcon::ActivationReason)));
	trayIcon->show();

	trayIconMenu = new QMenu;
	trayIconMenu->addAction(tr("update"));
	trayIconMenu->addAction(tr("exit"));
	connect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleFileAction(QAction*)));
	trayIcon->setContextMenu(trayIconMenu);

	trainingWidget = new SkillTraining(config, trayIcon, tr("skilltraining"), this);
	addDockWidget(Qt::TopDockWidgetArea, trainingWidget);
	//addToolBar(trainingWidget);
	trainingWidget->setObjectName("toolbar_training");

	hTimer = new QTimer(this);
	hTimer->setInterval(3600000); // 1h
	connect(hTimer, SIGNAL(timeout()), this, SLOT(onHTimer()));
	hTimer->start();

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
}

void MainWindow::handleAboutAction(QAction* a)
{
	if (a->text() == "ESW") QMessageBox::about ( this, tr("about"), "ESW by Psyjo - (GPL)");
	if (a->text() == "Qt") QMessageBox::aboutQt ( this, tr("about"));
}

void MainWindow::handleFileAction(QAction* a)
{
	if (a->text() == tr("exit")) close();
	if (a->text() == tr("input API")) onApiInput();
	if (a->text() == tr("update")) onHTimer();
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
}

MainWindow::~MainWindow()
{
	config->saveState(saveState());
	delete config;
}

