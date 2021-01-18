
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



#include "Signal.h"

using namespace std;
using namespace glm;

Signal::Signal() :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT,
	myGLObject::GLWIDGET_TYPE)


{
	m_contour_rect = new myRectangle(m_coutour_rectCoords);

    m_program = new gstd::gProgram("init");
    m_isProgram_set = 0;

	string vs_source_str;

	vs_source_str = "#version 150 core\n"
					"in vec2 R;\n"
					"//in vec4 color_in;\n"
					"//out vec4 color;\n"
					"uniform mat4 scaling_matrix;\n"
					"\n"
					"void main(void)\n"
					"{\n"
						"vec4 temp = vec4(R, 0.0, 1.0);\n"
						"gl_Position = scaling_matrix*temp;\n"
						"//color = vec4(0.0,1.0,0.0,1.0);\n"
					"}\n";

	string fs_source_str = "#version 150 core\n"
					"\n"
					"//in vec4 color;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"uniform vec4 color_uni;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	gl_FragColor=color_uni;\n"
					"}\n";

	gstd::gShader simpleDrawing_vs(gstd::VERTEX_SHADER);
		simpleDrawing_vs.setSource(vs_source_str, 0);
		simpleDrawing_vs.compile();

	gstd::gShader simpleDrawing_fs(gstd::FRAGMENT_SHADER);
		simpleDrawing_fs.setSource(fs_source_str, 0);
		simpleDrawing_fs.compile();

	gstd::gProgram program("program");
		program.addShader(simpleDrawing_vs);
		program.addShader(simpleDrawing_fs);
		program.linkShaders();

	setProgram(program);
	m_color = vec4(1,1,1,1);
	m_nbPts_insideRect = 0;
}


Signal::~Signal()
{
	delete m_contour_rect;

}

//"getters"
vec4 Signal::getColor()
{
    return m_color;
}

vec2 Signal::getValue(int value_idx)
{
    return  m_values[value_idx];
}

long Signal::getSize()
{
    return m_values.size();
}

vector<vec2> Signal::getValues_v(int value_start_idx, int value_end_idx)
{
    vector<vec2> temp_r(m_values.begin()+value_start_idx, m_values.begin()+value_end_idx+1);

    return  temp_r;
}

vector<vec2>& Signal::getValuesRef_v()
{
    return m_values;
}

//"setters"
void Signal::setColor(vec4 color)
{
    m_color = color;
}

void Signal::setValue(vec2 value, int value_idx)
{
    m_values[value_idx] = value;
    m_values_gv.clear();
    m_values_gv = gstd::gVector<vec2>(m_values);

	m_nbPts_insideRect = 0;

	computeContourRect();

}

void Signal::setProgram(gstd::gProgram program)
{
    *m_program = program;
    m_isProgram_set = 1;
}

void Signal::addValue(vec2 value)
{
    m_values.push_back(value);
	m_values_gv.push_back(value);
	computeContourRect();
}


void Signal::addValues(vector<vec2> values_v)
{
    m_values.insert(m_values.end(), values_v.begin(), values_v.end());
	m_values_gv.insert(m_values_gv.size(), values_v);
	computeContourRect();
}

void Signal::addValues(string file_dir)
{

}

//other
void Signal::addValues(vec2* values_a, int nb_values)
{
    for(int value_idx = 0; value_idx <= nb_values-1; value_idx++)
    {
        m_values.push_back(values_a[value_idx]);
		m_values_gv.push_back(values_a[value_idx]);
    }
	computeContourRect();
}

void Signal::clearValues()
{
    m_values.clear();
    m_values_gv.clear();

	m_nbPts_insideRect = 0;
}

void Signal::coutSignal()
{
    for(int value_idx = 0; value_idx <= m_values.size()-1; value_idx++)
    {
		cout<<m_values[value_idx].x<<"__"<<m_values[value_idx].y<<"\n";
    }
}

void Signal::saveSignal(string file_dir)
{
	ofstream file(file_dir.data());

	for(int value_idx = 0; value_idx <= m_values.size()-1; value_idx++)
	{
		file<<m_values[value_idx].x<<"\t"<<m_values[value_idx].y<<"\n";
	}
	file.close();
}

void Signal::render(myGLObject* screen)
{
	int nb_points = m_values.size();
	if(m_isProgram_set && isVisible() == true)
    {
		mat4 worldExtendedHom_matrix = screen->getWorldToExtendedHomMatrix();
		m_program->useProgram(1);

		gstd::myConnector<vec2>::connect(*m_program, string("R"), m_values_gv);
		gstd::connectUniform(*m_program, "color_uni", &m_color);
		gstd::connectUniform(*m_program, "scaling_matrix", &worldExtendedHom_matrix);

		glDrawArrays(GL_LINE_STRIP, 0, nb_points);
        glFlush();

        m_program->useProgram(0);
    }
}

gstd::gVector<vec2> Signal::getValues_gv()
{
    return m_values_gv;
}

void Signal::computeContourRect()
{
	int nb_values = m_values.size();
	if(nb_values == m_nbPts_insideRect) return; //->

	float max = numeric_limits<float>::max();
		vec2 bottom_left(max, max);
		vec2 top_right(-max, -max);

	if(m_nbPts_insideRect == 0)
	{
		bottom_left = vec2(max, max);
		top_right = vec2(-max, -max);
	}
	else
	{
		bottom_left = m_contour_rect->getBottomLeft();
		top_right = m_contour_rect->getTopRight();
	}

	for(int value_idx = m_nbPts_insideRect; value_idx <= nb_values-1; value_idx++)
	{
		if(m_values[value_idx].x <= bottom_left.x) bottom_left.x = m_values[value_idx].x;
		if(m_values[value_idx].y <= bottom_left.y) bottom_left.y = m_values[value_idx].y;

		if(m_values[value_idx].x >= top_right.x) top_right.x = m_values[value_idx].x;
		if(m_values[value_idx].y >= top_right.y) top_right.y = m_values[value_idx].y;
	}

	m_contour_rect->setDiag(bottom_left, top_right);
	m_nbPts_insideRect = m_values.size();
}

myRectangle& Signal::getRectangleContour()
{
	return *m_contour_rect;
}

int Signal::getNbPtsInsideRect()
{
	return m_nbPts_insideRect;
}

float Signal::getMean()
{
	float mean = 0;

	for(vec2 value : m_values)
	{
		mean += value.y;
	}

	mean /= m_values.size();
	return mean;
}
