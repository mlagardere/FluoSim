
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

#ifndef FLUOEVENT_H
#define FLUOEVENT_H

#include "cellEngine_library_global.h"

#include "toolBox_src/toolbox_library_global.h"
    #include "toolBox_src/otherFunctions/otherFunctions.h"

#include "glm.hpp"

#include <iostream>
#include <fstream>


enum FLUOEVENT_FILE_FORMAT {PALMTRACER_FORMAT, TRXYT_FORMAT, TRXYI_FORMAT, SVG_FORMAT,
							TRXYumT_FORMAT, TRXYumI_FORMAT, PALMTRACER2_FORMAT, SVG_MULTICOLOR_FORMAT,
							THUNDERSTORM_FORMAT};

struct FluoEvent
{
	int trace_number;
	int plane;
	float x;
	float y;
	double query;
	int region;
	float intensity;
};

CELLENGINE_LIBRARYSHARED_EXPORT void  getDataStr(FluoEvent &fluo, std::string &output, int trc_nb, bool clear, FLUOEVENT_FILE_FORMAT format, float dt, float px);
CELLENGINE_LIBRARYSHARED_EXPORT void  getDataBinary(FluoEvent &fluo, std::vector<glm::int8> &binary, int trc_nb, bool clear);
CELLENGINE_LIBRARYSHARED_EXPORT void  saveLocalisationsAsString(std::vector<FluoEvent>& events, std::string file_path, FLUOEVENT_FILE_FORMAT file_format, float dt, float px);


#endif // FLUOEVENT_H
