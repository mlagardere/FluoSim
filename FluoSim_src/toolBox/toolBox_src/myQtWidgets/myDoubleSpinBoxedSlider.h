
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

#ifndef MYDOUBLESPINBOXEDSLIDER_H
#define MYDOUBLESPINBOXEDDOUBLESLIDER_H

#include "toolbox_library_global.h"

#include "iostream"
#include "stdlib.h"
#include "QtWidgets/QSpinBox"
#include "QtWidgets/QLabel"
#include "QtWidgets/QSlider"
#include "QtWidgets/QHBoxLayout"
#include "QtWidgets/QComboBox"

class TOOLBOXSHARED_EXPORT myDoubleSpinBoxedSlider : public QWidget
{
    Q_OBJECT

public :

    enum PREFIX_MODE {QSPINBX_PREFIX_MODE, QLABEL_PREFIX_MODE, QCOMBOBX_PREFIX_MODE};

    myDoubleSpinBoxedSlider(QWidget* parent = nullptr);

    void setMin(double min);
    void setMax(double max);
    void setStep(double step);
    void setPageStep(double page_step);
    void setDecimals(int decimal);
    void setValue(double value);

    void setText(QString text);
    void setSpinBoxPrefix(QString prefix);
    void setSpinBoxPrefixes(QStringList prefixes_qstrl);
    void setSpinBoxPrefixMode(PREFIX_MODE mode);
    void setSpinBoxButtonSymbols(QAbstractSpinBox::ButtonSymbols button_symbols);
    void setComboBoxIndex(int index);
    void connectInternalSignals(bool areConnected);

    double value() const;
    double min() const;
    double max() const;
    double step() const;
    double pageStep() const;
    int decimal() const;

    QString getSpinBoxText() const;
    int getComboBoxIndex() const;
    void updateStyleSheet();

private :

    void setSliderRange();
    void setSliderValue(double value);

private slots :
    void sliderValueChanged(int value);
    void spinBxValueChanged(double value);
    void comboBxIndexChanged(int index);

//    int minimum() const;
//    int maximum() const;
//    uint step() const;
//    uint pageStep() const;
//    QString text() const;
//    QString spinBxPrefix() const;


signals :

    void valueChanged(double);
    void prefixIndexGetChanged(int);




private :

    double m_min;
    double m_max;
    double m_value;
    double m_step;
    double m_pageStep;
    int m_decimals;
    QString m_prefix;

    double m_sliderStep;

    PREFIX_MODE m_prefix_mode;
    QHBoxLayout m_layout;
    QLabel m_label; QSlider m_slider; QLabel m_prefix_label; QDoubleSpinBox m_dSpinBx;
                                      QComboBox m_prefix_comboBx;


public :
    QSlider& m_sliderRef;

};














#endif // MYDOUBLESPINBOXEDSLIDER_H
