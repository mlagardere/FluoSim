
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



mySelectionableObject::mySelectionableObject()
{
	m_isSelected = 0;
	m_isResizing = 0;
		m_selectedVertex = -1;
	m_selection_color = glm::vec4(0,0,1,1);

}

bool mySelectionableObject::isSelected()
{
	return m_isSelected;
}

bool mySelectionableObject::isResizing()
{
	return m_isResizing;
}

//virtual functions
void mySelectionableObject::select(bool select_state){}
void mySelectionableObject::onSelection_GLObject(bool selection){}
bool mySelectionableObject::isInsideSelectionContour(glm::vec2 r){}
float mySelectionableObject::getSelectionContourSurface(){}
bool mySelectionableObject::isNextToASelectionContourVertex(glm::vec2 r, float radius_inPxl, int& vertex_idx){}
