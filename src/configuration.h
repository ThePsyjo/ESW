
/************************************************************************
 * configuration.h							*
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QtXml>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QByteArray>

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
	void saveBool(QString, QString, bool);
	bool loadBool(QString, QString, QString);
public:
	ConfigHandler(QString, QString);
	virtual ~ConfigHandler();

	QString loadStyleSheet();

	QString loadStyle();
	void saveStyle(QString);
	
	apiInfo loadApiInfo();
	void saveApiInfo(apiInfo);

	bool loadIsVisible();
	void saveIsVisible(bool);

	void saveState(QByteArray);
	QByteArray loadState();

	bool loadOntop();
	void saveOntop(bool);

	bool loadShowTray();
	void saveShowTray(bool);

	bool loadAutoSync();
	void saveAutoSync(bool);

	bool loadProgressBar();
	void saveProgressBar(bool);

	bool loadCloseToTray();
	void saveCloseToTray(bool);

	bool loadCloseToTrayTip();
	void saveCloseToTrayTip(bool);

	bool loadShowFullQueueView();
	void saveShowFullQueueView(bool);

	QStringList loadAccounts();
	void saveAccounts(QStringList);
public slots:
	void saveFile();
};

#endif

