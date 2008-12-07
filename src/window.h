
/************************************************************************
 * window.h								*
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
	MainWindow( QWidget * parent =0, Qt::WFlags f =0 );
	virtual ~MainWindow();
private:
	QMenu *about, *mFile;
private slots:
	void handleAboutAction(QAction* a);
	void handleFileAction(QAction* a);
};















#endif

