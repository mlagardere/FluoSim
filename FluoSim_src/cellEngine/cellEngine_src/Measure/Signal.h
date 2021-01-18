
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

#ifndef SIGNAL_H
#define SIGNAL_H

#include "cellEngine_library_global.h"


#include "stdlib.h"
#include "vector"
#include "string"
#include <fstream>
#include "iostream"
#include "numeric"

#include "glm.hpp"
    #include <gtc/type_ptr.hpp>
    #include <gtc/matrix_transform.hpp>
    #include <gtc/matrix_access.hpp>

#include "gpuTools_library_global.h"
	#include "gl/gProgram.h"
	#include "gl/gVector.h"
	#include "gl/gToolBox.h"

#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/geometry/myGeomtricObject.h"
	#include "glGUI/glObjects/myGLObject.h"


class CELLENGINE_LIBRARYSHARED_EXPORT Signal: public myGLObject
{
public :

    Signal();
    ~Signal();

	glm::vec4 getColor();
	glm::vec2 getValue(int value_idx);
    long getSize();
	vector<glm::vec2> getValues_v(int value_start_idx, int value_endIncluded_idx);
	gstd::gVector<glm::vec2> getValues_gv();
	vector<glm::vec2>& getValuesRef_v();
	myRectangle& getRectangleContour();
	float getMean();

	void setColor(glm::vec4 color);
	void setValue(glm::vec2 value, int value_idx);
	void addValue(glm::vec2 value);
	void addValues(vector<glm::vec2> values_v);
	void addValues(glm::vec2* values_a, int nb_values);
    void addValues(string file_dir);
	void clearValues();

    void coutSignal(); //console
	void saveSignal(string file_dir); //file
	void render(myGLObject* screen); //to be actually displayed


protected :

//internal functions
	void setProgram(gstd::gProgram program);
	void computeContourRect();
	int getNbPtsInsideRect();

private:

	vector<glm::vec2> m_values;
	glm::vec4 m_color;

//GPU Memory Data
	gstd::gVector<glm::vec2> m_values_gv;
    gstd::gProgram* m_program;
        bool m_isProgram_set;

	myRectangle* m_contour_rect; //*!
		myMultiVector<glm::vec2> m_coutour_rectCoords;
		int m_nbPts_insideRect;

};
















#endif // SIGNAL_H
