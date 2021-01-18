
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


#include "myDoubleSpinBoxedSlider.h"

using namespace std;

myDoubleSpinBoxedSlider::myDoubleSpinBoxedSlider(QWidget* parent) : QWidget(parent),
    m_layout(this), m_slider(Qt::Horizontal), m_sliderRef(m_slider),
    m_min(0), m_max(100), m_value(1), m_step(1), m_pageStep(10)
{
    m_layout.addWidget(&m_label);
    m_layout.addWidget(&m_slider);
    m_layout.addWidget(&m_prefix_label);
    m_layout.addWidget(&m_prefix_comboBx);
    m_layout.addWidget(&m_dSpinBx);

    m_layout.setMargin(0);
    m_layout.setSpacing(1);
    m_dSpinBx.setAlignment(Qt::AlignRight);
    setSpinBoxPrefixMode(QLABEL_PREFIX_MODE);

    setMin(m_min);
    setMax(m_max);

    connectInternalSignals(true);

    m_dSpinBx.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_prefix_label.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_slider.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);

}

void myDoubleSpinBoxedSlider::setMin(double min)
{
    connectInternalSignals(false);
        m_dSpinBx.setMinimum(min);
        m_min = m_dSpinBx.minimum();
        m_max = m_dSpinBx.maximum();
        setSliderRange();
    connectInternalSignals(true);

    double value = m_dSpinBx.value();
    setValue(value);
}

void myDoubleSpinBoxedSlider::setMax(double max)
{
    connectInternalSignals(false);
        m_dSpinBx.setMaximum(max);
        m_min = m_dSpinBx.minimum();
        m_max = m_dSpinBx.maximum();
        setSliderRange();
    connectInternalSignals(true);

    double value = m_dSpinBx.value();
    setValue(value);
}


void myDoubleSpinBoxedSlider::setStep(double step)
{
    if(step <= 0) return;

    m_dSpinBx.setSingleStep(step);
    m_step = m_dSpinBx.singleStep();

    m_slider.setSingleStep(1);
    setSliderRange();
}

void myDoubleSpinBoxedSlider::setPageStep(double pageStep)
{
    //to be done !!!! -> ???
    if(pageStep <= 0) return;

    m_pageStep = pageStep;
    m_slider.setPageStep(m_pageStep);
}

void myDoubleSpinBoxedSlider::setDecimals(int decimal)
{
    //might change the minimum and maximum of the slider
    connectInternalSignals(false);
    m_dSpinBx.setDecimals(decimal);
    connectInternalSignals(true);
    m_decimals = m_dSpinBx.decimals();

    setMin(m_dSpinBx.minimum());
    setMax(m_dSpinBx.maximum());

    if(m_prefix_mode == QSPINBX_PREFIX_MODE) \
        m_dSpinBx.setMaximumWidth(0.7*m_dSpinBx.minimumSizeHint().width());
    else if(m_prefix_mode == QLABEL_PREFIX_MODE) \
        m_dSpinBx.setMaximumWidth(0.7*m_dSpinBx.minimumSizeHint().width());

}

void myDoubleSpinBoxedSlider::setValue(double value)
{
    connectInternalSignals(false);
        if(m_dSpinBx.value() != value) \
            m_dSpinBx.setValue(value);
        m_value = m_dSpinBx.value();
        setSliderValue(m_value);
    connectInternalSignals(true);

    emit(valueChanged(m_value));
}

void myDoubleSpinBoxedSlider::setText(QString text)
{
    m_label.setText(text);
}

void myDoubleSpinBoxedSlider::setSpinBoxPrefix(QString prefix)
{
    m_prefix = prefix;
    m_prefix_label.setText(prefix);

    if(m_prefix_mode == QSPINBX_PREFIX_MODE)
    {
        m_dSpinBx.setPrefix(prefix);
        m_dSpinBx.setMaximumWidth(0.7*m_dSpinBx.minimumSizeHint().width());
    }
    else if(m_prefix_mode == QLABEL_PREFIX_MODE)
    {
        m_dSpinBx.setPrefix("");
        m_dSpinBx.setMaximumWidth(0.7*m_dSpinBx.minimumSizeHint().width());
    }
}

void myDoubleSpinBoxedSlider::setSpinBoxPrefixes(QStringList prefixes_qstrl)
{
    m_prefix_comboBx.clear();
    m_prefix_comboBx.addItems(prefixes_qstrl);
}

void myDoubleSpinBoxedSlider::setSpinBoxPrefixMode(PREFIX_MODE prefix_mode)
{
    m_prefix_mode = prefix_mode;

    switch(prefix_mode)
    {
        case QSPINBX_PREFIX_MODE :
        {
            m_prefix_label.hide(); m_prefix_comboBx.hide();
            if(m_layout.indexOf(&m_prefix_label) != -1) m_layout.removeWidget(&m_prefix_label);
            if(m_layout.indexOf(&m_prefix_comboBx) != -1) m_layout.removeWidget(&m_prefix_comboBx);
            setSpinBoxPrefix(m_prefix);
        }
        break;

        case QLABEL_PREFIX_MODE :
        {
            m_prefix_label.show(); m_prefix_comboBx.hide();
            if(m_layout.indexOf(&m_prefix_comboBx) != -1) m_layout.removeWidget(&m_prefix_comboBx);
            if(m_layout.indexOf(&m_prefix_label) == -1) m_layout.insertWidget(2, &m_prefix_label);
            setSpinBoxPrefix(m_prefix);
        }
        break;

        case QCOMBOBX_PREFIX_MODE :
        {
            m_prefix_label.hide(); m_prefix_comboBx.show();
            if(m_layout.indexOf(&m_prefix_label) != -1) m_layout.removeWidget(&m_prefix_label);
            if(m_layout.indexOf(&m_prefix_comboBx) == -1) m_layout.insertWidget(2, &m_prefix_comboBx);
            setSpinBoxPrefix(m_prefix);
        }
        break;
   }
}


void myDoubleSpinBoxedSlider::setSpinBoxButtonSymbols(QAbstractSpinBox::ButtonSymbols button_symbols)
{
    m_dSpinBx.setButtonSymbols(button_symbols);
}

void myDoubleSpinBoxedSlider::setComboBoxIndex(int index)
{
    m_prefix_comboBx.setCurrentIndex(index);
}



void myDoubleSpinBoxedSlider::connectInternalSignals(bool areConnected)
{
    if(areConnected == true)
    {
       connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
       connect(&m_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(spinBxValueChanged(double)));
       connect(&m_prefix_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBxIndexChanged(int)));
    }
    else
    {
       disconnect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
       disconnect(&m_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(spinBxValueChanged(double)));
       disconnect(&m_prefix_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBxIndexChanged(int)));
    }
}

void myDoubleSpinBoxedSlider::setSliderRange()
{
    connectInternalSignals(false);

    //minimum
    m_slider.setMinimum(0);

    //maximum
    double N = (m_max - m_min)/m_step/* + 1*/;

    if(N <= std::numeric_limits<int>::max())
    {
        m_slider.setMaximum(lround(N));
        m_sliderStep = m_step;
    }
    else
    {
        m_slider.setMaximum(std::numeric_limits<int>::max());
        m_sliderStep = (m_max - m_min)/m_slider.maximum();
    }

    setSliderValue(m_value);

    connectInternalSignals(true);

}

void myDoubleSpinBoxedSlider::setSliderValue(double value)
{
    connectInternalSignals(false);

    long n = lround((value - m_min)/(m_max-m_min)*m_slider.maximum());
    m_slider.setValue(n);

    connectInternalSignals(true);
}


void myDoubleSpinBoxedSlider::sliderValueChanged(int slider_value)
{
    double value = m_min + slider_value*m_sliderStep;
    setValue(value);
}

void myDoubleSpinBoxedSlider::spinBxValueChanged(double value)
{
    setValue(value);
}

void myDoubleSpinBoxedSlider::comboBxIndexChanged(int index)
{
    emit(prefixIndexGetChanged(index));
}

double myDoubleSpinBoxedSlider::min() const
{
    return m_min;
}

double myDoubleSpinBoxedSlider::max() const
{
    return m_max;
}

double myDoubleSpinBoxedSlider::step() const
{
    return m_step;
}

double myDoubleSpinBoxedSlider::pageStep() const
{
    return m_pageStep;
}

int myDoubleSpinBoxedSlider::decimal() const
{
    return m_decimals;
}

double myDoubleSpinBoxedSlider::value() const
{
   return m_value;
}

QString myDoubleSpinBoxedSlider::getSpinBoxText() const
{
    return m_dSpinBx.text();
}

int myDoubleSpinBoxedSlider::getComboBoxIndex() const
{
    return m_prefix_comboBx.currentIndex();
}

void myDoubleSpinBoxedSlider::updateStyleSheet()
{




}

//void myDoubleSpinBoxedSlider::hide()
//{
//    m_isVisible = false;

//    m_label.setVisible(m_isVisible);
//    m_slider.setVisible(m_isVisible);
//    m_dSpinBx.setVisible(m_isVisible);
//    m_prefix_label.setVisible(m_isVisible);
//    m_prefix_comboBx.setVisible(m_isVisible);

//}

//void myDoubleSpinBoxedSlider::show()
//{
//    m_isVisible = true;

//    m_label.setVisible(true);
//    m_slider.setVisible(true);
//    m_dSpinBx.setVisible(true);

//    switch(m_prefix_mode)
//    {
//        case QSPINBX_PREFIX_MODE :
//        {
//            m_prefix_label.setVisible(false);
//            m_prefix_comboBx.setVisible(false);
//        }
//        break;

//        case QLABEL_PREFIX_MODE :
//        {
//            m_prefix_label.setVisible(true);
//            m_prefix_comboBx.setVisible(false);
//        }
//        break;

//        case MULTIPLE_PREFIX_MODE :
//        {
//            m_prefix_label.setVisible(false);
//            m_prefix_comboBx.setVisible(true);
//        }
//        break;

//    }
//}




