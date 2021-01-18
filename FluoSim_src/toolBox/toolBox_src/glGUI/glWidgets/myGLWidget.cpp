
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


#include "myGLWidget.h"

using namespace glm;

myGLWidget::myGLWidget(int size_x, int size_y,QWidget* parent, QGLWidget* sharedContext_glWdgt) :

	QGLWidget(parent, sharedContext_glWdgt, 0),
	myGLObject(myGLObject::IS_RENDERING_TARGET_OBJECT |
			   myGLObject::BECAME_WIDGET_OBJECT,
			   GLWIDGET_TYPE)

{
	QGLWidget::resize(size_x, size_y);
	context()->format().setDirectRendering(1);
	context()->format().setDoubleBuffer(1);
	setAutoBufferSwap(0);
	m_clearColor = vec4(0.1,0.1,0.1,1);

	this->makeCurrent();
	glewExperimental = true;
	glewInit();
}

void myGLWidget::initializeGL()
{
	glClearColor(0.1,0.1,0.1,1.0);
	glPointSize(2.0);
	glLineWidth(2.0);
	glClear(GL_COLOR_BUFFER_BIT);
	GLint bufs;
	GLint samples;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
	glGetIntegerv(GL_SAMPLES, &samples);

	QGLWidget::initializeGL();
}

void myGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

}

glm::vec2 myGLWidget::size()
{
	QSize size = QGLWidget::size();
	return glm::vec2(size.width(), size.height());
}
