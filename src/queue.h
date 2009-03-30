
/************************************************************************
 * queue.h								*
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

#ifndef QUEUE_H
#define QUEUE_H

#include <QLabel>
#include <QSystemTrayIcon>
#include <QToolBar>
//#include <QVBoxLayout>

#include "webDoc.h"
#include "configuration.h"

class SkillQueue : public QToolBar
{
Q_OBJECT
public:
	SkillQueue(ConfigHandler*, QSystemTrayIcon*, WebDoc*, WebDoc*, QString name, QWidget* parent =0);
	virtual ~SkillQueue();
private:
	ConfigHandler *conf;
	QSystemTrayIcon *tray;
	WebDoc *skillTree, *queue;
	QLabel *contentLabel;
//	QTimer *preDayTimer;
	QDateTime *startTime, *endTime;
	bool skillTreeAvailable;
	QString skillName(int);
	QString content;
	int cnt;
private slots:
//	void onPreDayTimer();
	void onSkillTreeDone(bool);
	void onQueueDone(bool);
public slots:
	void reload();
};

#endif

