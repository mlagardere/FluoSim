
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

#ifndef MYGLGEOMTRICOBJECT_H
#define MYGLGEOMTRICOBJECT_H

#include "gpuTools_library_global.h"

#include "gl/gVector.h"
#include "gl/gMultiVector.h"
#include "gl/gProgram.h"
#include "gl/gToolBox.h"

#include "glm.hpp"

#include "glGUI/glObjects/myGLObject.h"
#include "geometry/myGeomtricObject.h"

#include "toolbox_library_global.h"

/**************************
 *
 *	    class myGLGeometry
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myGLGeometry
{
public :

	myGLGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);
	gstd::gSubVector<glm::vec2>& getgVector();
	int getSubVectorId();


//	void updateCoords();

protected :

	void addPoint(glm::vec2 R);
	void addPoints(std::vector<glm::vec2> Rs_v);
	void removePoint(unsigned int point_idx);
	void clear();

	void setPoint(unsigned int pt_idx, glm::vec2 R);
	glm::vec2 getPoint(unsigned int point_idx);
	unsigned int getSize();
	gstd::gSubVector<glm::vec2>& getR();

	virtual void translate(glm::vec2 dR);
	void scale(glm::vec2 center, glm::vec2 ratio);

private :

	gstd::gSubVector<glm::vec2> m_R;
};



/**************************
 *
 *	 class myGLLoopedGeometry
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myGLLoopedGeometry : public myGLGeometry, public myGLObject
{
public :

	myGLLoopedGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);
	glm::vec2 getEdge(unsigned int edge_idx);

	virtual bool isInside(glm::vec2 R);
	virtual void render(myGLObject* renderingTarget);



private :
};


/**************************
 *
 *     class myGLRectangle
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myGLRectangle : public myGLLoopedGeometry
{

public :

	myGLRectangle(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);

	void setPoints(); //from quad corners to vertices

	void setBottomLeft(glm::vec2 bottom_left); //size changes
	void setTopRight(glm::vec2 top_right); //size changes
	void setTopLeft(glm::vec2 bottom_left);
	void setBottomRight(glm::vec2 bottom_right);

	void setDiag(glm::vec2 pt1, glm::vec2 pt2);
	void setWidth(float width); //right edge moves
	void setHeight(float height); //bottom edge moves

	void setHasMinimumSize(bool hasMinimum);
	void setMinimumSize(glm::vec2 min_size);

	float getWidth();
	float getHeight();
	glm::vec2 getSize();

	glm::vec2 getBottomLeft();
	glm::vec2 getTopLeft();
	glm::vec2 getTopRight();
	glm::vec2 getBottomRight();

	virtual bool isInside(glm::vec2 R); //reimplement isInside of myLoopedGeometry
	virtual void translate(glm::vec2 dR);

private :

	glm::vec2 m_bottomLeft;
	glm::vec2 m_size;

	bool m_hasMinimumSize;
	glm::vec2 m_minimum_size;


};

/**************************
 *
 *     class myGLCircle
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myGLCircle : public myGLLoopedGeometry
{
public :

	myGLCircle(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);

	void setPoints(); //from radius, n_points -> vertices

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
 *     class myGLPolygon
 *
 * ************************/



class TOOLBOXSHARED_EXPORT myGLPolygon : public myGLLoopedGeometry
{
public :

	myGLPolygon(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);

	void addPoint(glm::vec2 R);
	void addPoints(std::vector<glm::vec2> R_v);
	void removePoint(unsigned int point_idx);
	void clear();

	void setPoint(unsigned int pt_idx, glm::vec2 R);
	glm::vec2 getPoint(unsigned int point_idx);
	unsigned int getSize();

	virtual void translate(glm::vec2 dR);
	virtual void scale(glm::vec2 center, glm::vec2 ratio);


private :


};

/**************************
 *
 *	 class myGLMultiLine
 *
 * ************************/


class TOOLBOXSHARED_EXPORT myGLMultiLineGeometry : public myGLGeometry, public myGLObject
{
public :

	myGLMultiLineGeometry(gstd::gMultiVector<glm::vec2>& parent_gMultiVector);
	glm::vec2 getEdge(unsigned int edge_idx);
	virtual void render(myGLObject* renderingTarget);

	void addPoint(glm::vec2 R);
	void addPoints(vector<glm::vec2> R_v);
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
