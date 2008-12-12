
/************************************************************************
 * training.cpp								*
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

#include "training.h"

SkillTraining::SkillTraining(ConfigHandler* c, QWidget* parent)
{
	setParent(parent);

	conf = c;

	skillTreeAvailable = false;

	skillTree = new WebDoc("http://api.eve-online.com//eve/SkillTree.xml.aspx");
	characterTraining = new WebDoc("http://api.eve-online.com/char/SkillInTraining.xml.aspx");
	connect(skillTree, SIGNAL(done(bool)), this, SLOT(onSkillTreeDone(bool)));
	connect(characterTraining, SIGNAL(done(bool)), this, SLOT(onCharacterTrainingDone(bool)));

	skillTree->get();

	beginTime = new QDateTime;
	endTime   = new QDateTime;
	syncTime = new QTime;
	syncTime->setHMS(1,0,0);

	el = new QDomElement; // multi purpose element

	skillLabel = new QLabel(this);
	skillLevelLabel = new QLabel(this);
	spLabel = new QLabel(this);
	etaLabel = new QLabel(this);
	syncLabel = new QLabel(this);
	rateLabel = new QLabel(this);
	
	layout = new QGridLayout(this);

	layout->addWidget(skillLabel, 1, 1);
	layout->addWidget(skillLevelLabel, 1, 2);
	layout->addWidget(spLabel, 2, 1, 1, 2);
	layout->addWidget(etaLabel, 3, 1);
	layout->addWidget(rateLabel, 3, 2);
	layout->addWidget(syncLabel, 4, 2);

	adjustSize();

	hTimer = new QTimer(this);
	sTimer = new QTimer(this);

	connect(hTimer, SIGNAL(timeout()), this, SLOT(reload()));
	connect(sTimer, SIGNAL(timeout()), this, SLOT(onSTimer()));

	hTimer->start(3600000); // 1h
}

SkillTraining::~SkillTraining(){};

void SkillTraining::reload()
{
	if(skillTreeAvailable)
	{
		sTimer->stop();
		characterTraining->get(QString("?userID=%1&apiKey=%2&characterID=%3")
						.arg(conf->loadApiInfo().userID)
						.arg(conf->loadApiInfo().apiKey)
						.arg(conf->loadApiInfo().characterID)
						);
	}
}

QString SkillTraining::skillName(int id)
{
	QDomNodeList l = skillTree->document()->documentElement().elementsByTagName("row");
	qDebug() << QString("Number of rows: %1").arg(l.size());
	for(int i = 0; i < l.size(); i++)
	{
		if(l.item(i).toElement().attribute("typeID").toInt() == id)
		{
			qDebug() << QString("found @ %1").arg(i);
			if(l.item(i).toElement().hasAttribute("typeName"))
				return l.item(i).toElement().attribute("typeName", "<this schould not happend>");
		}
	}
	return "Unknown";
}

void SkillTraining::onSTimer()
{
	*syncTime = syncTime->addSecs(-1);
	syncLabel->setText(syncTime->toString("mm:ss"));
	spLabel->setText(QString("%1/%2 (%3%%)")
				.arg(el->firstChildElement("trainingDestinationSP").text().toInt() - (el->firstChildElement("trainingDestinationSP").text().toInt() - el->firstChildElement("trainingStartSP").text().toInt()))
				.arg(el->firstChildElement("trainingDestinationSP").text().toInt())
				.arg(el->firstChildElement("trainingStartSP").text().toDouble() / el->firstChildElement("trainingDestinationSP").text().toDouble() * 100)
	);
}

QString SkillTraining::iToRoman(int i)
{
	switch(i)
	{
	case 1: return "I";
	case 2: return "II";
	case 3: return "II";
	case 4: return "IV";
	case 5: return "V";
	default: return QString("%1").arg(i);
	}
}

void SkillTraining::onCharacterTrainingDone(bool ok)
{
	if(ok)
	{
		*el = characterTraining->document()->documentElement().firstChildElement("result");
		skillLabel->setText(skillName(el->firstChildElement("trainingTypeID").text().toInt()));
		skillLevelLabel->setText(QString("%1 -> %2")
						.arg(el->firstChildElement("trainingToLevel").text().toInt() - 1)
						.arg(el->firstChildElement("trainingToLevel").text().toInt())
						);
		sTimer->start();
		syncTime->setHMS(1,0,0);
		hTimer->stop();
		hTimer->start(3600000); // 1h
	}
}

void SkillTraining::onSkillTreeDone(bool ok)
{
	if(ok)
	{
		skillTreeAvailable = true;
		reload();
		sTimer->start(1000); // 1s
	}
}
