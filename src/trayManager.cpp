
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
	icon = ico;
	config = c;
	array = new QMap<QString, QString>;

	for(int i = 0; i < config->loadAccounts().count(); i++)
		array->insert(config->loadAccounts().at(i), "");

}

TrayManager::~TrayManager(){}

//passthrough
void TrayManager::showMessage(QString message, QString title, QSystemTrayIcon::MessageIcon icon , int time )
{
	icon->showMessage(title, message, icon, time);
}

void TrayManager::setToolTip(QString account, QString text )
{
	array->insert(account, text);
	tipText.clear();
	foreach(QString k, array->keys())
		tipText.append("\n%1").arg(array->take(k));

	icon->setToolTip(tipText);
}
