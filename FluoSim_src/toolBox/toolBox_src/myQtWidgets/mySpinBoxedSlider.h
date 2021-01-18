
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

#ifndef MYSPINBOXEDSLIDER_H
#define MYSPINBOXEDSLIDER_H

#include "toolbox_library_global.h"

#include "iostream"
#include "stdlib.h"
#include "QtWidgets/QSpinBox"
#include "QtWidgets/QLabel"
#include "QtWidgets/QSlider"
#include "QtWidgets/QHBoxLayout"

class TOOLBOXSHARED_EXPORT mySpinBoxedSlider : public QWidget
{
    Q_OBJECT

    enum PREFIX_MODE {QSPINBX_PREFIX_MODE, QLABEL_PREFIX_MODE};

public :

    mySpinBoxedSlider(QWidget* parent = nullptr);

    void setMin(int min);
    void setMax(int max);
    void setStep(uint step);
    void setPageStep(uint page_step);
    void setValue(int value);

    void setText(QString text);
    void setSpinBoxPrefix(QString prefix);
    void setSpinBoxPrefixMode(PREFIX_MODE mode);
    void setSpinBoxButtonSymbols(QAbstractSpinBox::ButtonSymbols button_symbols);
    void connectInternalSignals(bool areConnected);

    int value() const;

    void hide();
    void show();

private slots :
    void sliderValueChanged(int value);
    void spinBxValueChanged(int value);

//    int minimum() const;
//    int maximum() const;
//    uint step() const;
//    uint pageStep() const;
//    QString text() const;
//    QString spinBxPrefix() const;


signals :

    void valueChanged(int);



private :

    int m_min;
    int m_max;
    int m_value;
    uint m_step;
    uint m_pageStep;
    QString m_prefix;

    PREFIX_MODE m_prefix_mode;

    QHBoxLayout m_layout;
    QLabel m_label; QSlider m_slider; QLabel m_prefix_label; QSpinBox m_spinBx;
};









#endif // MYSPINBOXEDSLIDER_H
