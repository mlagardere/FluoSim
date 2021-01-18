
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

#ifndef FRAPHEAD_H
#define FRAPHEAD_H


#include "cellEngine_library_global.h"

#include "ChemicalSpecies.h"
#include "BiologicalWorld.h"
#include "Region_gpu.h"


class CELLENGINE_LIBRARYSHARED_EXPORT FrapHead
{
public :

    FrapHead(Region* rgn, FluorophoreSpecies* fluo_spc, BiologicalWorld* bio_world);
    void setRegion(Region* region);
	void bleachRegion();
	void bleachRegion(float k_off, float delta_t);
	void photoActivateRegion();
	void photoActivateRegion(float k_on, float delta_t);


private :

    Region* m_region;
    FluorophoreSpecies* m_fluoSpecie;
    BiologicalWorld* m_bio_world;
};





#endif // FRAPHEAD_H
