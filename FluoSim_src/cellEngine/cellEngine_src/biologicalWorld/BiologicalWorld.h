
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

#ifndef BIOLOGICALWORLD_H
#define BIOLOGICALWORLD_H

#include "cellEngine_library_global.h"


#include "ChemicalSpecies.h"
#include "map"
#include "toolBox_src/toolBox_library_global.h"
#include "toolBox_src/otherFunctions/otherFunctions.h"
#include "Particle.h" //include Region_gpu
#include "displayAndcontrol/ScreenHandler.h"
#include "physicsEngine/RandomNumberGenerator.h"


class CELLENGINE_LIBRARYSHARED_EXPORT BiologicalWorld
{
    friend class DiffusionSubEngine;
    friend class Probe;
    friend class FrapHead;
    friend class myGPURessourceManager;

public:

	enum RENDERING_TYPES_BITS{PARTICLE_BIT = 1,PARTICLE_TOWER_BIT = 2, REGION_BIT = 4};

    BiologicalWorld();
    ~BiologicalWorld();
    void renderBiologicalWorld(ScreenHandler &screen_handler, int renderedTypes = ~0, float pointingAccuracy_pdx = -1);
    void fitBiologicalWorld(ScreenHandler &screen_handler);

	void saveRegions(string region_filePath, GEOMETRY_FILE_FORMAT format = METAMORPH_GEOMETRY_FILE_FORMAT);

    void addParticles(int n, int rgn_idx, int spc_idx, int fluoSpecie_idx);
	void addParticles(int n, int associatedRgn_idx, int creationRgn_idx,
					  int spc_idx, int fluoSpecie_idx, bool areTrapped = false);
	void addParticles(int n, int associatedRgn_idx, int creationRgn_idx, int forbiddenRgn_idx,
					  int spc_idx, int fluoSpecie_idx, bool areTrapped = false);
	void addParticles(int n, int associatedRgn_idx, int creationRgn_idx, vector<int> forbiddenRgns_ids,
					  int spc_idx, int fluoSpecie_idx, bool areTrapped = false);

	void deleteParticlesWithMotherRgn(int n, int rgn_idx, int spc_idx);
	void deleteParticlesInRelationWith(int rgn_idx, int spc_idx);
	void removeParticlesWithChildRgn(int diff_rgn_idx, int spc_idx);
    void deleteAllParticles();

    void addFluorophoreSpecie(float k_on, float k_off);
    void addChemicalSpecie(ChemicalSpecies specie);
	void addChemicalSpecie(std::string spc_name, glm::vec4 color);

	void addRegion(std::vector<glm::vec2> r);
	void setIsACompartment(int rgn_idx, int spc_idx, bool new_state);

	bool deleteRegion(Region* rgn);
//	bool deleteAllRegions();

    void setD(int rgn_idx, int specie_idx, float D);
	void setTrappingAbundant(int rgn_idx, int specie_idx, float Dtrapped, float kon_abundant, float koff,
							 bool isDDistributed = false, float poisson_mean = -1.0f);
	void setTrappingNAbundant(int rgn_idx, int specie_idx, float Dtrapped, float kon_Nabundant, float site_density, float koff,
							  bool isDDistributed = false, float poisson_mean = -1.0f);
	void setCrossing(int rgn_idx, int specie_idx, float p_crossing_insideOut, float p_crossing_outsideIn);
	void setImmobileFraction(int spc_idx, float immobile_fraction, bool withChecking = true);
	void setFixation(bool isFixed);
	bool isFixed();

	Region& getRegionRef(int rgn_idx);

	int getNextRegionUniqueId();
	int getRegionIdx(Region* rgn);
	int getRegionIdx(string rgn_name);
    int getNbRegions();
    float getParticleDensity(int rgn_idx, int spc_idx, bool wVisibility = false);

    FluorophoreSpecies* getFluoSpecieAdr(int fluoSpecie_idx);
    ChemicalSpecies* getSpecieAdr(int spc_idx);

	void updatePosition(float d_t, bool isPre_calc);
	void updateTrappingState(float d_t);
	void updateD();

    void updateTrappingState(float d_t, Particle& ptcl, RandomNumberGenerator& );
	void updateD(Particle& ptcl);

	void setParticlePositions(std::vector<glm::vec2>& r);
	void setParticleColorMode(Particle::COLOR_MODE mode);
	std::vector<glm::vec2> getParticlePositions();

private:

	int m_nextRgn_uId;

	bool m_isFixed;
    std::list<ChemicalSpecies> m_species;
	std::list<Particle> m_particles;
	std::list<Region> m_regions;
    std::list<FluorophoreSpecies> m_fluo_species;

	myMultiVector<glm::vec2> m_region_r_mv;
	gstd::gMultiVector<glm::vec2> m_region_r_gmv;

    RandomNumberGenerator m_randomNumberFactory;
};



#endif // BIOLOGICALWORLD_H
