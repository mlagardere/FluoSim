
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

#ifndef STRING_MANIPULATION_H
#define STRING_MANIPULATION_H

#include "toolbox_library_global.h"

#include "stdlib.h"
#include "iostream"
#include <fstream>


TOOLBOXSHARED_EXPORT bool getWord(std::string &considered_string,
			 std::string &word, int beg_pos,
             int &end_pos,
			 std::string separating_char = std::string(" \n\t\r"));
TOOLBOXSHARED_EXPORT bool getLine(std::string &considered_string, std::string &line, int beg_pos, int &end_pos);
TOOLBOXSHARED_EXPORT long getNumberWords(std::string content, std::string separators = std::string(" \n\t\r"));
TOOLBOXSHARED_EXPORT long getNumberLines(std::string content);



#endif // STRING_MANIPULATION_H
