
/************************************************************************
 * queue.cpp								*
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
#ifndef SKILLQUEUE_H
#define SKILLQUEUE_H

#include "queue.h"
#define LESS "<<<"
#define MORE ">>>"

SkillQueue::SkillQueue(ConfigHandler* c, QSystemTrayIcon* ico, WebDoc *t, QString name, QString acc, QWidget* parent)
        : QDockWidget(name + " - " + acc, parent)
{
	conf = c;
	tray = ico;
	skillTree = t;
	account = acc;
	queue = new WebDoc("http://api.eve-online.com/char/skillqueue.xml.aspx", true, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/skillqueue.xml.aspx." + account + ".cache"));
	
	skillTreeAvailable = false;
	skillTree->get();

	connect(skillTree, SIGNAL(done(bool)), this, SLOT(onSkillTreeDone(bool)));
	connect(queue, SIGNAL(done(bool)), this, SLOT(onQueueDone(bool)));

	startTime = new QDateTime;
	endTime   = new QDateTime;

	startTime->setTimeSpec(Qt::UTC);
	endTime->setTimeSpec(Qt::UTC);

	contentWidget = new QWidget(this);
	layout = new QVBoxLayout(contentWidget);
//	contentWidget->setMargin(3)

	fullViewButton = new QPushButton(contentWidget);
	fullViewButton->setMaximumWidth(fullViewButton->height()); // only a sqare, not over full line
	QFont font;			//
	font.setBold(1);		// trashtype for bold font
	fullViewButton->setFont(font);	//
	connect(fullViewButton, SIGNAL(clicked()), this, SLOT(onFullViewButtonClick()));
	conf->loadShowFullQueueView() ? fullViewButton->setText(LESS) : fullViewButton->setText(MORE);
	conf->loadShowFullQueueView() ? fullViewButton->setToolTip(tr("collapse")) : fullViewButton->setToolTip(tr("expand"));
	fullView = conf->loadShowFullQueueView();

	contentLabel = new QLabel(contentWidget);
	contentLabel->setTextFormat(Qt::RichText);
	contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

	layout->addWidget(fullViewButton);
	layout->addWidget(contentLabel);

	setWidget(contentWidget);

//	contentWidget->setStyleSheet("* {background-color:red}");

//	preDayTimer = new QTimer(this);
//	connect(preDayTimer, SIGNAL(timeout()), this, SLOT(onPreDayTimer()));
}

SkillQueue::~SkillQueue(){};

QString SkillQueue::getAccount()
{
        return account;
}


void SkillQueue::reload()
{
	if(skillTreeAvailable)
	{
		queue->get(QString("?userID=%1&apiKey=%2&characterID=%3")
						.arg(conf->loadApiInfo(account).userID)
						.arg(conf->loadApiInfo(account).apiKey)
						.arg(conf->loadApiInfo(account).characterID)
						);
	}
	else
		skillTree->get();
}

void SkillQueue::onQueueDone(bool ok)
{
	if(ok)
	{
		QDomNodeList l = queue->document()->documentElement().elementsByTagName("row");

		content.clear();

		if(l.size() > 0)
		{
			for (cnt = 0; cnt < l.size(); cnt++)
			{
				*startTime = startTime->fromString(l.at(cnt).toElement().attribute("startTime"), "yyyy-MM-dd hh:mm:ss");
				*endTime = endTime->fromString(l.at(cnt).toElement().attribute("endTime"), "yyyy-MM-dd hh:mm:ss");
				startTime->setTimeSpec(Qt::UTC);	// apparently it isn't enough
				endTime->setTimeSpec(Qt::UTC);		// to set it in Ctor
				
				// 1 in queue	 && time ! active	      && less than 1 day remain
//				if(l.size() == 1 && ! preDayTimer->isActive() && endTimer->curentDateTime().secsTo(endTime) < 86400)
//					preDayTimer->start();

				if(cnt != 0)
					content.append("<br>");
				// #1
				content.append(QString("#%1 ").arg(l.at(cnt).toElement().attribute("queuePosition")));
				if(fullView) // if fullViewButton is clicked
					// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00
					content.append(QString(" <i>%1 - %2</i> ")
									.arg(startTime->toLocalTime().toString("yyyy-MM-dd (dddd) hh:mm:ss"))
									.arg(endTime->toLocalTime().toString("yyyy-MM-dd (dddd) hh:mm:ss")));
				// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00	someskill
				content.append(QString("<b>%1</b> ").arg(skillName(l.at(cnt).toElement().attribute("typeID").toInt())));
				// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00	someskill (0 -> 1)
				content.append( QString("(%1 -> %2)")
							.arg(QString::number(l.at(cnt).toElement().attribute("level").toInt()-1))
							.arg(l.at(cnt).toElement().attribute("level")));
			}
			contentLabel->setText(content);
		}
		else
			contentLabel->clear();
	}
}

void SkillQueue::onFullViewButtonClick()
{
	if(fullView)
	{
		fullView = false;
		conf->saveShowFullQueueView(false);
		fullViewButton->setText(MORE);
		fullViewButton->setToolTip(tr("expand"));
	}
	else
	{
		fullView = true;
		conf->saveShowFullQueueView(true);
		fullViewButton->setText(LESS);
		fullViewButton->setToolTip(tr("collapse"));
	}
	reload();
}

void SkillQueue::onSkillTreeDone(bool ok)
{
	if(ok)
	{
		skillTreeAvailable = true;
		reload();
	}
}

/*void SkillQueue::onPreDayTimer()
{
	tray->showMessage ( tr("Skillqueue"), tr("Last Entry in skillqueue will end in less than a day."), QSystemTrayIcon::NoIcon, 60000 );
}*/

QString SkillQueue::skillName(int id)
{
        QDomNodeList l = skillTree->document()->documentElement().elementsByTagName("row");
	for(int i = 0; i < l.size(); i++)
		if(l.item(i).toElement().attribute("typeID").toInt() == id)
			if(l.item(i).toElement().hasAttribute("typeName"))
				return l.item(i).toElement().attribute("typeName", "<this schould not happend>");
	return "Unknown";
}

#endif
