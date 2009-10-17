
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
	tmpNode = new QDomNode;
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
	fix_from_0_1_18();
};

void ConfigHandler::fix_from_0_1_18()
{
	QDomNodeList l;
	int i;
	bool hit = true;
	
	while(hit)
	{
		hit = false;
		l = doc->documentElement().firstChildElement("userID").childNodes();
		for(i = 0; i < l.count(); i++)
		{
			hit=true;
			if(!findChar(l.at(i).nodeName())) createChar(l.at(i).nodeName());
			tmpNode->toElement().setAttribute("userID",		doc->documentElement().firstChildElement("userID")	.firstChildElement(l.at(i).nodeName()).attribute("value"));
			tmpNode->toElement().setAttribute("characterID",	doc->documentElement().firstChildElement("characterID")	.firstChildElement(l.at(i).nodeName()).attribute("value"));
			tmpNode->toElement().setAttribute("apiKey", 		doc->documentElement().firstChildElement("apiKey")	.firstChildElement(l.at(i).nodeName()).attribute("value"));
			tmpNode->toElement().setAttribute("fullQueueView",	genTag ( doc->documentElement(), "Options" )		.firstChildElement(l.at(i).nodeName()).attribute("showFullQueueView"));

			// remove doc.userID.<character_name> ...
			doc->documentElement().firstChildElement("userID").removeChild(l.at(i));
			genTag ( doc->documentElement(), "Options" ).removeChild(genTag ( doc->documentElement(), "Options" ).firstChildElement(l.at(i).nodeName()));
		}
	}
	// remove remaining old nodes
	doc->documentElement().removeChild(doc->documentElement().firstChildElement("accounts"));
	doc->documentElement().removeChild(doc->documentElement().firstChildElement("userID"));
	doc->documentElement().removeChild(doc->documentElement().firstChildElement("characterID"));
	doc->documentElement().removeChild(doc->documentElement().firstChildElement("apiKey"));
}

bool ConfigHandler::findChar(QString name)
{
	QDomNodeList l = genTag ( doc->documentElement(), "Characters" ).childNodes();
	bool found = false;

	for (cnt = 0; cnt < l.size() ; cnt++)
	{	
		if(l.at(cnt).toElement().attribute("name", "") == name)
		{
			*tmpNode = l.at(cnt).toElement();
			found = true;
		}
	}
	if(found) return true;
	else return false;
}

QDomNode* ConfigHandler::findCharNode(QString name)
{
	findChar(name);
	return tmpNode;
}
void ConfigHandler::createChar(QString name)
{
	QDomNode n = genTag(doc->documentElement(), "Characters").appendChild(doc->createElement("char"));
	n.toElement().setAttribute("name", name);
	findChar(name);
}

void ConfigHandler::cleanup() // deprecated
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
			foreach(QString s, __loadAccounts())
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

QStringList ConfigHandler::__loadAccounts()
{
	QStringList ret;
	QDomElement e = genTag (doc->documentElement(), "accounts");
	for (int i = 0; i < e.attributes().count(); i++ )
	ret << e.attribute("value" + QString::number(i));
	return ret;
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
	QDomNodeList l = genTag ( doc->documentElement(), "Characters" ).childNodes();

	for (int i = 0; i < l.size(); i++ )
		ret << l.at(i).toElement().attribute("name");
	return ret;
}

void ConfigHandler::saveAccounts(QStringList v)
{
	// remove deleted chars
	foreach(QString s, loadAccounts())
		if(! v.contains(s)) genTag ( doc->documentElement(), "Characters" ).removeChild(*findCharNode(s));

	// add new ones
	foreach(QString s, v)
		if(! findChar(s)) createChar(s);
}

//ApiInfo//////////////////////////////////////////////////////////////////////////
void ConfigHandler::saveApiInfo(apiInfo v)
{
	if(! findChar(v.name)) createChar(v.name);
	tmpNode->toElement().setAttribute("userID", v.userID);
	tmpNode->toElement().setAttribute("characterID", v.characterID);
	tmpNode->toElement().setAttribute("apiKey", v.apiKey);
	saveFile();
}

apiInfo ConfigHandler::loadApiInfo(QString s)
{
	apiInfo v;
	v.name = s;
	findChar(s);
	v.userID	= tmpNode->toElement().attribute("userID").toInt();
	v.apiKey 	= tmpNode->toElement().attribute("apiKey");
	v. characterID	= tmpNode->toElement().attribute("characterID").toInt();
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

void ConfigHandler::saveBoolChar(QString attribute, bool b, QString acc)
{
	if(findChar(acc))
		b ? tmpNode->toElement().setAttribute(attribute, 1) : tmpNode->toElement().setAttribute(attribute, 0);
}

bool ConfigHandler::loadBoolChar(QString attribute, QString defaultValue, QString acc)
{
	return findCharNode( acc )->toElement().attribute(attribute, defaultValue).toInt();
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
	saveBoolChar( "fullQueueView", b, acc);
}

bool ConfigHandler::loadShowFullQueueView(QString acc)
{
	return loadBoolChar( "fullQueueView", "1", acc);
}

