
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

#ifndef GPU_STRING_MANIPULATION_H
#define GPU_STRING_MANIPULATION_H


#include "stdlib.h"
#include "iostream"
#include <fstream>
#include "vector"


using namespace std;

namespace gstd
{

//see C-str for tokens...
bool getWord(string &considered_string,
             string &word, int beg_pos,
             int &end_pos,
             string separating_char = string(" \n\t"),
             string unaccepted_startingChar = " \n\t/");

bool getSubWord(string& considered_str,
                string& subWord,
                string left_separators,
                string right_separators);



bool getLine(string &considered_string, string &line, int beg_pos, int &end_pos);


}

#endif // GPU_STRING_MANIPULATION_H
