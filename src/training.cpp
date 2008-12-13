
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

SkillTraining::SkillTraining(ConfigHandler* c, QSystemTrayIcon* ico, QWidget* parent)
{
	setParent(parent);

	conf = c;
	tray = ico;

	skillSP    = new QVector<int>(5);
	skillSP->insert(1, 250);
	skillSP->insert(2, 1415);
	skillSP->insert(3, 8000);
	skillSP->insert(4, 45255);
	skillSP->insert(5, 256000);

	todoTimeStringList = new QStringList();

	skillTreeAvailable = false;

	skillTree = new WebDoc("http://api.eve-online.com//eve/SkillTree.xml.aspx");
	characterTraining = new WebDoc("http://api.eve-online.com/char/SkillInTraining.xml.aspx");
	connect(skillTree, SIGNAL(done(bool)), this, SLOT(onSkillTreeDone(bool)));
	connect(characterTraining, SIGNAL(done(bool)), this, SLOT(onCharacterTrainingDone(bool)));

	skillTree->get();

	beginTime = new QDateTime;
	endTime   = new QDateTime;
	syncTime = new QDateTime;

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

	sTimer->setInterval(1000);
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
	for(int i = 0; i < l.size(); i++)
	{
		if(l.item(i).toElement().attribute("typeID").toInt() == id)
		{
			if(l.item(i).toElement().hasAttribute("typeName"))
				return l.item(i).toElement().attribute("typeName", "<this schould not happend>");
		}
	}
	return "Unknown";
}

int SkillTraining::skillRank(int id)
{
	QDomNodeList l = skillTree->document()->documentElement().elementsByTagName("row");
	for(int i = 0; i < l.size(); i++)
	{
		if(l.item(i).toElement().attribute("typeID").toInt() == id)
		{
			if(l.item(i).toElement().hasAttribute("typeName"))
				return l.item(i).firstChildElement("rank").text().toInt();
		}
	}
	return 1;
}

double SkillTraining::currentSP()
{
	return el->firstChildElement("trainingStartSP").text().toDouble()
		+ trainFactor * QDateTime::fromString(el->firstChildElement("trainingStartTime").text(), "yyyy-MM-dd hh:mm:ss").secsTo(QDateTime::currentDateTime());
}

void SkillTraining::onSTimer()
{
	if(currentSP() >= el->firstChildElement("trainingDestinationSP").text().toDouble())
	{
		sTimer->stop();
		tray->showMessage ( "", tr("Skilltraining \"%1\" (%2) completed.").arg(currentSkillName).arg(currentSkillLevel), QSystemTrayIcon::NoIcon, 60000 );		
	}
	syncLabel->setText(syncTime->fromTime_t(syncTime->currentDateTime().secsTo(*syncTime)).toString("mm:ss"));
	spLabel->setText(QString("%1  / %2 (%3%)")
				.arg(currentSP(), 0, 'f', 1)
				.arg(el->firstChildElement("trainingDestinationSP").text())
				//.arg((currentSP() - currentSkillRank * skillSP->at(currentSkillRank -1) )
				//	/ el->firstChildElement("trainingDestinationSP").text().toDouble() * 100, 0, 'f', 1)
				.arg(currentSP() / el->firstChildElement("trainingDestinationSP").text().toDouble() * 100, 0, 'f', 1)
	);
	*todoTimeStringList = endTime->fromTime_t(endTime->currentDateTime().secsTo(*endTime) - 86400).toString("d:h:h:m:s").split(":");
	etaLabel->setText(tr("%n d(s), ", "", todoTimeStringList->at(1).toInt())
			+ tr("%n h(s), ", "", todoTimeStringList->at(2).toInt())
			+ tr("%n m(s), ", "", todoTimeStringList->at(3).toInt())
			+ tr("%n s(s), ", "", todoTimeStringList->at(4).toInt())	
		);
	tray->setToolTip(skillLabel->text() + "\n" + skillLevelLabel->text() + "\n" + spLabel->text() + "\n" + etaLabel->text());
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
		if(el->firstChildElement("skillInTraining").text().toInt())
		{
			*beginTime = beginTime->fromString(el->firstChildElement("trainingStartTime").text(), "yyyy-MM-dd hh:mm:ss");
			*endTime = endTime->fromString(el->firstChildElement("trainingEndTime").text(), "yyyy-MM-dd hh:mm:ss");
			skillLabel->setText(skillName(el->firstChildElement("trainingTypeID").text().toInt()));
			currentSkillName = skillLabel->text();
			skillLevelLabel->setText(QString("%1 -> %2")
							.arg(iToRoman(el->firstChildElement("trainingToLevel").text().toInt() - 1))
							.arg(iToRoman(el->firstChildElement("trainingToLevel").text().toInt()))
							);
			currentSkillLevel = skillLevelLabel->text();
			currentSkillRank = skillRank(el->firstChildElement("trainingTypeID").text().toInt());
			// (destSP - startSP) / (secs from startTime to EndTime) == SP/sec
			trainFactor = (el->firstChildElement("trainingDestinationSP").text().toDouble() - el->firstChildElement("trainingStartSP").text().toDouble())
					/ beginTime->secsTo(QDateTime::fromString(el->firstChildElement("trainingEndTime").text(), "yyyy-MM-dd hh:mm:ss"));
			sTimer->start();
		}
		else
		{
			skillLabel->clear();
			skillLevelLabel->clear();
			tray->showMessage ( tr("Warning"), tr("There is currently no skill in Training!"), QSystemTrayIcon::NoIcon, 60000 );
		}
		*syncTime = syncTime->currentDateTime().addSecs(3600);
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
		//sTimer->start(1000); // 1s
	}
}
