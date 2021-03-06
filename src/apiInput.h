
/************************************************************************
 * apiInput.h								*
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

#ifndef API_INPUT_H
#define API_INPUT_H

#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QGridLayout>
#include <QValidator>
#include <QRegExp>

#include "configuration.h"
#include "webDoc.h"

class StringInput : public QDialog
{
Q_OBJECT
public:
	StringInput(QStringList, QWidget* = 0);
	virtual ~StringInput();
	QString getVal();
private:
	QValidator *validator;
	QStringList list;
	QString val;
	QLabel *text;
	QLineEdit *e;
	QPushButton *okButton, *cancelButton;
	QGridLayout *layout;
private slots:
	void onOkClick();
};

class ApiInput : public QDialog
{
Q_OBJECT
public:
	ApiInput(QString, ConfigHandler*, QWidget* = 0);
	virtual ~ApiInput();
private:
	QStringList *accs;
	ConfigHandler *conf;
	WebDoc *characters;
	QDomDocument *doc;
	QPushButton *saveButton, *closeButton, *connectButton, *newButton, *deleteButton;
	QLabel *lUserID, *lApiKey, *lCharacterID, *lText, *lAccount;
	QLineEdit *eUserID, *eApiKey;
	QComboBox *characterSelect, *accountSelect;
	QGridLayout *layout;
	QFont *defFont;
	bool gotData;
	void redel(QWidget*);
	void defel(QWidget*);
	bool validID(bool);
private slots:
	void onSaveClick();
	void onConnectClick();
	void onCharactersDocDone(bool);
	void handleAccountSelect(QString);
	void onNewClick();
	void onDeleteClick();
	void handleChange();
};


#endif

