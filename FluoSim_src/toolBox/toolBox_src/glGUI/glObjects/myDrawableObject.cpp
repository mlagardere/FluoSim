
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


/********************************
 *
 *		 myDrawableObject
 *
 * ******************************/


myDrawableObject::myDrawableObject()
{
	m_drawingState = WAITING_FOR_DRAWING;
}

bool myDrawableObject::isStartingDrawing()
{
	return (m_drawingState == STARTING_DRAWING);
}

bool myDrawableObject::isDrawing()
{
	return (m_drawingState == CURRENTLY_DRAWING
			|| m_drawingState == STARTING_DRAWING
			|| m_drawingState == DRAWING_TO_BE_FINISHED);
}

bool myDrawableObject::isWaitingForDrawing()
{
	return (m_drawingState == WAITING_FOR_DRAWING);
}


bool myDrawableObject::isDrawn()
{
	return (m_drawingState == DRAWING_FINISHED);
}

/*virtual*/ void myDrawableObject::draw(bool drawing_state)
{

}

bool myDrawableObject::isAborted()
{
	return (m_drawingState == DRAWING_ABORT);

}

/*virtual*/ void myDrawableObject::objectHasBeenDrawn() {}


