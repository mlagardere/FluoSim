
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


#include "gVector.h"

namespace gstd
{


void* align(size_t alignment, size_t size, void*& ptr,	std::size_t& space)
{
	//offset in [0,alignment-1]
	int nb_slots = alignment;
	int begSlot_offset = ((int) ptr)%alignment;
	int nb_emptySlots = nb_slots - begSlot_offset;

	if(size > nb_emptySlots && begSlot_offset!=0)
	{
		space -= nb_emptySlots;
		return ptr + nb_emptySlots;
	}
	else
	{
		return ptr;
	}
}

}
