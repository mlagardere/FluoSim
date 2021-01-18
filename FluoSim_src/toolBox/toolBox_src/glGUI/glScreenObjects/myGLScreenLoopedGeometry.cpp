
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



#include "glGUI/glScreenObjects/myGLScreenGeometricObjects.h"

using namespace glm;



myGLScreenLoopedGeometry::myGLScreenLoopedGeometry(myMultiVector<glm::vec2>& geom_MV,
												   gstd::gMultiVector<glm::vec2>& geom_gMV,
												   myMultiVector<glm::vec2>& contour_MV,
												   gstd::gMultiVector<glm::vec2>& contour_gMV,
												   GEOMETRY loopGeometry_type,
												   myGLWidget* glWidget) :


  myGLObject(myGLObject::IS_RENDERABLE_OBJECT |
			 myGLObject::IS_DRAWABLE_OBJECT |
			 myGLObject::IS_SELECTIONABLE_OBJECT |
			 myGLObject::IS_EDITABLE_OBJECT,
			 myGLObject::GLSCREEN_LOOPED_GEOMETRY_TYPE)


{
	m_loopGeometry_type = loopGeometry_type;
	m_glWidget = glWidget;

	switch(m_loopGeometry_type)
	{
		case RECTANGLE :
		{
			m_gl_geometry = new myGLRectangle(geom_gMV);
				m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);
			m_geometry = new myRectangle(geom_MV);

			m_gl_seletion_contour = new myGLRectangle(contour_gMV);
				m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_seletion_contour = new myRectangle(contour_MV);

			m_gl_edition_contour = new myGLRectangle(contour_gMV);
				m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_edition_contour = new myRectangle(contour_MV);
		}
		break;

		case CIRCLE :
		{
			m_gl_geometry = new myGLCircle(geom_gMV);
				m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);
			m_geometry = new myCircle(geom_MV);


			m_gl_seletion_contour = new myGLRectangle(contour_gMV);
				m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_seletion_contour = new myRectangle(contour_MV);


			m_gl_edition_contour = new myGLRectangle(contour_gMV);
				m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_edition_contour = new myRectangle(contour_MV);

		}
		break;

		case POLYGON :
		{
			m_gl_geometry = new myGLPolygon(geom_gMV);
				m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
			m_geometry = new myPolygon(geom_MV);

			m_gl_seletion_contour = new myGLRectangle(contour_gMV);
				m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_seletion_contour = new myRectangle(contour_MV);

			m_gl_edition_contour = new myGLRectangle(contour_gMV);
				m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_edition_contour = new myRectangle(contour_MV);

			myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
			myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
			myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
			myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

				vec2 minimum_size = m_glWidget->fromScrnToWorldCoordinate(vec2(20,20))
									- m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
				minimum_size = vec2(abs(minimum_size.x),
									abs(minimum_size.y));

				gl_selection_rectangle->setHasMinimumSize(1);
				selection_rectangle->setHasMinimumSize(1);
				gl_edition_rectangle->setHasMinimumSize(1);
				edition_rectangle ->setHasMinimumSize(1);

				gl_selection_rectangle->setMinimumSize(minimum_size);
				selection_rectangle->setMinimumSize(minimum_size);
				gl_edition_rectangle->setMinimumSize(minimum_size);
				edition_rectangle ->setMinimumSize(minimum_size);
		}
		break;
	}
}

myGLScreenLoopedGeometry::~myGLScreenLoopedGeometry()
{

		delete m_gl_geometry;
		delete m_geometry;

		delete m_gl_seletion_contour;
		delete m_seletion_contour;

		delete m_gl_edition_contour;
		delete m_edition_contour;
}

void myGLScreenLoopedGeometry::draw(bool drawing_state)
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

void myGLScreenLoopedGeometry::select(bool select_state)
{
	m_isSelected = select_state;
	m_selectedVertex = -1;
	m_isResizing = false;

	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));

	//we tell derived class that selection changed
	onSelection_GLObject(select_state);

}

void myGLScreenLoopedGeometry::edit(bool edit_state)
{
	m_isEdited = edit_state;
	m_editingPointIdx = -1;
	m_isEditingPoint = false;


	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
}

void myGLScreenLoopedGeometry::setSpace(OBJECT_SPACE space)
{
	m_gl_geometry->setSpace(space);
	m_gl_edition_contour->setSpace(space);
	m_gl_seletion_contour->setSpace(space);
}

float myGLScreenLoopedGeometry::getSelectionContourSurface()
{
	return m_seletion_contour->getSurface();
}

bool myGLScreenLoopedGeometry::isInsideSelectionContour(glm::vec2 r)
{
	return m_seletion_contour->isInside(r);
}

bool myGLScreenLoopedGeometry::isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx)
{
	return m_seletion_contour->isNextToAVertex(Rw, radius, vertex_idx);
}



bool myGLScreenLoopedGeometry::isNextToAnEditionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx)
{
	return m_edition_contour->isNextToAVertex(Rw, radius, vertex_idx);
}

bool myGLScreenLoopedGeometry::isInsideEditionContour(glm::vec2 r)
{
	return m_edition_contour->isInside(r);
}

float myGLScreenLoopedGeometry::getEditionSurface()
{
	return m_edition_contour->getSurface();
}



void myGLScreenLoopedGeometry::treatEvent(QEvent* event)
{


	m_glWidget->setMouseTracking(1);

	switch(m_loopGeometry_type)
	{
		case RECTANGLE :
		{
			myGLRectangle* gl_geometry_rectangle = (myGLRectangle*) m_gl_geometry;
			myRectangle* geometry_rectangle = (myRectangle*) m_geometry;
			myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
			myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
			myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
			myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;


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
								if(isInsideEditionContour(Rw_click))
								{
									//start displacement
									m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
									m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
									m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
									m_lastSelectionEventState.left_button = CLICKED;
									m_lastSelectionEventState.right_button = UNCLICKED;

									m_glWidget->setCursor(QCursor(Qt::SizeAllCursor));

								}

								/*
								else
								{
									m_lastSelectionEventState.left_button = UNCLICKED;
									m_lastSelectionEventState.right_button = UNCLICKED;
									edit(0);
								}
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
									gl_geometry_rectangle->setBottomLeft(Rw_move);
									geometry_rectangle->setBottomLeft(Rw_move);
								}
								break;

								case myRectangle::BOTTOM_RIGHT :
								{
									gl_selection_rectangle->setBottomRight(Rw_move);
									selection_rectangle->setBottomRight(Rw_move);
									gl_edition_rectangle->setBottomRight(Rw_move);
									edition_rectangle->setBottomRight(Rw_move);
									gl_geometry_rectangle->setBottomRight(Rw_move);
									geometry_rectangle->setBottomRight(Rw_move);
								}
								break;

								case myRectangle::TOP_LEFT :
								{
									gl_selection_rectangle->setTopLeft(Rw_move);
									selection_rectangle->setTopLeft(Rw_move);
									gl_edition_rectangle->setTopLeft(Rw_move);
									edition_rectangle->setTopLeft(Rw_move);
									gl_geometry_rectangle->setTopLeft(Rw_move);
									geometry_rectangle->setTopLeft(Rw_move);
								}
								break;

								case myRectangle::TOP_RIGHT :
								{
									gl_selection_rectangle->setTopRight(Rw_move);
									selection_rectangle->setTopRight(Rw_move);
									gl_edition_rectangle->setTopRight(Rw_move);
									edition_rectangle->setTopRight(Rw_move);
									gl_geometry_rectangle->setTopRight(Rw_move);
									geometry_rectangle->setTopRight(Rw_move);
								}
								break;
							}
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
								gl_geometry_rectangle->translate(Rw_move - Rw_click);
								geometry_rectangle->translate(Rw_move - Rw_click);

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

			if(isSelectionableObject() && isSelected())
			{
				if(event->type() == QEvent::MouseButtonPress)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;
					switch(mouse_event->button())
					{
						case  Qt::LeftButton :
						{
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
					cout<<"botLeft"<<selection_rectangle->getBottomLeft().x<<"__"<<selection_rectangle->getBottomLeft().y<<"\n";
					cout<<"topRight"<<selection_rectangle->getTopRight().x<<"__"<<selection_rectangle->getTopRight().y<<"\n";
					cout<<"botLeft"<<geometry_rectangle->getBottomLeft().x<<"__"<<geometry_rectangle->getBottomLeft().y<<"\n";
					cout<<"topRight"<<geometry_rectangle->getTopRight().x<<"__"<<geometry_rectangle->getTopRight().y<<"\n";

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

							geometry_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
							gl_geometry_rectangle->setDiag(Rw_firstCorner, Rw_firstCorner);
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
					if(m_lastEventState.left_button == CLICKED)
					{
						m_drawingState = DRAWING_FINISHED;
					}

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

						geometry_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						gl_geometry_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						gl_selection_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						selection_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						gl_edition_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
						edition_rectangle->setDiag(Rw_firstCorner, Rw_oppositeCorner);
					}
				}
			}
		}
		break;

		case CIRCLE :
		{
			myGLCircle* gl_geometry_circle = (myGLCircle*) m_gl_geometry;
			myCircle* geometry_circle = (myCircle*) m_geometry;
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

							if(selection_rectangle->isInside(Rw_click) == false)
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


							vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(7,7))
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
								//selection test
//								if(m_geometry->isInside(Rw_click))
//								if(selection_rectangle->isInside(Rw_click))
								if(isInsideEditionContour(Rw_click))
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

							vec2 center = gl_geometry_circle->getCenter();
							float radius = gl_geometry_circle->getRadius();
							float new_radius;
							switch(vertex_name)
							{
								case myRectangle::BOTTOM_LEFT :
								{
									new_radius = abs(radius + dot(Rw_move-center, vec2(-1/sqrt(2),-1/sqrt(2)))- radius*sqrt(2));
								}
								break;

								case myRectangle::BOTTOM_RIGHT :
								{
									new_radius = abs(radius + dot(Rw_move-center, vec2(+1/sqrt(2),-1/sqrt(2)))- radius*sqrt(2));
								}
								break;

								case myRectangle::TOP_LEFT :
								{
									new_radius = abs(radius + dot(Rw_move-center, vec2(-1/sqrt(2),+1/sqrt(2)))- radius*sqrt(2));
								}
								break;

								case myRectangle::TOP_RIGHT :
								{
									new_radius = abs(radius + dot(Rw_move-center, vec2(+1/sqrt(2),+1/sqrt(2)))- radius*sqrt(2));
								}
								break;
								default : break;
							}

							gl_selection_rectangle->setDiag(center + new_radius*vec2(-1,-1),
															center + new_radius*vec2(+1,+1));
							selection_rectangle->setDiag(center + new_radius*vec2(-1,-1),
														 center + new_radius*vec2(+1,+1));
							gl_edition_rectangle->setDiag(center + new_radius*vec2(-1,-1),
														  center + new_radius*vec2(+1,+1));
							edition_rectangle->setDiag(center + new_radius*vec2(-1,-1),
													   center + new_radius*vec2(+1,+1));

							gl_geometry_circle->setRadius(new_radius);
							geometry_circle->setRadius(new_radius);

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
								gl_geometry_circle->translate(Rw_move - Rw_click);
								geometry_circle->translate(Rw_move - Rw_click);

								m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
								m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
								m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
						}
					}
					else
					{
						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

						vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(7,7))
										  - m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
						float radius_w = length(radius_w_v);
						if(isNextToAnEditionContourVertex(Rw_move, radius_w, m_selectedVertex))
						{
							cout<<m_selectedVertex<<"_";
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
							m_lastEventState.last_scalingValues= m_glWidget->getScalingValues();
							m_lastEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
							m_lastEventState.left_button = CLICKED;
							m_lastEventState.right_button = UNCLICKED;

							m_drawingState = CURRENTLY_DRAWING;

							vec2 R_click = m_lastEventState.last_pos;
							vec2 center = m_glWidget->fromScrnToWorldCoordinate(R_click);

							geometry_circle->setCenter(center);
							gl_geometry_circle->setCenter(center);
						}
						break;

						case Qt::RightButton:
						{
							m_lastEventState.left_button = UNCLICKED;
							m_lastEventState.right_button = CLICKED;
							//leaving drawing
							draw(0);
						}
						break;
						default : break;
					}
				}

				if(event->type() == QEvent::MouseButtonRelease)
				{
					if(m_lastEventState.left_button == CLICKED)
					{
						m_drawingState = DRAWING_FINISHED;
					}

					m_lastEventState.left_button = UNCLICKED;
					m_lastEventState.right_button = UNCLICKED;
				}

				if(event->type() == QEvent::MouseMove)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(m_lastEventState.left_button == CLICKED)
					{
						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_circle = m_glWidget->fromScrnToWorldCoordinate(R_move);

						geometry_circle->setRadius(length(geometry_circle->getCenter() - Rw_circle));
						gl_geometry_circle->setRadius(length(geometry_circle->getCenter() - Rw_circle));

						gl_selection_rectangle->setDiag(geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(-1,-1),
														geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(+1,+1));
						selection_rectangle->setDiag(geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(-1,-1),
													 geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(+1,+1));

						gl_edition_rectangle->setDiag(geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(-1,-1),
													  geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(+1,+1));

						edition_rectangle->setDiag(geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(-1,-1),
												   geometry_circle->getCenter()+geometry_circle->getRadius()*vec2(+1,+1));
					}
				}
			}
		}
		break;

		case POLYGON :
		{
			myGLPolygon* gl_geometry_polygon = (myGLPolygon*) m_gl_geometry;
			myPolygon* geometry_polygon = (myPolygon*) m_geometry;
			myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
			myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
			myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
			myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

			vec2 minimum_size = m_glWidget->fromScrnToWorldCoordinate(vec2(3,3))
								- m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
			minimum_size = vec2(abs(minimum_size.x),
								abs(minimum_size.y));

			gl_selection_rectangle->setMinimumSize(minimum_size);
			selection_rectangle->setMinimumSize(minimum_size);
			gl_edition_rectangle->setMinimumSize(minimum_size);
			edition_rectangle ->setMinimumSize(minimum_size);

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


							vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(7,7))
											  - m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
							float radius_w = length(radius_w_v);


							if(isNextToAnEditionContourVertex(Rw_click, radius_w, m_selectedVertex))
							{
//								//start resizing
								m_isResizing = true;
								m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
								m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
								m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
								m_lastSelectionEventState.left_button = CLICKED;
								m_lastSelectionEventState.right_button = UNCLICKED;
							}
							else
							{
								if(geometry_polygon->isNextToAVertex(Rw_click, radius_w, m_editingPointIdx))
								{
	//								//start resizing
									m_isEditingPoint = true;
									m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
									m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
									m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
									m_lastSelectionEventState.left_button = CLICKED;
									m_lastSelectionEventState.right_button = UNCLICKED;
								}
								else
								{
									//selection test
									if(edition_rectangle->isInside(Rw_click))
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
				//                        m_r_button_clicked = 1;
						}
						break;
						default : break;
					}
				}

				if(event->type() == QEvent::MouseButtonRelease)
				{
					m_lastSelectionEventState.left_button = UNCLICKED;
					m_lastSelectionEventState.right_button = UNCLICKED;

					m_isEditingPoint = false;
					m_isResizing = false;
					m_editingPointIdx = -1;
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

							vec2 center_old, center_new;
							vec2 selected_pt;
							vec2 scaling_rect;
							vec2 scaling_polygon;

							myRectangle::VERTEX vertex_name = edition_rectangle->getVertexType(m_selectedVertex);
							center_old =  edition_rectangle->getVertexCoord(edition_rectangle->getOppositeVertexType(vertex_name));
							selected_pt = edition_rectangle->getVertexCoord(vertex_name);

							gl_selection_rectangle->setDiag(center_old, Rw_move);
							selection_rectangle->setDiag(center_old, Rw_move);
							gl_edition_rectangle->setDiag(center_old, Rw_move);
							edition_rectangle->setDiag(center_old, Rw_move);

							scaling_polygon.x = abs(edition_rectangle->getSize().x /(selected_pt - center_old).x);
							scaling_polygon.y = abs(edition_rectangle->getSize().y /(selected_pt - center_old).y);

							center_new =  edition_rectangle->getVertexCoord(edition_rectangle->getOppositeVertexType(vertex_name));

							gl_geometry_polygon->translate(center_new-center_old);
							geometry_polygon->translate(center_new-center_old);
							gl_geometry_polygon->scale(center_new, scaling_polygon);
							geometry_polygon->scale(center_new, scaling_polygon);

							m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
							m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
							m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
						}

						else
						{
							if(isEditingAPoint() == true)
							{

								//we are resizing

								vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
								vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

								gl_geometry_polygon->setPoint(m_editingPointIdx, Rw_move);
								geometry_polygon->setPoint(m_editingPointIdx, Rw_move);

								vec2 polygon_bottomLeft = vec2(geometry_polygon->getPoint(geometry_polygon->getLeftPointIdx()).x,
																geometry_polygon->getPoint(geometry_polygon->getBottomPointIdx()).y);

								vec2 polygon_topRight = vec2(geometry_polygon->getPoint(geometry_polygon->getRightPointIdx()).x,
															 geometry_polygon->getPoint(geometry_polygon->getTopPointIdx()).y);

								gl_selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								gl_edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);

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
									gl_geometry_polygon->translate(Rw_move - Rw_click);
									geometry_polygon->translate(Rw_move - Rw_click);

									m_lastSelectionEventState.last_pos = vec2(mouse_event->x(), mouse_event->y());
									m_lastSelectionEventState.last_scalingValues = m_glWidget->getScalingValues();
									m_lastSelectionEventState.last_translationVector = m_glWidget->getTranslationVector().xy();
							}
						}


					}
					else
					{

						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_move = m_glWidget->fromScrnToWorldCoordinate(R_move);

						vec2 radius_w_v = m_glWidget->fromScrnToWorldCoordinate(vec2(7,7))
										  - m_glWidget->fromScrnToWorldCoordinate(vec2(0,0));
						float radius_w = length(radius_w_v);

						if(isNextToASelectionContourVertex(Rw_move, radius_w, m_selectedVertex))
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
							if(geometry_polygon->isNextToAVertex(Rw_move, radius_w, m_editingPointIdx))
							{
								m_glWidget->setCursor(Qt::CrossCursor);
							}
							else
							{
								m_glWidget->setCursor(Qt::ArrowCursor);
							}
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
							if(m_drawingState == DRAWING_TO_BE_FINISHED)
							{
								vec2 polygon_bottomLeft = vec2(geometry_polygon->getPoint(geometry_polygon->getLeftPointIdx()).x,
																geometry_polygon->getPoint(geometry_polygon->getBottomPointIdx()).y);

								vec2 polygon_topRight = vec2(geometry_polygon->getPoint(geometry_polygon->getRightPointIdx()).x,
															 geometry_polygon->getPoint(geometry_polygon->getTopPointIdx()).y);

								gl_selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								gl_edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
								edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);

								int nb_point = geometry_polygon->getSize();
								geometry_polygon->removePoint(nb_point-1);
								gl_geometry_polygon->removePoint(nb_point-1);
								gl_geometry_polygon->setRenderingMode(LINE_LOOPED_RENDERING);
								m_drawingState = DRAWING_FINISHED;
							}
						}
						break;

						case Qt::RightButton:
						{

						}
						break;
						default : break;
					}
				}

				if(event->type() == QEvent::MouseButtonRelease)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;
					switch(mouse_event->button())
					{
						case  Qt::LeftButton :
						{
							m_drawingState = CURRENTLY_DRAWING;

							vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
							vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

							//au premier click on met deux points
							if(geometry_polygon->getSize() == 0)
							{
								geometry_polygon->addPoint(Rw_click);
								gl_geometry_polygon->addPoint(Rw_click);

								//to be modified by moving
								geometry_polygon->addPoint(Rw_click);
								gl_geometry_polygon->addPoint(Rw_click);
							}
							//apres on valide le point actuel
							else
							{
								geometry_polygon->addPoint(Rw_click);
								gl_geometry_polygon->addPoint(Rw_click);
							}
						}
						break;

						case Qt::RightButton:
						{
							vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
							vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

							if(geometry_polygon->getSize()>2)
							{
								unsigned int last_point_idx = geometry_polygon->getSize()-1;
								geometry_polygon->removePoint(last_point_idx);
								gl_geometry_polygon->removePoint(last_point_idx);

								geometry_polygon->setPoint(last_point_idx-1, Rw_click);
								gl_geometry_polygon->setPoint(last_point_idx-1, Rw_click);

								//to avoid finishing curve with too few points
								if(m_drawingState == DRAWING_TO_BE_FINISHED && last_point_idx<=3)
								{
									m_drawingState = CURRENTLY_DRAWING;
								}
							}
							else
							{
								//leaving drawing
								draw(0);
							}
						}
						break;
						default : break;
					}



//					if(m_lastEventState.left_button == CLICKED) m_drawingState = DRAWING_FINISHED;

					m_lastEventState.left_button = UNCLICKED;
					m_lastEventState.right_button = UNCLICKED;
				}

				bool canWeExitWithDoubleClick = false;
				if(canWeExitWithDoubleClick == true && event->type() == QEvent::MouseButtonDblClick)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(mouse_event->button() == Qt::LeftButton)
					{
//						vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
//						vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

							vec2 polygon_bottomLeft = vec2(geometry_polygon->getPoint(geometry_polygon->getLeftPointIdx()).x,
															geometry_polygon->getPoint(geometry_polygon->getBottomPointIdx()).y);

							vec2 polygon_topRight = vec2(geometry_polygon->getPoint(geometry_polygon->getRightPointIdx()).x,
														 geometry_polygon->getPoint(geometry_polygon->getTopPointIdx()).y);

							gl_selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
							selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
							gl_edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
							edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);

							int nb_point = geometry_polygon->getSize();
							geometry_polygon->removePoint(nb_point-1);
							gl_geometry_polygon->removePoint(nb_point-1);
							gl_geometry_polygon->setRenderingMode(LINE_LOOPED_RENDERING);
							m_drawingState = DRAWING_FINISHED;
					}
				}

				if(event->type() == QEvent::MouseMove)
				{
					QMouseEvent* mouse_event = (QMouseEvent*) event;

					if(m_drawingState == CURRENTLY_DRAWING || m_drawingState == DRAWING_TO_BE_FINISHED)
					{
						vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
						vec2 Rw_move= m_glWidget->fromScrnToWorldCoordinate(R_move);

						unsigned int last_point_idx = geometry_polygon->getSize()-1;
						if(last_point_idx >= 3 && (length(Rw_move - geometry_polygon->getPoint(0))
												  <= length(m_glWidget->fromScrnToWorldCoordinate(vec2(5,5))
															- m_glWidget->fromScrnToWorldCoordinate(vec2(0,0)))))
						{
							geometry_polygon->setPoint(last_point_idx, geometry_polygon->getPoint(0));
							gl_geometry_polygon->setPoint(last_point_idx, geometry_polygon->getPoint(0));

							m_drawingState = DRAWING_TO_BE_FINISHED;
						}
						else
						{
							geometry_polygon->setPoint(last_point_idx, Rw_move);
							gl_geometry_polygon->setPoint(last_point_idx, Rw_move);

							m_drawingState = CURRENTLY_DRAWING;
						}
					}
				}
			}
		}
		break;
	}
}


void myGLScreenLoopedGeometry::render(myGLObject* renderingTarget)
{
	if(renderingTarget->isRenderingTargetObject() == false) return; //->

		if(isSelected())
		{
			m_gl_seletion_contour->setRenderingMode(LINE_LOOPED_RENDERING);
			m_gl_seletion_contour->setRenderingColor(m_selection_color);
			m_gl_seletion_contour->render(renderingTarget);

			if(isRenderableObject()
				&& isVisible() && m_loopGeometry_type!=RECTANGLE)
			{
				m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);
				m_gl_geometry->render(renderingTarget);
			}

		}
		if(isEdited())
		{

			m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_gl_edition_contour->setRenderingColor(m_edition_color);
			m_gl_edition_contour->render(renderingTarget);

			if(isRenderableObject() == true &&
			   isVisible()			== true	&&
			   m_loopGeometry_type  != RECTANGLE)
			{
				if(m_loopGeometry_type == CIRCLE)
				{
					m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
				}
				else
				{
					m_gl_geometry->setRenderingMode(LINE_AND_POINT_RENDERING);
				}

				m_gl_geometry->render(renderingTarget);
			}
		}

		if(isRenderableObject()
			&& isVisible()
			&& !isWaitingForDrawing()
			&& !isStartingDrawing()
			&& !isEdited()
			&& !isSelected())
		{
			if(isDrawing())
			{
				if(m_loopGeometry_type == POLYGON) m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
				else m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);
			}
			else m_gl_geometry->setRenderingMode(LINE_LOOPED_RENDERING);

			m_gl_geometry->render(renderingTarget);
		}
}


void myGLScreenLoopedGeometry::loadGeometry(vector<vec2> Rw_v)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	switch(m_loopGeometry_type)
	{
		case RECTANGLE :
		{
			if(Rw_v.size() != 2) return; //->

			myGLRectangle* gl_geometry_rectangle = (myGLRectangle*) m_gl_geometry;
			myRectangle* geometry_rectangle = (myRectangle*) m_geometry;

			gl_geometry_rectangle->setDiag(Rw_v[0], Rw_v[1]);
			geometry_rectangle->setDiag(Rw_v[0], Rw_v[1]);
			gl_selection_rectangle->setDiag(Rw_v[0], Rw_v[1]);
			selection_rectangle->setDiag(Rw_v[0], Rw_v[1]);
			gl_edition_rectangle->setDiag(Rw_v[0], Rw_v[1]);
			edition_rectangle->setDiag(Rw_v[0], Rw_v[1]);

			m_drawingState = DRAWING_FINISHED;
		}
		break;

		case CIRCLE :
		{
			if(Rw_v.size() != 2) return; //->

			myGLCircle* gl_geometry_circle = (myGLCircle*) m_gl_geometry;
			myCircle* geometry_circle = (myCircle*) m_geometry;

			float radius = length(Rw_v[1]-Rw_v[0]);
			vec2 center = Rw_v[0];
			geometry_circle->setCenter(Rw_v[0]);
			geometry_circle->setRadius(length(radius));

			gl_geometry_circle->setCenter(Rw_v[0]);
			gl_geometry_circle->setRadius(length(radius));

			gl_selection_rectangle->setDiag(center - radius*vec2(1,1), center + radius*vec2(1,1));
			selection_rectangle->setDiag(center - radius*vec2(1,1), center + radius*vec2(1,1));
			gl_edition_rectangle->setDiag(center - radius*vec2(1,1), center + radius*vec2(1,1));
			edition_rectangle->setDiag(center - radius*vec2(1,1), center + radius*vec2(1,1));

			m_drawingState = DRAWING_FINISHED;
		}

		break;

		case POLYGON :
		{
			if(Rw_v.size() <= 2) return; //->

			myGLPolygon* gl_geometry_polygon = (myGLPolygon*) m_gl_geometry;
			myPolygon* geometry_polygon = (myPolygon*) m_geometry;

			auto Rw = Rw_v.begin();
			while(Rw != Rw_v.end())
			{
				geometry_polygon->addPoint(*Rw);
				gl_geometry_polygon->addPoint(*Rw);

				Rw++;
			}

			vec2 bottomLeft(geometry_polygon->getPoint(geometry_polygon->getLeftPointIdx()).x,
							geometry_polygon->getPoint(geometry_polygon->getBottomPointIdx()).y);
			vec2 topRight(geometry_polygon->getPoint(geometry_polygon->getRightPointIdx()).x,
						  geometry_polygon->getPoint(geometry_polygon->getTopPointIdx()).y);

			gl_selection_rectangle->setDiag(bottomLeft, topRight);
			selection_rectangle->setDiag(bottomLeft, topRight);
			gl_edition_rectangle->setDiag(bottomLeft, topRight);
			edition_rectangle->setDiag(bottomLeft, topRight);

			m_drawingState = DRAWING_FINISHED;
		}
		break;
	}
}


bool myGLScreenLoopedGeometry::isInside(glm::vec2 r)
{
	return m_geometry->isInside(r);
}


glm::vec2 myGLScreenLoopedGeometry::randomPointInside()
{
	myRectangle* rect = (myRectangle*) m_seletion_contour;

	glm::vec2 temp_point(uniformDistributiion(rect->getBottomLeft().x, rect->getTopRight().x),
						 uniformDistributiion(rect->getBottomLeft().y, rect->getTopRight().y));
	while(isInside(temp_point) == false)
	{
		temp_point.x = uniformDistributiion(rect->getBottomLeft().x, rect->getTopRight().x);
		temp_point.y = uniformDistributiion(rect->getBottomLeft().y, rect->getTopRight().y);
	}

	return temp_point;
}

int myGLScreenLoopedGeometry::getSubVectorId()
{
	return m_geometry->getSubVectorId();
}

vector<glm::vec2>  myGLScreenLoopedGeometry::getGeometry()
{
	return m_geometry->getR();
}

