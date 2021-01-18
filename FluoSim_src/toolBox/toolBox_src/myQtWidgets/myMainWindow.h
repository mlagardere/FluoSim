
/*
 *  FluoSim (simulation program of live cell fluorescence microscopy experiments)
 *  Copyright (C) 2020 Matthieu Lagardère
 *
 *  FluoSim is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FluoSim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License has been attached
 *  along with this program. It can also be found at <https://www.gnu.org/licenses/>.
 *
*/

#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include "toolbox_library_global.h"

#include "QMainWindow"
#include "stdlib.h"
#include "iostream"

#include "QEvent"

#include "stdlib.h"
#include "iostream"



class TOOLBOXSHARED_EXPORT myMainWindow : public QMainWindow
{
Q_OBJECT
public :

	myMainWindow();

	virtual void resizeEvent(QResizeEvent *event);
	virtual void moveEvent(QMoveEvent *event);
//	void hideEvent(QHideEvent *event);
//	void showEvent(QShowEvent *event);
	virtual void changeEvent(QEvent * event);
	virtual void closeEvent(QCloseEvent *event);


signals :

	void widgetGetResized_sig(QResizeEvent *event);
	void widgetGetMoved_sig(QMoveEvent *event);
//	void mainWindowGetHidden_sig();
	void widgetGetRaised_sig();
	void widgetGetLowered_sig();
	void widgetGetClosed();



private :



};

#endif // MYMAINWINDOW_H
