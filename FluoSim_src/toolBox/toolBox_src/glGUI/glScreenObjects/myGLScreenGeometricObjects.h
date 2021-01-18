
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

#ifndef MYGLSCREENOBJECTS_H
#define MYGLSCREENOBJECTS_H

#define GLM_SWIZZLE

#include "toolbox_library_global.h"

#include "gpuTools_library_global.h"
	#include "gl/gMultiVector.h"

#include "toolbox_library_global.h"
	#include "containers/myMultiVector.h"
    #include "otherFunctions/otherFunctions.h"

#include "glGUI/glObjects/myGLGeomtricObject.h"
#include "glGUI/glWidgets/myGLWidget.h"

#include "QtEvents"


enum FILE_FORMAT {METAMORPH_GEOMETRY_FORMAT, NONE_GEOMETRY_FORMAT};

class TOOLBOXSHARED_EXPORT myGLScreenLoopedGeometry : public myGLObject
{
public :
	myGLScreenLoopedGeometry(myMultiVector<glm::vec2>& geom_MV,
							 gstd::gMultiVector<glm::vec2>& geom_gMV,
							 myMultiVector<glm::vec2>& contour_MV,
							 gstd::gMultiVector<glm::vec2>& contour,
							 GEOMETRY loopGeometry_type,
							 myGLWidget* glWidget);

	~myGLScreenLoopedGeometry();

	void draw(bool drawing_state);
	void render(myGLObject* renderingTarget);
	void select(bool select_state);
	void edit(bool edit_state);

	virtual void setSpace(OBJECT_SPACE space);

	virtual void treatEvent(QEvent* event);

	virtual float getSelectionContourSurface();
	virtual bool isInsideSelectionContour(glm::vec2 r);
	virtual bool isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx);

	virtual float getEditionSurface();
	virtual bool isInsideEditionContour(glm::vec2 r);
	virtual bool isNextToAnEditionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx);

	void loadGeometry(vector<glm::vec2> Rw_v);
	vector<glm::vec2>  getGeometry();

	bool isInside(glm::vec2 r);
	glm::vec2 randomPointInside();


protected :

	int getSubVectorId();


//***Variable Members***
protected :

	myGLWidget* m_glWidget; //to deal with events...

private :

	GEOMETRY m_loopGeometry_type;

	myGLLoopedGeometry* m_gl_geometry;
	myLoopedGeometry* m_geometry;
	myGLLoopedGeometry* m_gl_seletion_contour;
	myLoopedGeometry* m_seletion_contour;
	myGLLoopedGeometry* m_gl_edition_contour;
	myLoopedGeometry* m_edition_contour;

};


class TOOLBOXSHARED_EXPORT myGLScreenMultiLineGeometry : public myGLObject
{
public :

	myGLScreenMultiLineGeometry(myMultiVector<glm::vec2>& geom_MV,
								gstd::gMultiVector<glm::vec2>& geom_gMV,
								myMultiVector<glm::vec2>& contour_MV,
								gstd::gMultiVector<glm::vec2>& contour,
								myGLWidget* glWidget);

	~myGLScreenMultiLineGeometry();



	void draw(bool drawing_state);
	void render(myGLObject* renderingTarget);
	void select(bool select_state);
	void edit(bool edit_state);

	virtual void setSpace(OBJECT_SPACE space);

	virtual void treatEvent(QEvent* event);
	virtual float getSelectionContourSurface();
	virtual bool isInsideSelectionContour(glm::vec2 r);
	bool isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx);

	virtual float getEditionSurface();
	virtual bool isInsideEditionContour(glm::vec2 r);

	void loadGeometry(vector<glm::vec2> Rw_v);
	void addPoint(glm::vec2 Rw);
	void addPoints(std::vector<glm::vec2> Rw_v);
	void removePoint(unsigned int point_idx);
	void setPoint(unsigned int pt_idx, glm::vec2 Rw);


protected :

	myGLWidget* m_glWidget;

private :

	myGLMultiLineGeometry* m_gl_geometry;
	myMultiLineGeometry* m_geometry;
	myGLLoopedGeometry* m_gl_seletion_contour;
	myLoopedGeometry* m_seletion_contour;
	myGLLoopedGeometry* m_gl_edition_contour;
	myLoopedGeometry* m_edition_contour;

};




#endif // MYGLSCREENOBJECTS_H
