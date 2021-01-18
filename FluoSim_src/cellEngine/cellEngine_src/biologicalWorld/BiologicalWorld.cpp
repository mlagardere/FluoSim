
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



#include "BiologicalWorld.h"

using namespace std;
using namespace glm;


BiologicalWorld::BiologicalWorld()
{
	m_nextRgn_uId = 0;
	m_isFixed = false;
//	m_randomNumberFactory.generatePreCalcRandomNumbers();
}

BiologicalWorld::~BiologicalWorld()
{
	m_regions.clear();
}

void BiologicalWorld::addChemicalSpecie(ChemicalSpecies specie)
{
	m_species.push_back(specie);

}

void BiologicalWorld::addChemicalSpecie(string spc_name, vec4 color)
{

    m_species.push_back(ChemicalSpecies(spc_name, color));

}

void BiologicalWorld::addFluorophoreSpecie(float k_on, float k_off)
{
    m_fluo_species.push_back(FluorophoreSpecies(k_on, k_off));
}


void BiologicalWorld::addRegion(vector<vec2> r)
{
	m_regions.emplace_back(m_region_r_mv, m_region_r_gmv, r);
	for(auto &spc : m_species)
	{
		m_regions.back().addDynamicParam(&spc);
	}

	for(Particle& ptcl : m_particles)
	{
		ptcl.addTower(&m_regions.back());
	}

	Region& rgn = m_regions.back();
	rgn.setName("region_"+to_string(m_nextRgn_uId));

	m_nextRgn_uId++;
}

void BiologicalWorld::setIsACompartment(int rgn_idx, int spc_idx, bool new_state)
{
	auto& rgn = getRegionRef(rgn_idx);
	auto& spc = *(getSpecieAdr(spc_idx));

	rgn.setIsACompartment(&spc, new_state);
	removeParticlesWithChildRgn(rgn_idx, spc_idx);


}

bool BiologicalWorld::deleteRegion(Region* rgn_adr)
{

	int rgn_idx = getRegionIdx(rgn_adr);
	if(rgn_idx == getNbRegions()) return false; //->

	int spc_idx = 0;
    for(ChemicalSpecies spc : m_species)
	{
		deleteParticlesWithMotherRgn(m_particles.size(), rgn_idx, spc_idx);
		removeParticlesWithChildRgn(rgn_idx, spc_idx);
		spc_idx++;
	}

	for(Particle& ptcl : m_particles)
	{
		ptcl.removeTower(rgn_adr);
	}

	auto rgn_it = m_regions.begin();
	advance(rgn_it, rgn_idx);

	m_regions.erase(rgn_it);

	if(m_regions.size() == 0) m_nextRgn_uId = 0;
	return true;
}



void BiologicalWorld::addParticles(int n, int rgn_idx, int spc_idx, int fluoSpecie_idx)
{

			if(rgn_idx > m_regions.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
				return;
			}

			if(spc_idx > m_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (spc_idx > m_species.size()-1)\n";
				return;
			}

			if(fluoSpecie_idx > m_fluo_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (fluoSpecie_idx > m_fluo_species.size()-1)\n";
				return;
			}

	auto rgn = m_regions.begin();
		advance(rgn,rgn_idx);
	auto  spc = m_species.begin();
		advance(spc, spc_idx);
	auto fluoSpc = m_fluo_species.begin();
		advance(fluoSpc, fluoSpecie_idx);

	if(rgn->isACompartment(&(*spc)) == false) return; //->

	for(int prtl_idx = 0; prtl_idx <= n-1; prtl_idx++)
	{
		m_particles.push_back(Particle(&(*rgn), &(*spc), &(*fluoSpc)));

		for(Region& rgn : m_regions)
		{
			m_particles.back().addTower(&rgn);
		}
	}
}

void BiologicalWorld::addParticles(int n, int associatedRgn_idx, int creationRgn_idx,
									 int spc_idx, int fluoSpecie_idx, bool areTrapped)
{
			if(associatedRgn_idx > m_regions.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
				return;
			}

			if(spc_idx > m_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (spc_idx > m_species.size()-1)\n";
				return;
			}

			if(fluoSpecie_idx > m_fluo_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (fluoSpecie_idx > m_fluo_species.size()-1)\n";
				return;
			}


	auto associated_rgn = m_regions.begin();
		advance(associated_rgn ,associatedRgn_idx);
	auto creation_rgn = m_regions.begin();
		advance(creation_rgn,creationRgn_idx);
	auto spc = m_species.begin();
		advance(spc, spc_idx);
	auto fluoSpc = m_fluo_species.begin();
		advance(fluoSpc, fluoSpecie_idx);


	if(creation_rgn->isACompartment(&(*spc)) == false) return; //->

	for(int prtl_idx = 0; prtl_idx <= n-1; prtl_idx++)
	{
		m_particles.push_back(Particle(&(*associated_rgn) , &(*creation_rgn),
										 &(*spc), &(*fluoSpc), areTrapped));
		for(Region& rgn : m_regions)
		{
			m_particles.back().addTower(&rgn);
		}
	}
}



void BiologicalWorld::addParticles(int n, int associatedRgn_idx, int creationRgn_idx, int forbiddenRgn_idx,
									 int spc_idx, int fluoSpecie_idx, bool areTrapped)
{
			if(associatedRgn_idx > m_regions.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
				return;
			}

			if(spc_idx > m_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (spc_idx > m_species.size()-1)\n";
				return;
			}

			if(fluoSpecie_idx > m_fluo_species.size()-1)
			{
                cout<<"In BiologicalWorld::addParticles: error (fluoSpecie_idx > m_fluo_species.size()-1)\n";
				return;
			}


	auto associated_rgn = m_regions.begin();
		advance(associated_rgn ,associatedRgn_idx);
	auto creation_rgn = m_regions.begin();
		advance(creation_rgn,creationRgn_idx);
	auto forbidden_rgn = m_regions.begin();
		advance(forbidden_rgn, forbiddenRgn_idx);
	auto spc = m_species.begin();
		advance(spc, spc_idx);
	auto fluoSpc = m_fluo_species.begin();
		advance(fluoSpc, fluoSpecie_idx);

	if(creation_rgn->isACompartment(&(*spc)) == false) return; //->

	for(int prtl_idx = 0; prtl_idx <= n-1; prtl_idx++)
	{
		m_particles.push_back(Particle(&*associated_rgn , &*creation_rgn, &*forbidden_rgn,
										 &*spc, &*fluoSpc, areTrapped));
		for(Region& rgn : m_regions)
		{
			m_particles.back().addTower(&rgn);
		}
	}
}

void BiologicalWorld::addParticles(int n, int associatedRgn_idx, int creationRgn_idx,
									 vector<int> forbiddenRgns_ids,int spc_idx,
									 int fluoSpecie_idx, bool areTrapped)
{
	if(associatedRgn_idx > m_regions.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
		return;
	}

	if(spc_idx > m_species.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (spc_idx > m_species.size()-1)\n";
		return;
	}

	if(fluoSpecie_idx > m_fluo_species.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (fluoSpecie_idx > m_fluo_species.size()-1)\n";
		return;
	}

	auto associated_rgn = m_regions.begin();
		advance(associated_rgn ,associatedRgn_idx);
	auto creation_rgn = m_regions.begin();
		advance(creation_rgn,creationRgn_idx);

	vector<Region*> forbiden_rgns_v;
	auto forbidden_rgn = m_regions.begin();
		for(int rgn_idx : forbiddenRgns_ids)
		{
			forbidden_rgn = m_regions.begin();
			advance(forbidden_rgn, rgn_idx);
			forbiden_rgns_v.push_back(&*forbidden_rgn);
		}
	auto spc = m_species.begin();
		advance(spc, spc_idx);
	auto fluoSpc = m_fluo_species.begin();
		advance(fluoSpc, fluoSpecie_idx);

	if(creation_rgn->isACompartment(&(*spc)) == false) return; //->

	for(int prtl_idx = 0; prtl_idx <= n-1; prtl_idx++)
	{
		m_particles.push_back(Particle(&*associated_rgn , &*creation_rgn, forbiden_rgns_v,
									 &*spc, &*fluoSpc, areTrapped));
		for(Region& rgn : m_regions)
		{
			m_particles.back().addTower(&rgn);
		}
	}
}


void BiologicalWorld::deleteParticlesWithMotherRgn(int n, int rgn_idx, int spc_idx)
{
	if(rgn_idx > m_regions.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
		return;
	}

	auto rgn = m_regions.begin();
	advance(rgn,rgn_idx);
	auto spc = m_species.begin();
	advance(spc, spc_idx);

	auto part = m_particles.begin();
	int nb_deleted_prtl = 0;

	while(part != m_particles.end() && nb_deleted_prtl <n)
	{
		if(part->getMotherRgn() == &(*rgn) && part->getSpecie()== &(*spc))
		{
			auto current_part = part;
			part++;
			m_particles.erase(current_part);
			nb_deleted_prtl++;
		}
		else
		{
			part++;
		}
	}
}

void BiologicalWorld::deleteParticlesInRelationWith(int rgn_idx, int spc_idx)
{
	if(rgn_idx > m_regions.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
		return;
	}

	auto rgn = m_regions.begin();
	advance(rgn,rgn_idx);
	auto spc = m_species.begin();
	advance(spc, spc_idx);

	auto part = m_particles.begin();

	while(part != m_particles.end())
	{
		if(part->getSpecie() == &(*spc) && (part->getMotherRgn() == &(*rgn) || part->getDiffRgn() == &(*rgn)))
		{
			auto current_part = part;
			part++;
			m_particles.erase(current_part);
		}
		else part++;
	}
}

void BiologicalWorld::removeParticlesWithChildRgn(int diff_rgnIdx, int spc_idx)
{
	if(diff_rgnIdx > m_regions.size()-1)
	{
        cout<<"In BiologicalWorld::addParticles: error (rgn_idx > m_regions.size()-1)\n";
		return;
	}

	auto rgn = m_regions.begin();
	advance(rgn, diff_rgnIdx);
	auto spc = m_species.begin();
	advance(spc, spc_idx);

	auto part = m_particles.begin();
	while(part != m_particles.end())
	{
		if(part->getSpecie() == &(*spc) && part->getMotherRgn() != &(*rgn) && part->getDiffRgn() == &(*rgn))
		{
			part->setDiffRgn(part->getMotherRgn());
			part->m_trapped = false;
			part->m_trappingStateChanged_flag = true;
		}

		part++;
	}
}


void BiologicalWorld::renderBiologicalWorld(ScreenHandler &screen_handler, int renderedTypes, float pointingAccuracy_px)
{
	if(renderedTypes & (int) PARTICLE_BIT)
		screen_handler.addParticles(m_particles, pointingAccuracy_px);

	if(renderedTypes & (int) REGION_BIT)
	{
        list<Region*> rgns;
        for(Region& rgn : m_regions) //so that highlighted regions are displayed last
        {
            if(rgn.isHighlighted() == true)
                rgns.push_back(&rgn);
            else
                rgns.push_front(&rgn);
        }
        for(Region* rgn : rgns)
		{
            rgn->render((myGLObject*) screen_handler.getRenderWindow());
		}
	}

//		screen_handler.addRegions(m_regions, 1);


//	if(renderedTypes & (int) PARTICLE_TOWER_BIT)
//	{
//		vector<vec2> m_towerR_v;
//		vector<vec4> m_towerColor_v;
//		for(Particle& ptcl : m_particles)
//		{
//			for(Region& rgn : m_regions)
//			{
//				m_towerR_v.push_back(ptcl.m_towers[&rgn].r);
//				m_towerColor_v.push_back({0,1,0,1});
//			}
//		}

//		screen_handler.addPoints(m_towerR_v, m_towerColor_v);
//	}

}

void BiologicalWorld::fitBiologicalWorld(ScreenHandler &screen_handler)
{
	bool isBottomLeft_set = false;
	bool isTopRight_set = false;

	vec2 temp_bottom_left, bottom_left;
	vec2 temp_top_right, top_right;

	for(Region &rgn : m_regions)
	{
		//bottom_left
		if(rgn.getBottomLeft(temp_bottom_left) == true)
		{
			if(isBottomLeft_set == false)
			{
				bottom_left = temp_bottom_left;
				isBottomLeft_set = true;
			}
			else
			{
				if(temp_bottom_left.x <= bottom_left.x)
				{
					bottom_left.x = temp_bottom_left.x;
				}

				if(temp_bottom_left.y <= bottom_left.y)
				{
					 bottom_left.y = temp_bottom_left.y;
				}
			}
		}

		//top_right
		if(rgn.getTopRight(temp_top_right) == true)
		{
			if(isTopRight_set == false)
			{
				top_right = temp_top_right;
				isTopRight_set = true;
			}
			else
			{
				if(temp_top_right.x >= top_right.x)
				{
					top_right.x = temp_top_right.x;
				}
				if(temp_top_right.y >= top_right.y)
				{
					top_right.y = temp_top_right.y;
				}
			}
		}
	}

	if(isBottomLeft_set == true &&
	   isTopRight_set == true)
	{
		//to evoid deformation...
		vec2 center = 0.5f*(bottom_left + top_right);
		vec2 rect_size = top_right - bottom_left;
		//1/1 -> tight fitting, 4/3 let some place...
		vec2 square_size(4.0f/3.0f*std::max(rect_size.x, rect_size.y),
						 4.0f/3.0f*std::max(rect_size.x, rect_size.y));

		screen_handler.setPixelView(center - 0.5f*square_size,
									square_size);
	}

}

void BiologicalWorld::saveRegions(string region_filePath, GEOMETRY_FILE_FORMAT format)
{
//	saveRegions()
	saveRegionsList(m_regions, region_filePath, format);

}

void BiologicalWorld::setD(int rgn_idx, int specie_idx, float D)
{
	getRegionRef(rgn_idx).setD(getSpecieAdr(specie_idx), D);

    ChemicalSpecies* specie = getSpecieAdr(specie_idx);
	Region* child_rgn = &getRegionRef(rgn_idx);

	for(auto& ptcl : m_particles)
	{
		if(ptcl.m_specie == specie && ptcl.m_child_rgn == child_rgn)
		{
			ptcl.m_trappingStateChanged_flag = true;
			ptcl.updateD(m_randomNumberFactory);
		}
	}
}

void BiologicalWorld::setTrappingAbundant(int rgn_idx, int specie_idx,
											float Dtrapped, float kon, float koff,
											bool isDDistributed, float poisson_mean)
{
	getRegionRef(rgn_idx).setIsTrappingEnable(getSpecieAdr(specie_idx), 1);
	getRegionRef(rgn_idx).setAreSitesAbundant(getSpecieAdr(specie_idx), 1);
	getRegionRef(rgn_idx).setDtrapped(getSpecieAdr(specie_idx),Dtrapped);
	getRegionRef(rgn_idx).setKonAbundant(getSpecieAdr(specie_idx),kon);
	getRegionRef(rgn_idx).setKoff(getSpecieAdr(specie_idx),koff);
	getRegionRef(rgn_idx).setIsDDistrubuted(getSpecieAdr(specie_idx),isDDistributed);
	getRegionRef(rgn_idx).setPoissonMean(getSpecieAdr(specie_idx),poisson_mean);

    ChemicalSpecies* specie = getSpecieAdr(specie_idx);
	Region* child_rgn = &getRegionRef(rgn_idx);

	for(auto& ptcl : m_particles)
	{
		if(ptcl.m_specie == specie && ptcl.m_child_rgn == child_rgn && ptcl.m_trapped == true)
		{
			ptcl.m_trappingStateChanged_flag = true;
			ptcl.updateD(m_randomNumberFactory);
		}
	}
}

void BiologicalWorld::setTrappingNAbundant(int rgn_idx, int specie_idx,
											 float Dtrapped, float kon_Nabundant, float site_density, float koff,
											 bool isDDistributed, float poisson_mean)
{
	getRegionRef(rgn_idx).setIsTrappingEnable(getSpecieAdr(specie_idx), 1);
	getRegionRef(rgn_idx).setAreSitesAbundant(getSpecieAdr(specie_idx), 0);
	getRegionRef(rgn_idx).setDtrapped(getSpecieAdr(specie_idx),Dtrapped);
	getRegionRef(rgn_idx).setKonNAbundant(getSpecieAdr(specie_idx),kon_Nabundant);
	getRegionRef(rgn_idx).setSiteDensity(getSpecieAdr(specie_idx), site_density);
	getRegionRef(rgn_idx).setKoff(getSpecieAdr(specie_idx),koff);
	getRegionRef(rgn_idx).setIsDDistrubuted(getSpecieAdr(specie_idx),isDDistributed);
	getRegionRef(rgn_idx).setPoissonMean(getSpecieAdr(specie_idx),poisson_mean);

    ChemicalSpecies* specie = getSpecieAdr(specie_idx);
	Region* child_rgn = &getRegionRef(rgn_idx);

	for(auto& ptcl : m_particles)
	{
		if(ptcl.m_specie == specie && ptcl.m_child_rgn == child_rgn && ptcl.m_trapped == true)
		{
			ptcl.m_trappingStateChanged_flag = true;
			ptcl.updateD(m_randomNumberFactory);
		}
	}
}

void BiologicalWorld::setCrossing(int rgn_idx, int spc_idx, float p_crossing_insideOut, float p_crossing_outsideIn)
{
	Region& rgn = getRegionRef(rgn_idx);
    ChemicalSpecies* spc = getSpecieAdr(spc_idx);

	rgn.setCrossing(spc, p_crossing_insideOut, p_crossing_outsideIn);
}

void BiologicalWorld::setImmobileFraction(int spc_idx, float immobile_fraction, bool withChecking)
{
    ChemicalSpecies* spc = getSpecieAdr(spc_idx);
	float last_immobile_fraction = spc->getImmobileFraction();
	if(withChecking == true && immobile_fraction == last_immobile_fraction) return; //->

	spc->setImmobileFraction(immobile_fraction);

	for(Particle& ptcl: m_particles)
	{
		if(ptcl.getSpecie() == spc)
		{
			if(uniformDistributiion(0.0f, 1.0f) <= spc->getImmobileFraction()) ptcl.setIsImmobile(true);
			else ptcl.setIsImmobile(false);
		}
	}
}

void BiologicalWorld::setFixation(bool isFixed)
{
	m_isFixed = isFixed;
}

bool BiologicalWorld::isFixed()
{
	return m_isFixed;
}

int BiologicalWorld::getNextRegionUniqueId()
{
	return m_nextRgn_uId;
}

Region& BiologicalWorld::getRegionRef(int rgn_idx)
{
	list<Region>::iterator it = m_regions.begin();
	advance(it, rgn_idx);

	return *it;
}

//bool BiologicalWorld::getRegionRef(Region& rgn, string rgn_name)
//{
//	bool rgn_found = false;
//	for(auto& temp_rgn : m_regions)
//	{
//		if((temp_rgn.getName() == rgn_name))
//		{
//			rgn = temp_rgn;
//			rgn_found = true;
//			break; //->
//		}
//	}

//	return rgn_found; //->
//}



int BiologicalWorld::getRegionIdx(Region* rgn_adr)
{
	int rgn_idx = 0;
	for(Region &rgn : m_regions)
	{
		if(&rgn == rgn_adr) break; //>
		rgn_idx++;
	}

	return rgn_idx;
}

int BiologicalWorld::getRegionIdx(string rgn_name)
{
	int rgn_idx = -1;
	int temp_idx = 0;
	for(auto& temp_rgn : m_regions)
	{
		if((temp_rgn.getName() == rgn_name))
		{
			rgn_idx = temp_idx;
			break; //->
		}
		temp_idx++;
	}

	return rgn_idx; //->
}


float BiologicalWorld::getParticleDensity(int rgn_idx, int spc_idx, bool wVisibility)
{
    if(rgn_idx >= m_regions.size() || spc_idx >= m_species.size()) return -1.0f; //->

    int nb_particles = 0;
    float surface = 0.0f;

    Region* rgn = &getRegionRef(rgn_idx);
    ChemicalSpecies* spc = getSpecieAdr(spc_idx);

    for(Particle& particle : m_particles)
    {
        if(particle.getSpecie() == spc &&
           particle.isInside(rgn) == true &&
           (!wVisibility || wVisibility && particle.getFluorophore()->getIntensity()))
        {
            nb_particles++;
        }
    }

    surface = rgn->getSurface();
    if(surface == 0) return -1.0; //->

    return nb_particles/surface;
}

ChemicalSpecies* BiologicalWorld::getSpecieAdr(int spc_idx)
{
    list<ChemicalSpecies>::iterator it = m_species.begin();
	advance(it, spc_idx);

	return &(*it);
}

FluorophoreSpecies* BiologicalWorld::getFluoSpecieAdr(int fluoSpecie_idx)
{
    list<FluorophoreSpecies>::iterator it = m_fluo_species.begin();
	advance(it, fluoSpecie_idx);

	return &(*it);


}

void BiologicalWorld::updatePosition( float d_t, bool isPre_calc)
{
	for(auto it = m_particles.begin(); it!=m_particles.end(); ++it)
	{
		(*it).updatePosition(d_t, isPre_calc, m_randomNumberFactory);
	}
}

void BiologicalWorld::updateTrappingState(float d_t)
{
	for(Particle& ptcl : m_particles)
	{
		if(ptcl.isImmobile() == true) continue;

		if(ptcl.isTrapped() == true)
		{
			if(m_randomNumberFactory.uniformRandomNumber() <= ptcl.m_child_rgn->getKoff(ptcl.m_specie)* d_t)
			{
				ptcl.m_trapped = false;
				ptcl.m_child_rgn->deleteATrappedPrtlToNb(ptcl.m_specie);
				ptcl.m_trappingStateChanged_flag = true;
			}
			return; //->
		}
		else
		{
			Region* old_rgn = ptcl.m_child_rgn;
			auto rgn = m_regions.rbegin();
			auto mother_rgn = m_regions.rbegin();
			advance(mother_rgn, getNbRegions() - 1 - getRegionIdx(ptcl.m_mother_rgn));

			while(rgn != mother_rgn)
			{
				if(rgn->isACompartment(ptcl.getSpecie()) && ptcl.isInside(&*rgn)) break;
//				if(rgn->isACompartment(ptcl.getSpecie() && rgn->isInside(ptcl.m_r))) break;
				rgn++;
			}

			ptcl.m_child_rgn = &(*rgn);
			if(&(*rgn) == ptcl.m_mother_rgn) ptcl.m_child_rgn = ptcl.m_mother_rgn;
			if(old_rgn != ptcl.m_child_rgn) ptcl.m_trappingStateChanged_flag = true;


			if(ptcl.m_child_rgn->isTrappingActivated(ptcl.m_specie))
			{
				float k_on;
				if(ptcl.m_child_rgn->areSitesAbundant(ptcl.m_specie))
				{
					k_on = ptcl.m_child_rgn->getKonAbundant(ptcl.m_specie);
				}
				else
				{
					k_on = ptcl.m_child_rgn->getKonNAbundant(ptcl.m_specie) * ptcl.m_child_rgn->getCurrentSiteDensity(ptcl.m_specie);
				}

				if(m_randomNumberFactory.uniformRandomNumber() <= k_on* d_t)
				{
					ptcl.m_trapped = true;
					ptcl.m_trappingStateChanged_flag = true;
					ptcl.m_child_rgn->addTrappedPrtlToNb(ptcl.m_specie);
				}
			}
		}
	}
}

void BiologicalWorld::updateTrappingState(float d_t, Particle& ptcl, RandomNumberGenerator& randomNumberFactory)
{
	if(ptcl.isImmobile() == true) return;

	if(ptcl.isTrapped() == true)
	{
		if(randomNumberFactory.uniformRandomNumber() <= ptcl.m_child_rgn->getKoff(ptcl.m_specie)* d_t)
		{
			ptcl.m_trapped = false;
			ptcl.m_child_rgn->deleteATrappedPrtlToNb(ptcl.m_specie);
			ptcl.m_trappingStateChanged_flag = true;
		}
		return; //->
	}
	else
	{
		Region* old_rgn = ptcl.m_child_rgn;
		auto rgn = m_regions.rbegin();
		auto mother_rgn = m_regions.rbegin();
		advance(mother_rgn, getNbRegions() - 1 - getRegionIdx(ptcl.m_mother_rgn));

		while(rgn != mother_rgn)
		{
			if(rgn->isACompartment(ptcl.getSpecie()) && ptcl.isInside(&*rgn)) break;
			rgn++;
		}

		ptcl.m_child_rgn = &(*rgn);
		if(&(*rgn) == ptcl.m_mother_rgn) ptcl.m_child_rgn = ptcl.m_mother_rgn;
		if(old_rgn != ptcl.m_child_rgn) ptcl.m_trappingStateChanged_flag = true;


		if(ptcl.m_child_rgn->isTrappingActivated(ptcl.m_specie))
		{
			float k_on;
			if(ptcl.m_child_rgn->areSitesAbundant(ptcl.m_specie))
			{
				k_on = ptcl.m_child_rgn->getKonAbundant(ptcl.m_specie);
			}
			else
			{
				k_on = ptcl.m_child_rgn->getKonNAbundant(ptcl.m_specie) * ptcl.m_child_rgn->getCurrentSiteDensity(ptcl.m_specie);
			}

			if(randomNumberFactory.uniformRandomNumber() <= k_on* d_t)
			{
				ptcl.m_trapped = true;
				ptcl.m_trappingStateChanged_flag = true;
				ptcl.m_child_rgn->addTrappedPrtlToNb(ptcl.m_specie);
			}
		}
	}
}

void BiologicalWorld::updateD()
{
	for(auto& ptcl : m_particles)
	{
		ptcl.updateD(m_randomNumberFactory);
	}
}

void BiologicalWorld::updateD(Particle& ptcl)
{
	ptcl.updateD(m_randomNumberFactory);
}

int BiologicalWorld::getNbRegions()
{
	return m_regions.size();
}

void BiologicalWorld::deleteAllParticles()
{
	m_particles.clear();

	list<Region>::iterator it_rgn = m_regions.begin();
    list<ChemicalSpecies>::iterator it_spc = m_species.begin();

	for(it_rgn = m_regions.begin(); it_rgn!=m_regions.end(); ++it_rgn)
	{
		for(it_spc = m_species.begin(); it_spc!= m_species.end(); ++it_spc)
		{
			(*it_rgn).setNumberTrappedPrtl(&(*it_spc), 0);
		}
	}
}

void BiologicalWorld::setParticlePositions(vector<vec2>& r)
{
	int ptl_idx = 0;
	auto particle = m_particles.begin();
	while(particle!=m_particles.end())
	{
		particle->setR(r[ptl_idx]);
		particle++;
		ptl_idx++;
	}
}

void BiologicalWorld::setParticleColorMode(Particle::COLOR_MODE mode)
{
	for(Particle& ptcl : m_particles)
	{
		ptcl.setColorMode(mode);
	}
}

vector<vec2> BiologicalWorld::getParticlePositions()
{
	vector<vec2> r_v;

	auto particle = m_particles.begin();
	while(particle!=m_particles.end())
	{
		r_v.push_back(particle->getR());
		particle++;
	}

	return r_v;
}



