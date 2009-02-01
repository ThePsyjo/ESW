
/************************************************************************
 * training.h								*
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

#ifndef TRAINING_H
#define TRAINING_H

#include <QLabel>
#include <QSystemTrayIcon>
#include <QDockWidget>
#include <QProgressBar>
#include <QVBoxLayout>

#include "webDoc.h"
#include "configuration.h"

class SkillTraining : public QDockWidget
{
Q_OBJECT
public:
	SkillTraining(ConfigHandler*, QSystemTrayIcon*, QString name, QWidget* parent =0);
	virtual ~SkillTraining();
private:
	ConfigHandler *conf;
	WebDoc *skillTree, *characterTraining;
	QLabel *contentLabel;
	QString skill, skillLevel, sp, eta, sync, rate, endTimeStr, endTimeStrFmt;
	QTimer *sTimer, *skillEndTimer, *preNotifyTimer;
	QDateTime *beginTime, *endTime;
	bool skillTreeAvailable;
	QString skillName(int);
	int getSkillRank(int);
	int skillRank, cnt;
	QDomElement *el;
	QString iToRoman(int);
	double trainFactor;
	double currentSP(), currentLevelSP(), lastSP(), destinationSP(), destinationLevelSP();
	QSystemTrayIcon *tray;
	QStringList *todoTimeStringList;
	void genContent();
	void genEndTime();
	QWidget *contentWidget;
	QVBoxLayout *contentWidgetLayout;
	QProgressBar *progressBar;
private slots:
	void onSTimer();
	void onSkillEndTimer();
	void onPreNotifyTimer();
	void onSkillTreeDone(bool);
	void onCharacterTrainingDone(bool);
public slots:
	void reload();
	void showProgressBar(bool);
};

#endif

