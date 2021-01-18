
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

using namespace glm;


myEditableObject::myEditableObject()
{
	m_isEdited = 0;
	m_isEditingPoint = 0;
	m_edition_color = vec4(1,0,0,1);
}

bool myEditableObject::isEdited()
{
	return m_isEdited;
}

bool myEditableObject::isEditingAPoint()
{
	return m_isEditingPoint;
}

void myEditableObject::edit(bool edit_state){}
bool myEditableObject::isInsideEditionContour(glm::vec2 r){}
bool myEditableObject::isNextToAnEditionContourVertex(glm::vec2 r, float radius_inPxl, int& vertex_idx){}
float myEditableObject::getEditionContourSurface(){}

