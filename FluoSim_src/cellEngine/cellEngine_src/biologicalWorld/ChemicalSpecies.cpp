
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




#include "ChemicalSpecies.h"


using namespace std;
using namespace glm;

ChemicalSpecies::ChemicalSpecies(const string& name,const vec4& color)
{
	m_name = name;
	m_color = color;
	m_color_trapped = vec4(1,1,1,0);
	m_immobileFraction = 0.0f;
}

ChemicalSpecies::ChemicalSpecies(const string& name, const vec4& color, const vec4& color_trapped)
{
	m_name = name;
	m_color = color;
	m_color_trapped = color_trapped;
	m_immobileFraction = 0.0f;
}


string ChemicalSpecies::getName() const
{
	return m_name;
}


vec4 ChemicalSpecies::setColor(const vec4& color)
{
	m_color = color;
}

vec4 ChemicalSpecies::setColorTrapped(const vec4& color_trapped)
{
	m_color_trapped = color_trapped;
}

string ChemicalSpecies::setName(const string& name)
{
	m_name = name;
}

void ChemicalSpecies::setImmobileFraction(float immobile_fraction)
{
	m_immobileFraction = immobile_fraction;
}

float ChemicalSpecies::getImmobileFraction()
{
	return m_immobileFraction;
}
