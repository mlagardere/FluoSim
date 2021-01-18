
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

#include "myGLScreen.h"

using namespace glm;

myGLScreenModel::myGLScreenModel(int x, int y, QWidget* parent) : myGLScreenView(x,y,parent)
{
	m_screen_mode = MOVEAROUND_MODE;
	m_drawing_type = NONE;
	m_drawing_state = NOT_DRAWING;

	m_mouse_state.left_button = UNCLICKED;
	m_mouse_state.middle_button = UNCLICKED;
	m_mouse_state.left_button = UNCLICKED;
	m_mouse_state.last_pos = vec2(0,0);
	m_mouse_state.last_translationVector = vec2(0,0);

	m_onFocus_object = 0;
}



void myGLScreenModel::addObject(myGLObject* obj)
{
	m_glObjects_v.push_back(obj);
}

void myGLScreenModel::removeObject(myGLObject* obj_toBeRemoved)
{
//	if(obj_toBeRemoved == 0) return; //->

	auto obj = m_glObjects_v.begin();
	while(*obj != obj_toBeRemoved && obj != m_glObjects_v.end())
	{
		obj++;
	}
	if(obj != m_glObjects_v.end())
	{
		if(*obj == m_onFocus_object) m_onFocus_object = 0;
//		delete *obj;
		m_glObjects_v.erase(obj);
	}
	else
	{
		cout<<"in : 'myGLScreenModel::removeObject(myGLObject* obj)' : object not found!!";
	}
}

void myGLScreenModel::deselectAllObjects()
{
	auto obj = m_glObjects_v.begin();
	while(obj != m_glObjects_v.end())
	{
		if((*obj)->isSelectionableObject()) (*obj)->select(0);
		obj++;
	}

	m_onFocus_object = 0;
}

void myGLScreenModel::deseditAllObjects()
{
	auto obj = m_glObjects_v.begin();
	while(obj != m_glObjects_v.end())
	{
		if((*obj)->isEditableObject()) (*obj)->edit(0);
		obj++;
	}

	m_onFocus_object = 0;


}

void myGLScreenModel::setOnFocusObject(myGLObject* obj_toBeFocused)
{
	auto obj = m_glObjects_v.begin();
	while(*obj != obj_toBeFocused && obj != m_glObjects_v.end())
	{
		obj++;
	}
	if(obj != m_glObjects_v.end())
	{
		m_onFocus_object = obj_toBeFocused;
	}
	else
	{
		cout<<"in : 'myGLScreenModel::setOnFocusObject(myGLObject* obj)' : object not found!!";
	}
}

myGLObject* myGLScreenModel::getOnFocusObject()
{
	return m_onFocus_object;
}

void myGLScreenModel::renderObjects()
{
	int obj_idx = 0;
	auto obj = m_glObjects_v.begin();
	while(obj != m_glObjects_v.end())
	{
		(*obj)->render(this);
		obj++;
		obj_idx++;
	}
}

myGLScreenLoopedGeometry* myGLScreenModel::drawLoopedGeometricObject(myMultiVector<glm::vec2>& geom_MV,
																	 gstd::gMultiVector<glm::vec2>& geom_gMV,
																	 myMultiVector<glm::vec2>& contour_MV,
																	 gstd::gMultiVector<glm::vec2>& contour_gMV,
																	 GEOMETRY geometry_type)
{
	if(m_onFocus_object && m_onFocus_object->isDrawn() == 0)
	{
		removeObject(m_onFocus_object);
		m_onFocus_object = 0;
	}

	myGLScreenLoopedGeometry* loop_geom = new myGLScreenLoopedGeometry(geom_MV, geom_gMV,
																	   contour_MV, contour_gMV,
																	   geometry_type, this);
	loop_geom->draw(1);
	addObject(loop_geom);

	m_onFocus_object = loop_geom;
	m_screen_mode = DRAWING_MODE;

	loop_geom->setRenderState(1);

	return loop_geom;	//->
}

myGLScreenMultiLineGeometry* myGLScreenModel::drawMultiLineGeometricObject(myMultiVector<glm::vec2>& geom_MV,
																		gstd::gMultiVector<glm::vec2>& geom_gMV,
																		myMultiVector<glm::vec2>& contour_MV,
																		gstd::gMultiVector<glm::vec2>& contour_gMV)
{
	if(m_onFocus_object && m_onFocus_object->isDrawn() == 0)
	{
		removeObject(m_onFocus_object);
		m_onFocus_object = 0;
	}

	myGLScreenMultiLineGeometry* multiLine_geom = new myGLScreenMultiLineGeometry(geom_MV, geom_gMV,
																				  contour_MV, contour_gMV,
																				  this);
	multiLine_geom->draw(1);
	addObject(multiLine_geom);

	m_onFocus_object = multiLine_geom;
	m_screen_mode = DRAWING_MODE;

	multiLine_geom->setRenderState(1);

	return multiLine_geom;	//->
}

//void  myGLScreenModel::drawGeometricObject(GEOMETRY geometry_type)
//{
//	if(m_onFocus_object && m_onFocus_object->isDrawn() == 0)
//	{
//		removeObject(m_onFocus_object);
//		m_onFocus_object = 0;
//	}

//	if(geometry_type == MULTILINE)
//	{
//		myGLScreenMultiLineGeometry* multiLine_geom = new myGLScreenMultiLineGeometry(this);
//		multiLine_geom->draw(1);
//		addObject(multiLine_geom);

//		m_onFocus_object = multiLine_geom;
//		m_screen_mode = DRAWING_MODE;

//		multiLine_geom->setRenderState(1);

//		return;	//->
//	}
//	else
//	{
//		myGLScreenLoopedGeometry* loop_geom = new myGLScreenLoopedGeometry(geometry_type, this);
//		loop_geom->draw(1);
//		addObject(loop_geom);

//		m_onFocus_object = loop_geom;
//		m_screen_mode = DRAWING_MODE;

//		loop_geom->setRenderState(1);

//		return;	//->
//	}
//}

//void myGLScreenModel::drawImageObject(string file_dir)
//{
//	if(m_onFocus_object && m_onFocus_object->isDrawn() == 0)
//	{
//		removeObject(m_onFocus_object);
//		m_onFocus_object = 0;
//	}

//	myGLScreenImage* image = new myGLScreenImage(file_dir, this);
//	image->draw(1);
//	addObject(image);

//	m_onFocus_object = image;
//	m_screen_mode = DRAWING_MODE;

//	image->setRenderState(1);
//}

bool myGLScreenModel::isInsideASelectionContour(vec2 Rw_click, myGLObject* &selected_obj)
{
	selected_obj = 0;

	auto obj = m_glObjects_v.begin();
	while(obj!=m_glObjects_v.end())
	{
		if((*obj)->isSelectionableObject() && (*obj)->isInsideSelectionContour(Rw_click))
		{
			if(selected_obj == 0) selected_obj = *obj;
			else
			{
				if(selected_obj->getSelectionContourSurface() >= (*obj)->getSelectionContourSurface())
				{
					selected_obj = *obj;
				}
			}
		}
		obj++;
	}

	if(selected_obj == 0) return 0;
	else
	{
		return 1;
	}
}

bool myGLScreenModel::isInsideAnEditionContour(vec2 Rw_click, myGLObject* &edited_obj)
{
	edited_obj = 0;

	auto obj = m_glObjects_v.begin();
	while(obj!=m_glObjects_v.end())
	{
		if((*obj)->isEditableObject() && (*obj)->isInsideEditionContour(Rw_click))
		{
			if(edited_obj == 0) edited_obj = *obj;
			else
			{
				if(edited_obj->getEditionContourSurface() >= (*obj)->getEditionContourSurface())
				{
					edited_obj = *obj;
				}
			}
		}
		obj++;
	}

	if(edited_obj == 0) return 0;
	else
	{
		return 1;
	}
}

void myGLScreenModel::setScreenMode(SCREEN_MODE mode)
{
	m_screen_mode = mode;
}
