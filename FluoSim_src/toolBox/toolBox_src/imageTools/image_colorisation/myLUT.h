
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

#ifndef MYLUT_H
#define MYLUT_H

#include "glm.hpp"
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_access.hpp>

#include "stdlib.h"
#include "vector"

#include "toolbox_library_global.h"
	#include "fileAndstring_manipulation/file_manipulation.h"




class TOOLBOXSHARED_EXPORT myLUT
{
public :

	myLUT();

	void loadLUT(const std::string& path);
	glm::vec4 getColor(float normalized_value); //0->1
	std::vector<glm::vec4> getColors();

private :

	std::vector<glm::vec4> m_colors;
};


#endif // MYLUT_H
