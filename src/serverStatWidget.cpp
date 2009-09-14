
/************************************************************************
 * serverStatWidget.cpp							*
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

#include "serverStatWidget.h"

ServerStatWidget::ServerStatWidget(QString name, TrayManager* traymgr, QWidget * parent)
        : QDockWidget(name, parent)
{
	icon = traymgr;
	content  = new QLabel(this);
	content->setMargin(3);
	content->setAlignment(Qt::AlignCenter);
	setWidget(content);
	serverStatDoc = new WebDoc("http://api.eve-online.com/Server/ServerStatus.xml.aspx");
	//serverStatDoc = new WebDoc("http://api.eve-online.com/Server/ServerStatus.xml.aspx", false, QDir::toNativeSeparators(QDir::homePath ()  + "/.esw/ServerStatus.xml.aspx"));
	connect(serverStatDoc, SIGNAL(done(bool)), this, SLOT(onWebDoc(bool)));
	setServerStartupSingleShot();
}

ServerStatWidget::~ServerStatWidget(){}

void ServerStatWidget::onWebDoc(bool ok)
{
	if(ok)
	{
		lastStat = serverStat;
		QDomElement e = serverStatDoc->document()->documentElement().firstChildElement("result");
		if(! e.firstChildElement("error").isNull())
		{
			serverStat = "error";
			content->setText(tr("error %1\n\"%2\"")
						.arg(e.firstChildElement("error").attribute("code"))
						.arg(e.firstChildElement("error").text())
			);
		}
		else
		{
			serverStat = e.firstChildElement("serverOpen").text();

			if(serverStat == "True") serverStat = tr("Online", "server online message");
			// wait 5mins and try again
			else if(serverStat == "False") { QTimer::singleShot(300000, this, SLOT(reload())); serverStat = tr("Offline", "server offline message"); }
			else if(serverStat.isEmpty()) serverStat = tr("Unknown", "no server message");
			else serverStat = tr("Other \"%1\"").arg(serverStat);

			if(serverStat == tr("Online", "server online message") && lastStat == tr("Offline", "server offline message"))
				setServerStartupSingleShot();

			content->setText(tr("Tranquility: %1\nPlayers: %2", "serverMessage, playerCount")
						.arg(serverStat)
						.arg(e.firstChildElement("onlinePlayers").text()));
		}

		if(lastStat != serverStat && ! lastStat.isEmpty())
			icon->showMessage ( tr("Server status changed"), tr("Tranquility is now %1").arg(serverStat), QSystemTrayIcon::NoIcon, 60000 );
	}
	else	content->setText(tr("no live information available"));
		
}

void ServerStatWidget::reload()
{
	serverStatDoc->get();
}

void ServerStatWidget::setServerStartupSingleShot()
{
	QDateTime to = QDateTime(QDate::currentDate(), QTime(12, 1, 0), Qt::UTC).toLocalTime();
	if(QDateTime::currentDateTime().secsTo(to) < 1) to = to.addDays(1);
	QTimer::singleShot(QDateTime::currentDateTime().secsTo(to) * 1000, this, SLOT(reload()));
}
