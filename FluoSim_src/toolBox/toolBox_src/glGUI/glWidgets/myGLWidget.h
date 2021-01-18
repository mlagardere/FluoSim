
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

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H


#include "toolbox_library_global.h"

#include "GL_glew/glew.h"
#include "gl.h"

#include "glm.hpp"

#include "glGUI/glObjects/myGLObject.h" //include glew.h
#include "QGLWidget"

#include "stdlib.h"


class TOOLBOXSHARED_EXPORT myGLWidget : public myGLObject,
										  public QGLWidget

{
public :

	myGLWidget(int size_x, int size_y, QWidget* parent = 0, QGLWidget* sharedContext_glWdgt = 0);
	virtual void initializeGL();
	void resizeGL(int w, int h);
	virtual glm::vec2 size();

};





#endif // MYGLWIDGET_H
