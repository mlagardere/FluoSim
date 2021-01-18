
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



#include "FluorophoreSpecies.h"


FluorophoreSpecies::FluorophoreSpecies()
{
	m_Kon = 0.0;
	m_Koff = 0.0;
}

FluorophoreSpecies::FluorophoreSpecies(float k_on, float k_off)
{
	m_Kon = k_on;
	m_Koff = k_off;
}

float FluorophoreSpecies::getKon() const
{
	return m_Kon;
}

void FluorophoreSpecies::setKon(float K_on)
{
	m_Kon = K_on;
}

float FluorophoreSpecies::getKoff() const
{
	return m_Koff;
}

void FluorophoreSpecies::setKoff(float K_off)
{
	m_Koff = K_off;
}

