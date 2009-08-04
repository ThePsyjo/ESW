
/************************************************************************
 * configuration.cpp							*
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

#include "configuration.h"

ConfigHandler::ConfigHandler(QString fileLocation, QString fileName)
{
	QFileInfo i(fileLocation);
	if(! i.dir().exists())
	{
		QDir d;		// only create if not existing
		d.mkpath(i.path());
	}

	doc = new QDomDocument ( fileName );
	f = new QFile (fileLocation);
	timer = new QTimer(this);
	doSave = true;

	f->open( QIODevice::ReadOnly );

	QString errorStr;
	int errorLine;
	int errorColumn;

	// load if (file exist AND file is not empty)
	if (f->exists() && f->size() > 0 && !doc->setContent(f, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::warning(NULL, tr("parse error"),
						tr("Parse error in file %4:\nat line %1, column %2:\n\"%3\"\n\nconfig will not be written")
						.arg(errorLine)
						.arg(errorColumn)
						.arg(errorStr)
						.arg(fileLocation));
		doSave = false;
	}
	if ( ! (doc->documentElement().tagName() == fileName) )
	{
		QDomElement root = doc->createElement(fileName);
		doc->appendChild(root);
		QMessageBox::information(NULL, "info", tr("%1 created in %2.").arg(fileName).arg(f->fileName()));
	}
	if(doSave)
	{
		f->remove(fileLocation + ".bak");
		f->copy(fileLocation + ".bak");
	}
	cleanup();
	f->close();
};

void ConfigHandler::cleanup()
{
	bool found;
	QStringList nodes;
	nodes << "userID" << "characterID" << "apiKey" << "Options";
	foreach(QString n, nodes)
	{
		QDomNodeList l = doc->documentElement().firstChildElement(n).childNodes();
		for (int i = 0; i < l.size(); i++)
		{
			found = false;
			foreach(QString s, loadAccounts())
				if(l.at(i).nodeName() == s )
				{
					found = true;
					break;
				}
			if(!found) 
				doc->documentElement().firstChildElement(n).removeChild(l.at(i));
		}
	}
}

void ConfigHandler::saveFile()
{
	if(doSave)
	{
		f->open( QIODevice::WriteOnly );
		QTextStream stream( f );
		doc->save(stream,3);
		f->close();
	}
}

ConfigHandler::~ConfigHandler()
{
	saveFile();
};

QDomElement ConfigHandler::genTag(QDomElement inMe, QString tagInMe)
{
	QDomElement tag = inMe.firstChildElement(tagInMe);
	if ( tag.tagName() != tagInMe )
	{
		inMe.appendChild(doc->createElement(tagInMe));
		tag = inMe.firstChildElement(tagInMe);
		tagCreated = true;
		saveFile();
	}
	else tagCreated = false;
	return tag;
}

QString ConfigHandler::loadStyleSheet()
{
	return genTag(doc->documentElement(), "Style").text();
}

//WindowStyle//////////////////////////////////////////////////////////////////////
QString ConfigHandler::loadStyle()
{
	return genTag(doc->documentElement(), "WindowStyle").attribute("value", "default");
}

void ConfigHandler::saveStyle(QString s)
{
	genTag(doc->documentElement(), "WindowStyle").setAttribute("value", s);
}

//Accounts/////////////////////////////////////////////////////////////////////////

QStringList ConfigHandler::loadAccounts()
{
	QStringList ret;
	QDomElement e = genTag (doc->documentElement(), "accounts");
	for (int i = 0; i < e.attributes().count(); i++ )
		ret << e.attribute("value" + QString::number(i));
	return ret;
}

void ConfigHandler::saveAccounts(QStringList v)
{
	QDomElement e = genTag (doc->documentElement(), "accounts");
	for (int i = e.attributes().count(); i >= 0; i-- )
		e.removeAttribute("value" + QString::number(i));
	for (int i = 0; i < v.count(); i++ )
		e.setAttribute("value" + QString::number(i), v.at(i));
}

//ApiInfo//////////////////////////////////////////////////////////////////////////
void ConfigHandler::saveApiInfo(apiInfo v)
{
	genTag ( genTag ( doc->documentElement(), "userID" ), v.name).setAttribute("value", v.userID);
	genTag ( genTag ( doc->documentElement(), "apiKey" ), v.name).setAttribute("value", v.apiKey);
	genTag ( genTag ( doc->documentElement(), "characterID" ), v.name).setAttribute("value", v.characterID);
	saveFile();
}

apiInfo ConfigHandler::loadApiInfo(QString s)
{
	apiInfo v;
	v.name = s;
	v.userID	= genTag ( genTag ( doc->documentElement(), "userID" ), v.name).attribute("value").toInt();
	v.apiKey 	= genTag ( genTag ( doc->documentElement(), "apiKey" ), v.name).attribute("value");
	v. characterID	= genTag ( genTag ( doc->documentElement(), "characterID" ), v.name).attribute("value").toInt();
	return v;
}

//Bool-Operations//////////////////////////////////////////////////////////////////
void ConfigHandler::saveBool(QString tag, QString attribute, bool b)
{
	b ? genTag ( doc->documentElement(), tag ).setAttribute(attribute, 1) : genTag ( doc->documentElement(), tag ).setAttribute(attribute, 0);
}

bool ConfigHandler::loadBool(QString tag, QString attribute, QString defaultValue)
{
	return genTag ( doc->documentElement(), tag ).attribute(attribute, defaultValue).toInt();
}

void ConfigHandler::saveBool(QString acc, QString tag, QString attribute, bool b)
{
	b ? genTag( genTag ( doc->documentElement(), tag ), acc ).setAttribute(attribute, 1) : genTag( genTag ( doc->documentElement(), tag ), acc ).setAttribute(attribute, 0);
}

bool ConfigHandler::loadBool(QString acc, QString tag, QString attribute, QString defaultValue)
{
	return genTag ( genTag ( doc->documentElement(), tag ), acc ).attribute(attribute, defaultValue).toInt();
}

//Visibility///////////////////////////////////////////////////////////////////////
void ConfigHandler::saveIsVisible(bool b)
{
	saveBool( "Options", "visible", b);
}

bool ConfigHandler::loadIsVisible()
{
	return loadBool( "Options", "visible", "1");
}

//widgetState//////////////////////////////////////////////////////////////////////
void ConfigHandler::saveState(QByteArray d)
{	
	//compression stats
	//qDebug() << d.toHex().size() << " +z " << qCompress(d).toHex().size();
	genTag ( doc->documentElement(), "WindowSettings" ).setAttribute("state", qCompress(d).toHex().data());
}

QByteArray ConfigHandler::loadState()
{
	QByteArray ba;
	ba = qUncompress(QByteArray::fromHex(QByteArray( genTag ( doc->documentElement(), "WindowSettings" ).attribute("state").toAscii())));
	if(ba.isEmpty())
		ba = QByteArray::fromHex(QByteArray( genTag ( doc->documentElement(), "WindowSettings" ).attribute("state").toAscii()));
	return ba;
}

//ontop////////////////////////////////////////////////////////////////////////////
void ConfigHandler::saveOntop(bool b)
{
	saveBool( "Options", "ontop", b);
}

bool ConfigHandler::loadOntop()
{
	return loadBool( "Options", "ontop", "0");
}

//ShowTray////////////////////////////////////////////////////////////////////////
void ConfigHandler::saveShowTray(bool b)
{
	saveBool( "Options", "tray_visible", b);
}

bool ConfigHandler::loadShowTray()
{
	return loadBool( "Options", "tray_visible", "1");
}

//AutoSync////////////////////////////////////////////////////////////////////////
void ConfigHandler::saveAutoSync(bool b)
{
	saveBool( "Options", "autosync", b);
}

bool ConfigHandler::loadAutoSync()
{
	return loadBool( "Options", "autosync", "1");
}

//ProgressBar/////////////////////////////////////////////////////////////////////
void ConfigHandler::saveProgressBar(bool b)
{
	saveBool( "Options", "progressbar", b);
}

bool ConfigHandler::loadProgressBar()
{
	return loadBool( "Options", "progressbar", "1");
}

//CloseToTray//////////////////////////////////////////////////////////////////////
void ConfigHandler::saveCloseToTray(bool b)
{
	saveBool( "Options", "close2tray", b);
}

bool ConfigHandler::loadCloseToTray()
{
	return loadBool( "Options", "close2tray", "1");
}

//CloseToTrayTip///////////////////////////////////////////////////////////////////
void ConfigHandler::saveCloseToTrayTip(bool b)
{
	saveBool( "Options", "close2trayTip", b);
}

bool ConfigHandler::loadCloseToTrayTip()
{
	return loadBool( "Options", "close2trayTip", "1");
}

//FullQueueView////////////////////////////////////////////////////////////////////
void ConfigHandler::saveShowFullQueueView(QString acc, bool b)
{
	saveBool( acc, "Options", "showFullQueueView", b);
}

bool ConfigHandler::loadShowFullQueueView(QString acc)
{
	return loadBool( acc, "Options", "showFullQueueView", "1");
}

