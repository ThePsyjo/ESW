
/************************************************************************
 * serverStatWidget.h							*
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

#ifndef SERVERSTATWIDGET_H
#define SERVERSTATWIDGET_H

#include <QLabel>
#include <QDockWidget>
#include "webDoc.h"
#include "trayManager.h"

class ServerStatWidget : public QDockWidget
{
Q_OBJECT
public:
	ServerStatWidget(QString, TrayManager*, QWidget * parent =0);
	virtual ~ServerStatWidget();
private:
	QLabel *content;
	WebDoc *serverStatDoc;
	QString serverStat, lastStat;
	TrayManager *icon;
	void setServerStartupSingleShot();
private slots:
	void onWebDoc(bool);
public slots:
	void reload(bool = 0);
};

#endif
