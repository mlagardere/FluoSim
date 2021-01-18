
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

#ifndef PARTICLE_H
#define PARTICLE_H


#include "cellEngine_library_global.h"

#include "Fluorophore.h"
#include "glm.hpp"
#include "Region_gpu.h"
#include "ChemicalSpecies.h"
#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/otherFunctions/otherFunctions.h"
#include <list>

#include "physicsEngine/RandomNumberGenerator.h"
#include "Measure/Trace.h"

class Tower
{
    friend class Particle;
    friend class BiologicalWorld;

public :

    Tower();
	bool isInsideScope(const glm::vec2 &r) const;

private :

	glm::vec2 r;
	float radius_squared;
	bool isSet;
	bool isInsideRgn;
	bool isToBe_recalculated = true;
};

class BiologicalWorld;
class Probe;
class CELLENGINE_LIBRARYSHARED_EXPORT Particle
{
 friend class BiologicalWorld;
 friend class Probe;

public:

	enum COLOR_MODE {UNIQUE_COLOR, SPECIES_COLOR, TRAPPING_STATE_COLOR};

    Particle(glm::vec2 r, Region* rgn_name, ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie);
    Particle(Region* region, ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie);
    Particle(Region* associated_region, Region* creation_region,
               ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped = false);
    Particle(Region* associated_region, Region* creation_region, Region* forbidden_region,
               ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped = false);
    Particle(Region* associated_region, Region* creation_region, vector<Region*> forbidden_regions,
               ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped = false);

	void setR(glm::vec2 r);
	glm::vec2 getR();
    void setPositionRandomly();

	void setColorMode(COLOR_MODE mode);
	COLOR_MODE getColorMode();
	void setUniqueColor(glm::vec4 color);
	void changeUniqueColor();

	bool isImmobile();
	void setIsImmobile(bool isImmobile);

	glm::vec4 getColor();
	float getD();
    Region* getMotherRgn();
	void setDiffRgn(Region* diff_rgn);
    Region* getDiffRgn();
    const FluorophoreSpecies* getFluoSpecie();

    void updatePosition(float delta_t, bool pre_calc, RandomNumberGenerator& randomFactory);
    void updatePosition(float delta_t, list<Region>& rgns_l, RandomNumberGenerator& factory);
    void updateTrappingState(float delta_t, std::list<Region>& regions, RandomNumberGenerator& factory);
    void updatePhotophysicState(float delta_t, RandomNumberGenerator& factory);
    void updateD(RandomNumberGenerator& factory);

    ChemicalSpecies* getSpecie();
    bool isTrapped();
	bool isInside(Region* rgn);
    Fluorophore* getFluorophore();
    float getIntensity();

	void addTower(Region* rgn);
	void removeTower(Region* rgn);

public :

	static long m_nbReflection;

private:



    std::map<Region*, Tower> m_towers;

    bool m_trapped;
	COLOR_MODE m_color_mode;

	bool m_isImmobile;

	CROSSING_DIRECTION m_cross_dir;
	glm::vec2 m_r;
	glm::vec4 m_color;
	bool m_trappingStateChanged_flag;
	float m_D;

    Region* m_mother_rgn;
    Region* m_child_rgn;

    ChemicalSpecies* m_specie;
    Fluorophore m_fluorophore;

    bool m_not_inside;


};


CELLENGINE_LIBRARYSHARED_EXPORT
void reflectParticle(Particle& ptcl, glm::vec2 dr_start, map<Region*, int> rgns_to_avoid_start,
					 glm::vec2& dr_end, map<Region*, int>& rgns_to_avoid_end,
					 list<Region*> rgns_l,  CROSSING_DIRECTION& cross_dir,
                     RandomNumberGenerator& factory);


#endif // PARTICLE_H
