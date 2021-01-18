
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

#ifndef MYGLSCREENPROCEDURALIMAGE_H
#define MYGLSCREENPROCEDURALIMAGE_H


#include "myGLScreenImageObjects.h"
#include "glGUI/glObjects/myGLObject.h"

#include "glm.hpp"


//#include "GL_glew/glew.h"
//#include "gl.h"



class TOOLBOXSHARED_EXPORT myGLScreenProceduralImage : public myGLScreenImage
{
public :

	myGLScreenProceduralImage(gstd::gMultiVector<glm::vec2>& geom_gMV,
							  myMultiVector<glm::vec2>& contour_MV,
							  gstd::gMultiVector<glm::vec2>& contour_gMV, myGLWidget* glWidget);
	void render(myGLObject* renderingTarget);
	virtual void setSize(glm::vec2 size);
	void addObject(myGLObject* obj);

	void setRange(glm::vec2 bottom_left, glm::vec2 top_right);

private :

	GLuint m_frameBuffer;
	list<myGLObject*> m_GLObjects;
	glm::vec2 m_texSize;

	glm::vec2 m_bottom_left;
	glm::vec2 m_top_right;

};







#endif // MYGLSCREENPROCEDURALIMAGE_H

