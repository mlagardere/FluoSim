
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

using namespace glm;
using namespace std;


myRenderingTargetObject::myRenderingTargetObject()
{
	m_zoom_matrix = mat4(1.0,0,0,0,
						 0,1.0,0,0,
						 0,0,1.0,0,
						 0,0,0,1.0);

	m_isIsometric = true;
}

void myRenderingTargetObject::clearColorBuffer()
{
	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2] , m_clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void myRenderingTargetObject::setClearColor(glm::vec4 color)
{
	m_clearColor = color;
}

glm::vec4 myRenderingTargetObject::getClearColor()
{
	return m_clearColor;
}


void myRenderingTargetObject::setIsIsometric(bool isIsometric)
{
	m_isIsometric = isIsometric;
}

void myRenderingTargetObject::setZoomMatrix(mat4 new_zoom_matrix)
{
	m_zoom_matrix = new_zoom_matrix;
}

void myRenderingTargetObject::setScalingValues(vec3 alpha)
{
	m_zoom_matrix[0][0] = alpha.x;
	m_zoom_matrix[1][1] = alpha.y;
	m_zoom_matrix[2][2] = alpha.z;
}


void myRenderingTargetObject::setTranslationVector(vec3 t)
{
	m_zoom_matrix[3][0] = t.x;
	m_zoom_matrix[3][1] = t.y;
	m_zoom_matrix[3][2] = t.z;
}


vec3 myRenderingTargetObject::getTranslationVector()
{
	return  vec3(m_zoom_matrix[3][0],
				 m_zoom_matrix[3][1],
				 m_zoom_matrix[3][2]);
}

vec3 myRenderingTargetObject::getScalingValues()
{
	return vec3(m_zoom_matrix[0][0],
				m_zoom_matrix[1][1],
				m_zoom_matrix[2][2]);
}

const glm::mat4& myRenderingTargetObject::getZoomMatrix()
{
	return m_zoom_matrix;
}

glm::mat4 myRenderingTargetObject::getWorldToExtendedHomMatrix()
{
	mat2 fromHomToExtendedHomMatrix_2x2 = fromHomToExtendedHomMatrix();
	mat4 fromHomToExtendedHomMatrix_4x4;
		fromHomToExtendedHomMatrix_4x4[0][0] = fromHomToExtendedHomMatrix_2x2[0][0];
		fromHomToExtendedHomMatrix_4x4[1][1] = fromHomToExtendedHomMatrix_2x2[1][1];

	return fromHomToExtendedHomMatrix_4x4*m_zoom_matrix;
}

void myRenderingTargetObject::increaseScalingMatrix(float alpha)
{
	m_zoom_matrix = m_zoom_matrix + alpha*glm::transpose(glm::mat4( 1,0,0,0,
																	0,1,0,0,
																	0,0,1,0,
																	0,0,0,0) );
}

void myRenderingTargetObject::multiplyScalingMatrix(float alpha)
{
	m_zoom_matrix[0][0] *= alpha;
	m_zoom_matrix[1][1] *= alpha;
	m_zoom_matrix[2][2] *= alpha;
}

void myRenderingTargetObject::fitQuadToScreen(glm::vec2 bottomLeft, glm::vec2 topRight)
{
	vec2 alpha;
	vec2 t;
	vec2 diag = topRight - bottomLeft;

	alpha.x = 2/(diag.x);
	alpha.y = 2/(diag.y);

	t.x = 1-alpha.x * topRight.x;
	t.y = 1-alpha.y * topRight.y;

	setScalingValues(vec3(alpha,0.0f));
	setTranslationVector(vec3(t, 0.0));
}

void myRenderingTargetObject::captureScreenBackBuffer(gstd::gTexture& text)
{
	//reszie the texture

	vec2 size = this->size();
	text.loadTexture(size.x, size.y, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);//we bind the default frameBuffer
	glReadBuffer(GL_BACK);//we select the back and front color buffer
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, size.x, size.y, 0);


}


void myRenderingTargetObject::fitQuadToQuad(glm::vec2 bottomLeft_W, glm::vec2 topRight_W,
											glm::vec2 bottomLeft_hom, glm::vec2 topRight_hom) //loose fit
{
	vec2 A = bottomLeft_W;
	vec2 a = bottomLeft_hom;

	vec2 DIAG = topRight_W - bottomLeft_W;
	vec2 diag = topRight_hom - bottomLeft_hom;

	vec2 t;
	vec2 alpha;

	t.x = a.x - A.x* diag.x/DIAG.x;
	t.y = a.y - A.y* diag.y/DIAG.y;

	alpha.x = diag.x/DIAG.x;
	alpha.y = diag.y/DIAG.y;

	setScalingValues(vec3(alpha,0.0f));
	setTranslationVector(vec3(t, 0.0));
}

void myRenderingTargetObject::setOnFocusObject(myGLObject* obj){}


//*********************
//*	   From World ->  *
//*********************

vec2 myRenderingTargetObject::fromWorldToHomCoordinate(glm::vec2 Rw)
{
	vec4 r = m_zoom_matrix*vec4(Rw,0,1);
	return vec2(r.x, r.y);
}

vec2 myRenderingTargetObject::fromWorldToExtendedHomCoordinate(glm::vec2 Rw)
{
	return fromHomToExtendedHomCoordinate(fromWorldToHomCoordinate(Rw));
}


vec2 myRenderingTargetObject::fromWorldToScreenCoordinate(glm::vec2 Rw)
{
	return fromExtendedHomToScreenCoordinate(fromWorldToExtendedHomCoordinate(Rw));
}

vec2 myRenderingTargetObject::fromWorldToGLScreenCoordinate(glm::vec2 Rw)
{
	return fromExtendedHomToGLScreenCoordinate(fromWorldToExtendedHomCoordinate(Rw));
}

//*********************
//*	   From Hom ->  *
//*********************


vec2 myRenderingTargetObject::fromHomToExtendedHomCoordinate(glm::vec2 r)
{
	return fromHomToExtendedHomMatrix()*r;
}

vec2 myRenderingTargetObject::fromHomToScreenCoordinate(glm::vec2 r)
{
	return fromExtendedHomToScreenCoordinate(fromHomToExtendedHomCoordinate(r));
}

vec2 myRenderingTargetObject::fromHomToGLScreenCoordinate(glm::vec2 r)
{
	return fromExtendedHomToGLScreenCoordinate(fromHomToExtendedHomCoordinate(r));
}



//*********************
//*	   From ExtHom -> *
//*********************


glm::vec2 myRenderingTargetObject::fromExtendedHomToScreenCoordinate(glm::vec2 r_ext)
{
	vec2 screen_size(size().x,
					 size().y);
	mat2 M(screen_size.x/2.0f, 0.0,
			 0.0, -screen_size.y/2.0f);

	return M*(r_ext + vec2(1,-1));
}

glm::vec2 myRenderingTargetObject::fromExtendedHomToGLScreenCoordinate(glm::vec2 r_ext)
{
	vec2 screen_size(size().x,
					 size().y);
	mat2 M(screen_size.x/2.0f, 0.0,
			 0.0, +screen_size.y/2.0f);

	return M*(r_ext + vec2(1,1));
}


//*********************
//*	  From Screen <-  *
//*********************


vec2 myRenderingTargetObject::fromScrnToExtendedHomCoordinate(vec2 R)
{
	return fromScrnToExtendedHomMatrix()*R + fromScrnToExtendedHomVector();
}

vec2 myRenderingTargetObject::fromScrnToHomCoordinate(vec2 R)
{
	return fromExtendedHomToHomMatrix()*fromScrnToExtendedHomCoordinate(R);
}

vec2 myRenderingTargetObject::fromScrnToWorldCoordinate(vec2 R)
{
	vec2 r = fromScrnToHomCoordinate(R);

	return fromHomToWorldCoordinate(fromScrnToHomCoordinate(R));
}



//*********************
//*	 From GLScreen <- *
//*********************


vec2 myRenderingTargetObject::fromGLScrnToExtendedHomCoordinate(vec2 R)
{
	return fromGLScrnToExtendedHomMatrix()*R + fromGLScrnToExtendedHomVector();
}

vec2 myRenderingTargetObject::fromGLScrnToHomCoordinate(vec2 R)
{
	return fromExtendedHomToHomCoordinate(fromGLScrnToHomCoordinate(R));
}

vec2 myRenderingTargetObject::fromGLScrnToWorldCoordinate(vec2 R)
{
	return fromHomToWorldCoordinate(fromGLScrnToHomCoordinate(R));
}



//*************************
//*	 From Extended Hom <- *
//*************************


vec2 myRenderingTargetObject::fromExtendedHomToHomCoordinate(vec2 r_ext)
{
	return fromExtendedHomToHomMatrix()*r_ext;
}

vec2 myRenderingTargetObject::fromExtendedHomToWorldCoordinate(vec2 r_ext)
{
	return fromHomToWorldCoordinate(fromExtendedHomToHomMatrix()*r_ext);
}


//*************************
//*	     From Hom <-      *
//*************************

vec2 myRenderingTargetObject::fromHomToWorldCoordinate(vec2 r)
{
	mat2 scaling_inv = mat2(1.0/getScalingValues().x,0,0, 1.0/getScalingValues().y);
	vec2 t = vec2(getTranslationVector().x,
				  getTranslationVector().y);

	return scaling_inv*(r - t);
}



mat2 myRenderingTargetObject::fromScrnToExtendedHomMatrix()
{
	vec2 screen_size(size().x,
					 size().y);
	return mat2(2.0/screen_size.x, 0.0,
				0.0, -2.0/screen_size.y);
}

vec2 myRenderingTargetObject::fromScrnToExtendedHomVector()
{
	return vec2(-1,1);
}

mat2 myRenderingTargetObject::fromGLScrnToExtendedHomMatrix()
{
	vec2 screen_size(size().x,
					 size().y);
	return mat2(2.0/screen_size.x, 0.0,
				0.0, 2.0/screen_size.y);
}

vec2 myRenderingTargetObject::fromGLScrnToExtendedHomVector()
{
	return vec2(-1,-1);
}

glm::mat2 myRenderingTargetObject::fromHomToExtendedHomMatrix()
{
	mat2 fromHomToExtendedHom_matrix = mat2(1.0f);

	if(m_isIsometric == true)
	{
		vec2 scrn_size = this->size();

		if(scrn_size.x >= scrn_size.y)
		{
			 fromHomToExtendedHom_matrix[0][0] = scrn_size.y/scrn_size.x;
			 fromHomToExtendedHom_matrix[1][1] = 1.0f;
		}
		else
		{
			 fromHomToExtendedHom_matrix[0][0] = 1.0f;
			 fromHomToExtendedHom_matrix[1][1] = scrn_size.x/scrn_size.y;
		}
	}
	return fromHomToExtendedHom_matrix;

}

glm::mat2 myRenderingTargetObject::fromExtendedHomToHomMatrix()
{
	mat2 fromExtendedHomtoHom_matrix = mat2(1.0f);
	if(m_isIsometric == true)
	{
		vec2 scrn_size = this->size();

		if(scrn_size.x >= scrn_size.y)
		{
			 fromExtendedHomtoHom_matrix[0][0] = 1.0f/(scrn_size.y/scrn_size.x);
			 fromExtendedHomtoHom_matrix[1][1] = 1.0f;
		}
		else
		{
			 fromExtendedHomtoHom_matrix[0][0] = 1.0f;
			 fromExtendedHomtoHom_matrix[1][1] = 1.0f/(scrn_size.x/scrn_size.y);
		}
	}
	return fromExtendedHomtoHom_matrix;
}




/*virtual*/ glm::vec2 myRenderingTargetObject::size(){} //to be reimplemented in each derived class!!!!


