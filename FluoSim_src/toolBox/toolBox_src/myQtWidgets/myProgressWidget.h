
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

#ifndef MYPROGRESSWIDGET_H
#define MYPROGRESSWIDGET_H

#include "toolbox_library_global.h"

#include "QWidget"
#include "QDialog"
#include "QProgressBar"
#include "QLayout"
#include "QLabel"

#include "stdlib.h"
#include "iostream"
#include <string>



class TOOLBOXSHARED_EXPORT myProgressWidget: public QDialog
{
public :

	myProgressWidget(QWidget* parent);
	QProgressBar* getProgressBar();

	void setText(std::string text);

private :

	QVBoxLayout m_VLayout;
		QHBoxLayout m_hLayout1; QLabel m_label;
		QHBoxLayout m_hLayout2; QProgressBar m_progressbar;

};

#endif // MYPROGRESSWIDGET_H
