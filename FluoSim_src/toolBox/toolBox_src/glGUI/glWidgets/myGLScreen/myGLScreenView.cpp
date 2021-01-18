
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


#include "myGLScreen.h"

myGLScreenView::myGLScreenView(int x, int y, QWidget* parent): myGLWidget(x,y,parent)
{
//	grabKeyboard();

}

void myGLScreenView::mouseDoubleClickEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);
}

void myGLScreenView::mouseMoveEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);
//	grabKeyboard();

}

void myGLScreenView::mousePressEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);
}

void myGLScreenView::mouseReleaseEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);
}

void myGLScreenView::wheelEvent(QWheelEvent * event)
{
	treatEvent((QEvent*) event);
}

void myGLScreenView::keyPressEvent(QKeyEvent* event)
{
	treatEvent((QEvent*) event);
}

void myGLScreenView::keyReleaseEvent(QKeyEvent* event)
{
	treatEvent((QEvent*) event);
}

/* virtual */ void myGLScreenView::treatEvent(QEvent* event)
{
}



