
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

#ifndef MYDOCKWIDGET_H
#define MYDOCKWIDGET_H

#include "toolbox_library_global.h"

#include "QDockWidget"

class TOOLBOXSHARED_EXPORT myDockWidget : public QDockWidget
{
	Q_OBJECT
public:

	myDockWidget(QWidget *parent = 0);

	virtual void closeEvent(QCloseEvent* event);

signals:

	void widgetGetClosed_sig();


};

#endif // MYDOCKWIDGET_H
