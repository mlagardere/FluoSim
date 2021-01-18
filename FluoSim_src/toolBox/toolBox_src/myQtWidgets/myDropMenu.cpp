
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

//****Qt Style Sheets Reference****
//http://doc.qt.io/qt-4.8/stylesheet-reference.html#border-image

#include "myDropMenu.h"


using namespace std;

bool myDropMenu::m_darkTheme = false;

myDropMenu::myDropMenu() : QWidget()
{
	m_menuState = UNROLLED_STATE;

	m_wdgt = 0;
	setText("text");

	if(m_darkTheme == true)
	{
		m_pushButton.setStyleSheet("QPushButton{"
								   "padding : 2px;"
								   "margin : 0px;"
								   "border : outset;"
								   "	border-width: 1px;"
								   "	border-bottom-width : 0;"
								   "	border-right-width : 0;"
								   "	border-left-width : 0;"
								   "text-align : left;"
								   "font-size : 11px;"
								   "font-weight : bold;"
								   "color : rgb(255,255,255);"
								   "border-color: rgb(115,115,115);}");
	}

	else
	{
		m_pushButton.setStyleSheet("QPushButton{"
								   "padding : 2px;"
								   "margin : 0px;"
								   "border : outset;"
								   "	border-width: 1px;"
								   "	border-bottom-width : 0;"
								   "	border-right-width : 0;"
								   "	border-left-width : 0;"
								   "text-align : left;"
								   "font-size : 11px;"
								   "font-weight : bold;"
								   "border-color: rgb(115,115,115);}");
	}

	m_VBLayout.setMargin(0);
	m_VBLayout.setContentsMargins(3,0,3,0);
	m_VBLayout.setSpacing(3);

	m_VBLayout.addWidget(&m_pushButton);

	this->setLayout(&m_VBLayout);

	connectSignals();


}


myDropMenu::myDropMenu(std::string text, QWidget* wdgt) :

	QWidget()


{
	m_menuState = UNROLLED_STATE;

	m_wdgt = wdgt;
	setText(text);

	if(m_darkTheme == true)
	{
		m_pushButton.setStyleSheet("QPushButton{"
								   "padding : 2px;"
								   "margin : 0px;"
								   "border : outset;"
								   "	border-width: 1px;"
								   "	border-bottom-width : 0;"
								   "	border-right-width : 0;"
								   "	border-left-width : 0;"
								   "text-align : left;"
								   "font-size : 11px;"
								   "font-weight : bold;"
								   "color : rgb(255,255,255);"
								   "border-color: rgb(115,115,115);}");
	}

	else
	{
		m_pushButton.setStyleSheet("QPushButton{"
								   "padding : 2px;"
								   "margin : 0px;"
								   "border : outset;"
								   "	border-width: 1px;"
								   "	border-bottom-width : 0;"
								   "	border-right-width : 0;"
								   "	border-left-width : 0;"
								   "text-align : left;"
								   "font-size : 11px;"
								   "font-weight : bold;"
								   "border-color: rgb(115,115,115);}");
	}


	m_VBLayout.setMargin(0);
	m_VBLayout.setContentsMargins(3,0,3,0);
	m_VBLayout.setSpacing(3);

	m_VBLayout.addWidget(&m_pushButton);
	m_VBLayout.addWidget(m_wdgt);

	this->setLayout(&m_VBLayout);

	connectSignals();
}


void myDropMenu::connectSignals()
{
	connect(&m_pushButton, SIGNAL(clicked()), this, SLOT(buttonGetClicked()));
}

void myDropMenu::setText(std::string text)
{
	m_pushButton.setText(text.data());
}

std::string myDropMenu::getText()
{
	return string(m_pushButton.text().toLocal8Bit().data());
}

void myDropMenu::setWidget(QWidget *wdgt)
{
	if(m_wdgt == 0)
	{
		m_VBLayout.addWidget(wdgt);
	}
	else
	{
		m_VBLayout.removeWidget(m_wdgt);
		m_VBLayout.addWidget(wdgt);
	}

	m_wdgt = wdgt;
}

QWidget* myDropMenu::getWidget()
{
	return m_wdgt;
}

void myDropMenu::buttonGetClicked()
{
	if(m_wdgt == 0) return; //->

	switch(m_menuState)
	{
		case UNROLLED_STATE :
		{
			m_menuState = ROLLED_STATE;

			int size_x = this->size().width();
				m_wdgt->hide();
			this->setMinimumSize(size_x, m_VBLayout.minimumSize().height());
				this->adjustSize();
			this->setMinimumSize(m_VBLayout.minimumSize());

		}
		break;


		case ROLLED_STATE :
		{
			m_menuState = UNROLLED_STATE;

			int size_x = this->size().width();
				m_wdgt->show();
			this->setMinimumSize(size_x, m_VBLayout.minimumSize().height());
				this->adjustSize();
			this->setMinimumSize(m_VBLayout.minimumSize());

		}
		break;
	}
}

void myDropMenu::unroll()
{
	if(m_wdgt == 0) return; //->


	m_menuState = UNROLLED_STATE;

	int size_x = this->size().width();
		m_wdgt->show();
	this->setMinimumSize(size_x, m_VBLayout.minimumSize().height());
		this->adjustSize();
	this->setMinimumSize(m_VBLayout.minimumSize());
}

void myDropMenu::roll()
{
	if(m_wdgt == 0) return; //->


	m_menuState = ROLLED_STATE;

	int size_x = this->size().width();
		m_wdgt->hide();
	this->setMinimumSize(size_x, m_VBLayout.minimumSize().height());
		this->adjustSize();
	this->setMinimumSize(m_VBLayout.minimumSize());
}

void myDropMenu::setRolled(bool rolled)
{
	if(rolled == true) roll();
	else unroll();
}




