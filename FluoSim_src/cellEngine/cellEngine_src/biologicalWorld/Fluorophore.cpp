
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


#include "Fluorophore.h"

using namespace std;
using namespace glm;

Fluorophore::Fluorophore()
{
	m_fluoSpecie = NULL;
	m_Bleached = 0;
	m_Blinked = 0;
}

Fluorophore::Fluorophore(const FluorophoreSpecies* fluoSpecie)
{
	m_fluoSpecie = fluoSpecie;
	m_Bleached = 0;
	m_Blinked = 0;
}

void Fluorophore::setFluoSpecie(const FluorophoreSpecies* fluoSpecie)
{
	m_fluoSpecie = fluoSpecie;
}

const FluorophoreSpecies* Fluorophore::getFluoSpecie() const
{
	return m_fluoSpecie;
}

bool Fluorophore::isBleached() const
{
	return m_Bleached;
}

void Fluorophore::setBleached(bool bleached)
{
	m_Bleached = bleached;
}

bool Fluorophore::isBlinked() const
{
	return m_Blinked;
}

void Fluorophore::setBlinked(bool blinked)
{
	m_Blinked = blinked;
}

bool Fluorophore::getIntensity() const
{
	return (!(m_Blinked || m_Bleached));
}

void Fluorophore::bleach(float koff, float dt, RandomNumberGenerator& factory)
{
	if(!isBleached())
	{
		float rdm_number = factory.uniformRandomNumber();

		if(rdm_number < koff* dt)
		{
			setBleached(true);
		}
	}
}


void Fluorophore::updatePhotophysicState(float delta_t, RandomNumberGenerator& factory)
{

	if(!isBleached())
	{
		if(isBlinked())
		{
			float k_on = m_fluoSpecie->getKon();
//			float rdm_number = uniformDistributiion(float(0.0),float(1.0));
			float rdm_number = factory.uniformRandomNumber();

			if(rdm_number < k_on* delta_t)
			{
				setBlinked(0);
			}
		}
		else
		{
			float k_off = m_fluoSpecie->getKoff();
			float rdm_number = factory.uniformRandomNumber();

			if(rdm_number < k_off* delta_t)
			{
				setBlinked(1);
			}
		}
	}
}

