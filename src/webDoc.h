
/************************************************************************
 * webDoc.h								*
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

#ifndef WEBDOC_H
#define WEBDOC_H

#include <QtXml>
#include <QMessageBox>
#include <QBuffer>
#include <QtNetwork/QHttp>
#include <QDateTime>

class WebDoc : public QObject
{
Q_OBJECT
public:
	WebDoc(QString, bool = false, QString = "");
	virtual ~WebDoc();
	void get(QString, bool = 0);
	void get(bool = 0);
	bool isCached();
	QDomDocument* document();
private:
	QHttp *http;
	QUrl *url;
	QBuffer *buf;
	QDomDocument *doc;
	QFile *f;
	QString cacheFile;
	QDateTime *cacheTime;
	void _get(QString, bool);
	bool errorCodeHandle, busy, ok, cached;
	bool setCacheFile();
	void saveCacheFile();
private slots:
	void httpGetDone(bool);
signals:
	void done(bool);
};

#endif

