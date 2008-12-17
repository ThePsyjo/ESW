
/************************************************************************
 * syncWidget.h								*
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

#ifndef SYNCWIDGET_H
#define SYNCWIDGET_H

#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QToolBar>

class SyncWidget : public QToolBar
{
Q_OBJECT
public:
	//SyncWidget(QString, QString, QWidget*,  Qt::WFlags);
	SyncWidget(QString, QString, QWidget * parent =0);//, Qt::WFlags f =0);
	virtual ~SyncWidget();
private:
	QTimer *sTimer;
	QDateTime *syncTime;
	QString *timeFormat;
	QLabel *syncLabel;
	int storedSecs;
private slots:
	void onSTimer();
public slots:
	void set(int);
	void enable();
	void disable();
};

#endif
