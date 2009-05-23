
/************************************************************************
 * trayManager.cpp							*
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

#include "trayManager.h"

TrayManager::TrayManager(ConfigHandler* c, QSystemTrayIcon* ico)
{
	trayIcon = ico;
	config = c;
	busy = false;
	toolTipArray = new QMap<QString, QString>;
	iconArray = new QMap<QString, bool>;

	flush();
	messageStack = new QStack<MessageData>;
	connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(freeStack()));
}

TrayManager::~TrayManager(){}

void TrayManager::setToolTip(QString account, QString text )
{
	toolTipArray->insert(account, text);
	tipText.clear();
	foreach(QString k, toolTipArray->keys())
		tipText.append(QString("%1:\n%2\n\n").arg(k).arg(toolTipArray->value(k)));
	trayIcon->setToolTip(tipText);
}

void TrayManager::showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon , int time )
{
	MessageData d;
	d.title = title;
	d.message = message;
	d.icon = icon;
	d.time = time;

	messageStack->push(d);
	checkMessageStack();
}
void TrayManager::setIcon(QString account, bool warn)
{
	iconArray->insert(account, warn);
	warn = false;
	foreach(bool b, iconArray->values())
		if(b) warn = true;
	
	warn ? trayIcon->setIcon(QIcon(":/appicon_warn")) : trayIcon->setIcon(QIcon(":/appicon"));
}

void TrayManager::checkMessageStack()
{
	if(!busy)
	{
		if(! messageStack->isEmpty())
		{
			busy = true;
			MessageData d;
			d = messageStack->pop();
			trayIcon->showMessage(d.title, d.message, d.icon, d.time);
			QTimer::singleShot(d.time, this, SLOT(freeStack()));
		}
	}
}

void TrayManager::freeStack()
{
	busy = false;
	checkMessageStack();
}

void TrayManager::flush()
{

	toolTipArray->clear();
	iconArray->clear();
	for(int i = 0; i < config->loadAccounts().count(); i++)
	{
		toolTipArray->insert(config->loadAccounts().at(i), "");
		iconArray->insert(config->loadAccounts().at(i), 0);
	}
}
