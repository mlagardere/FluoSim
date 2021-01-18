
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

#ifndef MYGLOBJECT_H
#define MYGLOBJECT_H

#include "toolbox_library_global.h"

#include "glm.hpp"
#include "QObject"
#include "QTimer"
#include "gl/gProgram.h"
#include "gl/gTexture.h"

typedef unsigned char uint8;

enum MOUSE_BUTTON_STATE {UNCLICKED, CLICKED};
struct mouseState
{
	MOUSE_BUTTON_STATE	left_button;
	MOUSE_BUTTON_STATE	right_button;
	MOUSE_BUTTON_STATE  middle_button;

	glm::vec2 last_pos;
	glm::vec2 last_translationVector;
	glm::vec3 last_scalingValues;
};




/********************************
 *
 *		 myEditableObject
 *
 * ******************************/

class TOOLBOXSHARED_EXPORT myEditableObject
{
public :

	myEditableObject();

	bool isEdited();
	bool isEditingAPoint();
	virtual bool isInsideEditionContour(glm::vec2 r);
	virtual bool isNextToAnEditionContourVertex(glm::vec2 r, float radius_inPxl, int& vertex_idx);
	virtual float getEditionContourSurface();

	virtual void edit(bool edit_state);

protected :

	bool m_isEdited;
	bool m_isEditingPoint;
	int m_editingPointIdx;
	glm::vec4 m_edition_color;
};


/********************************
 *
 *		 mySelectionObject
 *
 * ******************************/

class TOOLBOXSHARED_EXPORT mySelectionableObject
{
public :

	mySelectionableObject();

	bool isSelected();
	bool isResizing();
	virtual bool isInsideSelectionContour(glm::vec2 r);
	virtual bool isNextToASelectionContourVertex(glm::vec2 r, float radius_inPxl, int& vertex_idx);
	virtual float getSelectionContourSurface();

	//to add functionality
	virtual void select(bool select_state);

	//to communicate with derived classes
	virtual void onSelection_GLObject(bool selection);

protected  :

	bool m_isSelected;
	bool m_isResizing;
		int m_selectedVertex;
	glm::vec4 m_selection_color;

	mouseState m_lastSelectionEventState;

};


/********************************
 *
 *		 myDrawableObject
 *
 * ******************************/

class TOOLBOXSHARED_EXPORT myDrawableObject
{

public :

	enum DRAWING_STATE {WAITING_FOR_DRAWING, STARTING_DRAWING, CURRENTLY_DRAWING, DRAWING_ABORT, DRAWING_TO_BE_FINISHED, DRAWING_FINISHED};

	myDrawableObject();

	bool isDrawn();
	bool isStartingDrawing();
	bool isDrawing();
	bool isWaitingForDrawing();
	bool isAborted();

	virtual void draw(bool drawing_state);

	//intra object communication methods
	virtual void objectHasBeenDrawn();

protected  :

	mouseState m_lastEventState;
	DRAWING_STATE m_drawingState;

};


/****************************************
 *
 *		 myRenderingTargetObject
 *
 * **************************************/


class rawImage
{
public :

	rawImage();


	rawImage(gstd::gTexture text)
	{
		if(text.isLoaded() == false) return;

		m_image_format = text.getTextureFormat();
		m_size = text.getSize();
		text.getTextureData(m_data);
	}

	bool isEmpty()
	{
		return m_data.size();
	}

public :

	glm::vec2 m_size;
	std::vector<uint8> m_data;
    GLint m_image_format;
};


class myGLObject;

class TOOLBOXSHARED_EXPORT myRenderingTargetObject
{

public :

	myRenderingTargetObject();

	void clearColorBuffer();
	void setClearColor(glm::vec4 color);
	glm::vec4 getClearColor();

	void setZoomMatrix(glm::mat4 new_zoom_matrix);
	void setScalingValues(glm::vec3 alpha);
	void setTranslationVector(glm::vec3 t); //t operates in the homogeneous space

	virtual const glm::mat4& getZoomMatrix();
	glm::mat4 getWorldToExtendedHomMatrix();
	glm::vec3 getTranslationVector();
	glm::vec3 getScalingValues();
	void multiplyScalingMatrix(float alpha);
	void increaseScalingMatrix(float alpha);

	void setIsIsometric(bool isIsometric);
	virtual void setOnFocusObject(myGLObject* obj);
	virtual glm::vec2 size(); //to be reimplemented in each derived class!!!!
	void fitQuadToScreen(glm::vec2 bottomLeft, glm::vec2 topRight);

	virtual void captureScreenBackBuffer(gstd::gTexture& text);

	void fitQuadToQuad(glm::vec2 bottomLeft_W, glm::vec2 topRight_W,
					   glm::vec2 bottomLeft_hom, glm::vec2 topRight_hom); //loose fit


	//two screen systems of coordinates : Qt(lowerLeft = (0,Sy)) and GL(lowerLeft = (0,0))
	// their y-axis are inverted and coordinate of origin is not the same

	//->
	glm::vec2 fromWorldToHomCoordinate(glm::vec2 Rw);
	glm::vec2 fromWorldToExtendedHomCoordinate(glm::vec2 Rw);
	glm::vec2 fromWorldToScreenCoordinate(glm::vec2 Rw);
	glm::vec2 fromWorldToGLScreenCoordinate(glm::vec2 Rw);

	glm::vec2 fromHomToExtendedHomCoordinate(glm::vec2 r);
	glm::vec2 fromHomToScreenCoordinate(glm::vec2 r);
	glm::vec2 fromHomToGLScreenCoordinate(glm::vec2 r);

	glm::vec2 fromExtendedHomToScreenCoordinate(glm::vec2 r);
	glm::vec2 fromExtendedHomToGLScreenCoordinate(glm::vec2 r);

	//<-
	glm::vec2 fromScrnToExtendedHomCoordinate(glm::vec2 R);
	glm::vec2 fromScrnToHomCoordinate(glm::vec2 R);
	glm::vec2 fromScrnToWorldCoordinate(glm::vec2 R);

	glm::vec2 fromGLScrnToExtendedHomCoordinate(glm::vec2 R);
	glm::vec2 fromGLScrnToHomCoordinate(glm::vec2 R);
	glm::vec2 fromGLScrnToWorldCoordinate(glm::vec2 R);

	glm::vec2 fromExtendedHomToHomCoordinate(glm::vec2 r);
	glm::vec2 fromExtendedHomToWorldCoordinate(glm::vec2 r);

	glm::vec2 fromHomToWorldCoordinate(glm::vec2 r);


	//matrices
	glm::mat2 fromScrnToExtendedHomMatrix();
	glm::vec2 fromScrnToExtendedHomVector();

	glm::mat2 fromGLScrnToExtendedHomMatrix();
	glm::vec2 fromGLScrnToExtendedHomVector();


//protected :

	glm::mat4 m_zoom_matrix;
//	glm::mat4 m_projection_matrix;
	glm::vec4 m_clearColor;
	bool m_isIsometric;


private :

	glm::mat2 fromHomToExtendedHomMatrix();
	glm::mat2 fromExtendedHomToHomMatrix();


};




/********************************
 *
 *	    myRenderableObject
 *
 * ******************************/

class myGLObject;

class TOOLBOXSHARED_EXPORT myRenderableObject : public QObject
{
Q_OBJECT

public :

	enum RENDERING_MODE {LINE_STRIP_RENDERING, LINE_LOOPED_RENDERING, POINT_RENDERING, LINE_AND_POINT_RENDERING,
						 IMAGE_GS_8_16_24_RENDERING, IMAGE_GS_32_RENDERING,
						 IMAGE_RGBA_RENDERING, IMAGE_GS_R16_RENDERING};
	enum OBJECT_SPACE {WORLD_SPACE, HOMOGENEOUS_SCREEN_SPACE, GL_SCREEN_SPACE, SCREEN_SPACE};

	myRenderableObject();
	~myRenderableObject();


//	virtual void render(myGLWidget* screen); //pure virtual
	virtual void render(myGLObject* renderingTarget); //pure virtual

	void setRenderState(bool render_state); //isRendered <- render_state;
	void setBlinking(bool blinking);
	void setPeriod(float period_inSeconds);
	virtual void setSpace(OBJECT_SPACE space);

	OBJECT_SPACE getSpace();

	void hide(); //isRendered <- 0;
	void show(); //isRendered <- 1;

	bool isVisible(); // = !isBlinked && isRendered
	bool isBlinking();
	bool isBlinked();

	void setRenderingMode(RENDERING_MODE mode);

	void addProgram(gstd::gProgram& pg);
	void removeProgram(long program_idx);
	gstd::gProgram& getProgram(long pg_idx);

	const glm::vec4& getRenderingColor();
	void setRenderingColor(glm::vec4 color);


private slots :

	void blink();

private :

	bool m_isRendered;
	bool m_isBlinking;
		bool m_isBlinked;
		float m_blinkingPeriod; //in seconds
		QTimer m_blinking_timer;

protected :

	RENDERING_MODE m_rendering_mode;
	OBJECT_SPACE m_space;

	glm::vec4 m_rendering_color;
	glm::mat4 m_modelWorldMatrix;
	std::vector<gstd::gProgram> m_gPrograms_v;
};


/********************************
 *
 *	    myGLObject
 *
 * ******************************/

//no need to make virtual the desctructors of base classes : deletion should be used at myGLObject level...
//but need to be propagated down the object (trough its base classes, which is done automatically)


class TOOLBOXSHARED_EXPORT myGLObject : public myRenderableObject,
										  public myEditableObject,
										  public mySelectionableObject,
										  public myDrawableObject,
										  public myRenderingTargetObject
{
Q_OBJECT

public :

	static const int IS_EDITABLE_OBJECT = 1<<0;
	static const int IS_SELECTIONABLE_OBJECT = 1<<1;
	static const int IS_RENDERABLE_OBJECT = 1<<2;
	static const int IS_DRAWABLE_OBJECT = 1<<3;
	static const int IS_RENDERING_TARGET_OBJECT = 1<<4;
	static const int BECAME_WIDGET_OBJECT = 1<<5;


	enum OBJECT_ABILITIES_BITS {EDITABLE_ABILITY_BIT = 1,
								SELECTIONABLE_ABILITY_BIT  = 2,
								RENDERABLE_ABILITY_BIT  = 4,
								DRAWABLE_ABILITY_BIT  = 8,
								RENDERING_TARGET_ABILITY_BIT  = 16,
								BECAME_WIDGET_ABILITY_BIT  = 32};

	enum OBJECT_TYPE {UNDEFINED_TYPE,
					  GLWIDGET_TYPE,
					  GL_LOOPED_GEOMETRY_TYPE, GL_MULTILINE_GEOMETRY_TYPE, GL_IMAGE_TYPE,
					  GLSCREEN_LOOPED_GEOMETRY_TYPE, GLSCREEN_MULTILINE_GEOMETRY_TYPE, GLSCREEN_IMAGE_TYPE,
					  GLSCREEN_PARTICLE_SYSTEM_TYPE};

	myGLObject(int OBJECT_ABILITIES, OBJECT_TYPE type);
    /*virtual*/ ~myGLObject(); //no need : inherits QObject which has a virtual destructor : depends on the object type being deleted !

	bool isRenderableObject();
	bool isEditableObject();
	bool isSelectionableObject();
	bool isDrawableObject();
	bool isRenderingTargetObject();

	bool becameWidgetObject();

	virtual void treatEvent(QEvent* event);

	OBJECT_TYPE getObjectType();

signals :

	void hasBeenDrawn_sig();

public :

	virtual void objectHasBeenDrawn();

protected :

	int m_OBJECT_ABILITIES;
	OBJECT_TYPE m_objectType;
};






#endif // MYGLSCREENOBJECT_H
