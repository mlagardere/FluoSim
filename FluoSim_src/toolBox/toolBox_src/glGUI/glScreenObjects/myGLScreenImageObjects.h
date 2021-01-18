
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



#ifndef MYGLSCREENIMAGEOBJECTS_H
#define MYGLSCREENIMAGEOBJECTS_H

#define GLM_SWIZZLE




#include "glGUI/glObjects/myGLImageObjects.h"
#include "glGUI/glObjects/myGLGeomtricObject.h"
#include "glGUI/glWidgets/myGLWidget.h"
#include "QtEvents"

class TOOLBOXSHARED_EXPORT myGLScreenImage : public myGLObject //myGLObject(1,1,1,1)
{
public :

	myGLScreenImage(std::string file_dir,
					gstd::gMultiVector<glm::vec2>& geom_gMV,
					myMultiVector<glm::vec2>& contour_MV,
					gstd::gMultiVector<glm::vec2>& contour_gMV,
					myGLWidget* glWidget, bool toBeDrawn = 0);



	myGLScreenImage(gstd::gMultiVector<glm::vec2>& geom_gMV,
					myMultiVector<glm::vec2>& contour_MV,
					gstd::gMultiVector<glm::vec2>& contour_gMV,
					myGLWidget* glWidget, bool toBeDrawn = 0);

	~myGLScreenImage();

	void setBottomLeft(glm::vec2 bottom_left_Rw);
	virtual void setSize(glm::vec2 size);

	virtual void setSpace(OBJECT_SPACE space);

	glm::vec2 getResolution();

	void setIsXInverted(bool isInverted);
	void setIsYInverted(bool isInverted);

	void draw(bool drawing_state);
	void render(myGLObject* renderingTarget);
	void select(bool select_state);
	void edit(bool edit_state);

	virtual void treatEvent(QEvent* event);

	virtual float getSelectionContourSurface();
	virtual bool isInsideSelectionContour(glm::vec2 r);
	bool isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx);

	virtual float getEditionSurface();
	virtual bool isInsideEditionContour(glm::vec2 r);
	bool isNextToAnEditionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx);

protected :

	myGLWidget* m_glWidget;

	myGLImage* m_gl_image;
	myGLLoopedGeometry* m_gl_seletion_contour;
	myLoopedGeometry* m_seletion_contour;
	myGLLoopedGeometry* m_gl_edition_contour;
	myLoopedGeometry* m_edition_contour;

};



#endif // MYGLSCREENIMAGEOBJECTS_H

