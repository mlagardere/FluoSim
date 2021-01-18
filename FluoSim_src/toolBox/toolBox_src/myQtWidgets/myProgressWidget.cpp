
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


#include "myProgressWidget.h"

using namespace std;

myProgressWidget::myProgressWidget(QWidget* parent) : QDialog(parent)
{
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog);


	m_hLayout1.addWidget(&m_label);
    m_label.setText("");

	m_progressbar.setTextVisible(true);
	m_progressbar.setValue(0);
	m_hLayout2.addWidget(&m_progressbar);

	m_VLayout.addLayout(&m_hLayout1);
	m_VLayout.addLayout(&m_hLayout2);

	this->setLayout(&m_VLayout);
	this->move(50,50);
	this->setFixedSize(300,60);
	this->hide();

}

QProgressBar* myProgressWidget::getProgressBar()
{
	return &m_progressbar;
}

void myProgressWidget::setText(string text)
{
	m_label.setText(text.data());
}
