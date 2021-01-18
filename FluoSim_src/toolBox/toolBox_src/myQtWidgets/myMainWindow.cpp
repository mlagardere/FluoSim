
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



#include "myMainWindow.h"


using namespace std;


myMainWindow::myMainWindow() : QMainWindow()
{
}

void myMainWindow::resizeEvent(QResizeEvent *event)
{
	emit widgetGetResized_sig(event);

}

void myMainWindow::moveEvent(QMoveEvent* event)
{
	emit widgetGetMoved_sig(event);
}


void myMainWindow::changeEvent(QEvent* event)
{
	if(event->type() == QEvent::QEvent::WindowStateChange)
	{
		if(windowState().testFlag(Qt::WindowMinimized))
		{
			emit(widgetGetLowered_sig());
			return;
		}

		if(windowState().testFlag(Qt::WindowMaximized))
		{
			emit(widgetGetRaised_sig());
		}

		if(windowState().testFlag(Qt::WindowFullScreen))
		{
			emit(widgetGetRaised_sig());
		}

		if(windowState().testFlag(Qt::WindowActive))
		{
//			emit(widgetGetRaised_sig());
		}

	}
}

void myMainWindow::closeEvent(QCloseEvent *event)
{
	emit widgetGetClosed();
}
