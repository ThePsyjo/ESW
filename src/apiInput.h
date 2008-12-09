
/************************************************************************
 * apiInput.h								*
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

#ifndef API_INPUT_H
#define API_INPUT_H

#include <QPushButton>
#include <QMenu>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QDialog>
#include <QBuffer>
#include <QtNetwork/QHttp>
#include <QGridLayout>

#include "configuration.h"

class ApiInput : public QDialog
{
Q_OBJECT
public:
	ApiInput(QString, ConfigHandler*, QWidget* = 0);
	virtual ~ApiInput();
private:
	ConfigHandler *conf;
	QPushButton *okButton, *cancelButton, *connectButton, *characterButton;
	QMenu *characterMenu;
	QLabel *lUserID, *lApiKey, *lCharacterID, *lText;
	QLineEdit *eUserID, *eApiKey;
	QGridLayout *layout;
	QHttp *http;
	QUrl *url;
	QBuffer *buf;
	bool gotData;
	QDomDocument *doc;
	void redel(QWidget*);
	void defel(QWidget*);
	bool validID(bool);
private slots:
	void onOkClick();
	void onConnectClick();
	void onCharacterMenuAction(QAction*);
	void httpGetDone(bool);
};


#endif

