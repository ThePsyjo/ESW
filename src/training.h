
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

#include <QDockWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>

#include "webDoc.h"
#include "configuration.h"

class SkillTraining : public QGroupBox
{
Q_OBJECT
public:
	SkillTraining(ConfigHandler*, QWidget*);
	virtual ~SkillTraining();
private:
	ConfigHandler *conf;
	WebDoc *skillTree, *characterTraining;
	QLabel *skillLabel, *skillLevelLabel, *spLabel, *etaLabel, *syncLabel, *rateLabel;
	QTimer *hTimer, *sTimer;
	QDateTime *beginTime, *endTime;
	QTime *syncTime;
	QGridLayout *layout;
	bool skillTreeAvailable;
	QString skillName(int);
	int skillRank(int);
	QDomElement *el;
	QString iToRoman(int);
	QVector<int> *skillSP;
	int currentSkillRank;
	double trainFactor;
	double currentSP();
private slots:
	void onSTimer();
	void onSkillTreeDone(bool);
	void onCharacterTrainingDone(bool);
public slots:
	void reload();
};
