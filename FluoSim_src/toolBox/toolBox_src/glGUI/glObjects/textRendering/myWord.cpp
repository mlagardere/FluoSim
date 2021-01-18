
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



#include "myWord.h"

using namespace glm;


myWord::myWord(myFont* font, gstd::gProgram* program)
{
	m_font = font;

	if(program !=0) m_program = program;
	else
	{
		string vsWriter_src;
		vsWriter_src +=	"#version 150 core\n"
						"\n"
						"in vec2 r_leftLow_vsIn;\n"
						"in vec2 r_rightHigh_vsIn;\n"
						"in vec2 r_tex_leftLow_vsIn;\n"
						"in vec2 r_tex_rightHigh_vsIn;\n"
						"\n"
						"out vec2 r_leftLow_vsOut;\n"
						"out vec2 r_rightHigh_vsOut;\n"
						"out vec2 r_tex_leftLow_vsOut;\n"
						"out vec2 r_tex_rightHigh_vsOut;\n"
						"\n"
						"uniform float size_letterE;\n"
						"uniform float  size_letter;\n"
						"\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"	r_leftLow_vsOut = size_letter/size_letterE*(r_leftLow_vsIn);\n"
						"	r_rightHigh_vsOut = size_letter/size_letterE*(r_rightHigh_vsIn);\n"
						"	r_tex_leftLow_vsOut = r_tex_leftLow_vsIn;\n"
						"   r_tex_rightHigh_vsOut = r_tex_rightHigh_vsIn;\n"
						"\n"
						"return;\n"
						"}\n";


		string gsWriter_src;
		gsWriter_src +=	"#version 150 core\n"
						"\n"
						"layout(points) in;\n"
						"layout (triangle_strip, max_vertices = 4) out;\n"
						"\n"
						"in vec2 r_leftLow_vsOut[];\n"
						"in vec2 r_rightHigh_vsOut[];\n"
						"in vec2 r_tex_leftLow_vsOut[];\n"
						"in vec2 r_tex_rightHigh_vsOut[];\n"
						"\n"
						"out vec2 r_tex_gsOut;\n"
						"\n"
						"uniform mat2 scaling;\n"
						"uniform mat2 rotation;\n"
						"uniform vec2 origine;\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"	gl_Position = vec4(scaling*(origine + rotation*r_leftLow_vsOut[0]) -vec2(1,1),0,1);\n"
						"	r_tex_gsOut = r_tex_leftLow_vsOut[0];\n"
						"	EmitVertex();\n"
						"\n"
						"	gl_Position = vec4(scaling*(origine + rotation*vec2(r_leftLow_vsOut[0].x,r_rightHigh_vsOut[0].y))-vec2(1,1),0,1);\n"
						"	r_tex_gsOut = vec2(r_tex_leftLow_vsOut[0].x,r_tex_rightHigh_vsOut[0].y);\n"
						"	EmitVertex();\n"
						"\n"
						"	gl_Position = vec4(scaling*(origine + rotation*vec2(r_rightHigh_vsOut[0].x, r_leftLow_vsOut[0].y))-vec2(1,1),0,1);\n"
						"	r_tex_gsOut = vec2(r_tex_rightHigh_vsOut[0].x, r_tex_leftLow_vsOut[0].y);\n"
						"   EmitVertex();\n"
						"\n"
						"	gl_Position = vec4(scaling*(origine + rotation*r_rightHigh_vsOut[0])-vec2(1,1), 0, 1);\n"
						"	r_tex_gsOut = r_tex_rightHigh_vsOut[0];\n"
						"	EmitVertex();\n"
						"\n"
						"return;\n"
						"}\n";


		string fsWriter_src;
		fsWriter_src +=	"#version 150 core\n"
						"\n"
						"in vec2 r_tex_gsOut;\n"
						"\n"
						"out vec4 gl_FragColor;\n"
						"\n"
						"uniform sampler2D font_texture;\n"
						"uniform vec4 color;\n"
						"\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"	gl_FragColor =  color*texture(font_texture, r_tex_gsOut);\n"
						"\n"
						"return;\n"
						"}\n";

		gstd::gShader vs_writer(gstd::VERTEX_SHADER);
			vs_writer.setSource(vsWriter_src, 0);
			vs_writer.compile();

		gstd::gShader gs_writer(gstd::GEOMETRY_SHADER);
			gs_writer.setSource(gsWriter_src, 0);
			string output;
			gs_writer.compile(output);

		gstd::gShader fs_writer(gstd::FRAGMENT_SHADER);
			fs_writer.setSource(fsWriter_src, 0);
			fs_writer.compile();

		m_program = new gstd::gProgram("writer");

		string log;
		m_program->addShader(vs_writer);
		m_program->addShader(gs_writer);
		m_program->addShader(fs_writer);
		m_program->linkShaders(log);
		m_program->useProgram(0);
	}




	m_letter_size_proportion = 0;
	m_letter_size_pxl = 0;
	m_color = vec4(0.0,0.0,1.0,1.0);


	//flag initialization
	m_isTextSet = 0;
	m_isScalable = 0;

}

myWord::~myWord()
{

}





void myWord::draw(myGLWidget* glWidget)
{
	if(!isTextSet() || !m_program->isProgramLink()) return;

	if(isScalable())
	{
		float size_letterE = m_font->getCharacterDescription('e').height;
		mat2 scaling = mat2(2.0f/glWidget->size().x, 0,
							0, 2.0f/glWidget->size().y);

		mat2 rotation = mat2(m_direction.x, m_direction.y,
							 -m_direction.y, m_direction.x);

		glWidget->makeCurrent();
		m_program->useProgram(1);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//attribute
		gstd::myConnector<vec2>::connect(*m_program,"r_leftLow_vsIn", m_r_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program,"r_rightHigh_vsIn", m_r_rightHigh_gv);
		gstd::myConnector<vec2>::connect(*m_program,"r_tex_leftLow_vsIn", m_r_texture_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program,"r_tex_rightHigh_vsIn", m_r_texture_rightHigh_gv);

		//uniform
		gstd::connectUniform(*m_program, "color", &m_color);
		gstd::connectUniform(*m_program, "size_letterE", &size_letterE);
		gstd::connectUniform(*m_program, "size_letter", &m_letter_size_pxl);
		gstd::connectUniform(*m_program, "scaling", &(scaling));
		gstd::connectUniform(*m_program, "rotation", &(rotation));
		gstd::connectUniform(*m_program, "origine", &m_position_pxl);

		//texture
		gstd::connectTexture(*m_program, "font_texture", m_font->getTexture());

		glDrawArrays(GL_POINTS, 0, m_r_leftLow_gv.size());
		glBlendFunc(GL_ONE, GL_ZERO);
		m_program->useProgram(0);
	}

	else
	{
		float size_letterE = m_font->getCharacterDescription('e').height;
		mat2 scaling = mat2(2.0f/glWidget->size().x, 0,
							0, 2.0f/glWidget->size().y);
		mat2 rotation = mat2(m_direction.x, m_direction.y,
							 -m_direction.y, m_direction.x);

		glWidget->makeCurrent();
		m_program->useProgram(1);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0);

		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		//attribute
		gstd::myConnector<vec2>::connect(*m_program, "r_leftLow_vsIn", m_r_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_rightHigh_vsIn", m_r_rightHigh_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_tex_leftLow_vsIn", m_r_texture_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_tex_rightHigh_vsIn",m_r_texture_rightHigh_gv);

		//uniform
		gstd::connectUniform(*m_program, "color", &m_color);
		gstd::connectUniform(*m_program, "size_letterE", &size_letterE);
		gstd::connectUniform(*m_program, "size_letter", &m_letter_size_pxl);
		gstd::connectUniform(*m_program, "scaling", &(scaling));
		gstd::connectUniform(*m_program, "rotation", &(rotation));
		gstd::connectUniform(*m_program, "origine", &m_position_pxl);

		//texture
		gstd::connectTexture(*m_program, "font_texture", m_font->getTexture());

		glDrawArrays(GL_POINTS, 0, m_r_leftLow_gv.size());

		m_program->useProgram(0);

		m_program->useProgram(1);

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);

		//attribute
		gstd::myConnector<vec2>::connect(*m_program, "r_leftLow_vsIn",m_r_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_rightHigh_vsIn",m_r_rightHigh_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_tex_leftLow_vsIn",m_r_texture_leftLow_gv);
		gstd::myConnector<vec2>::connect(*m_program, "r_tex_rightHigh_vsIn",m_r_texture_rightHigh_gv);

		//uniform
		gstd::connectUniform(*m_program, "color", &m_color);
		gstd::connectUniform(*m_program, "size_letterE", &size_letterE);
		gstd::connectUniform(*m_program, "size_letter", &m_letter_size_pxl);
		gstd::connectUniform(*m_program, "scaling", &(scaling));
		gstd::connectUniform(*m_program, "rotation", &(rotation));
		gstd::connectUniform(*m_program, "origine", &m_position_pxl);

		//texture
		gstd::connectTexture(*m_program, "font_texture", m_font->getTexture());

		glDrawArrays(GL_POINTS, 0, m_r_leftLow_gv.size());

		m_program->useProgram(0);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	return;
}



/*
 *	Setters
*/



void myWord::setFont(myFont* font)
{
	m_font = font;
}

void myWord::setProgram(gstd::gProgram* program)
{
	m_program = program;
}

void myWord::setText(string text)
{
    if(!m_font->isLoaded()) return; //not font available
    if(m_word == text) return; //same word

	m_r_leftLow_gv.clear();
	m_r_rightHigh_gv.clear();
	m_r_texture_leftLow_gv.clear();
	m_r_texture_rightHigh_gv.clear();

//	m_r_word_gv.clear();
//	m_r_word_v.clear();

	//0 for min -> 1 for max
	vec2 x_boundary = vec2(pow(10.0f,99.0f),-pow(10.0f,99.0f));
	vec2 y_boundary = vec2(pow(10.0f,99.0f),-pow(10.0f,99.0f));

	vec2 temp_r_leftLow;
	vec2 temp_r_rightHigh;

	vector<vec2> r_leftLow_v;
	vector<vec2> r_rightHigh_v;
	vector<vec2> r_texture_leftLow_v;
	vector<vec2> r_texture_rightHigh_v;

	float current_letterPos = 0;
	charDescription char_description;

	auto letter = text.begin();
	while(letter != text.end())
	{
		char_description = m_font->getCharacterDescription(*letter);

		current_letterPos += char_description.xOffset;

		temp_r_leftLow = vec2(current_letterPos, char_description.yOffset - char_description.height);
			r_leftLow_v.push_back(temp_r_leftLow);
		temp_r_rightHigh = vec2(current_letterPos + char_description.width, char_description.yOffset);
			r_rightHigh_v.push_back(temp_r_rightHigh);

		current_letterPos += char_description.width + char_description.xAdvance;

		r_texture_leftLow_v.push_back(vec2(char_description.x/m_font->getTextureSize().x,
							   (char_description.y + char_description.height)/m_font->getTextureSize().y));
		r_texture_rightHigh_v.push_back(vec2((char_description.x + char_description.width)/m_font->getTextureSize().x,
									 char_description.y/m_font->getTextureSize().y));


        //boundary finding...
		if(temp_r_leftLow.x <= x_boundary[0]) x_boundary[0] = temp_r_leftLow.x;
		if(temp_r_leftLow.y <= y_boundary[0]) y_boundary[0] = temp_r_leftLow.y;
		if(temp_r_rightHigh.x >= x_boundary[1]) x_boundary[1] = temp_r_rightHigh.x;
		if(temp_r_rightHigh.y >= y_boundary[1]) y_boundary[1] = temp_r_rightHigh.y;
        //...boundary finding

		letter++;
	}

	m_r_leftLow_gv.insert(0, r_leftLow_v);
	m_r_rightHigh_gv.insert(0, r_rightHigh_v);

	m_r_texture_leftLow_gv.insert(0, r_texture_leftLow_v);
	m_r_texture_rightHigh_gv.insert(0, r_texture_rightHigh_v);

	m_word = text;
	m_isTextSet = 1;

}

void myWord::setPositionPxl(vec2 position)
{
	m_position_pxl = position;
}

void myWord::setLetterSizePxl(float size)
{
	m_letter_size_pxl = size;
}

void myWord::setColor(vec4 color)
{
	m_color = color;
}

void myWord::setDirection(vec2 direction)
{
	m_direction = normalize(direction);
}

/*
 *	Getters
*/

string myWord::getText()
{
	return m_word;
}

vec2 myWord::getPositionPxl()
{
	return m_position_pxl;
}

float myWord::getLetterSizePxl()
{
	return m_letter_size_pxl;
}

vec4 myWord::getColor()
{
	return m_color;
}

vec2 myWord::getDirection()
{
	return m_direction;
}

/*
 *	Flag Handlers
 *
*/


bool myWord::isTextSet()
{
	return m_isTextSet;

}

bool myWord::isScalable()
{
	return m_isScalable;
}

glm::vec2 myWord::getSizePxl()
{
	int nb_letters = m_r_leftLow_gv.size();
	if(nb_letters == 0 ) return glm::vec2(0,0);

	float size_letterE = m_font->getCharacterDescription('e').height;

	vec2 word_size = (glm::vec2) m_r_rightHigh_gv[nb_letters-1] - (glm::vec2)m_r_leftLow_gv[0];
	for(int letter_idx = 0; letter_idx <= nb_letters-1; letter_idx++)
	{
		if(((vec2)m_r_rightHigh_gv[letter_idx]).y >= word_size.y) word_size.y = ((vec2) m_r_rightHigh_gv[letter_idx]).y;
	}

	(glm::vec2) m_r_rightHigh_gv[nb_letters-1] - (glm::vec2)m_r_leftLow_gv[0];
	vec2 word_sizePxl = (m_letter_size_pxl/size_letterE)*word_size;

	return word_sizePxl;
}


