
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



#include "Axis.h"


using namespace glm;

Axis::Axis(AXIS_ORIENTATION orientation, float axis_offset,
			   gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :


	myGLObject(IS_RENDERABLE_OBJECT, GLWIDGET_TYPE),
	m_axisGeom(parent_gMultiVector),
	m_axisText_font("fontFull_whiteOnVoid.png",test_desc),
	m_axisText_word(&m_axisText_font)


{
	m_orientation = orientation;
	m_axis_offset = axis_offset;

	glm::vec2 temp_point;

	if(m_orientation == HORIZONTAL)
	{
		temp_point = glm::vec2(-1, m_axis_offset);
		m_axisGeom.addPoint(temp_point);

		temp_point = glm::vec2(+1, m_axis_offset);
		m_axisGeom.addPoint(temp_point);
	}

	if(m_orientation == VERTICAL)
	{
		temp_point = glm::vec2(m_axis_offset, -1);
		m_axisGeom.addPoint(temp_point);

		temp_point = glm::vec2(m_axis_offset, +1);
		m_axisGeom.addPoint(temp_point);
	}

	m_axisText_word.setLetterSizePxl(8);
}



void Axis::render(myGLObject* renderingTarget)
{
	glm::vec2 temp_point;
	glm::vec2 p;

	if(m_orientation == HORIZONTAL)
	{
		//axis bar
			p = renderingTarget->fromHomToWorldCoordinate(glm::vec2(-1,0));
		temp_point = glm::vec2(p.x, m_axis_offset);
		m_axisGeom.setPoint(0, temp_point);

			p = renderingTarget->fromHomToWorldCoordinate(glm::vec2(+1,0));
		temp_point = glm::vec2(p.x, m_axis_offset);
		m_axisGeom.setPoint(1, temp_point);

		//axis text
		vec2 pos_text = renderingTarget->fromWorldToGLScreenCoordinate(temp_point);
			pos_text.x -= m_axisText_word.getSizePxl().x + 5;
			pos_text.y += 2;

		m_axisText_word.setPositionPxl(pos_text); //GL...
		m_axisText_word.setDirection({1,0});
		m_axisText_word.setColor(glm::vec4(1,1,1,1));
		m_axisText_word.draw((myGLWidget*) renderingTarget);

	}

	if(m_orientation == VERTICAL)
	{
  //	axis bar
			p = renderingTarget->fromHomToWorldCoordinate(glm::vec2(0,-1));
		temp_point = glm::vec2(m_axis_offset, p.y);
		m_axisGeom.setPoint(0, temp_point);

			p = renderingTarget->fromHomToWorldCoordinate(glm::vec2(0,+1));
		temp_point = glm::vec2(m_axis_offset, p.y);
		m_axisGeom.setPoint(1, temp_point);

		//axis text
		vec2 pos_text = renderingTarget->fromWorldToGLScreenCoordinate(temp_point);
			pos_text.x += 2;
			pos_text.y -= 2 + m_axisText_word.getSizePxl().y;

		m_axisText_word.setPositionPxl(pos_text); //GL...
		m_axisText_word.setDirection({1,0});
		m_axisText_word.setColor(glm::vec4(1,1,1,1));
		m_axisText_word.draw((myGLWidget*) renderingTarget);
	}

	m_axisGeom.render(renderingTarget);
}


void Axis::setText(string text)
{
	m_axisText_word.setText(text);
}

string Axis::getText()
{
	return m_axisText_word.getText();
}

void Axis::setAxisOffset(float axis_offset)
{
	m_axis_offset = axis_offset;
}

float Axis::getAxisOffset()
{
	return m_axis_offset;
}


