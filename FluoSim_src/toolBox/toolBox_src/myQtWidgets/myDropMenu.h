
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

#ifndef MYDROPMENU_H
#define MYDROPMENU_H

#include "toolbox_library_global.h"

//#include "QtWidgets/QtWidgets"
#include "QtWidgets/QWidget"
#include "QtWidgets/QLayout"
#include "QtWidgets/QPushButton"

#include "stdlib.h"
#include "iostream"



class TOOLBOXSHARED_EXPORT myDropMenu : public QWidget
{
Q_OBJECT

public :

	enum DROP_MENU_STATE {UNROLLED_STATE, ROLLED_STATE};

	myDropMenu();
	myDropMenu(std::string tex, QWidget* wdgt);

	void connectSignals();

	void setText(std::string text);
	std::string getText();

	void setWidget(QWidget *m_wdgt);
	QWidget* getWidget();

	void unroll();
	void roll();

	void setRolled(bool rolled);

public slots :

	void buttonGetClicked();


public :
	static bool m_darkTheme;

private :

	DROP_MENU_STATE m_menuState;

	QVBoxLayout m_VBLayout;

	QPushButton m_pushButton;
	QWidget *m_wdgt;




};

#endif // MYDROPMENU_H
