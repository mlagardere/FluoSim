
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



#include "Graphic.h"

using namespace std;


Graphic::Graphic(QApplication* app, QWidget* parent, QGLWidget* sharedContext_glWdgt) :

	myGLWidget(200,200, parent, sharedContext_glWdgt)

{
	this->makeCurrent();

	m_isIsometric = false;
	m_XAxis = new Axis(Axis::HORIZONTAL,0,m_axisGeom_gMV);
	m_YAxis = new Axis(Axis::VERTICAL,0,m_axisGeom_gMV);

	m_graphicMode = FITTEST_MODE;
    m_app = app;
}

Graphic::~Graphic()
{
	delete m_XAxis;
	delete m_YAxis;
}


void Graphic::Graphic::renderSignals()
{
	switch(m_graphicMode)
	{
		case FITTEST_MODE :
		{
			float max = numeric_limits<float>::max();
				glm::vec2 bottom_left(max, max);
				glm::vec2 top_right(-max, -max);

            for(Signal* signal : m_signals_v)
			{
				if(signal->isVisible() == false) continue; //<

				if(signal->getRectangleContour().getBottomLeft().x < bottom_left.x )
				{
					bottom_left.x = signal->getRectangleContour().getBottomLeft().x;
				}

				if(signal->getRectangleContour().getBottomLeft().y < bottom_left.y )
				{
					bottom_left.y = signal->getRectangleContour().getBottomLeft().y;
				}

				if(signal->getRectangleContour().getTopRight().x > top_right.x )
				{
					top_right.x = signal->getRectangleContour().getTopRight().x;
				}

				if(signal->getRectangleContour().getTopRight().y > top_right.y )
				{
					top_right.y = signal->getRectangleContour().getTopRight().y;
				}
			}

			if(bottom_left.y == top_right.y)
			{
				if(bottom_left.y >= 0) bottom_left.y = 0.0f;
				if(top_right.y <= 0) top_right.y = 0.0f;
			}

			glm::vec2 bottom_left_h, top_right_h;
				bottom_left_h = 0.85f*glm::vec2(-1,-1);
				top_right_h = 0.85f*glm::vec2(+1,+1);

			this->fitQuadToQuad(bottom_left, top_right,
								bottom_left_h, top_right_h);


            for(Signal* signal : m_signals_v)
			{
				signal->render(this);
			}

			m_XAxis->setAxisOffset(0);
			m_YAxis->setAxisOffset(bottom_left.x);

			m_XAxis->render(this);
			m_YAxis->render(this);

		}
		break;

		case ORIGIN_VISIBLE_MODE :
		{
			float max = numeric_limits<float>::max();
				glm::vec2 bottom_left(max, max);
				glm::vec2 top_right(-max, -max);

            for(Signal* signal : m_signals_v)
			{
				if(signal->isVisible() == false) continue; //<

				if(signal->getRectangleContour().getBottomLeft().x < bottom_left.x )
				{
					bottom_left.x = signal->getRectangleContour().getBottomLeft().x;
				}

				if(signal->getRectangleContour().getBottomLeft().y < bottom_left.y )
				{
					bottom_left.y = signal->getRectangleContour().getBottomLeft().y;
				}

				if(signal->getRectangleContour().getTopRight().x > top_right.x )
				{
					top_right.x = signal->getRectangleContour().getTopRight().x;
				}

				if(signal->getRectangleContour().getTopRight().y > top_right.y )
				{
					top_right.y = signal->getRectangleContour().getTopRight().y;
				}
			}

			if(bottom_left.y >= 0) bottom_left.y = 0.0f;
			if(top_right.y <= 0) top_right.y = 0.0f;

			glm::vec2 bottom_left_h, top_right_h;
				bottom_left_h = 0.85f*glm::vec2(-1,-1);
				top_right_h = 0.85f*glm::vec2(+1,+1);

			this->fitQuadToQuad(bottom_left, top_right,
								bottom_left_h, top_right_h);


            for(Signal* signal : m_signals_v)
			{
				signal->render(this);
			}

			m_XAxis->setAxisOffset(0);
			m_YAxis->setAxisOffset(bottom_left.x);

			m_XAxis->render(this);
			m_YAxis->render(this);
		}
		break;
	}
}

void Graphic::addSignal(Signal* signal)
{
	m_signals_v.push_back(signal);
}

void Graphic::removeSignal(Signal* signal)
{
	auto sig = std::find(m_signals_v.begin(), m_signals_v.end(), signal);

	if(sig == m_signals_v.end()) return; //->
	else m_signals_v.erase(sig);
}

void Graphic::clearSignals()
{
	m_signals_v.clear();
}

void Graphic::setXAxisText(string text)
{
	m_XAxis->setText(text);
}

void Graphic::setYAxisText(string text)
{
	m_YAxis->setText(text);
}

void Graphic::drawScreen()
{

}

void Graphic::clearScreen()
{

}
