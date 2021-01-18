
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


#include "myGLObject.h"




myGLObject::myGLObject(int OBJECT_ABILITIES, OBJECT_TYPE type)
{
	m_OBJECT_ABILITIES = OBJECT_ABILITIES;
	m_space = WORLD_SPACE;

	m_objectType = type;
}

/*virtual*/ myGLObject::~myGLObject()
{

}

void myGLObject::treatEvent(QEvent* event)
{


}

bool myGLObject::isRenderableObject()
{
	return (m_OBJECT_ABILITIES & IS_RENDERABLE_OBJECT) == IS_RENDERABLE_OBJECT;
}

bool myGLObject::isEditableObject()
{
	return (m_OBJECT_ABILITIES & IS_EDITABLE_OBJECT) == IS_EDITABLE_OBJECT;
}

bool myGLObject::isSelectionableObject()
{
	return (m_OBJECT_ABILITIES & IS_SELECTIONABLE_OBJECT) == IS_SELECTIONABLE_OBJECT;
}

bool myGLObject::isDrawableObject()
{
	return (m_OBJECT_ABILITIES & IS_DRAWABLE_OBJECT) == IS_DRAWABLE_OBJECT;
}

bool myGLObject::isRenderingTargetObject()
{
	return (m_OBJECT_ABILITIES & IS_RENDERING_TARGET_OBJECT) == IS_RENDERING_TARGET_OBJECT;
}

bool myGLObject::becameWidgetObject()
{
	return (m_OBJECT_ABILITIES & BECAME_WIDGET_OBJECT) == BECAME_WIDGET_OBJECT;
}

myGLObject::OBJECT_TYPE myGLObject::getObjectType()
{
	return m_objectType;
}

/*virtual*/ void myGLObject::objectHasBeenDrawn()
{
	emit(hasBeenDrawn_sig());
}




