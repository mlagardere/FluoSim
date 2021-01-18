
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


#include "FrapHead.h"

using namespace std;
using namespace glm;

FrapHead::FrapHead(Region* rgn, FluorophoreSpecies* fluo_spc, BiologicalWorld* bio_world)
{
	m_region = rgn;
	m_fluoSpecie = fluo_spc;
	m_bio_world = bio_world;
}


void FrapHead::setRegion(Region* region)
{
	m_region = region;

}



void FrapHead::bleachRegion(float k_off, float delta_t)
{

	for(Particle& ptcl : m_bio_world->m_particles)
	{
		if(ptcl.getFluorophore()->isBleached() == true) continue;
		//<-

//		if(ptcl.getFluoSpecie() == m_fluoSpecie && m_region->isInside((ptcl.getR())))
		if(ptcl.getFluoSpecie() == m_fluoSpecie && ptcl.isInside(m_region))
		{
			float rdm_number = uniformDistributiion(float(0.0),float(1.0));
			if(rdm_number < k_off* delta_t)
			{
				ptcl.getFluorophore()->setBleached(true);
			}
		}
	}
}


void FrapHead::bleachRegion()
{
	for(Particle& ptcl : m_bio_world->m_particles)
	{
		if(ptcl.getFluorophore()->isBleached() == true) continue;
		//<-

		if(ptcl.getFluoSpecie() == m_fluoSpecie && ptcl.isInside(m_region))
//		if(ptcl.getFluoSpecie() == m_fluoSpecie && m_region->isInside((ptcl.getR())))
		{
			ptcl.getFluorophore()->setBleached(1);
		}
	}
}


void FrapHead::photoActivateRegion()
{
	for(Particle &ptcl : m_bio_world->m_particles)
	{
		if(ptcl.getFluorophore()->isBlinked() == false) continue;
		//<-

		if(ptcl.getFluoSpecie() == m_fluoSpecie && m_region->isInside(ptcl.getR()))
		{
		   ptcl.getFluorophore()->setBlinked(false);
		}
	}
}


void FrapHead::photoActivateRegion(float k_on, float delta_t)
{
	for(Particle &ptcl : m_bio_world->m_particles)
	{
		if(ptcl.getFluorophore()->isBlinked() == false) continue;
		//<-

		if(ptcl.getFluoSpecie() == m_fluoSpecie && m_region->isInside(ptcl.getR()))
		{
			float rdm_number = uniformDistributiion(float(0.0),float(1.0));
			if(rdm_number < k_on* delta_t)
			{
				ptcl.getFluorophore()->setBlinked(false);
			}
		}
	}
}
