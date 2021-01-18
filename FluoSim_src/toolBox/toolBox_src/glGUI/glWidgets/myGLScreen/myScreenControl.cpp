
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



#define GLM_SWIZZLE


#include "myGLScreen.h"

using namespace std;
using namespace glm;

myGLScreen::myGLScreen(int x, int y, QWidget* parent) : myGLScreenModel(x, y, parent)
{

}

myGLScreen::~myGLScreen()
{
}

void myGLScreen::treatEvent(QEvent* event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent* key_event = (QKeyEvent*) event;
		QWidget::keyPressEvent(key_event);
	}

	switch(m_screen_mode)
	{
		case MOVEAROUND_MODE:
		{
			if(event->type() == QEvent::Wheel)
			{
				QWheelEvent* wheel_event = (QWheelEvent*) event;

				float delta = wheel_event->delta()/8.0f;
				vec2 R_wheel = vec2(wheel_event->x(), wheel_event->y());
				vec2 r_wheel = fromScrnToHomCoordinate(R_wheel);

				float alpha = 1+0.02*delta;
					if(alpha <=0) alpha = 1;
				myGLScreenModel::setTranslationVector(vec3(vec2(alpha*myGLScreenModel::getTranslationVector().x,
																alpha*myGLScreenModel::getTranslationVector().y) -(alpha-1)*r_wheel
																,0.0));
				myGLScreenModel::multiplyScalingMatrix(alpha);
			}

			if(event->type() == QEvent::MouseButtonPress)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				switch(mouse_event->button())
				{
					case Qt::LeftButton :
					{
						m_mouse_state.left_button = CLICKED;

						m_mouse_state.last_pos[0] = mouse_event->x();
						m_mouse_state.last_pos[1] = mouse_event->y();

						m_mouse_state.last_translationVector = vec2(myGLScreenModel::getTranslationVector().x,
																	myGLScreenModel::getTranslationVector().y);
					}
					break;

					case Qt::RightButton :
					{
						m_mouse_state.right_button = CLICKED;
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
					case Qt::LeftButton :
					{   m_mouse_state.left_button = UNCLICKED;}
					break;

					case Qt::RightButton :
					{	m_mouse_state.right_button = UNCLICKED;}
					break;
					default : break;
				}
			}

			if(event->type() == QEvent::MouseMove)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				if(m_mouse_state.left_button == CLICKED)
				{
					vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
					vec2 delta_R = R_move - m_mouse_state.last_pos;

                    //new translation vector in the homogeneous space
//					vec2 t_new = m_mouse_state.last_translationVector + fromScrnToHomMatrix() * delta_R;
					vec2 t_new = m_mouse_state.last_translationVector +
								 fromScrnToHomCoordinate(R_move) - fromScrnToHomCoordinate(m_mouse_state.last_pos);
					myGLScreenModel::setTranslationVector(vec3(t_new, 0.0));
				}
			}
		}
		break;

		case MOVEAROUND_AND_SELECTION_MODE:
		{
			if(event->type() == QEvent::MouseButtonPress)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
				vec2 Rw_click = fromScrnToWorldCoordinate(R_click);


				if(mouse_event->button() == Qt::LeftButton)
				{
				// *** SELECTION_PART ***
					// *** OFF_FOCUS ***
					if(m_onFocus_object == 0)
					{
						myGLObject* selected_obj = 0;
						if(isInsideASelectionContour(Rw_click, selected_obj) == true)
						{
							m_onFocus_object = selected_obj;
							m_onFocus_object->select(1);
						}
					}

					// *** ON_FOCUS ***
					if(m_onFocus_object != 0 &&
					   m_onFocus_object->isSelectionableObject() == true &&
					   m_onFocus_object->isSelected() == true)
					{
						myGLObject* selected_obj = 0;
						if(isInsideASelectionContour(Rw_click, selected_obj) == true)
						{
							if(selected_obj != m_onFocus_object)
							{
								m_onFocus_object->select(0);
								m_onFocus_object = selected_obj;
								m_onFocus_object->select(1);
							}
							else //same object
							{
								m_onFocus_object->treatEvent(event);
							}
						}
						else //could be used to desactive the object
						{
							m_onFocus_object->treatEvent(event);
						}
					}

				// *** MOVEAROUND_PART ***
					m_mouse_state.left_button = CLICKED;

					m_mouse_state.last_pos[0] = mouse_event->x();
					m_mouse_state.last_pos[1] = mouse_event->y();

					m_mouse_state.last_translationVector = vec2(myGLScreenModel::getTranslationVector().x,
																myGLScreenModel::getTranslationVector().y);


				}


				if(mouse_event->button() == Qt::RightButton)
				{
				// *** SELECTION_PART ***
					// *** ON_FOCUS ***
					if(m_onFocus_object != 0 &&
					   m_onFocus_object->isSelectionableObject() == true &&
					   m_onFocus_object->isSelected() == true)
					{
						m_onFocus_object->select(0);
						m_onFocus_object = 0;
					}

				// *** MOVEAROUND_PART ***
					m_mouse_state.right_button = CLICKED;

					m_mouse_state.last_pos[0] = mouse_event->x();
					m_mouse_state.last_pos[1] = mouse_event->y();

					m_mouse_state.last_translationVector = vec2(myGLScreenModel::getTranslationVector().x,
																myGLScreenModel::getTranslationVector().y);

				}
			}

			if(event->type() == QEvent::MouseButtonRelease)
			{
				if(m_onFocus_object != 0 &&
				   m_onFocus_object->isSelectionableObject() == true &&
				   m_onFocus_object->isSelected() == true)
				{

					m_onFocus_object->treatEvent(event);
				}

				// *** MOVEAROUND_PART ***
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				switch(mouse_event->button())
				{
					case Qt::LeftButton :
					{   m_mouse_state.left_button = UNCLICKED;}
					break;

					case Qt::RightButton :
					{	m_mouse_state.right_button = UNCLICKED;}
					break;
					default : break;
				}
			}

			if(event->type() == QEvent::MouseMove)
			{
				// *** MOVEAROUND_PART ***
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				if(m_mouse_state.left_button == CLICKED)
				{
					vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
					vec2 delta_R = R_move - m_mouse_state.last_pos;

                    //new translation vector in the homogeneous space
//					vec2 t_new = m_mouse_state.last_translationVector + fromScrnToHomMatrix() * delta_R;
//					vec2 t_new = m_mouse_state.last_translationVector + inv_screen_matrix * fromScrnToHomMatrix() * delta_R;
					vec2 t_new = m_mouse_state.last_translationVector +
								fromScrnToHomCoordinate(R_move) - fromScrnToHomCoordinate(m_mouse_state.last_pos);
					myGLScreenModel::setTranslationVector(vec3(t_new, 0.0));
				}

				// *** SELECTION_PART ***
				if(m_onFocus_object != 0 &&
				   m_onFocus_object->isSelectionableObject() == true &&
				   m_onFocus_object->isSelected() == true)
				{
					m_onFocus_object->treatEvent(event);
				}
			}

			if(event->type() == QEvent::Wheel)
			{
				// *** MOVEAROUND_PART ***
				QWheelEvent* wheel_event = (QWheelEvent*) event;

				float delta = wheel_event->delta()/8.0f;
				vec2 R_wheel = vec2(wheel_event->x(), wheel_event->y());
				vec2 r_wheel = fromScrnToHomCoordinate(R_wheel);

				float alpha = 1.0f+0.02f*delta;
				if(alpha <=0) alpha = 1.0f;
				myGLScreenModel::setTranslationVector(vec3(vec2(alpha*myGLScreenModel::getTranslationVector().x,
																alpha*myGLScreenModel::getTranslationVector().y) -(alpha-1)*r_wheel
																,0.0));
				myGLScreenModel::multiplyScalingMatrix(alpha);
			}

			if(event->type() == QEvent::MouseButtonDblClick)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;

				vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
				vec2 Rw_click = fromScrnToWorldCoordinate(R_click);

				myGLObject* edited_obj = 0;
				isInsideAnEditionContour(Rw_click, edited_obj);
				if(isInsideAnEditionContour(Rw_click, edited_obj) == true)
				{
					m_onFocus_object->select(0);
					m_onFocus_object->edit(1);

					m_screen_mode = EDITION_MODE;
					treatEvent(event);
				}
			}

		}
		break;

		case EDITION_MODE:
		{			
			if(event->type() == QEvent::MouseButtonPress)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
				vec2 Rw_click = fromScrnToWorldCoordinate(R_click);

				vec2 radius_w_v = fromScrnToWorldCoordinate(vec2(10,10))
								  - fromScrnToWorldCoordinate(vec2(0,0));
				float radius_w = length(radius_w_v);



				if(mouse_event->button() == Qt::LeftButton)
				{
					//** ON_FOCUS ***
					if(m_onFocus_object != 0 &&
					   m_onFocus_object->isEditableObject() == true &&
					   m_onFocus_object->isEdited() == true)
					{
						myGLObject* selected_obj = 0;
						if(isInsideASelectionContour(Rw_click, selected_obj) == true)
						{
							int _vert_idx;
							if(selected_obj != m_onFocus_object &&
							   m_onFocus_object->isNextToAnEditionContourVertex(Rw_click, radius_w, _vert_idx) == false)
							{
								m_onFocus_object->edit(0);
								m_onFocus_object = selected_obj;
								m_onFocus_object->select(1);

								m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
								treatEvent(event);
							}
							else //same object
							{
								int _vert_idx;
								if(m_onFocus_object->isInsideEditionContour(Rw_click) == true ||
								   m_onFocus_object->isNextToAnEditionContourVertex(Rw_click, radius_w, _vert_idx) == true)
								{
									m_onFocus_object->treatEvent(event);
								}
								else
								{
									m_onFocus_object->edit(0);
									m_onFocus_object = 0;

									m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
									treatEvent(event);
								}
							}
						}
						else
						{
							int _vert_idx;
							if(m_onFocus_object->isInsideEditionContour(Rw_click) == true ||
							   m_onFocus_object->isNextToAnEditionContourVertex(Rw_click, radius_w, _vert_idx) == true)
							{
								m_onFocus_object->treatEvent(event);
							}
							else
							{
								m_onFocus_object->edit(0);
								m_onFocus_object = 0;

								m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
								treatEvent(event);
							}
						}
					}
				}


				if(mouse_event->button() == Qt::RightButton)
				{
					// *** ON_FOCUS ***
					if(m_onFocus_object != 0 &&
					   m_onFocus_object->isEditableObject() == true &&
					   m_onFocus_object->isEdited() == true)
					{
						m_onFocus_object->edit(0);
						m_onFocus_object = 0;

						m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
						treatEvent(event);
					}
				}
			}

			if(event->type() == QEvent::MouseButtonRelease)
			{
				if(m_onFocus_object != 0 &&
				   m_onFocus_object->isEditableObject() == true &&
				   m_onFocus_object->isEdited() == true)
				{

					m_onFocus_object->treatEvent(event);
				}


				// *** MOVEAROUND_PART ***

				QMouseEvent* mouse_event = (QMouseEvent*) event;
				switch(mouse_event->button())
				{
					case Qt::LeftButton :
					{   m_mouse_state.left_button = UNCLICKED;}
					break;

					case Qt::RightButton :
					{	m_mouse_state.right_button = UNCLICKED;}
					break;
					default : break;
				}
			}

			if(event->type() == QEvent::MouseMove)
			{
				if(m_onFocus_object != 0 &&
				   m_onFocus_object->isEditableObject() == true &&
				   m_onFocus_object->isEdited() == true)
				{

					m_onFocus_object->treatEvent(event);
				}
			}

			if(event->type() == QEvent::Wheel)
			{
				// *** MOVEAROUND_PART ***

				QWheelEvent* wheel_event = (QWheelEvent*) event;

				float delta = wheel_event->delta()/8.0f;
				vec2 R_wheel = vec2(wheel_event->x(), wheel_event->y());
				vec2 r_wheel = fromScrnToHomCoordinate(R_wheel);

				float alpha = 1+0.02*delta;
				if(alpha <=0) alpha = 1.0f;
				myGLScreenModel::multiplyScalingMatrix(alpha);
				myGLScreenModel::setTranslationVector(vec3(vec2(alpha*myGLScreenModel::getTranslationVector().x,
																alpha*myGLScreenModel::getTranslationVector().y) -(alpha-1)*r_wheel
																,0.0));
			}

		}
		break;

		case DRAWING_MODE:
		{
			//if not, transfer the event to the object to be processed!!
			if(m_onFocus_object) m_onFocus_object->treatEvent(event);
				if(m_onFocus_object && m_onFocus_object->isAborted())
				{
					removeObject(m_onFocus_object);
					m_onFocus_object = 0;
					m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
				}

				if(m_onFocus_object && m_onFocus_object->isDrawn())
				{
					m_onFocus_object->objectHasBeenDrawn();
					m_onFocus_object = 0;
					m_screen_mode = MOVEAROUND_AND_SELECTION_MODE;
				}

			if(event->type() == QEvent::Wheel)
			{
				QWheelEvent* wheel_event = (QWheelEvent*) event;

				float delta = wheel_event->delta()/8.0f;
				vec2 R_wheel = vec2(wheel_event->x(), wheel_event->y());
				vec2 r_wheel = fromScrnToHomCoordinate(R_wheel);

				float alpha = 1+0.02*delta;
				myGLScreenModel::multiplyScalingMatrix(alpha);
				myGLScreenModel::setTranslationVector(vec3(vec2(alpha*myGLScreenModel::getTranslationVector().x,
																alpha*myGLScreenModel::getTranslationVector().y) -(alpha-1)*r_wheel
																,0.0));
			}

		}
		break;
	}

}


/*
void myGLScreen::treatEvent(QEvent* event)
{
	if(event->type() == QEvent::Wheel)
	{
		QWheelEvent* wheel_event = (QWheelEvent*) event;

		float delta = wheel_event->delta()/8.0f;
		vec2 R_wheel = vec2(wheel_event->x(), wheel_event->y());
		vec2 r_wheel = fromScrnToHomCoordinate(R_wheel);

		float alpha = 1+0.02*delta;
		myGLScreenModel::multiplyScalingMatrix(alpha);
		myGLScreenModel::setTranslationVector(vec3(vec2(alpha*myGLScreenModel::getTranslationVector().x,
														alpha*myGLScreenModel::getTranslationVector().y) -(alpha-1)*r_wheel
														,0.0));
	}

	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyPress_event = (QKeyEvent*) event;

		if(keyPress_event->key() == Qt::Key_Delete && m_onFocus_object != 0) //abandon drawing mode
		{
			m_onFocus_object->draw(0);
			m_onFocus_object->edit(0);
			m_onFocus_object->select(0);

			myGLObject* old_onFocus_object = m_onFocus_object;
			m_onFocus_object = 0;
			// to avoid any risk...if deletion triggers signals that change the onFocus_object, that's great! If not
			//we do not have to worry about it, it has been set to 0...

			removeObject(old_onFocus_object);
			m_screen_mode = EDITION_AND_SELECTION_MODE;
		}
		else
		{
		}
	}

	switch (m_screen_mode)
	{
	case MOVEAROUND_MODE:
	{
		//lateral displacement
		if(event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouse_event = (QMouseEvent*) event;
			switch(mouse_event->button())
			{
				case Qt::LeftButton :
				{
					m_mouse_state.left_button = CLICKED;

					m_mouse_state.last_pos[0] = mouse_event->x();
					m_mouse_state.last_pos[1] = mouse_event->y();

					m_mouse_state.last_translationVector = vec2(myGLScreenModel::getTranslationVector().x,
																myGLScreenModel::getTranslationVector().y);
				}
				break;

				case Qt::RightButton :
				{
					m_mouse_state.right_button = CLICKED;
				}
				break;
			}
		}

		if(event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouse_event = (QMouseEvent*) event;
			switch(mouse_event->button())
			{
				case Qt::LeftButton :
				{   m_mouse_state.left_button = UNCLICKED;}
				break;

				case Qt::RightButton :
				{	m_mouse_state.right_button = UNCLICKED;}
				break;
			}
		}

		if(event->type() == QEvent::MouseMove)
		{
			QMouseEvent* mouse_event = (QMouseEvent*) event;
			if(m_mouse_state.left_button == CLICKED)
			{
				vec2 R_move = vec2(mouse_event->x(), mouse_event->y());
				vec2 delta_R = R_move - m_mouse_state.last_pos;

				vec2 t_new = m_mouse_state.last_translationVector + fromScrnToHomMatrix() * delta_R;
				myGLScreenModel::setTranslationVector(vec3(t_new, 0.0));
			}
		}
	}
	break;


	case DRAWING_MODE:
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyPress_event = (QKeyEvent*) event;
			if(keyPress_event->key() == Qt::Key_Escape) //abandon drawing mode
			{
				m_onFocus_object->draw(0);
				removeObject(m_onFocus_object);
				m_onFocus_object = 0;
				m_screen_mode = EDITION_AND_SELECTION_MODE;
			}
		}

		//if not, transfer the event to the object to be processed!!
		else
		{
			if(m_onFocus_object) m_onFocus_object->treatEvent(event);
				if(m_onFocus_object && m_onFocus_object->isAborted())
				{
					removeObject(m_onFocus_object);
					m_onFocus_object = 0;
					m_screen_mode = EDITION_AND_SELECTION_MODE;
				}
				if(m_onFocus_object && m_onFocus_object->isDrawn())
				{
					m_onFocus_object = 0;
					m_screen_mode = EDITION_AND_SELECTION_MODE;
				}
		}
	}
	break;

	case EDITION_AND_SELECTION_MODE:
	{
		//NO FOCUS OBJECT
		if(m_onFocus_object == 0) // we try to find an object !
		{
			if(event->type() == QEvent::MouseButtonPress)
			{
				QMouseEvent* mouse_event = (QMouseEvent*) event;
				if(mouse_event->button() == Qt::LeftButton)
				{
					vec2 R_click = vec2(mouse_event->x(), mouse_event->y());
					vec2 Rw_click = fromScrnToWorldCoordinate(R_click);

					myGLObject* selected_obj = 0;
					if(isInsideASelectionContour(Rw_click, selected_obj) == true)
					{
						m_onFocus_object = selected_obj;
						m_onFocus_object->select(1);
					}
				}
			}
		}

		//THERE IS A FOCUS OBJECT
		if(m_onFocus_object != 0)
		{
			//IT IS SELECTED
			if(m_onFocus_object->isSelected())
			{
				m_onFocus_object->treatEvent(event);
				if(!m_onFocus_object->isSelected() && !m_onFocus_object->isEdited())
				{
					m_onFocus_object = 0;
					treatEvent(event);
				}
			}

			//IT IS EDITED
			if(m_onFocus_object != 0 && m_onFocus_object->isEdited())
			{
				m_onFocus_object->treatEvent(event);
				if(!m_onFocus_object->isEdited())
				{
					m_onFocus_object = 0;
					treatEvent(event);
				}
			}
		}
	}
	break;

	}

} //function_end


*/
