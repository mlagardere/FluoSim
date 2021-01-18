
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

#ifndef CHEMICALSPECIE_H
#define CHEMICALSPECIE_H


#include "cellEngine_library_global.h"

#include "glm.hpp"
#include "stdlib.h"
#include "string"
#include "vector"


class CELLENGINE_LIBRARYSHARED_EXPORT ChemicalSpecies
{
public :

    ChemicalSpecies(const std::string& name,const glm::vec4& color);
    ChemicalSpecies(const std::string& name,const glm::vec4& color,const glm::vec4& color_trapped);

	glm::vec4 setColor(const glm::vec4 &color);
	glm::vec4 setColorTrapped(const glm::vec4 &color_trapped);
	std::string setName(const std::string& name);

	glm::vec4 getColor() const{return m_color;}
	glm::vec4 getColorTrapped() const{return m_color_trapped;}
	std::string getName()const;

	void setImmobileFraction(float immobile_fraction);
	float getImmobileFraction();


private :

    std::string m_name;
	glm::vec4 m_color;
	glm::vec4 m_color_trapped;
	float m_immobileFraction;

};



#endif // CHEMICALSPECIE_H
