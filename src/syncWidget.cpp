
/************************************************************************
 * syncWidget.cpp							*
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

#include "syncWidget.h"

SyncWidget::SyncWidget(QString name, QString format, QWidget * parent)//, Qt::WFlags f)
        : QToolBar(name, parent)//, f)
{
	sTimer = new QTimer(this);
	connect(sTimer, SIGNAL(timeout()), this, SLOT(onSTimer()));
	sTimer->start(1000);

	timeFormat = new QString();
	*timeFormat = format;
	syncTime   = new QDateTime();

	syncLabel  = new QLabel(this);
	syncLabel->setAlignment(Qt::AlignCenter);
	addWidget(syncLabel);
}

SyncWidget::~SyncWidget(){}

void SyncWidget::onSTimer()
{
	syncLabel->setText(syncTime->fromTime_t(syncTime->currentDateTime().secsTo(*syncTime)).toString(*timeFormat));
}
#include <QDebug>
void SyncWidget::set(int i)
{
	*syncTime = syncTime->currentDateTime().addSecs(i);
}
