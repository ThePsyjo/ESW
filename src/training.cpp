
/************************************************************************
 * training.cpp								*
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

#include "training.h"

SkillTraining::SkillTraining(ConfigHandler* c, TrayManager* traymgr, WebDoc *t, QString name, QString acc, QWidget* parent)
        : QDockWidget(name + " - " + acc, parent)
{
	conf = c;
	tray = traymgr;
	account = acc;

	todoTimeStringList = new QStringList();
	skillTree = t;
	characterTraining = new WebDoc("http://api.eve-online.com/char/skillqueue.xml.aspx", true, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/skillqueue.xml.aspx." + account + ".cache"));

	clipboard = QApplication::clipboard();

	connect(skillTree, SIGNAL(done(bool)), this, SLOT(onSkillTreeDone(bool)));
	connect(characterTraining, SIGNAL(done(bool)), this, SLOT(onCharacterTrainingDone(bool)));

	skillTreeAvailable = false;

	skillTree->get();

	beginTime = new QDateTime;
	endTime   = new QDateTime;

	beginTime->setTimeSpec(Qt::UTC);
	endTime->setTimeSpec(Qt::UTC);

	el = new QDomElement;

	contentWidget = new QWidget(this);

	skillLabel	= new QLabel(contentWidget);
	spLabel		= new QLabel(contentWidget);
	etaLabel	= new QLabel(contentWidget);
	endTimeStrLabel	= new QLabel(contentWidget);
	rateLabel	= new QLabel(contentWidget);

	clipBoardButton = new QPushButton(":::", contentWidget);
	clipBoardButton->setToolTip(tr("copy to clipboard"));
	clipBoardButton->setMaximumWidth(clipBoardButton->height()); // only a sqare, not over full cell
	
	connect(clipBoardButton, SIGNAL(clicked()), this, SLOT(onClipBoardButtonClicked()));

	progressBar = new QProgressBar(contentWidget);
	progressBar->setRange(0, 1000);

	contentWidgetLayout = new QGridLayout(contentWidget);

	contentWidgetLayout->addWidget(skillLabel,	0, 0, 1, 2);
	contentWidgetLayout->addWidget(spLabel,		1, 0, 1, 2);
	contentWidgetLayout->addWidget(etaLabel,	2, 0, 1, 2);
	contentWidgetLayout->addWidget(endTimeStrLabel,	3, 0, 1, 2);
	contentWidgetLayout->addWidget(rateLabel,	4, 0);
	contentWidgetLayout->addWidget(clipBoardButton,	4, 1);
	contentWidgetLayout->addWidget(progressBar,	5, 0, 1, 2);

	setWidget(contentWidget);

	sTimer = new QTimer(this);
	connect(sTimer, SIGNAL(timeout()), this, SLOT(onSTimer()));
	sTimer->setInterval(1000);

	skillEndTimer = new QTimer(this);
	skillEndTimer->setSingleShot(true);
	connect(skillEndTimer, SIGNAL(timeout()), this, SLOT(onSkillEndTimer()));

	preNotifyTimer = new QTimer(this);
	preNotifyTimer->setSingleShot(true);
	connect(preNotifyTimer, SIGNAL(timeout()), this, SLOT(onPreNotifyTimer()));
}

SkillTraining::~SkillTraining(){};

QString SkillTraining::getAccount()
{
        return account;
}


void SkillTraining::onClipBoardButtonClicked()
{
	tray->showMessage ( tr("Info"), tr("copyed to clipboard"), QSystemTrayIcon::NoIcon, 5000 );
	clipboard->setText(QString("%1\n%2\n%3\n%4\n%5")
		.arg(skillLabel->text())
		.arg(spLabel->text())
		.arg(etaLabel->text())
		.arg(endTimeStrLabel->text())
		.arg(rateLabel->text()));
}

void SkillTraining::reload()
{
	if(skillTreeAvailable)
	{
		sTimer->stop();
		characterTraining->get(QString("?userID=%1&apiKey=%2&characterID=%3")
						.arg(conf->loadApiInfo(account).userID)
						.arg(conf->loadApiInfo(account).apiKey)
						.arg(conf->loadApiInfo(account).characterID)
						);
	}
	else
		skillTree->get();
}

void SkillTraining::genEndTime()
{
	if ( endTime->toLocalTime().date().day() == endTime->toLocalTime().currentDateTime().date().day() )
		endTimeStrFmt = "yyyy-MM-dd hh:mm:ss";		// if different day ->
	else	endTimeStrFmt = "yyyy-MM-dd (dddd) hh:mm:ss";	// show ending day

	// endtime > date @ endtime 1100  AND  endtime < date @ endtime 1200
	// -> downtime
	if (*endTime > QDateTime(endTime->date(), QTime(11, 0, 0), Qt::UTC) &&
	   (*endTime < QDateTime(endTime->date(), QTime(12, 0, 0), Qt::UTC)))
		endTimeStrLabel->setText(QString(tr("%1", "endTimeStr in downtime")  ).arg(endTime->toLocalTime().toString(endTimeStrFmt)));
	else	endTimeStrLabel->setText(QString(tr("%1", "endTimeStr ! in downtime")).arg(endTime->toLocalTime().toString(endTimeStrFmt)));
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

int SkillTraining::getSkillRank(int id)
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
	return el->attribute("startSP").toDouble()
		+ trainFactor * beginTime->secsTo(beginTime->currentDateTime());
}

double SkillTraining::lastSP()
{
// return SP of last skillLevel
	return pow(2, ((2.5 * ( el->attribute("level").toInt() - 1 )) - 2.5)/*/pow*/) * 250 * skillRank;
//				^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//					current skill level
}

double SkillTraining::currentLevelSP()
{
	if(el->attribute("level").toInt() == 1)
	// training 0 -> 1
		return currentSP();
	else
		return currentSP() - lastSP();
}

double SkillTraining::destinationSP()
{
	return el->attribute("endSP").toDouble();
}

double SkillTraining::destinationLevelSP()
{
	if(el->attribute("level").toInt() == 1)
	// training 0 -> 1 // same as above
		return destinationSP();
	else
		return destinationSP() - lastSP();
}

void SkillTraining::onSTimer()
{
//	if(*endTime >= endTime->currentDateTime())
//	{
		spLabel->setText(QString("%L1  / %L2 (%3%)")
					.arg(currentSP(), 3, 'f', 1)
					.arg(destinationSP(), 3, 'f', 1)
					.arg(currentLevelSP() / destinationLevelSP() * 100, 0, 'f', 1))
		;
		progressBar->setValue(currentLevelSP() / destinationLevelSP() * 1000);

		*todoTimeStringList = endTime->fromTime_t(endTime->currentDateTime().secsTo(*endTime)).toUTC().toString("h:m:s").split(":");
		// only time
		eta.clear();
		cnt = endTime->currentDateTime().secsTo(*endTime);
		if(cnt >= 86400)eta += tr("%n d(s), ", "", int(cnt / 86400)); // int(secs / 86400) = full days
		if(cnt >= 3600)	eta += tr("%n h(s), ", "", todoTimeStringList->at(0).toInt());
		if(cnt >= 60)	eta += tr("%n m(s), ", "", todoTimeStringList->at(1).toInt());
		if(cnt >= 0)	eta += tr("%n s(s)",   "", todoTimeStringList->at(2).toInt());
		etaLabel->setText(eta);

		tray->setToolTip(account, skillLabel->text() + "\n" + spLabel->text() + "\n" + eta);
//	}
}

QString SkillTraining::iToRoman(int i)
{
	switch(i)
	{
	case 0: return "";
	case 1: return "I";
	case 2: return "II";
	case 3: return "III";
	case 4: return "IV";
	case 5: return "V";
	default: return QString("%1").arg(i);
	}
}

void SkillTraining::onCharacterTrainingDone(bool ok)
{
	if(ok)
	{
		QDomNodeList l = characterTraining->document()->documentElement().elementsByTagName("row");
		*el = l.item(0).toElement();

		if(l.size() > 0)
		{
			*beginTime = beginTime->fromString(el->attribute("startTime"), "yyyy-MM-dd hh:mm:ss");
			*endTime = endTime->fromString(el->attribute("endTime"), "yyyy-MM-dd hh:mm:ss");
			beginTime->setTimeSpec(Qt::UTC);	// apparently it isn't enough
			endTime->setTimeSpec(Qt::UTC);		// to set it in Ctor

			genEndTime(); // generate endTimeStr // red if in downtime
			skillEndTimer->start((endTime->currentDateTime().secsTo(*endTime) + 2) * 1000); // set event when skilltraining is finished
			preNotifyTimer->start((endTime->currentDateTime().secsTo(*endTime) - 300) * 1000); // set event 5 minutes before training ends
			skillRank = getSkillRank(el->attribute("typeID").toInt());
			skillLabel->setText(QString("%1   %2 >>> %3")
							.arg(skillName(el->attribute("typeID").toInt()))
							.arg(iToRoman(el->attribute("level").toInt() - 1))
							.arg(iToRoman(el->attribute("level").toInt())))
							;
			trainFactor = (el->attribute("endSP").toDouble() - el->attribute("startSP").toDouble())
					/ beginTime->secsTo(*endTime);
			rateLabel->setText(QString("%1 SP/h").arg(trainFactor * 3600 , 0, 'f', 0));

			tray->setIcon(QIcon(":/appicon"));
			sTimer->start();
		}
		else
		{
			el->clear();
			skillLabel->clear();
			spLabel	->clear();
			etaLabel->clear();
			endTimeStrLabel->clear();
			rateLabel->clear();
			progressBar->reset();
			tray->showMessage ( tr("Warning"), tr("There is currently no skill in Training for \"%1\"!").arg(account), QSystemTrayIcon::NoIcon, 60000 );
			tray->setToolTip(account, tr("There is currently no skill in Training!"));
			tray->setIcon(QIcon(":/appicon_warn"));
		}
	}
}

void SkillTraining::onSkillTreeDone(bool ok)
{
	if(ok)
	{
		skillTreeAvailable = true;
		reload();
	}
}

void SkillTraining::onSkillEndTimer()
{
	tray->showMessage ( tr("Skilltraining"), tr("\"%1\" has completed Skilltraining \"%2\".").arg(account).arg(skillLabel->text()), QSystemTrayIcon::NoIcon, 60000 );
	skillEndTimer->singleShot(60000, this, SLOT(reload())); // reload after 1 minute
}

void SkillTraining::onPreNotifyTimer()
{
	tray->showMessage ( tr("Skilltraining"), tr("\"%1\" has completed Skilltraining \"%2\" soon.").arg(account).arg(skillLabel->text()).arg(account), QSystemTrayIcon::NoIcon, 60000 );
}

void SkillTraining::showProgressBar(bool b)
{ b ? progressBar->show() : progressBar->hide(); }
