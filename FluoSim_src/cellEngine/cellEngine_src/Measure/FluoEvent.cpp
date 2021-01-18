
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


#include "FluoEvent.h"

using namespace std;
using namespace glm;


void getDataStr(FluoEvent& fluo, string& output, int trc_nb, bool clear, FLUOEVENT_FILE_FORMAT format, float dt, float px)
{
	if(clear == true)	output.clear();

	char* data_cstr;

	if(format == PALMTRACER_FORMAT)
	{
		data_cstr = new char[6*11+6+1];
		sprintf(data_cstr, "%11d\t%11d\t%11.4f\t%11.4f\t%11.4f\t%11.4f\n\0", trc_nb, fluo.plane, fluo.x, fluo.y, fluo.query, fluo.intensity);
		output += string(data_cstr);
		delete[] data_cstr;
	}

	if(format == TRXYI_FORMAT)
	{
		data_cstr = new char[4*11+6+1];
		sprintf(data_cstr, "%11d\t%11.4f\t%11.4f\t%11d\n\0", trc_nb, fluo.x, fluo.y, fluo.plane);
		output += string(data_cstr);
		delete[] data_cstr;
	}

	if(format == TRXYT_FORMAT)
	{
		data_cstr = new char[4*11+6+1];
		sprintf(data_cstr, "%11d\t%11.4f\t%11.4f\t%11.4f\n\0", trc_nb, fluo.x, fluo.y, fluo.plane * dt);
		output += string(data_cstr);
		delete[] data_cstr;
	}

	if(format == TRXYumI_FORMAT)
	{
		data_cstr = new char[4*11+6+1];
		sprintf(data_cstr, "%11d\t%11.4f\t%11.4f\t%11d\n\0", trc_nb, fluo.x*px, fluo.y*px, fluo.plane);
		output += string(data_cstr);
		delete[] data_cstr;
	}

	if(format == TRXYumT_FORMAT)
	{
		data_cstr = new char[4*11+6+1];
		sprintf(data_cstr, "%11d\t%11.4f\t%11.4f\t%11.4f\n\0", trc_nb, fluo.x*px, fluo.y*px, fluo.plane * dt);
		output += string(data_cstr);
		delete[] data_cstr;
	}

	if(format == THUNDERSTORM_FORMAT)
	{
		float _sigma = 200;//nm
		data_cstr = new char[8*11+13+1];
		sprintf(data_cstr, "%11d,%11d,%11.4f,%11.4f,%11.4f,%11.4f,%11.4f,%11.4f\n\0", trc_nb, fluo.plane+1, fluo.x*px*1000, fluo.y*px*1000, _sigma, fluo.intensity, 0.0f, 0.0f); //[px] = µm -> [px*1000] = nm
		output += string(data_cstr);
		delete[] data_cstr;
	}
}

void getDataBinary(FluoEvent &fluo, vector<int8> &binary, int trc_nb, bool clear)
{
	if(clear == true) binary.clear();

	int8* u;
	fluo.trace_number = trc_nb;
	u = (int8*) &fluo;
	binary.insert(binary.end(), u, u + sizeof(FluoEvent));

	delete[] u;
}

void  saveLocalisationsAsString(std::vector<FluoEvent>& events, std::string file_path, FLUOEVENT_FILE_FORMAT file_format, float dt, float px)
{
	fstream myfile;
	myfile.open(file_path.data(), ios_base::out);
	if(myfile.is_open()==false) return;

	myfile<<"\"id\",\"frame\",\"x [nm]\",\"y [nm]\",\"sigma [nm]\",\"intensity [photon]\",\"offset [photon]\",\"bkgstd [photon]\"\n";

	switch(file_format)
	{
		case THUNDERSTORM_FORMAT :
		{
			string data_str;
			uint localisation_idx = 1;
			for(FluoEvent& fluo_event : events)
			{
				getDataStr(fluo_event, data_str, localisation_idx, true, THUNDERSTORM_FORMAT, dt, px);
				myfile<<data_str;
				localisation_idx++;
			}
		}
		break;

		default :
		break;
	}

	myfile.close();
}


