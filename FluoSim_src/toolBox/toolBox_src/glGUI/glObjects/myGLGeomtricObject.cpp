
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



#include "myGLGeomtricObject.h"

using namespace glm;


/********************************
 *
 *		    myGLGeometry
 *
 * ******************************/

myGLGeometry::myGLGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) : m_R(parent_gMultiVector.addSubVector())
{


}

gstd::gSubVector<glm::vec2>& myGLGeometry::getgVector()
{
	return m_R;
}

//void myGLGeometry::updateCoords()
//{
//	long pt_idx = 0;
//	while(pt_idx <= m_R.size()-1)
//	{

//		vec2 rand = m_R[pt_idx];
//		rand += random_gaussian(0.2);

//		m_R[pt_idx] = rand;
//		pt_idx++;
//	}

//}

int myGLGeometry::getSubVectorId()
{
	return m_R.getParam()->m_id;
}

void myGLGeometry::addPoint(glm::vec2 R)
{
	m_R.push_back(R);
}

void myGLGeometry::addPoints(std::vector<glm::vec2> R_v)
{
	m_R.insert(m_R.size(), R_v);
}

void myGLGeometry::removePoint(unsigned int point_idx)
{
	m_R.erase(point_idx);
}

void myGLGeometry::clear()
{
	m_R.clear();
}

void myGLGeometry::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	m_R[pt_idx] = R;

}

glm::vec2 myGLGeometry::getPoint(unsigned int point_idx)
{
	return m_R[point_idx];

}

unsigned int myGLGeometry::getSize()
{
	return m_R.size();
}

gstd::gSubVector<glm::vec2>& myGLGeometry::getR()
{
	return m_R;
}

void myGLGeometry::translate(glm::vec2 dR)
{
	for(int pt_idx = 0; pt_idx <= getSize()-1; pt_idx++)
	{
		vec2 new_R = m_R[pt_idx];
			 new_R += dR;

		m_R[pt_idx] = new_R;
	}
}

void myGLGeometry::scale(glm::vec2 center, vec2 ratio)
{
	mat2 scaling(ratio.x,0,
				 0,ratio.y);

	for(int pt_idx = 0; pt_idx <= getSize()-1; pt_idx++)
	{
		vec2 new_R = m_R[pt_idx];
			 new_R = center + scaling*(new_R - center);

		m_R[pt_idx] = new_R;
	}
}



/***************************************
 *
 *		    myGLLoopedGeometry
 *
 * *************************************/


myGLLoopedGeometry::myGLLoopedGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT, myGLObject::GL_LOOPED_GEOMETRY_TYPE),
	myGLGeometry(parent_gMultiVector)

{
	string vs_src;
	vs_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"uniform vec4 color_in;\n"
				"uniform mat4 zoom_matrix;\n"
				"uniform vec4 translation_vector;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0)+translation_vector;\n"
				"	gl_Position = zoom_matrix*temp;\n"
				"	color = color_in;\n"
				"}\n";

	string fs_src;
	fs_src +=	"#version 150 core\n\n"
				""
				"in vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=color;\n"
				"}\n";


	gstd::gProgram pgm("normal_rendering");

		gstd::gShader vs(gstd::VERTEX_SHADER);
			vs.setSource(vs_src, 0);
			vs.compile();
//			vs.coutEntryPoints_v();
		gstd::gShader fs(gstd::FRAGMENT_SHADER);
			fs.setSource(fs_src, 0);
			fs.compile();

	pgm.addShader(vs);
	pgm.addShader(fs);
	pgm.linkShaders();

	myGLObject::addProgram(pgm);
}

glm::vec2 myGLLoopedGeometry::getEdge(unsigned int edge_idx)
{
	if(edge_idx >= getSize())
	{
		//error
		return vec2(-1,-1);
	}
	else
	{
		vec2 edge = getPoint((edge_idx+1)%getSize()) - getPoint(edge_idx);
		return edge;
	}
}

bool myGLLoopedGeometry::isInside(vec2 R)
{
	bool in=0;

	int N = getSize();

	vec2 vec_u;
	vec2 vec_t;
	vec2 delta;

	float t, t_bis;
	float D;
	int N_intersection=0;

	vec_u.x=0.5;
	vec_u.y=0.5;

		for(int pt_idx=0; pt_idx<=N-1; pt_idx++)
		{
//            if(pt_idx == N-1) vec_t = m_R[0] - m_R[pt_idx];
//            else
//            {
//                vec_t = m_R[pt_idx+1] - m_R[pt_idx];
//            }

			vec_t = getEdge(pt_idx);

//            delta = r - m_R[pt_idx];
			delta = R - getPoint(pt_idx);
			D=vec_t[1]*vec_u[0]-vec_t[0]*vec_u[1];

			if(D!=0)
			{
				t=(delta[1]*vec_u[0]-delta[0]*vec_u[1])/D; //along the curve line
				t_bis=(delta[1]*vec_t[0]-delta[0]*vec_t[1])/D;

			if(t>=0 && t<1 && t_bis>=0) {N_intersection++;}
			}
		}

		if(N_intersection%2==0)
		{
            in=0; //even number of intersection -> outside
		}
		else
		{
			in=1;
		}

	return in;
}


void myGLLoopedGeometry::render(myGLObject* renderingTarget)
{
//	if(renderingTarget->isRenderingTargetObject() == false) return; //->

	vec2 S = renderingTarget->size();
	mat4 zoom_matrix;
	switch(m_space)
	{
		case WORLD_SPACE : zoom_matrix = renderingTarget->getWorldToExtendedHomMatrix();
		break;

		case HOMOGENEOUS_SCREEN_SPACE : zoom_matrix = mat4(1.0);
		break;

		case GL_SCREEN_SPACE : zoom_matrix = mat4(2/S.x, 0, 0, -1,
												  0, 2/S.y, 0, -1,
												  0, 0, 0, 0,
												  0, 0, 0, 1);
		break;

		case SCREEN_SPACE : zoom_matrix = mat4(+2/S.x, 0, 0, -1,
											   0, -2/S.y, 0, +1,
											   0, 0, 0, 0,
											   0, 0, 0, 1);
		break;
	}

	switch(m_rendering_mode)
	{
		case LINE_STRIP_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R", getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glDrawArrays(GL_LINE_STRIP, 0, getSize());

			pgm.useProgram(0);
		}
		break;

		case LINE_LOOPED_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R", getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glDrawArrays(GL_LINE_LOOP, 0, getSize());

			pgm.useProgram(0);
		}
		break;

		case POINT_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R",getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glPointSize(7.0);
			glDrawArrays(GL_POINTS, 0, getSize());

			pgm.useProgram(0);
		}
		break;
		case LINE_AND_POINT_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R",getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glDrawArrays(GL_LINE_LOOP, 0, getSize());
			glPointSize(7.0);
			glDrawArrays(GL_POINTS, 0, getSize());

			pgm.useProgram(0);
		}
		break;
	}
}


/******************************************
 *
 *		        myGLRectangle
 *
 * ****************************************/



myGLRectangle::myGLRectangle(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :

	myGLLoopedGeometry(parent_gMultiVector)

{
	addPoint(vec2(0,0)); //topLeft
	addPoint(vec2(0,0)); //topRight
	addPoint(vec2(0,0)); //lowerRight
	addPoint(vec2(0,0)); //lowerLeft

	m_bottomLeft = vec2(0,0);
	m_size = vec2(0,0);

	m_hasMinimumSize = 0;
	m_minimum_size = vec2(0.00,0.00);
}

void myGLRectangle::setPoints()
{
	setPoint(0, m_bottomLeft);
	setPoint(1, m_bottomLeft + vec2(0, m_size.y));
	setPoint(2, m_bottomLeft + m_size);
	setPoint(3, m_bottomLeft + vec2(m_size.x, 0));
}

void myGLRectangle::setTopRight(glm::vec2 top_right)
{
	setDiag( m_bottomLeft, top_right);
	setPoints();
}

void myGLRectangle::setBottomLeft(glm::vec2 bottom_left)
{
	setDiag(bottom_left, m_bottomLeft + m_size);
	setPoints();
}

void myGLRectangle::setTopLeft(glm::vec2 top_left)
{
	setDiag(vec2(top_left.x, m_bottomLeft.y),
			vec2(getTopRight().x, top_left.y));
	setPoints();
}

void myGLRectangle::setBottomRight(glm::vec2 bottom_right)
{
	setDiag(vec2(m_bottomLeft.x, bottom_right.y),
			vec2(bottom_right.x, getTopLeft().y));
	setPoints();
}


void myGLRectangle::setDiag(glm::vec2 pt1, glm::vec2 pt2)
{
	m_bottomLeft = vec2(fmin(pt1.x, pt2.x), fmin(pt1.y, pt2.y));
	vec2 topRight = vec2(fmax(pt1.x, pt2.x), fmax(pt1.y, pt2.y));

	if(m_hasMinimumSize)
	{
		if(topRight.x - m_bottomLeft.x <= m_minimum_size.x) topRight.x = m_bottomLeft.x + m_minimum_size.x;
		if(topRight.y - m_bottomLeft.y <= m_minimum_size.y) topRight.y = m_bottomLeft.y + m_minimum_size.y;
	}

	m_size = topRight - m_bottomLeft;
	setPoints();
}


void myGLRectangle::setWidth(float width)
{
	if(m_hasMinimumSize && m_minimum_size.x > width) m_size.x = m_minimum_size.x;
	else m_size.x = width;

	setPoints();
}


void myGLRectangle::setHeight(float height)
{
	if(m_hasMinimumSize && m_minimum_size.y > height) m_size.y = m_minimum_size.y;
	else m_size.y = height;

	setPoints();
}

void myGLRectangle::setHasMinimumSize(bool hasMinimumSize)
{
	m_hasMinimumSize = hasMinimumSize;
}

void myGLRectangle::setMinimumSize(glm::vec2 min_size)
{
	m_minimum_size = min_size;
}

float myGLRectangle::getWidth()
{
	return m_size.x;
}


float myGLRectangle::getHeight()
{
	return m_size.y;
}

glm::vec2 myGLRectangle::getSize()
{
	return m_size;
}

glm::vec2 myGLRectangle::getBottomLeft()
{
	return m_bottomLeft;
}

glm::vec2 myGLRectangle::getTopLeft()
{
	return m_bottomLeft + vec2(0,m_size.y);
}

glm::vec2 myGLRectangle::getTopRight()
{
	return m_bottomLeft + m_size;
}

glm::vec2 myGLRectangle::getBottomRight()
{
	return m_bottomLeft + vec2(m_size.x,0);
}


/*virtual*/ bool myGLRectangle::isInside(glm::vec2 R)
{
	return (R.x > m_bottomLeft.x &&
			R.y > m_bottomLeft.y &&
			(R.x < m_bottomLeft.x + m_size.x) &&
			(R.y < m_bottomLeft.y + m_size.y));
}

void myGLRectangle::translate(glm::vec2 dR)
{
	m_bottomLeft = m_bottomLeft + dR;
	setPoints();
}

/************************************
 *
 *		      myGLCircle
 *
 * **********************************/

myGLCircle::myGLCircle(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :

	myGLLoopedGeometry(parent_gMultiVector)

{
	m_nb_pts = 100;
	m_radius = 0;
	m_center = vec2(0,0);

	float angle;

	for(int pt_idx = 0; pt_idx <= m_nb_pts-1; pt_idx++)
	{
		addPoint(vec2(0,0));
	}
}

void myGLCircle::setPoints()
{
	//resize the vector
    if(m_nb_pts > getSize()) //possibly an error...
	{
		int nb_pointToRemove = getSize() - m_nb_pts;
		for(int pt_toBeRemoved_idx = 0; pt_toBeRemoved_idx<= nb_pointToRemove-1; pt_toBeRemoved_idx++)
		{
			removePoint(pt_toBeRemoved_idx);
		}
	}
	if(m_nb_pts > getSize())
	{
		int nb_pointToRemove = getSize() - m_nb_pts;
		for(int pt_toBeRemoved_idx = 0; pt_toBeRemoved_idx<= nb_pointToRemove-1; pt_toBeRemoved_idx++)
		{
			addPoint(vec2(0,0));
		}
	}

//	change pts
	float angle = 0;
	float d_angle = d_angle = 2.0f*3.14f/m_nb_pts;
	for(int pt_idx = 0; pt_idx <= m_nb_pts-1; pt_idx++)
	{
		setPoint(pt_idx, m_center + vec2(m_radius*cos(angle),
										 m_radius*sin(angle)));
		angle += d_angle;
	}
}

void myGLCircle::setCenter(glm::vec2 center)
{
	m_center = center;
	setPoints();
}
void myGLCircle::setRadius(float radius)
{
	m_radius = radius;
	setPoints();
}
void myGLCircle::setNbPoints(int nb_points)
{
	m_nb_pts = nb_points;
	setPoints();
}

glm::vec2 myGLCircle::getCenter()
{
	return m_center;
}
float myGLCircle::getRadius()
{
	return m_radius;
}
int myGLCircle::getNbPoints()
{
	return m_nb_pts;
}

bool myGLCircle::isInside(glm::vec2 R)
{
	if(   R.x < m_center.x-m_radius || R.x > m_center.x+m_radius
	   || R.y < m_center.y-m_radius || R.y > m_center.y+m_radius) return 0; //->

	if( length(R - m_center) > m_radius) return 0; //->
	else return 1; //->
}

void myGLCircle::translate(glm::vec2 dR)
{
	m_center += dR;
	setPoints();
}



/**************************
 *
 *     class myGLPolygon
 *
 * ************************/

myGLPolygon::myGLPolygon(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :

	myGLLoopedGeometry(parent_gMultiVector)

{

}

void myGLPolygon::addPoint(glm::vec2 R)
{
	myGLGeometry::addPoint(R);
}

void myGLPolygon::addPoints(vector<glm::vec2> R_v)
{
	myGLGeometry::addPoints(R_v);
}

void myGLPolygon::removePoint(unsigned int point_idx)
{
	myGLGeometry::removePoint(point_idx);
}

void myGLPolygon::clear()
{
	myGLGeometry::clear();
}

void myGLPolygon::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	myGLGeometry::setPoint(pt_idx, R);
}

glm::vec2 myGLPolygon::getPoint(unsigned int point_idx)
{
	return myGLGeometry::getPoint(point_idx);
}

unsigned int myGLPolygon::getSize()
{
	return myGLGeometry::getSize();
}

void myGLPolygon::translate(glm::vec2 dR)
{
	myGLGeometry::translate(dR);
}

void myGLPolygon::scale(glm::vec2 center, glm::vec2 ratio)
{
	myGLGeometry::scale(center, ratio);
}


/**************************
 *
 *	 class myGLMultiLine
 *
 * ************************/

myGLMultiLineGeometry::myGLMultiLineGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector) :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT, myGLObject::GL_MULTILINE_GEOMETRY_TYPE),
	myGLGeometry(parent_gMultiVector)

{
	string vs_src;
	vs_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"uniform vec4 color_in;\n"
				"uniform mat4 zoom_matrix;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = zoom_matrix*temp;\n"
				"	color = color_in;\n"
				"}\n";

	string fs_src;
	fs_src +=	"#version 150 core\n\n"
				""
				"in vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=color;\n"
				"}\n";


	gstd::gProgram pgm("normal_rendering");

		gstd::gShader vs(gstd::VERTEX_SHADER);
			vs.setSource(vs_src, 0);
			vs.compile();
//			vs.coutEntryPoints_v();
		gstd::gShader fs(gstd::FRAGMENT_SHADER);
			fs.setSource(fs_src, 0);
			fs.compile();

	pgm.addShader(vs);
	pgm.addShader(fs);
	pgm.linkShaders();

	myGLObject::addProgram(pgm);

	setRenderingMode(LINE_STRIP_RENDERING);
}

glm::vec2 myGLMultiLineGeometry::getEdge(unsigned int edge_idx)
{
	if(edge_idx >= getSize()-1)
	{
		//error
		return vec2(-1,-1);
	}
	else
	{
		vec2 edge = getPoint(edge_idx+1) - getPoint(edge_idx);
		return edge;
	}
}


void myGLMultiLineGeometry::render(myGLObject* renderingTarget)
{
//	if(renderingTarget->isRenderingTargetObject() == false) return; //->

	vec2 S = renderingTarget->size();
	mat4 zoom_matrix;
	switch(m_space)
	{
		case WORLD_SPACE : zoom_matrix = renderingTarget->getWorldToExtendedHomMatrix();
		break;

		case HOMOGENEOUS_SCREEN_SPACE : zoom_matrix = mat4(1.0);
		break;

		case GL_SCREEN_SPACE : zoom_matrix = mat4(2/S.x, 0, 0, -1,
												  0, 2/S.y, 0, -1,
												  0, 0, 0, 0,
												  0, 0, 0, 1);
		break;

		case SCREEN_SPACE : zoom_matrix = mat4(+2/S.x, 0, 0, -1,
											   0, -2/S.y, 0, +1,
											   0, 0, 0, 0,
											   0, 0, 0, 1);
		break;
	}

	switch(m_rendering_mode)
	{
		case LINE_STRIP_RENDERING :
		{
			int nb_points = getSize();
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R", getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glDrawArrays(GL_LINE_STRIP, 0, getSize());
			pgm.useProgram(0);

		}
		break;

		case POINT_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R", getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glPointSize(7.0);
			glDrawArrays(GL_POINTS, 0, getSize());

			pgm.useProgram(0);
		}
		break;
		case LINE_AND_POINT_RENDERING :
		{
			gstd::gProgram& pgm = getProgram(0);
			pgm.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm,"R", getR());
			gstd::connectUniform(pgm, "color_in", &(getRenderingColor()), 1);
			gstd::connectUniform(pgm, "zoom_matrix", &zoom_matrix, 1);

			glDrawArrays(GL_LINE_STRIP, 0, getSize());
			glPointSize(7.0);
			glDrawArrays(GL_POINTS, 0, getSize());

			pgm.useProgram(0);
		}
		break;
	}

}

void myGLMultiLineGeometry::addPoint(glm::vec2 R)
{
	myGLGeometry::addPoint(R);
}

void myGLMultiLineGeometry::addPoints(vector<glm::vec2> R_v)
{
	myGLGeometry::addPoints(R_v);
}

void myGLMultiLineGeometry::removePoint(unsigned int point_idx)
{
	myGLGeometry::removePoint(point_idx);
}

void myGLMultiLineGeometry::clear()
{
	myGLGeometry::clear();
}

void myGLMultiLineGeometry::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	myGLGeometry::setPoint(pt_idx, R);
}

glm::vec2 myGLMultiLineGeometry::getPoint(unsigned int point_idx)
{
	return myGLGeometry::getPoint(point_idx);
}

unsigned int myGLMultiLineGeometry::getSize()
{
	return myGLGeometry::getSize();
}

void myGLMultiLineGeometry::translate(glm::vec2 dR)
{
	myGLGeometry::translate(dR);
}

void myGLMultiLineGeometry::scale(glm::vec2 center, glm::vec2 ratio)
{
	myGLGeometry::scale(center, ratio);
}

