
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



#include "mySpinBoxedSlider.h"

using namespace std;

mySpinBoxedSlider::mySpinBoxedSlider(QWidget* parent) : QWidget(parent),
    m_layout(this), m_slider(Qt::Horizontal),
    m_min(0), m_max(100), m_value(1), m_step(1), m_pageStep(10)
{
    m_layout.addWidget(&m_label);
    m_layout.addWidget(&m_slider);
    m_layout.addWidget(&m_prefix_label);
    m_layout.addWidget(&m_spinBx);
    m_layout.setMargin(0);
    m_layout.setSpacing(0);

    m_spinBx.setAlignment(Qt::AlignRight);
    setSpinBoxPrefixMode(QLABEL_PREFIX_MODE);

    setMin(m_min);
    setMax(m_max);

    connectInternalSignals(true);

    m_spinBx.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_prefix_label.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_slider.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
}

void mySpinBoxedSlider::setMin(int min)
{
    m_min = min;

    if(m_max < m_min)
    {
        m_max = m_min;
        m_value = m_min;
    }

    connectInternalSignals(false); //in some cases, changing min value induce emission of the valueChanged signal...

    m_slider.setMinimum(m_min);
    m_spinBx.setMinimum(m_min);

    m_slider.setMaximum(m_max);
    m_spinBx.setMaximum(m_max);

    connectInternalSignals(true);

    setValue(m_value);
}

void mySpinBoxedSlider::setMax(int max)
{
   m_max = max;
   if(m_min > m_max)
   {
      m_min = m_max;
      m_value = m_max;
   }

   connectInternalSignals(false);

   m_slider.setMinimum(m_min);
   m_spinBx.setMinimum(m_min);

   m_spinBx.setMaximum(m_max);
   m_slider.setMaximum(m_max);

   connectInternalSignals(true);

   setValue(m_value);
}

void mySpinBoxedSlider::setStep(uint step)
{
    if(step == 0) return;

    m_step = step;
    m_slider.setSingleStep(m_step);
    m_spinBx.setSingleStep(step);
}

void mySpinBoxedSlider::setPageStep(uint pageStep)
{

    if(pageStep == 0) return;

    m_pageStep = pageStep;
    m_slider.setPageStep(m_pageStep);
}

void mySpinBoxedSlider::setValue(int value)
{
    m_value = value;
    if(value < m_min) m_value = m_min;
    if(value > m_max) m_value = m_max;

    connectInternalSignals(false); //to avoid cross modifications and hence sending of useless signals
        m_slider.setValue(m_value);
        m_spinBx.setValue(m_value);
    connectInternalSignals(true);

    emit(valueChanged(m_value));
    emit(valueChanged((float) m_value));
}

void mySpinBoxedSlider::setText(QString text)
{
    m_label.setText(text);
}

void mySpinBoxedSlider::setSpinBoxPrefix(QString prefix)
{
    m_prefix = prefix;
    m_prefix_label.setText(prefix);

    if(m_prefix_mode == QSPINBX_PREFIX_MODE)
    {
        m_spinBx.setPrefix(prefix);
//        m_spinBx.setMaximumWidth(0.7*m_spinBx.minimumSizeHint().width());
    }
    else if(m_prefix_mode == QLABEL_PREFIX_MODE)
    {
        m_spinBx.setPrefix("");
//        m_spinBx.setMaximumWidth(0.6*m_spinBx.minimumSizeHint().width());
    }

}

void mySpinBoxedSlider::setSpinBoxPrefixMode(PREFIX_MODE prefix_mode)
{
   switch(prefix_mode)
   {
        case QSPINBX_PREFIX_MODE :
        {
            if(m_layout.count() == 4) \
                m_layout.removeWidget(&m_prefix_label);
            m_prefix_mode = prefix_mode;
            setSpinBoxPrefix(m_prefix);
        }
        break;

        case QLABEL_PREFIX_MODE :
        {
            if(m_layout.count() == 3) \
                m_layout.insertWidget(2, &m_prefix_label);
            m_prefix_mode = prefix_mode;
            setSpinBoxPrefix(m_prefix);
        }
        break;
   }
}

void mySpinBoxedSlider::setSpinBoxButtonSymbols(QAbstractSpinBox::ButtonSymbols button_symbols)
{
    m_spinBx.setButtonSymbols(button_symbols);
}


void mySpinBoxedSlider::connectInternalSignals(bool areConnected)
{
    if(areConnected == true)
    {
       connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
       connect(&m_spinBx, SIGNAL(valueChanged(int)), this, SLOT(spinBxValueChanged(int)));
    }
    else
    {
       disconnect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
       disconnect(&m_spinBx, SIGNAL(valueChanged(int)), this, SLOT(spinBxValueChanged(int)));
    }
}

void mySpinBoxedSlider::sliderValueChanged(int value)
{
    setValue(value);
}

void mySpinBoxedSlider::spinBxValueChanged(int value)
{
    setValue(value);
}


int mySpinBoxedSlider::value() const
{
   return m_value;
}

void mySpinBoxedSlider::hide()
{
    m_label.hide();
    m_slider.hide();
    m_prefix_label.hide();
    m_spinBx.hide();
}

void mySpinBoxedSlider::show()
{
    m_label.show();
    m_slider.show();
    m_prefix_label.show();
    m_spinBx.show();
}



