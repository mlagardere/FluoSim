
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

#include "glGUI/glScreenObjects/myGLScreenImageObjects.h"

using namespace glm;



myGLScreenImage::myGLScreenImage(std::string file_dir,
								 gstd::gMultiVector<glm::vec2>& geom_gMV,
								 myMultiVector<glm::vec2>& contour_MV,
								 gstd::gMultiVector<glm::vec2>& contour_gMV,
								 myGLWidget* glWidget, bool toBeDrawn) :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT |
			   myGLObject::IS_DRAWABLE_OBJECT |
			   myGLObject::IS_SELECTIONABLE_OBJECT |
			   myGLObject::IS_EDITABLE_OBJECT |
			   myGLObject::IS_RENDERING_TARGET_OBJECT,
			   myGLObject::GLSCREEN_IMAGE_TYPE)

{
	m_glWidget = glWidget;
	m_gl_image = new myGLImage(file_dir, vec2(0,0), vec2(1,1), geom_gMV);


	m_gl_seletion_contour = new myGLRectangle(contour_gMV);
		m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_seletion_contour = new myRectangle(contour_MV);

	m_gl_edition_contour = new myGLRectangle(contour_gMV);
		m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_edition_contour = new myRectangle(contour_MV);

	if(toBeDrawn == true) m_drawingState = WAITING_FOR_DRAWING;
	else m_drawingState = DRAWING_FINISHED;
}

myGLScreenImage::myGLScreenImage(gstd::gMultiVector<glm::vec2>& geom_gMV,
								 myMultiVector<glm::vec2>& contour_MV,
								 gstd::gMultiVector<glm::vec2>& contour_gMV,
								 myGLWidget* glWidget, bool toBeDrawn) :


	myGLObject(myGLObject::IS_RENDERABLE_OBJECT |
			   myGLObject::IS_DRAWABLE_OBJECT |
			   myGLObject::IS_SELECTIONABLE_OBJECT |
			   myGLObject::IS_EDITABLE_OBJECT,
			   GLSCREEN_IMAGE_TYPE)

{
	m_glWidget = glWidget;
	m_gl_image = new myGLImage(vec2(0,0), vec2(1,1), geom_gMV);


	m_gl_seletion_contour = new myGLRectangle(contour_gMV);
		m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_seletion_contour = new myRectangle(contour_MV);

	m_gl_edition_contour = new myGLRectangle(contour_gMV);
		m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_edition_contour = new myRectangle(contour_MV);

	if(toBeDrawn == true) m_drawingState = WAITING_FOR_DRAWING;
	else m_drawingState = DRAWING_FINISHED;
}

myGLScreenImage::~myGLScreenImage()
{
	delete m_gl_seletion_contour;
	delete m_seletion_contour;

	delete m_gl_edition_contour;
	delete m_edition_contour;
}

void myGLScreenImage::setBottomLeft(glm::vec2 bottom_left_Rw)
{

	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	m_gl_image->getRect().setBottomLeft(bottom_left_Rw);
	gl_selection_rectangle->setBottomLeft(bottom_left_Rw);
	selection_rectangle->setBottomLeft(bottom_left_Rw);
	gl_edition_rectangle->setBottomLeft(bottom_left_Rw);
	edition_rectangle->setBottomLeft(bottom_left_Rw);
}

void myGLScreenImage::setSize(glm::vec2 size)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	vec2 bottomLeft = selection_rectangle->getBottomLeft();

	m_gl_image->getRect().setDiag(bottomLeft, bottomLeft + size);
	gl_selection_rectangle->setDiag(bottomLeft, bottomLeft + size);
	selection_rectangle->setDiag(bottomLeft, bottomLeft + size);
	gl_edition_rectangle->setDiag(bottomLeft, bottomLeft + size);
	edition_rectangle->setDiag(bottomLeft, bottomLeft + size);
}

void myGLScreenImage::setSpace(OBJECT_SPACE space)
{
	m_gl_image->setSpace(space);
	m_gl_edition_contour->setSpace(space);
	m_gl_seletion_contour->setSpace(space);
}


glm::vec2 myGLScreenImage::getResolution()
{
	return  m_gl_image->getTexture().getSize();
}

void myGLScreenImage::setIsXInverted(bool isInverted)
{
	m_gl_image->setIsXInverted(isInverted);
}

void myGLScreenImage::setIsYInverted(bool isInverted)
{
	m_gl_image->setIsYInverted(isInverted);
}



void myGLScreenImage::draw(bool drawing_state)
{
	if(drawing_state == 0)//we abandon drawing
	{
		m_drawingState = DRAWING_ABORT;
		//or to be deleted state!!!
	}
	else
	{
		if(m_drawingState == WAITING_FOR_DRAWING)
		{
			m_drawingState = STARTING_DRAWING;
		}
	}
}

void myGLScreenImage::select(bool select_state)
{
	m_isSelected = select_state;
	m_selectedVertex = -1;
	m_isResizing = false;
	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));


	//we tell derived class that selection changed
	onSelection_GLObject(select_state);
}

void myGLScreenImage::edit(bool edit_state)
{
	m_isEdited = edit_state;
	m_editingPointIdx = -1;
	m_isEditingPoint = false;
	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));


	//we tell derived class that selection changed
	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
}

float myGLScreenImage::getSelectionContourSurface()
{
	return m_seletion_contour->getSurface();
}

bool myGLScreenImage::isInsideSelectionContour(glm::vec2 r)
{
	return m_seletion_contour->isInside(r);
}

bool myGLScreenImage::isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx)
{
	return m_seletion_contour->isNextToAVertex(Rw, radius, vertex_idx);
}

bool myGLScreenImage::isNextToAnEditionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx)
{
	return m_edition_contour->isNextToAVertex(Rw, radius, vertex_idx);
}

bool myGLScreenImage::isInsideEditionContour(glm::vec2 r)
{
	return m_edition_contour->isInside(r);
}

float myGLScreenImage::getEditionSurface()
{
	return m_edition_contour->getSurface();
}

void myGLScreenImage::treatEvent(QEvent* event)
{
	m_glWidget->setMouseTracking(1);

			myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
			myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
			myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
			myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

			if(isSelectionableObject() && isSelected())
			{
				if(event->type() == QEvent::MouseButtonPress)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;
					switch(mouse_event->button())
					{
						case  Qt::LeftButton :
						{
							//isInside contour?
							vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
							vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

							if(isInsideSelectionContour(Rw_click) == false)
							{
								/*
								m_lastSelectionEventState.left_button = UNCLICKED;
								m_lastSelectionEventState.right_button = UNCLICKED;
								select(0);
								*/
							}
						}
						break;

						case Qt::RightButton:
						{
							/*
							m_lastSelectionEventState.left_button = UNCLICKED;
							m_lastSelectionEventState.right_button = CLICKED;
							//leaving drawing
							m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
							select(0);
							*/
				//                        m_r_button_clicked = 1;
						}
						break;
						default : break;
					}
				}

				/*
				if(event->type() == QEvent::MouseButtonDblClick)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
					vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

					if(isInsideEditionContour(Rw_click) == true)
					{
						edit(1);
						m_selectedVertex = -1;
						m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
						select(0);
					}
				}
				*/

				if(event->type() == QEvent::MouseButtonRelease)
				{
					m_lastSelectionEventState.left_button = UNCLICKED;
					m_lastSelectionEventState.right_button = UNCLICKED;

					m_isResizing = false;
					m_selectedVertex = -1;
					m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
				}
			}

			if(isEditableObject() && isEdited())
			{
				if(event->type() == QEvent::MouseButtonPress)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;
					switch(mouse_event->button())
					{
						case  Qt::LeftButton :
						{
							//isInside contour?
							vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
							vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);


							vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(10,10))
											  - m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
							float radius_w = length(radius_w_v);
							if(isNextToAnEditionContourVertex(Rw_click, radius_w, m_selectedVertex))
							{
								//start resizing
								m_isResizing = true;
								m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
								m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
								m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
								m_lastSelectionEventState.left_button = CLICKED;
								m_lastSelectionEventState.right_button = UNCLICKED;
							}
							else
							{
								if(isInsideSelectionContour(Rw_click))
								{
									//start displacement
									m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
									m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
									m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
									m_lastSelectionEventState.left_button = CLICKED;
									m_lastSelectionEventState.right_button = UNCLICKED;

									m_glWidget->setCursor(QCursor(Qt::SizeAllCursor));

								}

								else
								{
									/*
									m_lastSelectionEventState.left_button = UNCLICKED;
									m_lastSelectionEventState.right_button = UNCLICKED;
									edit(0);
									*/
								}
							}
						}
						break;

						case Qt::RightButton:
						{
							/*
							m_lastSelectionEventState.left_button = UNCLICKED;
							m_lastSelectionEventState.right_button = CLICKED;
							//leaving drawing
							m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
							edit(0);
							*/
						}
						break;
						default : break;
					}
				}

				if(event->type() == QEvent::MouseButtonRelease)
				{
					m_lastSelectionEventState.left_button = UNCLICKED;
					m_lastSelectionEventState.right_button = UNCLICKED;

					m_isResizing = false;
					m_selectedVertex = -1;
					m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
				}

				if(event->type() == QEvent::MouseMove)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(m_lastSelectionEventState.left_button == CLICKED)
					{
						if(isResizing() == true)
						{
							//we are resizing

							vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
							vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

							myRectangle::VERTEX vertex_name = edition_rectangle->getVertexType(m_selectedVertex);
							switch(vertex_name)
							{
								case myRectangle::BOTTOM_LEFT :
								{
									gl_selection_rectangle->setBottomLeft(Rw_move);
									selection_rectangle->setBottomLeft(Rw_move);
									gl_edition_rectangle->setBottomLeft(Rw_move);
									edition_rectangle->setBottomLeft(Rw_move);
									m_gl_image->getRect().setBottomLeft(Rw_move);
//									gl_geometry_rectangle->setBottomLeft(Rw_move);
//									geometry_rectangle->setBottomLeft(Rw_move);
								}
								break;

								case myRectangle::BOTTOM_RIGHT :
								{
									gl_selection_rectangle->setBottomRight(Rw_move);
									selection_rectangle->setBottomRight(Rw_move);
									gl_edition_rectangle->setBottomRight(Rw_move);
									edition_rectangle->setBottomRight(Rw_move);
									m_gl_image->getRect().setBottomRight(Rw_move);

//									gl_geometry_rectangle->setBottomRight(Rw_move);
//									geometry_rectangle->setBottomRight(Rw_move);
								}
								break;

								case myRectangle::TOP_LEFT :
								{
									gl_selection_rectangle->setTopLeft(Rw_move);
									selection_rectangle->setTopLeft(Rw_move);
									gl_edition_rectangle->setTopLeft(Rw_move);
									edition_rectangle->setTopLeft(Rw_move);
									m_gl_image->getRect().setTopLeft(Rw_move);
//									gl_geometry_rectangle->setTopLeft(Rw_move);
//									geometry_rectangle->setTopLeft(Rw_move);
								}
								break;

								case myRectangle::TOP_RIGHT :
								{
									gl_selection_rectangle->setTopRight(Rw_move);
									selection_rectangle->setTopRight(Rw_move);
									gl_edition_rectangle->setTopRight(Rw_move);
									edition_rectangle->setTopRight(Rw_move);
									m_gl_image->getRect().setTopRight(Rw_move);
//									gl_geometry_rectangle->setTopRight(Rw_move);
//									geometry_rectangle->setTopRight(Rw_move);
								}
								break;
								default : break;
							}


								//second_corner from current state


//								gl_selection_rectangle->translate(Rw_move - Rw_click);
//								selection_rectangle->translate(Rw_move - Rw_click);
//								gl_edition_rectangle->translate(Rw_move - Rw_click);
//								edition_rectangle->translate(Rw_move - Rw_click);
//								gl_geometry_rectangle->translate(Rw_move - Rw_click);
//								geometry_rectangle->translate(Rw_move - Rw_click);

								m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
								m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
								m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
						}

						else
						{
							//we are displacing

							//first_corner from last saved-state
							vec2 R_click = m_lastSelectionEventState.last_pos;
							vec2 r_click = m_glWidget->fromScrnToHomCoordinate(R_click);

							mat2 scaling_inv_click = mat2(1.0/m_lastSelectionEventState.last_scalingValues.x,0,
														  0, 1.0/m_lastSelectionEventState.last_scalingValues.y);
							vec2 t_click = m_lastSelectionEventState.last_translationVector;

							vec2 Rw_click = scaling_inv_click*(r_click - t_click);

								//second_corner from current state
								vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
								vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

								gl_selection_rectangle->translate(Rw_move - Rw_click);
								selection_rectangle->translate(Rw_move - Rw_click);
								gl_edition_rectangle->translate(Rw_move - Rw_click);
								edition_rectangle->translate(Rw_move - Rw_click);
								m_gl_image->getRect().translate(Rw_move - Rw_click);
//								gl_geometry_rectangle->translate(Rw_move - Rw_click);
//								geometry_rectangle->translate(Rw_move - Rw_click);

								m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
								m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
								m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
						}



					}
					else
					{
						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

						vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(10,10))
										  - m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
						float radius_w = length(radius_w_v);
						if(isNextToAnEditionContourVertex(Rw_move, radius_w, m_selectedVertex))
						{
							if(edition_rectangle->isBottomLeft(m_selectedVertex)
							   || edition_rectangle->isTopRight(m_selectedVertex))
							{
								m_glWidget->setCursor(Qt::SizeBDiagCursor);
							}

							if(edition_rectangle->isBottomRight(m_selectedVertex)
							   || edition_rectangle->isTopLeft(m_selectedVertex))
							{
								m_glWidget->setCursor(Qt::SizeFDiagCursor);
							}
						}
						else
						{
							m_glWidget->setCursor(Qt::ArrowCursor);
						}
					}

				}
			}

			if(isDrawableObject() && isDrawing())
			{
				if(event->type() == QEvent::MouseButtonPress)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;
					switch(mouse_event->button())
					{
						case  Qt::LeftButton :
						{
							m_lastEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
							m_lastEventState.last_scalingValues = m_glWidget->getScalingValues();
							m_lastEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
							m_lastEventState.left_button = CLICKED;
							m_lastEventState.right_button = UNCLICKED;

							m_drawingState = CURRENTLY_DRAWING;

							vec2 R_click = m_lastEventState.last_pos;
							vec2 Rw_firstCorner = m_glWidget->fromScrnToWorldCoordinate(R_click);

//							geometry_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
//							gl_geometry_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
							m_gl_image->getRect().setDiag(Rw_firstCorner, Rw_firstCorner);
							gl_selection_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
							selection_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
							gl_edition_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
							edition_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
						}
						break;

						case Qt::RightButton:
						{
							m_lastEventState.left_button = UNCLICKED;
							m_lastEventState.right_button = CLICKED;
							//leaving drawing
							draw(0);
				//                        m_r_button_clicked = 1;
						}
						break;
						default : break;
					}

				}
				if(event->type() == QEvent::MouseButtonRelease)
				{
//					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(m_lastEventState.left_button == CLICKED) m_drawingState = DRAWING_FINISHED;

					m_lastEventState.left_button = UNCLICKED;
					m_lastEventState.right_button = UNCLICKED;
				}
				if(event->type() == QEvent::MouseMove)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(m_lastEventState.left_button == CLICKED)

				//                if(m_l_button_clicked)
					{
						//first_corner from last saved-state
						vec2 R_click = m_lastEventState.last_pos;
						vec2 r_click = m_glWidget->fromScrnToHomCoordinate(R_click);
						mat2 scaling_inv_click = mat2(1.0/m_lastEventState.last_scalingValues.x,0,0, 1.0/m_lastEventState.last_scalingValues.y);
						vec2 t_click = m_lastEventState.last_translationVector;
						vec2 Rw_firstCorner = scaling_inv_click*(r_click - t_click);

						//second_corner from current state
						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_oppositeCorner = m_glWidget->fromScrnToWorldCoordinate(R_move);

						m_gl_image->getRect().setDiag(Rw_firstCorner, Rw_oppositeCorner);
						gl_selection_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						selection_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						gl_edition_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						edition_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
					}
				}
			}

}


void myGLScreenImage::render(myGLObject* renderingTarget)
{
	if(renderingTarget->isRenderingTargetObject() == false) return; //->

		if(isSelected())
		{
			m_gl_seletion_contour->setRenderingMode(LINE_LOOPED_RENDERING);
			m_gl_seletion_contour->setRenderingColor(m_selection_color);
			m_gl_seletion_contour->render(renderingTarget);

			if(isRenderableObject() && isVisible())
			{
				m_gl_image->render(renderingTarget);
			}

		}
		if(isEdited())
		{

			m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_gl_edition_contour->setRenderingColor(m_edition_color);
			m_gl_edition_contour->render(renderingTarget);

			if(isRenderableObject() && isVisible())
			{
				m_gl_image->render(renderingTarget);
			}
		}

		if(isRenderableObject()
			&& isVisible()
			&& !isWaitingForDrawing()
			&& !isStartingDrawing()
			&& !isEdited()
			&& !isSelected())
		{
//			if(isDrawing()) m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
//			else m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);
			m_gl_image->render(renderingTarget);


//			m_gl_geometry->render(screen);
		}

}













