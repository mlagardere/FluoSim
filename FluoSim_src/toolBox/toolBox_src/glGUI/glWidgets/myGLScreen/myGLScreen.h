
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

#ifndef MYGLSCREEN_H
#define MYGLSCREEN_H

#include "toolbox_library_global.h"

#include "glGUI/glWidgets/myGLWidget.h"

#include "string"
#include "glm.hpp"
#include "stdlib.h"
#include "iostream"

#include "QtEvents"

#include "glGUI/glObjects/myGLObject.h"
#include "geometry/myGeomtricObject.h"
#include "glGUI/glScreenObjects/myGLScreenGeometricObjects.h"
#include "glGUI/glScreenObjects/myGLScreenImageObjects.h"

class TOOLBOXSHARED_EXPORT myGLScreenView : public myGLWidget
{
public :

	myGLScreenView(int x, int y, QWidget* parent = 0);

	void mouseDoubleClickEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
//	void moveEvent(QMoveEvent * event);
	void wheelEvent(QWheelEvent * event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	virtual void treatEvent(QEvent* event);

private :



};


class TOOLBOXSHARED_EXPORT myGLScreenModel : public myGLScreenView
{
public :

	//event part
	enum SCREEN_MODE {MOVEAROUND_MODE, //for alt-button
					  MOVEAROUND_AND_SELECTION_MODE, //other cases...
					  EDITION_MODE,
					  DRAWING_MODE};

//	enum SCREEN_MODE {MOVEAROUND_MODE, EDITION_AND_SELECTION_MODE, DRAWING_MODE};
		enum DRAWING_TYPE {NONE, CIRCLE_TYPE, RECTANGLE_TYPE, POLYGON_TYPE};
		enum DRAWING_STATE {CURRENTLY_DRAWING, NOT_DRAWING, DRAWING_FINISHED};
			enum MOUSE_BUTTON_STATE {UNCLICKED, CLICKED};

			struct mouseState
			{
				MOUSE_BUTTON_STATE	left_button;
				MOUSE_BUTTON_STATE	right_button;
				MOUSE_BUTTON_STATE  middle_button;

				glm::vec2 last_pos;
				glm::vec2 last_translationVector;
			};

	myGLScreenModel(int x, int y, QWidget* parent);


	//object part
	void addObject(myGLObject* obj);
	void removeObject(myGLObject* obj);

	void deselectAllObjects();
	void deseditAllObjects();

	void setOnFocusObject(myGLObject* obj);
	myGLObject* getOnFocusObject();

	//use to create objects (its interface and its data)
	myGLScreenLoopedGeometry* drawLoopedGeometricObject(myMultiVector<glm::vec2>& geom_MV,
														gstd::gMultiVector<glm::vec2>& geom_gMV,
														myMultiVector<glm::vec2>& contour_MV,
														gstd::gMultiVector<glm::vec2>& contour_gMV,
														GEOMETRY geometry_type);

	myGLScreenMultiLineGeometry* drawMultiLineGeometricObject(myMultiVector<glm::vec2>& geom_MV,
															  gstd::gMultiVector<glm::vec2>& geom_gMV,
															  myMultiVector<glm::vec2>& contour_MV,
															  gstd::gMultiVector<glm::vec2>& contour_gMV);

	void renderObjects();

	bool isInsideASelectionContour(glm::vec2 Rw_click, myGLObject* &selected_obj);
	bool isInsideAnEditionContour(glm::vec2 Rw_click, myGLObject* &edited_obj);

	//mode part
	void setScreenMode(SCREEN_MODE mode);


protected :


	//screen mode
	SCREEN_MODE m_screen_mode;
		DRAWING_TYPE m_drawing_type;
		DRAWING_STATE m_drawing_state;

	//mouse state
	mouseState m_mouse_state;

	std::vector<myGLObject*> m_glObjects_v;
	myGLObject* m_onFocus_object;


};


class TOOLBOXSHARED_EXPORT myGLScreen : public myGLScreenModel
{
public :

	myGLScreen(int x, int y, QWidget* parent = 0);
	~myGLScreen();


	virtual void treatEvent(QEvent* event);

private :





};

#endif // MYGLSCREEN_H
