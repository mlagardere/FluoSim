
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

#ifndef FLUOROPHORE_H
#define FLUOROPHORE_H


#include "cellEngine_library_global.h"

#include "FluorophoreSpecies.h"

#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/otherFunctions/otherFunctions.h"

#include "physicsEngine/RandomNumberGenerator.h"


class CELLENGINE_LIBRARYSHARED_EXPORT Fluorophore
{
public :

    Fluorophore();
    Fluorophore(const FluorophoreSpecies* fluoSpecie);

    void setFluoSpecie(const FluorophoreSpecies* fluoSpecie);
    const FluorophoreSpecies* getFluoSpecie() const;

	bool isBleached() const;
    void setBleached(bool bleached);
	bool isBlinked() const;
    void setBlinked(bool blinked);
	bool getIntensity() const;

	void bleach(float koff, float dt, RandomNumberGenerator& factory);
	void updatePhotophysicState(float delta_t, RandomNumberGenerator& factory);

private:

    const FluorophoreSpecies* m_fluoSpecie;
    bool m_Bleached;
    bool m_Blinked;

	std::default_random_engine m_defaultGenerator;

};


#endif // FLUOROPHORE_H
