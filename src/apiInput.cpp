
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

	gotData=false;

	buf  = new QBuffer(this);
	http = new QHttp(this);
	url = new QUrl("http://api.eve-online.com/account/Characters.xml.aspx");
	connect(http, SIGNAL(done(bool)), this, SLOT(httpGetDone(bool)));

	doc = new QDomDocument;

        okButton	= new QPushButton (tr("&Save"), this);
        cancelButton	= new QPushButton (tr("&Cancel"), this);
        connectButton	= new QPushButton (tr("c&onnect"), this);
        characterButton	= new QPushButton (this);

	characterMenu = new QMenu(this);
	characterButton->setMenu(characterMenu);

	eUserID = new QLineEdit(QString("%1").arg(conf->loadApiInfo().userID), this);
	eApiKey = new QLineEdit(conf->loadApiInfo().apiKey, this);
	eApiKey->setMinimumWidth(400);

	lUserID = new QLabel(tr("UserID"));
	lApiKey = new QLabel(tr("ApiKey"));
	lCharacterID = new QLabel(tr("CharacterID"));
	lText = new QLabel(tr("Insert your UserID and your API-Key in the specified Fields.\nClick on connect to select your character.\nYou can get your own API-Information here : http://myeve.eve-online.com/api/default.asp ."));

	layout = new QGridLayout(this);

	layout->addWidget(lText, 1, 1, 1, 2);
	
	layout->addWidget(lUserID, 2, 1);
	layout->addWidget(eUserID, 2, 2);
	
	layout->addWidget(lApiKey, 3, 1);
	layout->addWidget(eApiKey, 3, 2);

	layout->addWidget(connectButton, 4, 2);

	layout->addWidget(lCharacterID, 5, 1);
	layout->addWidget(characterButton, 5, 2);
	

	layout->addWidget(okButton, 1, 3);
	layout->addWidget(cancelButton, 2, 3);


	connect(okButton	, SIGNAL(clicked()), this, SLOT(onOkClick()));
	connect(cancelButton	, SIGNAL(clicked()), this, SLOT(reject()));
	connect(connectButton	, SIGNAL(clicked()), this, SLOT(onConnectClick()));
	connect(characterMenu	, SIGNAL(triggered(QAction*)), this, SLOT(onCharacterMenuAction(QAction*)));

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
		if( characterButton->text().isEmpty() )
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

	QDomElement el = doc->documentElement().firstChildElement("result").firstChildElement("rowset").firstChildElement("row");
	for(int i = 0; i < 3; i++)
	{
		if(el.attribute("name") == characterButton->text())
		{
			v.characterID = el.attribute("characterID").toInt();
			break;
		}
		el = el.nextSiblingElement("row");
	}

	conf->saveApiInfo(v);

	accept();
}

void ApiInput::onConnectClick()
{
	if( ! validID(0)) return;
	setCursor(Qt::WaitCursor);
	buf->reset();
	http->setHost(url->host());
	http->get(url->path() + "?userID=" + eUserID->text() + "&apiKey=" + eApiKey->text(), buf);
}

void ApiInput::httpGetDone(bool error)
{
	if(error)
	{
		QMessageBox::warning(this, tr("download error"),tr("error while downloading character list.\npage: \"%1\"\n\"%2\"")
								.arg(url->host() + url->path())
								.arg(http->errorString()));
		return;
	}

//puts("----------");
//qDebug() << buf->data();
//puts("----------");

	QString errorStr;
	int errorLine;
	int errorColumn;
        if (!doc->setContent(buf->data(), true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::warning(this, tr("parse error"),
		tr("Parse error in character list\nat line %1, column %2:\n\"%3\"")
		.arg(errorLine)
		.arg(errorColumn)
		.arg(errorStr));
		return;
	}

	gotData=true;
	
	characterMenu->clear();

	QDomElement el = doc->documentElement().firstChildElement("result").firstChildElement("rowset").firstChildElement("row");
	for(int i = 0; i < 3; i++)
	{
		characterMenu->addAction(el.attribute("name"));
		el = el.nextSiblingElement("row");
	}
	characterButton->setText(doc->documentElement().firstChildElement("result").firstChildElement("rowset").firstChildElement("row").attribute("name"));

	setCursor(Qt::ArrowCursor);
}

void ApiInput::onCharacterMenuAction(QAction *a)
{	characterButton->setText(a->text()); validID(1);	}
