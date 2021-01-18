
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


myGLScreenMultiLineGeometry::myGLScreenMultiLineGeometry(myMultiVector<glm::vec2>& geom_MV,
														 gstd::gMultiVector<glm::vec2>& geom_gMV,
														 myMultiVector<glm::vec2>& contour_MV,
														 gstd::gMultiVector<glm::vec2>& contour_gMV,
														 myGLWidget* glWidget) :


  myGLObject(myGLObject::IS_RENDERABLE_OBJECT |
			 myGLObject::IS_DRAWABLE_OBJECT |
			 myGLObject::IS_SELECTIONABLE_OBJECT |
			 myGLObject::IS_EDITABLE_OBJECT,
			 myGLObject::GLSCREEN_MULTILINE_GEOMETRY_TYPE)


{
	m_glWidget = glWidget;

	m_gl_geometry = new myGLMultiLineGeometry(geom_gMV);
		m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
	m_geometry = new myMultiLineGeometry(geom_MV);

	m_gl_seletion_contour = new myGLRectangle(geom_gMV);
		m_gl_seletion_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_seletion_contour = new myRectangle(geom_MV);

	m_gl_edition_contour = new myGLRectangle(geom_gMV);
		m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
	m_edition_contour = new myRectangle(geom_MV);

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

myGLScreenMultiLineGeometry::~myGLScreenMultiLineGeometry()
{
	delete m_gl_geometry;
	delete m_geometry;

	delete m_gl_seletion_contour;
	delete m_seletion_contour;

	delete m_gl_edition_contour;
	delete m_edition_contour;
}

void myGLScreenMultiLineGeometry::draw(bool drawing_state)
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

void myGLScreenMultiLineGeometry::select(bool select_state)
{
	m_isSelected = select_state;
	m_selectedVertex = -1;
	m_isResizing = false;

	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));

	//we tell derived class that selection changed
	onSelection_GLObject(select_state);
}

void myGLScreenMultiLineGeometry::edit(bool edit_state)
{
	m_isEdited = edit_state;
	m_editingPointIdx = -1;
	m_isEditingPoint = false;


	m_glWidget->setCursor(QCursor(Qt::ArrowCursor));
}

void myGLScreenMultiLineGeometry::setSpace(OBJECT_SPACE space)
{
	m_gl_geometry->setSpace(space);
	m_gl_edition_contour->setSpace(space);
	m_gl_seletion_contour->setSpace(space);
}

float myGLScreenMultiLineGeometry::getSelectionContourSurface()
{
	return m_seletion_contour->getSurface();
}

bool myGLScreenMultiLineGeometry::isInsideSelectionContour(glm::vec2 r)
{
	return m_seletion_contour->isInside(r);
}

bool myGLScreenMultiLineGeometry::isNextToASelectionContourVertex(glm::vec2 Rw, float radius, int& vertex_idx)
{
	return m_seletion_contour->isNextToAVertex(Rw, radius, vertex_idx);
}

bool myGLScreenMultiLineGeometry::isInsideEditionContour(glm::vec2 r)
{
	return m_edition_contour->isInside(r);
}

float myGLScreenMultiLineGeometry::getEditionSurface()
{
	return m_edition_contour->getSurface();
}


void myGLScreenMultiLineGeometry::treatEvent(QEvent* event)
{
	m_glWidget->setMouseTracking(1);

	myGLMultiLineGeometry* gl_geometry_multiLine = (myGLMultiLineGeometry*) m_gl_geometry;
	myMultiLineGeometry* geometry_multiLine = (myMultiLineGeometry*) m_geometry;
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
						if(geometry_multiLine->isNextToAVertex(Rw_click, radius_w, m_editingPointIdx))
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

					gl_geometry_multiLine->translate(center_new-center_old);
					geometry_multiLine->translate(center_new-center_old);
					gl_geometry_multiLine->scale(center_new, scaling_polygon);
					geometry_multiLine->scale(center_new, scaling_polygon);

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

						gl_geometry_multiLine->setPoint(m_editingPointIdx, Rw_move);
						geometry_multiLine->setPoint(m_editingPointIdx, Rw_move);

						vec2 polygon_bottomLeft = vec2(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
														geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);

						vec2 polygon_topRight = vec2(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
													 geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

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
							gl_geometry_multiLine->translate(Rw_move - Rw_click);
							geometry_multiLine->translate(Rw_move - Rw_click);

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
					if(geometry_multiLine->isNextToAVertex(Rw_move, radius_w, m_editingPointIdx))
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

                    //we set two points at the fist mouse click
					if(geometry_multiLine->getSize() == 0)
					{
						geometry_multiLine->addPoint(Rw_click);
						gl_geometry_multiLine->addPoint(Rw_click);

						//to be modified by moving
						geometry_multiLine->addPoint(Rw_click);
						gl_geometry_multiLine->addPoint(Rw_click);
					}
                    //then we validate the current point
					else
					{
						geometry_multiLine->addPoint(Rw_click);
						gl_geometry_multiLine->addPoint(Rw_click);
					}
				}
				break;

				case Qt::RightButton:
				{
					vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
					vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

					if(geometry_multiLine->getSize()>2)
					{
						unsigned int last_point_idx = geometry_multiLine->getSize()-1;
						geometry_multiLine->removePoint(last_point_idx);
						gl_geometry_multiLine->removePoint(last_point_idx);

						geometry_multiLine->setPoint(last_point_idx-1, Rw_click);
						gl_geometry_multiLine->setPoint(last_point_idx-1, Rw_click);

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

			m_lastEventState.left_button = UNCLICKED;
			m_lastEventState.right_button = UNCLICKED;
		}

		if(event->type() == QEvent::MouseButtonDblClick)
		{
			QMouseEvent* mouse_event = (QMouseEvent*) event;
			if(mouse_event->button() == Qt::LeftButton)
			{
//				vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
//				vec2 Rw_click = m_glWidget->fromScrnToWorldCoordinate(R_click);

					vec2 polygon_bottomLeft = vec2(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
													geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);

					vec2 polygon_topRight = vec2(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
												 geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

					gl_selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
					selection_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
					gl_edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);
					edition_rectangle->setDiag(polygon_bottomLeft, polygon_topRight);

					int nb_point = geometry_multiLine->getSize();
					geometry_multiLine->removePoint(nb_point-1);
					gl_geometry_multiLine->removePoint(nb_point-1);
					gl_geometry_multiLine->setRenderingMode(LINE_LOOPED_RENDERING);
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

				unsigned int last_point_idx = geometry_multiLine->getSize()-1;

				geometry_multiLine->setPoint(last_point_idx, Rw_move);
				gl_geometry_multiLine->setPoint(last_point_idx, Rw_move);

				m_drawingState = CURRENTLY_DRAWING;
			}
		}
	}
}


void myGLScreenMultiLineGeometry::render(myGLObject* renderingTarget)
{
	if(renderingTarget->isRenderingTargetObject() == false) return; //->

		if(isSelected())
		{
			m_gl_edition_contour->setRenderingMode(LINE_AND_POINT_RENDERING);
			m_gl_seletion_contour->setRenderingColor(m_selection_color);
			m_gl_seletion_contour->render(renderingTarget);

			if(isRenderableObject()
				&& isVisible())
			{
				m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
				m_gl_geometry->render(renderingTarget);
			}

		}
		if(isEdited())
		{
			m_gl_edition_contour->setRenderingMode(LINE_LOOPED_RENDERING);
			m_gl_edition_contour->setRenderingColor(m_edition_color);
			m_gl_edition_contour->render(renderingTarget);

			if(isRenderableObject()
				&& isVisible())

			{
				m_gl_geometry->setRenderingMode(LINE_AND_POINT_RENDERING);
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
			if(isDrawing()) m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);
			else m_gl_geometry->setRenderingMode(LINE_STRIP_RENDERING);

			m_gl_geometry->render(renderingTarget);
		}

}


void myGLScreenMultiLineGeometry::loadGeometry(vector<vec2> Rw_v)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	if(Rw_v.size() <= 2) return; //->

	myGLPolygon* gl_geometry_multiLine = (myGLPolygon*) m_gl_geometry;
	myPolygon* geometry_multiLine = (myPolygon*) m_geometry;

	geometry_multiLine->addPoints(Rw_v);
	gl_geometry_multiLine->addPoints(Rw_v);

	vec2 bottomLeft(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
					geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);
	vec2 topRight(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
				  geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

	gl_selection_rectangle->setDiag(bottomLeft, topRight);
	selection_rectangle->setDiag(bottomLeft, topRight);
	gl_edition_rectangle->setDiag(bottomLeft, topRight);
	edition_rectangle->setDiag(bottomLeft, topRight);

	m_drawingState = DRAWING_FINISHED;
}

void myGLScreenMultiLineGeometry::addPoint(glm::vec2 Rw)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	myGLPolygon* gl_geometry_multiLine = (myGLPolygon*) m_gl_geometry;
	myPolygon* geometry_multiLine = (myPolygon*) m_geometry;

	geometry_multiLine->addPoint(Rw);
	gl_geometry_multiLine->addPoint(Rw);

	vec2 bottomLeft(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
					geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);
	vec2 topRight(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
				  geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

	gl_selection_rectangle->setDiag(bottomLeft, topRight);
	selection_rectangle->setDiag(bottomLeft, topRight);
	gl_edition_rectangle->setDiag(bottomLeft, topRight);
	edition_rectangle->setDiag(bottomLeft, topRight);

}


void myGLScreenMultiLineGeometry::addPoints(std::vector<glm::vec2> Rw_v)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	myGLPolygon* gl_geometry_multiLine = (myGLPolygon*) m_gl_geometry;
	myPolygon* geometry_multiLine = (myPolygon*) m_geometry;

	geometry_multiLine->addPoints(Rw_v);
	gl_geometry_multiLine->addPoints(Rw_v);

	vec2 bottomLeft(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
					geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);
	vec2 topRight(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
				  geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

	gl_selection_rectangle->setDiag(bottomLeft, topRight);
	selection_rectangle->setDiag(bottomLeft, topRight);
	gl_edition_rectangle->setDiag(bottomLeft, topRight);
	edition_rectangle->setDiag(bottomLeft, topRight);

}


void myGLScreenMultiLineGeometry::removePoint(unsigned int point_idx)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	myGLPolygon* gl_geometry_multiLine = (myGLPolygon*) m_gl_geometry;
	myPolygon* geometry_multiLine = (myPolygon*) m_geometry;

	geometry_multiLine->removePoint(point_idx);
	gl_geometry_multiLine->removePoint(point_idx);

	vec2 bottomLeft(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
					geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);
	vec2 topRight(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
				  geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

	gl_selection_rectangle->setDiag(bottomLeft, topRight);
	selection_rectangle->setDiag(bottomLeft, topRight);
	gl_edition_rectangle->setDiag(bottomLeft, topRight);
	edition_rectangle->setDiag(bottomLeft, topRight);

}


void myGLScreenMultiLineGeometry::setPoint(unsigned int pt_idx, glm::vec2 Rw)
{
	myGLRectangle* gl_selection_rectangle = (myGLRectangle*) m_gl_seletion_contour;
	myRectangle* selection_rectangle = (myRectangle*)  m_seletion_contour;
	myGLRectangle* gl_edition_rectangle = (myGLRectangle*) m_gl_edition_contour;
	myRectangle* edition_rectangle = (myRectangle*) m_edition_contour;

	myGLPolygon* gl_geometry_multiLine = (myGLPolygon*) m_gl_geometry;
	myPolygon* geometry_multiLine = (myPolygon*) m_geometry;

	geometry_multiLine->setPoint(pt_idx, Rw);
	gl_geometry_multiLine->setPoint(pt_idx, Rw);

	vec2 bottomLeft(geometry_multiLine->getPoint(geometry_multiLine->getLeftPointIdx()).x,
					geometry_multiLine->getPoint(geometry_multiLine->getBottomPointIdx()).y);
	vec2 topRight(geometry_multiLine->getPoint(geometry_multiLine->getRightPointIdx()).x,
				  geometry_multiLine->getPoint(geometry_multiLine->getTopPointIdx()).y);

	gl_selection_rectangle->setDiag(bottomLeft, topRight);
	selection_rectangle->setDiag(bottomLeft, topRight);
	gl_edition_rectangle->setDiag(bottomLeft, topRight);
	edition_rectangle->setDiag(bottomLeft, topRight);

	m_drawingState = DRAWING_FINISHED;
}



