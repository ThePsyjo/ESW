
/************************************************************************
 * characterWidget.h							*
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

#ifndef CHARACTERWIDGET_H
#define CHARACTERWIDGET_H

#include <QLabel>
#include <QToolBar>
#include "webDoc.h"
#include "configuration.h"

class CharacterWidget : public QToolBar
{
Q_OBJECT
public:
	CharacterWidget(QString, ConfigHandler*, QWidget * parent =0);
	virtual ~CharacterWidget();
private:
	ConfigHandler *conf;
	QLabel *content;
	WebDoc *characterInfo;
	QString getTitle();
private slots:
	void onWebDoc(bool);
public slots:
	void reload();
};

#endif
