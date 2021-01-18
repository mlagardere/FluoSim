
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




#include "glGUI/glObjects/myGLObject.h"

using namespace std;
using namespace glm;

myRenderableObject::myRenderableObject() : QObject()
{
	m_isRendered = 0;
	m_isBlinking = 0;
		m_blinkingPeriod = 2.0;//s;
	m_isBlinked = 0;

	m_rendering_color = vec4(1,1,1,1);

	m_blinking_timer.setSingleShot(0); // to have repetive pulses
	m_blinking_timer.setInterval(m_blinkingPeriod*500);
	connect(&m_blinking_timer, SIGNAL(timeout()), this, SLOT(blink()));

	m_rendering_mode = LINE_LOOPED_RENDERING;
}

myRenderableObject::~myRenderableObject()
{
	disconnect(&m_blinking_timer, SIGNAL(timeout()), this, SLOT(blink()));
}


void myRenderableObject::render(myGLObject* renderingTarget)
{
}

void myRenderableObject::setRenderState(bool render_state)
{
	m_isRendered = render_state;
}

void myRenderableObject::setBlinking(bool blinking)
{
    if(m_isBlinking == blinking) return;

	m_isBlinking = blinking;
	if(m_isBlinking == 1)
	{
		m_isBlinked = 0;
		m_blinking_timer.start();
	}
	else
	{
		m_blinking_timer.stop();
		m_isBlinked = 0;
	}
}

void myRenderableObject::setPeriod(float period_inSeconds)
{
	m_blinkingPeriod = period_inSeconds;
	m_blinking_timer.setInterval(m_blinkingPeriod*1000);
}

void myRenderableObject::setSpace(OBJECT_SPACE space)
{
	m_space = space;
}

myRenderableObject::OBJECT_SPACE myRenderableObject::getSpace()
{
	return m_space;
}

void myRenderableObject::hide()
{
	m_isRendered = 0;
}

void myRenderableObject::show()
{
	m_isRendered = 1;
}

bool myRenderableObject::isVisible()
{
	return (!m_isBlinked && m_isRendered);
}

bool myRenderableObject::isBlinking()
{
	return m_isBlinking;
}

bool myRenderableObject::isBlinked()
{
	return m_isBlinked;
}

void myRenderableObject::blink()
{
	if(isBlinking()) m_isBlinked = !m_isBlinked;
}

void myRenderableObject::setRenderingMode(RENDERING_MODE mode)
{
	m_rendering_mode = mode;
}


void myRenderableObject::addProgram(gstd::gProgram& pg)
{
	m_gPrograms_v.push_back(pg);
}

void myRenderableObject::removeProgram(long program_idx)
{
	if(program_idx >= m_gPrograms_v.size()) return;
	m_gPrograms_v.erase(m_gPrograms_v.begin()+program_idx);
}


gstd::gProgram& myRenderableObject::getProgram(long pg_idx)
{
	return m_gPrograms_v[pg_idx];
}

const glm::vec4& myRenderableObject::getRenderingColor()
{
	return m_rendering_color;
}

void myRenderableObject::setRenderingColor(glm::vec4 color)
{
	m_rendering_color = color;
}

