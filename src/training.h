
/************************************************************************
 * training.h								*
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

#ifndef TRAINING_H
#define TRAINING_H

#include <QLabel>
#include <QSystemTrayIcon>
#include <QDockWidget>
//#include <QToolBar>

#include "webDoc.h"
#include "configuration.h"

class SkillTraining : public QDockWidget
//class SkillTraining : public QToolBar
{
Q_OBJECT
public:
	SkillTraining(ConfigHandler*, QSystemTrayIcon*, QString name, QWidget* parent =0);
	virtual ~SkillTraining();
private:
	ConfigHandler *conf;
	WebDoc *skillTree, *characterTraining;
	QLabel *content;
	QString skill, skillLevel, sp, eta, sync, rate;
	QTimer *sTimer;
	QDateTime *beginTime, *endTime;
	bool skillTreeAvailable;
	QString skillName(int);
	int skillRank(int);
	QDomElement *el;
	QString iToRoman(int);
	QVector<int> *skillSP;
	double trainFactor;
	double currentSP();
	QSystemTrayIcon *tray;
	QStringList *todoTimeStringList;
	void genContent();
private slots:
	void onSTimer();
	void onSkillTreeDone(bool);
	void onCharacterTrainingDone(bool);
	void retrySkillTree();
public slots:
	void reload();
};

#endif

