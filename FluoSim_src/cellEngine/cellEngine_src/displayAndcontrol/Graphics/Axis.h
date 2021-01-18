
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


#ifndef AXIS_H
#define AXIS_H


#include "cellEngine_library_global.h"

#include "toolBox_src/toolbox_library_global.h"
    #include "toolBox_src/glGUI/glObjects/textRendering/myFont.h"
    #include "toolBox_src/glGUI/glObjects/textRendering/myWord.h"
    #include "glGUI/glObjects/myGLGeomtricObject.h"
    #include "glGUI/glObjects/myGLObject.h"

class CELLENGINE_LIBRARYSHARED_EXPORT Axis : public myGLObject
{
public :

	enum AXIS_ORIENTATION{HORIZONTAL, VERTICAL};

    Axis(AXIS_ORIENTATION orientation, float axis_offset,
		   gstd::gMultiVector<glm::vec2>& parent_gMultiVector);

	void render(myGLObject* renderingTarget);

	void setText(string text);
	string getText();

	void setAxisOffset(float axis_offset);
	float getAxisOffset();


private :

	AXIS_ORIENTATION m_orientation;
	myGLMultiLineGeometry m_axisGeom;

	float m_axis_offset;
//	string m_axisText;

	myFont m_axisText_font;
	myWord m_axisText_word;





};


#endif // AXIS_H
