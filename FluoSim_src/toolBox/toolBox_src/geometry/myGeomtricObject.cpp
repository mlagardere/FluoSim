
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



#include "myGeomtricObject.h"

using namespace glm;
using namespace std;


/************************************
 *
 *            myGeometry
 *
 ***********************************/
myGeometry::myGeometry(myMultiVector<vec2>& parent_multiVector) : m_R(parent_multiVector.addSubVector())
{
}

bool myGeometry::isNextToAVertex(glm::vec2 R, float dR, int& vertex_idx)
{

	auto closest_vertex = m_R.end();
		vertex_idx = -1;
	int temp_vertex_idx = 0;

	auto vertex = m_R.begin();
	while(vertex != m_R.end())
	{
		if(length(*vertex - R) <= dR)
		{
			if(closest_vertex == m_R.end())
			{
				closest_vertex = vertex;
				vertex_idx = temp_vertex_idx;
			}
			else
			{
				if(length(*vertex - R) <= length(*closest_vertex - R))
				{
					closest_vertex = vertex;
					vertex_idx = temp_vertex_idx;
				}
			}
		}
		vertex++;
		temp_vertex_idx ++;
	}

	if(closest_vertex == m_R.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int myGeometry::getSubVectorId()
{
	return m_R.getParam()->m_id;
}

std::vector<glm::vec2> myGeometry::getR()
{
	vector<vec2> R_v;
	R_v.insert(R_v.end(), m_R.begin(), m_R.end());

	return R_v;
}


void myGeometry::addPoint(glm::vec2 R)
{
	m_R.push_back(R);
}

void myGeometry::addPoints(vector<glm::vec2> R_v)
{
//	m_R.insert(m_R.end(), R_v.begin(), R_v.end());
	unsigned int end_pos = m_R.size();
	m_R.insert(end_pos, R_v);
}

void myGeometry::removePoint(unsigned int point_idx)
{
//	m_R.erase(m_R.begin() + point_idx);
	m_R.erase(point_idx);
}

void myGeometry::clear()
{
	m_R.clear();
}

void myGeometry::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	m_R[pt_idx] = R;

}

glm::vec2 myGeometry::getPoint(unsigned int point_idx)
{
	return m_R[point_idx];

}

unsigned int myGeometry::getSize()
{
	return m_R.size();
}

void myGeometry::translate(glm::vec2 dR)
{
	auto R = m_R.begin();
	while(R != m_R.end())
	{
		vec2 new_R = *R;
			 new_R += dR;
		*R = new_R;
		R++;
	}
}

void myGeometry::scale(glm::vec2 center, vec2 ratio)
{
	auto R = m_R.begin();
	mat2 scaling(ratio.x,0,
				 0,ratio.y);

	while(R != m_R.end())
	{
		*R = center + scaling*(*R - center);
		R++;
	}
}



/***********************************
 *
 *		  myLoopedGeometry
 *
 * *********************************/


myLoopedGeometry::myLoopedGeometry(myMultiVector<vec2>& parent_multiVector) : myGeometry(parent_multiVector)
{
}

glm::vec2 myLoopedGeometry::getEdge(unsigned int edge_idx)
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

bool myLoopedGeometry::isInside(vec2 R)
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
            in=0; //even number of intersections -> outside
        }
        else
        {
            in=1;
        }

    return in;
}

float myLoopedGeometry::getSurface()
{



}


/*****************************
 *
 *		   myRectangle
 *
 * ***************************/



myRectangle::myRectangle(myMultiVector<vec2>& parent_multiVector) : myLoopedGeometry(parent_multiVector)
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

void myRectangle::setPoints()
{
	setPoint(0, m_bottomLeft);
	setPoint(1, m_bottomLeft + vec2(0, m_size.y));
	setPoint(2, m_bottomLeft + m_size);
	setPoint(3, m_bottomLeft + vec2(m_size.x, 0));
}

void myRectangle::setTopRight(glm::vec2 top_right)
{
	setDiag(m_bottomLeft, top_right);
	setPoints();
}

void myRectangle::setBottomLeft(glm::vec2 bottom_left)
{
	setDiag(bottom_left, m_bottomLeft + m_size);
	setPoints();
}

void myRectangle::setTopLeft(glm::vec2 top_left)
{
	setDiag(vec2(top_left.x, m_bottomLeft.y),
			vec2(getTopRight().x, top_left.y));
	setPoints();
}

void myRectangle::setBottomRight(glm::vec2 bottom_right)
{
	setDiag(vec2(m_bottomLeft.x, bottom_right.y),
			vec2(bottom_right.x, getTopLeft().y));
	setPoints();
}


void myRectangle::setDiag(glm::vec2 pt1, glm::vec2 pt2)
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

void myRectangle::setWidth(float width)
{
	if(m_hasMinimumSize && m_minimum_size.x > width) m_size.x = m_minimum_size.x;
	else m_size.x = width;

	setPoints();
}


void myRectangle::setHeight(float height)
{
	if(m_hasMinimumSize && m_minimum_size.y > height) m_size.y = m_minimum_size.y;
	else m_size.y = height;

	setPoints();
}

void myRectangle::setHasMinimumSize(bool hasMinimumSize)
{
	m_hasMinimumSize = hasMinimumSize;
}

void myRectangle::setMinimumSize(glm::vec2 min_size)
{
	m_minimum_size = min_size;
}

float myRectangle::getWidth()
{
	return m_size.x;
}


float myRectangle::getHeight()
{
	return m_size.y;
}

glm::vec2 myRectangle::getSize()
{
	return m_size;
}

glm::vec2 myRectangle::getBottomLeft()
{
	return m_bottomLeft;
}

glm::vec2 myRectangle::getTopLeft()
{
	return m_bottomLeft + vec2(0,m_size.y);
}

glm::vec2 myRectangle::getTopRight()
{
	return m_bottomLeft + m_size;
}

glm::vec2 myRectangle::getBottomRight()
{
	return m_bottomLeft + vec2(m_size.x,0);
}

myRectangle::VERTEX myRectangle::getVertexType(int vertex_idx)
{
	if(isBottomLeft(vertex_idx)) return BOTTOM_LEFT;
	if(isBottomRight(vertex_idx)) return BOTTOM_RIGHT;

	if(isTopLeft(vertex_idx)) return TOP_LEFT;
	if(isTopRight(vertex_idx)) return TOP_RIGHT;

	return NONE;
}

myRectangle::VERTEX myRectangle::getOppositeVertexType(VERTEX vertex)
{
	switch(vertex)
	{
		case BOTTOM_LEFT : return TOP_RIGHT;
			break;
		case BOTTOM_RIGHT : return TOP_LEFT;
			break;
		case TOP_LEFT : return BOTTOM_RIGHT;
			break;
		case TOP_RIGHT : return BOTTOM_LEFT;
			break;
	}
}

glm::vec2 myRectangle::getVertexCoord(VERTEX vertex)
{
	switch(vertex)
	{
		case BOTTOM_LEFT : return getBottomLeft();
			break;
		case BOTTOM_RIGHT : return getBottomRight();
			break;
		case TOP_LEFT : return getTopLeft();
			break;
		case TOP_RIGHT : return getTopRight();
			break;
	}
}

/*virtual*/ bool myRectangle::isInside(glm::vec2 R)
{
	return (R.x > m_bottomLeft.x &&
			R.y > m_bottomLeft.y &&
			(R.x < m_bottomLeft.x + m_size.x) &&
			(R.y < m_bottomLeft.y + m_size.y));
}

float myRectangle::getSurface()
{
	return m_size.x*m_size.y;
}

void myRectangle::translate(glm::vec2 dR)
{
	m_bottomLeft = m_bottomLeft + dR;
	setPoints();
}


bool myRectangle::isBottomLeft(int pt_idx)
{
	return pt_idx == 0;
}

bool myRectangle::isTopLeft(int pt_idx)
{
	return pt_idx == 1;
}

bool myRectangle::isTopRight(int pt_idx)
{
	return pt_idx == 2;
}

bool myRectangle::isBottomRight(int pt_idx)
{
	return pt_idx == 3;
}



/******************************
 *
 *           myCircle
 *
 * ****************************/



myCircle::myCircle(myMultiVector<vec2>&  parent_multiVector) : myLoopedGeometry(parent_multiVector)
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

void myCircle::setPoints()
{
	//resize the vector
	if(m_nb_pts > getSize())
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

void myCircle::setCenter(glm::vec2 center)
{
	m_center = center;
	setPoints();
}
void myCircle::setRadius(float radius)
{
	m_radius = radius;
	setPoints();
}
void myCircle::setNbPoints(int nb_points)
{
	m_nb_pts = nb_points;
	setPoints();
}

glm::vec2 myCircle::getCenter()
{
	return m_center;
}
float myCircle::getRadius()
{
	return m_radius;
}
int myCircle::getNbPoints()
{
	return m_nb_pts;
}

bool myCircle::isInside(glm::vec2 R)
{
	if(   R.x < m_center.x-m_radius || R.x > m_center.x+m_radius
	   || R.y < m_center.y-m_radius || R.y > m_center.y+m_radius) return 0; //->

	if( length(R - m_center) > m_radius) return 0; //->
	else return 1; //->
}

void myCircle::translate(glm::vec2 dR)
{
	m_center += dR;
	setPoints();
}





/**************************
 *
 *     class myPolygon
 *
 * ************************/



myPolygon::myPolygon(myMultiVector<vec2>& parent_multiVector) : myLoopedGeometry(parent_multiVector)
{

}

void myPolygon::addPoint(glm::vec2 R)
{
	myGeometry::addPoint(R);
}

void myPolygon::addPoints(std::vector<glm::vec2> R_v)
{
	myGeometry::addPoints(R_v);
}


void myPolygon::removePoint(unsigned int point_idx)
{
	myGeometry::removePoint(point_idx);
}

void myPolygon::clear()
{
	myGeometry::clear();
}

void myPolygon::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	myGeometry::setPoint(pt_idx, R);
}

glm::vec2 myPolygon::getPoint(unsigned int point_idx)
{
	return myGeometry::getPoint(point_idx);
}

unsigned int myPolygon::getSize()
{
	return myGeometry::getSize();
}

int myPolygon::getLeftPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_bottomLeft_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_bottomLeft_idx).x > getPoint(pt_idx).x)
		{
			temp_bottomLeft_idx = pt_idx;
		}
	}

	return temp_bottomLeft_idx; //->
}

int myPolygon::getRightPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_topRight_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_topRight_idx).x < getPoint(pt_idx).x)
		{
			temp_topRight_idx = pt_idx;
		}
	}
	return temp_topRight_idx; //->
}

int myPolygon::getBottomPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_bottomLeft_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_bottomLeft_idx).y > getPoint(pt_idx).y)
		{
			temp_bottomLeft_idx = pt_idx;
		}
	}

	return temp_bottomLeft_idx; //->
}

int myPolygon::getTopPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_topRight_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_topRight_idx).y < getPoint(pt_idx).y)
		{
			temp_topRight_idx = pt_idx;
		}
	}
	return temp_topRight_idx; //->
}


void myPolygon::translate(glm::vec2 dR)
{
	myGeometry::translate(dR);
}

void myPolygon::scale(glm::vec2 center, glm::vec2 ratio)
{
	myGeometry::scale(center, ratio);
}



/**************************************
 *
 *		   myMultiLineGeometry
 *
 * ************************************/


myMultiLineGeometry::myMultiLineGeometry(myMultiVector<vec2>& parent_multiVector) : myGeometry(parent_multiVector)
{
}

glm::vec2 myMultiLineGeometry::getEdge(unsigned int edge_idx)
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



void myMultiLineGeometry::addPoint(glm::vec2 R)
{
	myGeometry::addPoint(R);
}

void myMultiLineGeometry::addPoints(std::vector<glm::vec2> R_v)
{
	myGeometry::addPoints(R_v);
}


void myMultiLineGeometry::removePoint(unsigned int point_idx)
{
	myGeometry::removePoint(point_idx);
}

void myMultiLineGeometry::clear()
{
	myGeometry::clear();
}

void myMultiLineGeometry::setPoint(unsigned int pt_idx, glm::vec2 R)
{
	myGeometry::setPoint(pt_idx, R);
}

glm::vec2 myMultiLineGeometry::getPoint(unsigned int point_idx)
{
	return myGeometry::getPoint(point_idx);
}

unsigned int myMultiLineGeometry::getSize()
{
	return myGeometry::getSize();
}

int myMultiLineGeometry::getLeftPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_bottomLeft_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_bottomLeft_idx).x > getPoint(pt_idx).x)
		{
			temp_bottomLeft_idx = pt_idx;
		}
	}

	return temp_bottomLeft_idx; //->
}

int myMultiLineGeometry::getRightPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_topRight_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_topRight_idx).x < getPoint(pt_idx).x)
		{
			temp_topRight_idx = pt_idx;
		}
	}
	return temp_topRight_idx; //->
}

int myMultiLineGeometry::getBottomPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_bottomLeft_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_bottomLeft_idx).y > getPoint(pt_idx).y)
		{
			temp_bottomLeft_idx = pt_idx;
		}
	}

	return temp_bottomLeft_idx; //->
}

int myMultiLineGeometry::getTopPointIdx()
{
	int nb_point = getSize();
	if(nb_point <= 0) return -1; //->

	int temp_topRight_idx = 0;
	for(int pt_idx = 0; pt_idx <= nb_point-1; pt_idx++)
	{
		if(getPoint(temp_topRight_idx).y < getPoint(pt_idx).y)
		{
			temp_topRight_idx = pt_idx;
		}
	}
	return temp_topRight_idx; //->
}


void myMultiLineGeometry::translate(glm::vec2 dR)
{
	myGeometry::translate(dR);
}

void myMultiLineGeometry::scale(glm::vec2 center, glm::vec2 ratio)
{
	myGeometry::scale(center, ratio);
}




