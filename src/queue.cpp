
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

#include "queue.h"

SkillQueue::SkillQueue(ConfigHandler* c, QSystemTrayIcon* ico, WebDoc *t, WebDoc *q, QString name, QWidget* parent)
        : QDockWidget(name, parent)
{
	conf = c;
	tray = ico;
	queue = q;
	skillTree = t;
	
	skillTreeAvailable = false;
	skillTree->get();

	connect(skillTree, SIGNAL(done(bool)), this, SLOT(onSkillTreeDone(bool)));
	connect(queue, SIGNAL(done(bool)), this, SLOT(onQueueDone(bool)));

	startTime = new QDateTime;
	endTime   = new QDateTime;

	startTime->setTimeSpec(Qt::UTC);
	endTime->setTimeSpec(Qt::UTC);

	contentLabel = new QLabel(this);
	contentLabel->setTextFormat(Qt::RichText);
	contentLabel->setMargin(3);
	contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

	setWidget(contentLabel);

//	preDayTimer = new QTimer(this);
//	connect(preDayTimer, SIGNAL(timeout()), this, SLOT(onPreDayTimer()));
}

SkillQueue::~SkillQueue(){};

void SkillQueue::reload()
{
	if(skillTreeAvailable)
	{
		queue->get(QString("?userID=%1&apiKey=%2&characterID=%3")
						.arg(conf->loadApiInfo().userID)
						.arg(conf->loadApiInfo().apiKey)
						.arg(conf->loadApiInfo().characterID)
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

				// #1	
				content.append("#" + l.at(cnt).toElement().attribute("queuePosition") + " <i>");
				// #1	2009-01-01 (someday) 00:00:00
				content.append(startTime->toString("yyyy-MM-dd (dddd) hh:mm:ss"));
				// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00
				content.append(" - " + endTime->toString("yyyy-MM-dd (dddd) hh:mm:ss"));
				// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00	someskill
				content.append("</i> <b>" + skillName(l.at(cnt).toElement().attribute("typeID").toInt()));
				// #1	2009-01-01 (someday) 00:00:00 - 2009-02-02 (someotherday) 00:00:00	someskill (0 -> 1)
				content.append("</b> (" + QString::number(l.at(cnt).toElement().attribute("level").toInt()-1) + " -> " + l.at(cnt).toElement().attribute("level") + ")<br>");
			}
			contentLabel->setText(content);
		}
		else
			contentLabel->clear();
	}
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
