
/*
 *  FluoSim (simulation program of live cell fluorescence microscopy experiments)
 *  Copyright (C) 2020 Matthieu Lagardère
 *
 *	FluoSim is free software: you can redistribute it and/or modify
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

#ifndef GPU_OTHERFUNCTIONS_H
#define GPU_OTHERFUNCTIONS_H

#include "stdlib.h"
#include "time.h"
#include "cstdlib"
#include "glm.hpp"
#include "vector"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "math.h"

namespace gstd
{

void dataBiner(double *data_in, int n_data, double* *data_out_x, double* *data_out_y,
			   int n_subdivision, double x_min_imposed=-1, double x_max_imposed=-1, int type=0);
int convertCharToInt(std::vector <char> numberChar);
int readNumber(char* file_str, int readIndex, int* endReadIndex, char end_char =' ');
float uniformDistributiion(float min_value, float max_value);
int uniformDistributiion(int min_value, int max_value);
bool binomial_1(float p);
glm::vec2 random_gaussian (float sigma);

class myGaussainRNG
{
public:

	myGaussainRNG();
	void generateRandomVectors(int n);
	glm::vec2 getRandomVector();

private:

	std::vector<glm::vec2> m_random_vector;
};

std::string floatToString(float my_float);

}

#endif // GPU_OTHERFUNCTIONS_H
