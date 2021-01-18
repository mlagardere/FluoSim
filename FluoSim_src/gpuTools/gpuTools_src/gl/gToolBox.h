
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

#ifndef GTOOLBOX_H
#define GTOOLBOX_H

#include "gpuTools_library_global.h"

#include "string.h"
#include "gProgram.h"
#include "gVector.h"
#include "gMultiVector.h"
#include "gTexture.h"
#include "gEnum.h"

using namespace std;

namespace gstd {


bool GPUTOOLS_LIBRARYSHARED_EXPORT connectUniform(gProgram program, string entry_point_str, void const* data_ptr, int vector_size = 1);
bool GPUTOOLS_LIBRARYSHARED_EXPORT connectTexture(gProgram program, string entry_point_str, gTexture texture, uint textureUnit_idx = 0);

template<typename type>
class GPUTOOLS_LIBRARYSHARED_EXPORT myConnector
{
private :
	myConnector();

public :
	static bool connect(gProgram program, string entry_point_str, gVector<type> vector, int interleaved_component_idx = -1);
	static bool connect(gProgram program, string entry_point_str, gSubVector<type>& subVector, int interleaved_component_idx = -1);
};



template<typename type>
bool myConnector<type>::connect(gProgram program, string entry_point_str, gVector<type> vector, int interleaved_component_idx)
{
	entryPoint entry_point = program.getEntryPoint(entry_point_str);

	if(entry_point.location == -1) return 0;

	vector.bindVector(1);
	gTypeDescriptor descriptor =  getgTypeDescription(entry_point.dataType);

	if(interleaved_component_idx == -1) //data is not interleaved
	{
		if(descriptor.component_atomicType == GL_INT || descriptor.component_atomicType == GL_UNSIGNED_INT)
		{
			glVertexAttribIPointer(entry_point.location, descriptor.nb_components, descriptor.component_atomicType, 0, 0);
		}
		else
		{
			glVertexAttribPointer(entry_point.location, descriptor.nb_components, descriptor.component_atomicType, GL_FALSE, 0, 0);
		}
	}
	else
	{
		if(descriptor.component_atomicType == GL_INT || descriptor.component_atomicType == GL_UNSIGNED_INT)
		{
			glVertexAttribIPointer(entry_point.location,
								  descriptor.nb_components,
								  descriptor.component_atomicType,
								  sizeof(type), (void*) vector.getComponentOffset(interleaved_component_idx));
		}
		else
		{
			glVertexAttribPointer(entry_point.location,
								  descriptor.nb_components,
								  descriptor.component_atomicType,
								  GL_FALSE,
								  sizeof(type), (void*) vector.getComponentOffset(interleaved_component_idx));
		}
	}


	glEnableVertexAttribArray(entry_point.location);

	return 1;
}


template<typename type>
bool myConnector<type>::connect(gProgram program, string entry_point_str, gSubVector<type>& subVector, int interleaved_component_idx)
{
	entryPoint entry_point = program.getEntryPoint(entry_point_str);

	if(entry_point.location == -1) return 0;

	subVector.bindSubVector(1);
	gTypeDescriptor descriptor =  getgTypeDescription(entry_point.dataType);

	if(interleaved_component_idx == -1) //data is not interleaved
	{
		if(descriptor.component_atomicType == GL_INT || descriptor.component_atomicType == GL_UNSIGNED_INT)
		{
			glVertexAttribIPointer(entry_point.location, descriptor.nb_components, descriptor.component_atomicType, 0, (void*) subVector.getOffsetInBytes());
		}
		else
		{
			glVertexAttribPointer(entry_point.location, descriptor.nb_components, descriptor.component_atomicType, GL_FALSE, 0, (void*) subVector.getOffsetInBytes());
		}
	}
	else
	{
		if(descriptor.component_atomicType == GL_INT || descriptor.component_atomicType == GL_UNSIGNED_INT)
		{
			glVertexAttribIPointer(entry_point.location,
								  descriptor.nb_components,
								  descriptor.component_atomicType,
								  sizeof(type), (void*) (subVector.getOffsetInBytes() + subVector.getComponentOffset(interleaved_component_idx)));
		}
		else
		{
			glVertexAttribPointer(entry_point.location,
								  descriptor.nb_components,
								  descriptor.component_atomicType,
								  GL_FALSE,
								  sizeof(type), (void*) (subVector.getOffsetInBytes() +  subVector.getComponentOffset(interleaved_component_idx)));
		}
	}


	glEnableVertexAttribArray(entry_point.location);

	return 1;
}


}


#endif // GTOOLBOX_H
