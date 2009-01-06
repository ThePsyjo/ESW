
/************************************************************************
 * window.h								*
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QStatusBar>

#include "configuration.h"
#include "apiInput.h"
#include "training.h"
#include "syncWidget.h"
#include "serverStatWidget.h"
#include "characterWidget.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
	MainWindow( QWidget * parent =0, Qt::WFlags f =0 );
	virtual ~MainWindow();
private:
	QMenu *about, *mFile, *mAction, *mOption;
	QAction *ontopAction, *showTrayAction, *updateAction, *autoSyncAction, *showProgressBarAction;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	ConfigHandler *config;
	SkillTraining *trainingWidget;
	SyncWidget *syncWidget;
	ServerStatWidget *serverStat;
	CharacterWidget *characterWidget;
	QTimer *hTimer;
	void onApiInput();
	QStatusBar *statusBar;
private slots:
	void handleAboutAction(QAction* a);
	void handleFileAction(QAction* a);
	void handleTrayIcon(QSystemTrayIcon::ActivationReason);
	void onHTimer();
	void onOntopAction(bool);
	void onShowTrayAction(bool);
	void onAutoSyncAction(bool);
	void onShowProgressBarAction(bool);
};

#endif

