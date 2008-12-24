
/************************************************************************
 * apiInput.cpp								*
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

#include "apiInput.h"

ApiInput::ApiInput( QString name, ConfigHandler *c, QWidget* parent )
{
	setParent(parent);
	setWindowFlags(Qt::Dialog);
	setWindowTitle(name);
	setModal(1);
	conf = c;

	characters = new WebDoc("http://api.eve-online.com/account/Characters.xml.aspx");
	connect(characters, SIGNAL(done(bool)), this, SLOT(onCharactersDocDone(bool)));
	gotData=false;

        okButton	= new QPushButton (tr("&Save"), this);
        cancelButton	= new QPushButton (tr("&Cancel"), this);
        connectButton	= new QPushButton (tr("c&onnect"), this);

        characterSelect	= new QComboBox (this);

	eUserID = new QLineEdit(QString("%1").arg(conf->loadApiInfo().userID), this);
	eApiKey = new QLineEdit(conf->loadApiInfo().apiKey, this);
	eUserID->setInputMask("00000000000000000000");
	eApiKey->setInputMask(">HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
	eApiKey->setMinimumWidth(400);

	lUserID = new QLabel(tr("UserID"));
	lApiKey = new QLabel(tr("ApiKey"));
	lCharacterID = new QLabel(tr("CharacterID"));
	lText = new QLabel(tr("Insert your UserID and your API-Key in the specified Fields.\nClick on connect to select your character.\nYou can get your own API-Information here : http://myeve.eve-online.com/api/default.asp ."));
	lText->setOpenExternalLinks(true);

	layout = new QGridLayout(this);

	layout->addWidget(lText, 1, 1, 1, 2);
	
	layout->addWidget(lUserID, 2, 1);
	layout->addWidget(eUserID, 2, 2);
	
	layout->addWidget(lApiKey, 3, 1);
	layout->addWidget(eApiKey, 3, 2);

	layout->addWidget(connectButton, 4, 2);

	layout->addWidget(lCharacterID, 5, 1);
	layout->addWidget(characterSelect, 5, 2);
	

	layout->addWidget(okButton, 1, 3);
	layout->addWidget(cancelButton, 2, 3);


	connect(okButton	, SIGNAL(clicked()), this, SLOT(onOkClick()));
	connect(cancelButton	, SIGNAL(clicked()), this, SLOT(reject()));
	connect(connectButton	, SIGNAL(clicked()), this, SLOT(onConnectClick()));

	adjustSize();
}

ApiInput::~ApiInput()
{}

void ApiInput::redel(QWidget* w)
{w->setStyleSheet("color:red;");}

void ApiInput::defel(QWidget* w)
{w->setStyleSheet("color:black;");}

bool ApiInput::validID(bool b)
{
	if( eUserID->text().isEmpty() )
	{
		redel(lUserID);
		return false;
	}
	else
		defel(lUserID);

	if( eApiKey->text().isEmpty() )
	{
		redel(lApiKey);
		return false;
	}
	else
		defel(lApiKey);

	if(b)
	{
		if( characterSelect->currentText().isEmpty() )
		{
			redel(lCharacterID);
			return false;
		}
		else
			defel(lCharacterID);
	}

	return true;
}

void ApiInput::onOkClick()
{
	if( ! validID(1)) return;
	if( ! gotData) return;
	
	apiInfo v;
	v.userID = eUserID->text().toInt();
	v.apiKey = eApiKey->text();

	QDomNodeList l = characters->document()->documentElement().elementsByTagName("row");
	for (int i = 0; i < l.size(); i++)
	{
		if(l.item(i).toElement().attribute("name") == characterSelect->currentText())
		{
			v.characterID = l.item(i).toElement().attribute("characterID").toInt();
			break;
		}
	}

	conf->saveApiInfo(v);

	accept();
}

void ApiInput::onConnectClick()
{
	if( ! validID(0)) return;
	setCursor(Qt::WaitCursor);
	characters->get("?userID=" + eUserID->text() + "&apiKey=" + eApiKey->text());
}

void ApiInput::onCharactersDocDone(bool ok)
{
	if(!ok) {setCursor(Qt::ArrowCursor);return;}
	characterSelect->clear();

	QDomElement e = characters->document()->documentElement().firstChildElement("error");
	if(e.hasAttribute("code"))
	{
		QMessageBox::warning(this, tr("API error"),
			tr("errorcode %1\n\n%2")
			.arg(e.attribute("code"))
			.arg(e.text())			
			);
		setCursor(Qt::ArrowCursor);
		return;
	}

	QDomNodeList l = characters->document()->documentElement().elementsByTagName("row");
	for (int i = 0; i < l.size(); i++)
		characterSelect->addItem(l.item(i).toElement().attribute("name"));

	gotData=true;

	setCursor(Qt::ArrowCursor);
}
