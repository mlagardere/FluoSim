
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



#include "myGLScreenProceduralImage.h"

using namespace glm;


myGLScreenProceduralImage::myGLScreenProceduralImage(gstd::gMultiVector<glm::vec2>& geom_gMV,
													 myMultiVector<glm::vec2>& contour_MV,
													 gstd::gMultiVector<glm::vec2>& contour_gMV,
													 myGLWidget* glWidget) :

	myGLScreenImage(geom_gMV, contour_MV, contour_gMV, glWidget)

{
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gl_image->getTexture().getTextureHdl(), 0);

	m_bottom_left = vec2(-1,-1);
	m_top_right = vec2(+1,+1);

//	setClearColor({0.05,0.05,0.2,1});
	m_texSize = vec2(1024,1024);
	m_gl_image->setTexSize(m_texSize);
	setSize(vec2(0.2,0.2));
//	setSize(vec2(1,1));

//	m_gl_image->getTexture().genMipMap();
//	m_gl_image->getTexture().enableMipMap(1);
//	m_gl_image->getTexture().setMinFilter(GL_LINEAR);
//	m_gl_image->getTexture().setMagFilter(GL_LINEAR);


	m_drawingState = DRAWING_FINISHED;
}


void myGLScreenProceduralImage::setSize(glm::vec2 size)
{
	myGLScreenImage::setSize(size);
}


void myGLScreenProceduralImage::render(myGLObject* renderingTarget)
{
//	if(renderingTarget->isRenderingTargetObject() == false) return; //->
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glEnable(GL_LINE_SMOOTH);
	glClearColor(1,1,1,1);
//	clearColorBuffer();
	glViewport(0,0,1024,1024);
	glLineWidth(5.0);
	glClear(GL_COLOR_BUFFER_BIT);

	myGLWidget* gl = (myGLWidget*) renderingTarget;
	gl->updateGL();

//	m_gl_image->fitQuadToScreen(vec2(-10,-10), vec2(+2,1000));
	m_gl_image->fitQuadToScreen(m_bottom_left, m_top_right);
	m_gl_image->setIsYInverted(false);

	auto obj = m_GLObjects.begin();
	while(obj != m_GLObjects.end())
	{
		(*obj)->render(m_gl_image);
		obj++;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_LINE_SMOOTH);


	glViewport(0,0, m_glWidget->size().x,m_glWidget->size().y);
	myGLScreenImage::render(m_glWidget);

//	myGLScreenImage::m_gl_seletion_contour->render(renderingTarget);
//	gl->swapBuffers();

}

void myGLScreenProceduralImage::addObject(myGLObject* obj)
{
	m_GLObjects.push_back(obj);
}

void myGLScreenProceduralImage::setRange(glm::vec2 bottom_left, glm::vec2 top_right)
{
	m_bottom_left = bottom_left;
	m_top_right = top_right;
}
