
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

class WebDoc : public QObject
{
Q_OBJECT
public:
	WebDoc(QString, bool = false);
	virtual ~WebDoc();
	void get(QString);
	void get();
	QDomDocument* document();
private:
	QHttp *http;
	QUrl *url;
	QBuffer *buf;
	QDomDocument *doc;
	void _get(QString);
	bool errorCodeHandle, busy, ok;
private slots:
	void httpGetDone(bool);
signals:
	void done(bool);
};

#endif

