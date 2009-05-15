
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
#include <QStyleFactory>
#include <QApplication>
#include <QCloseEvent>
#include <QList>

#include "configuration.h"
#include "apiInput.h"
#include "training.h"
#include "queue.h"
#include "syncWidget.h"
#include "serverStatWidget.h"
#include "characterWidget.h"
#include "webDoc.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
	MainWindow( QWidget * parent =0, Qt::WFlags f =0 );
	virtual ~MainWindow();
protected:
	void closeEvent ( QCloseEvent *event );
private:
	QMenu *about, *mFile, *mAction, *mOption, *mStyle;
	QAction *ontopAction, *showTrayAction, *autoSyncAction, *showProgressBarAction, *closeToTrayAction;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	ConfigHandler *config;
	QList<SkillTraining*> *trainingWidget;
	QList<SkillQueue*> *queueWidget;
	QList<CharacterWidget*> *characterWidget;
	SyncWidget *syncWidget;
	ServerStatWidget *serverStat;
	QTimer *hTimer;
	QStatusBar *statusBar;
	WebDoc *skillTree;
	QWidget *dummy;
	int accs;
private slots:
	void handleAboutEswAction();
	void handleAboutQtAction();
	void handleTrayIcon(QSystemTrayIcon::ActivationReason);
	void onHTimer();
	void onOntopAction(bool);
	void onShowTrayAction(bool);
	void onAutoSyncAction(bool);
	void onShowProgressBarAction(bool);
	void onCloseToTrayAction(bool);
	void onStyleMenu(QAction*);
	void handleMinimizedTip();
	void disconnectMinimizedTip();
	void handleExitAction();
	void handleInputApiAction();
};

#endif

