
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

#ifndef MYWORD_H
#define MYWORD_H

#include "gpuTools_library_global.h"
#include "gl/gProgram.h"
#include "gl/gVector.h"
#include "gl/gToolBox.h"

#include "glm.hpp"
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_access.hpp>

#include "myFont.h"

#include "gpuTools_library_global.h"
#include "gl/gProgram.h"
#include "gl/gVector.h"
#include "gl/gToolBox.h"

#include "toolbox_library_global.h"
#include "glGUI/glWidgets/myGLWidget.h"


class TOOLBOXSHARED_EXPORT myWord
{
public :

	myWord(myFont* font, gstd::gProgram* program = 0);
	~myWord();

	void draw(myGLWidget* glWidget);

	void setFont(myFont* font);
	void setProgram(gstd::gProgram* program);
	void setText(string text);
	void setPositionPxl(glm::vec2 position);
	void setLetterSizePxl(float size);
	void setColor(glm::vec4 color);
	void setDirection(glm::vec2 direction);

	string getText();
	glm::vec2 getPositionPxl();
	float getLetterSizePxl();
	glm::vec4 getColor();
	glm::vec2 getDirection();
	glm::vec2 getSize();
	glm::vec2 getSizePxl();

	bool isTextSet();
	bool isScalable();

private:

//gpu
	myFont* m_font;
	gstd::gProgram* m_program;

	//letters
	gstd::gVector<glm::vec2> m_r_leftLow_gv;
	gstd::gVector<glm::vec2> m_r_rightHigh_gv;

	gstd::gVector<glm::vec2> m_r_texture_leftLow_gv;
	gstd::gVector<glm::vec2> m_r_texture_rightHigh_gv;

//cpu
	string m_word;
	glm::vec2 m_position_pxl;

	float m_letter_size_pxl;
	float m_letter_size_proportion;

	glm::vec4 m_color;
	glm::vec2 m_direction;

	//flags
	bool m_isTextSet;
	bool m_isScalable;
};

#endif // MYWORD_H
