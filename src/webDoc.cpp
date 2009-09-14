
/************************************************************************
 * webDoc.cpp								*
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

#include "webDoc.h"

WebDoc::WebDoc(QString u, bool _errorCodeHandle, QString cacheF)
{
	buf  = new QBuffer(this);
	buf->open(QBuffer::ReadWrite);
	http = new QHttp(this);
	url = new QUrl(u);
	connect(http, SIGNAL(done(bool)), this, SLOT(httpGetDone(bool)));
	doc = new QDomDocument;
	errorCodeHandle = _errorCodeHandle;
	busy=false;

	cacheTime = new QDateTime;

	cacheFile = cacheF;
	if(! cacheFile.isEmpty())
		f = new QFile(cacheFile);
}

WebDoc::~WebDoc(){};

void WebDoc::_get(QString urlargs)
{
	if(! busy)
	{
		busy=true;

		if(! doc->isNull())
		{
			if(! doc->documentElement().firstChildElement("cachedUntil").isNull())
			{
				*cacheTime = cacheTime->fromString(doc->documentElement().firstChildElement("cachedUntil").text(), "yyyy-MM-dd HH:mm:ss");
				cacheTime->setTimeSpec(Qt::UTC);
			}
			else
			{
				cacheTime->setDate(cacheTime->currentDateTime().date());
				cacheTime->setTime(cacheTime->currentDateTime().time());
				cacheTime->addSecs(-10);
			}
		}
		
		// only fetch if cachetime is reached or document is empty
		if(cacheTime->toLocalTime() < cacheTime->currentDateTime() || doc->isNull())
		{
			buf->reset();
			buf->buffer().clear();
			http->setHost(url->host());
			http->get(url->path() + urlargs, buf);
		}
		else 	busy = false;
	}
}
void WebDoc::get(QString urlargs)
{	_get(urlargs);	}
void WebDoc::get()
{	_get("");	}

bool WebDoc::setCacheFile()
{
	if( f->exists() )
	{
		f->open( QIODevice::ReadOnly );
		buf->close();
		buf->setData(f->readAll());
		buf->open(QBuffer::ReadWrite);
		f->close();
		return true;
	}
	else	return false;
}

void WebDoc::saveCacheFile()
{
	f->open( QIODevice::WriteOnly );
	QTextStream stream( f );
	doc->save(stream,3);
	f->close();
}

void WebDoc::httpGetDone(bool error)
{
//puts("----------");
//qDebug() << buf->data();
//puts("----------");
	ok = !error;
	cached = false;

	if(ok)
	{
		QString errorStr;
		int errorLine;
		int errorColumn;
        	if (!doc->setContent(buf->data(), true, &errorStr, &errorLine, &errorColumn))
		{
			QMessageBox::warning(0, tr("parse error"), tr("Parse error in %4\nat line %1, column %2:\n\"%3\"")
			.arg(errorLine)
			.arg(errorColumn)
			.arg(errorStr)
			.arg(url->toString()));
			ok = false;
		}
	}

	if(ok)
	{
		if(doc->documentElement().firstChildElement("error").hasAttribute("code"))
		{
			ok=false; // set false if there is an errorcode
			if(errorCodeHandle)
				QMessageBox::warning(0, tr("Server Error"), tr("Server has reported an error\n\nCode:\t%1\nMessage:\t%2")
					.arg(doc->documentElement().firstChildElement("error").attribute("code", "unknown"))
					.arg(doc->documentElement().firstChildElement("error").text()));
		}
	}

	if(!ok && !cacheFile.isEmpty())	// if something went wrong ...
	{				// but only if cachefile is present ...
		if(! setCacheFile())	// try to load cache
		{			// on error, handle it
			QMessageBox::warning(0, tr("download error"),tr("error while downloading %3.\npage: \"%1\"\n\"%2\"")
									.arg(url->host() + url->path())
									.arg(http->errorString())
									.arg(url->toString()));
			ok = false;
		}
		else
		{
			doc->setContent(buf->data());
			cached = true;	// cache should always be ok
		}

	}

	if(ok && ! cacheFile.isEmpty()) saveCacheFile();
	busy=false;
	emit done(ok || cached);
}

bool WebDoc::isCached()
{return cached;}

QDomDocument* WebDoc::document()
{	return doc;	}
