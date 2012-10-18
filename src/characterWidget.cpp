
/************************************************************************
 * characterWidget.cpp							*
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

#include "characterWidget.h"

CharacterWidget::CharacterWidget(QString name, QString acc, ConfigHandler *c, QWidget * parent)
        : QDockWidget(name + " - " + acc, parent)
{
	conf = c;
	account = acc;
	content  = new QLabel(this);
	content->setMargin(3);
	content->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	setWidget(content);
	characterInfo = new WebDoc("https://api.eve-online.com/char/CharacterSheet.xml.aspx", false, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/CharacterSheet.xml.aspx." + account + ".cache"));
	connect(characterInfo, SIGNAL(done(bool)), this, SLOT(onWebDoc(bool)));
}

CharacterWidget::~CharacterWidget(){}

QString CharacterWidget::getAccount()
{
	return account;
}

QString CharacterWidget::getTitle()
{
	QDomNodeList l =  characterInfo->document()->documentElement().elementsByTagName("row");
	for(int i = l.size(); i >= 0; i--)
		if (l.item(i).toElement().hasAttribute("titleName"))
			return l.item(i).toElement().attribute("titleName");
		else
			continue;
	return "";
}

void CharacterWidget::onWebDoc(bool ok)
{
	if(ok)
	{
		QDomElement e = characterInfo->document()->documentElement().firstChildElement("result");
		content->setText(tr("%1\nMember of %2\nTitle %3\n%L4 ISK", "name, corpname, rank, balance")
					.arg(e.firstChildElement("name").text())
					.arg(e.firstChildElement("corporationName").text())
					.arg(getTitle())
					.arg(e.firstChildElement("balance").text().toDouble(), 3, 'f', 2)
		);
		if(characterInfo->isCached()) content->setText("<i>---cached---</i><br><br>" + content->text()); // translate ?
		
	}
}

void CharacterWidget::reload(bool force)
{
	characterInfo->get(	QString("?userID=%1&apiKey=%2&characterID=%3")
					.arg(conf->loadApiInfo(account).userID)
					.arg(conf->loadApiInfo(account).apiKey)
					.arg(conf->loadApiInfo(account).characterID), 
				force );
}
