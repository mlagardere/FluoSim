
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

#ifndef MYGEOMTRICOBJECT_H
#define MYGEOMTRICOBJECT_H

#include "toolbox_library_global.h"

#include "glm.hpp"
#include "containers/myMultiVector.h"


/**************************
 *
 *	    class myGeometry
 *
 * ************************/


enum GEOMETRY{RECTANGLE, CIRCLE, POLYGON, MULTILINE};


class TOOLBOXSHARED_EXPORT myGeometry
{
public :

	myGeometry(myMultiVector<glm::vec2>& parent_multiVector);
	bool isNextToAVertex(glm::vec2 R, float dR, int& vertex_idx);
	int getSubVectorId();

	std::vector<glm::vec2> getR();

protected :

	void addPoint(glm::vec2 R);
	void addPoints(std::vector<glm::vec2> R_v);
	void removePoint(unsigned int point_idx);
	void clear();

	void setPoint(unsigned int pt_idx, glm::vec2 R);
	glm::vec2 getPoint(unsigned int point_idx);
	unsigned int getSize();
	virtual void translate(glm::vec2 dR);
	void scale(glm::vec2 center, glm::vec2 ratio);

private :

	mySubVector<glm::vec2> m_R;
};



/**************************
 *
 *	 class myLoopedGeometry
 *
 * ************************/

class TOOLBOXSHARED_EXPORT myLoopedGeometry : public myGeometry
{
public :

	myLoopedGeometry(myMultiVector<glm::vec2>& parent_multiVector);
	glm::vec2 getEdge(unsigned int edge_idx);
	virtual bool isInside(glm::vec2 R);
	virtual float getSurface();

private :
};


/**************************
 *
 *     class myRectangle  
 * 
 * ************************/


class TOOLBOXSHARED_EXPORT myRectangle : public myLoopedGeometry
{
public :

	enum VERTEX {NONE, BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT};

	myRectangle(myMultiVector<glm::vec2>& parent_multiVector);

	void setPoints();

	void setBottomLeft(glm::vec2 bottom_left); //size changes
	void setTopRight(glm::vec2 top_right); //size changes
	void setTopLeft(glm::vec2 bottom_left);
	void setBottomRight(glm::vec2 bottom_right);

	void setDiag(glm::vec2 pt1, glm::vec2 pt2);
	void setWidth(float width); //right edge moves
	void setHeight(float height); //bottom edge moves

	void setHasMinimumSize(bool hasMinimumSize);
	void setMinimumSize(glm::vec2 min_size);

	float getWidth();
	float getHeight();
	glm::vec2 getSize();

	glm::vec2 getBottomLeft();
	glm::vec2 getTopLeft();
	glm::vec2 getTopRight();
	glm::vec2 getBottomRight();


	VERTEX getVertexType(int vertex_idx);
	VERTEX getOppositeVertexType(VERTEX vertex);
	glm::vec2 getVertexCoord(VERTEX vertex);

	bool isBottomLeft(int pt_idx);
	bool isTopLeft(int pt_idx);
	bool isTopRight(int pt_idx);
	bool isBottomRight(int pt_idx);

	virtual void translate(glm::vec2 dR);
	virtual bool isInside(glm::vec2 R); //reimplement isInside of myLoopedGeometry
	virtual float getSurface();


private :

	glm::vec2 m_bottomLeft;
	glm::vec2 m_size;

	bool m_hasMinimumSize;
	glm::vec2 m_minimum_size;


};

/**************************
 *
 *     class myCircle
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myCircle : public myLoopedGeometry
{
public :

	myCircle(myMultiVector<glm::vec2>& parent_multiVector);

	void setPoints();

	void setCenter(glm::vec2 center);
	void setRadius(float radius);
	void setNbPoints(int nb_points);

	glm::vec2 getCenter();
	float getRadius();
	int getNbPoints();

	virtual bool isInside(glm::vec2 R);
	virtual void translate(glm::vec2 dR);

private :

	int m_nb_pts;
	glm::vec2 m_center;
	float m_radius;


};

/**************************
 *
 *     class myPolygon
 *
 * ************************/



class TOOLBOXSHARED_EXPORT myPolygon : public myLoopedGeometry
{
public :

	myPolygon(myMultiVector<glm::vec2>& parent_multiVector);

	void addPoint(glm::vec2 R);
	void addPoints(std::vector<glm::vec2> R_v);
	void removePoint(unsigned int point_idx);
	void clear();

	void setPoint(unsigned int pt_idx, glm::vec2 R);
	glm::vec2 getPoint(unsigned int point_idx);
	unsigned int getSize();

	int getBottomPointIdx();
	int getTopPointIdx();
	int getLeftPointIdx();
	int getRightPointIdx();

	virtual void translate(glm::vec2 dR);
	void scale(glm::vec2 center, glm::vec2 ratio);


private :


};

/**************************
 *
 *	 class myMultiLineGeometry
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myMultiLineGeometry : public myGeometry
{
public :

	myMultiLineGeometry(myMultiVector<glm::vec2>& parent_multiVector);
	glm::vec2 getEdge(unsigned int edge_idx);

	void addPoint(glm::vec2 R);
	void addPoints(std::vector<glm::vec2> R_v);
	void removePoint(unsigned int point_idx);
	void clear();

	void setPoint(unsigned int pt_idx, glm::vec2 R);
	glm::vec2 getPoint(unsigned int point_idx);
	unsigned int getSize();

	int getBottomPointIdx();
	int getTopPointIdx();
	int getLeftPointIdx();
	int getRightPointIdx();

	virtual void translate(glm::vec2 dR);
	void scale(glm::vec2 center, glm::vec2 ratio);

private :
};



#endif // MYGEOMTRICOBJECT_H
