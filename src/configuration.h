
/************************************************************************
 * configuration.h							*
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QtXml>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QTimer>

class apiInfo
{
public:
	int userID;
	QString apiKey;
	int characterID;
};

class ConfigHandler : public QObject
{
Q_OBJECT
private:
	QDomDocument *doc;
	QFile *f;
	QDomElement genTag(QDomElement, QString);
	bool tagCreated, doSave, change;
	QTimer *timer;
public:
	ConfigHandler(QString, QString);
	virtual ~ConfigHandler();

	QString loadStyleSheet();
	apiInfo loadApiInfo();
public slots:
	void saveApiInfo(apiInfo);
	void saveFile();
};

#endif

