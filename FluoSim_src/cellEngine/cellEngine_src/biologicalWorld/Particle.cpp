
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




#include "Particle.h"

using namespace std;
using namespace glm;


//********************************
//*
//*			Tower
//*
//********************************

long Particle::m_nbReflection = 0;

Tower::Tower()
{
}

bool Tower::isInsideScope(const glm::vec2 &r) const
{
	vec2 dr = r - this->r;
	return ((dr.x*dr.x + dr.y*dr.y) < radius_squared);
}


//********************************
//*
//*			myParticle
//*
//********************************


Particle::Particle(vec2 r, Region* region, ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie)
{

	m_r = r;
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);

	m_color_mode = SPECIES_COLOR;
	m_mother_rgn = region;
	m_child_rgn = region;
	m_specie = specie;
	m_fluorophore.setFluoSpecie(fluoSpecie);
	m_trapped = false;
	m_D = 0;
	m_trappingStateChanged_flag = true;

	m_isImmobile = uniformDistributiion(0.0f, 1.0f) <= specie->getImmobileFraction();
}


Particle::Particle(Region* region, ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie)
{
	m_r = vec2(-1,-1);
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);

	m_color_mode = SPECIES_COLOR;
	m_mother_rgn = region;
	m_child_rgn = m_mother_rgn;
	m_specie = specie;
	m_fluorophore.setFluoSpecie(fluoSpecie);
	setPositionRandomly();
	m_trapped = false;
	m_D = 0;
	m_trappingStateChanged_flag = true;

	m_isImmobile = uniformDistributiion(0.0f, 1.0f) <= specie->getImmobileFraction();
}




Particle::Particle(Region* associated_region, Region* creation_region,
		   ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped)
{


	m_r = vec2(-1,-1);
	m_mother_rgn = associated_region;
	if(isTrapped) m_child_rgn = creation_region;
	else m_child_rgn = m_mother_rgn;
	m_color_mode = SPECIES_COLOR;
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);
	m_specie = specie;
	m_fluorophore.setFluoSpecie(fluoSpecie);
	m_trapped = isTrapped;
	m_D = 0;
	m_trappingStateChanged_flag = true;


	vec2 bary_r = creation_region->getBarycenter();
	float radius = std::sqrt(creation_region->getRadiusSquared());
	m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
			   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));
	while(creation_region->isInside(m_r) == false || associated_region->isInside(m_r) == false)
	{
		m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
				   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));
	}

	m_isImmobile = uniformDistributiion(0.0f, 1.0f) <= specie->getImmobileFraction();
}

Particle::Particle(Region* associated_region, Region* creation_region, Region* forbidden_region,
					   ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped)
{


	m_r = vec2(-1,-1);
	m_mother_rgn = associated_region;
	if(isTrapped) m_child_rgn = creation_region;
	else m_child_rgn = m_mother_rgn;
	m_color_mode = SPECIES_COLOR;
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);
	m_specie = specie;
	m_fluorophore.setFluoSpecie(fluoSpecie);
	m_trapped = isTrapped;
	m_D = 0;
	m_trappingStateChanged_flag = true;



	vec2 bary_r = creation_region->getBarycenter();
	float radius = std::sqrt(creation_region->getRadiusSquared());
	m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
			   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));

	while(creation_region->isInside(m_r) == false
		  || associated_region->isInside(m_r) == false
		  || forbidden_region->isInside(m_r) == true)
	{
		m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
				   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));
	}

	m_isImmobile = uniformDistributiion(0.0f, 1.0f) <= specie->getImmobileFraction();
}

Particle::Particle(Region* associated_region, Region* creation_region, vector<Region*> forbidden_regions,
					   ChemicalSpecies* specie, FluorophoreSpecies* fluoSpecie, bool isTrapped)
{
	m_r = vec2(-1,-1);
	m_mother_rgn = associated_region;
	if(isTrapped) m_child_rgn = creation_region;
	else m_child_rgn = m_mother_rgn;
	m_color_mode = SPECIES_COLOR;
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);
	m_specie = specie;
	m_fluorophore.setFluoSpecie(fluoSpecie);
	m_trapped = isTrapped;
	m_D = 0;
	m_trappingStateChanged_flag = true;


	vec2 bary_r = creation_region->getBarycenter();
	float radius = std::sqrt(creation_region->getRadiusSquared());

	bool isInsideForbidenRegion = true;
	while(creation_region->isInside(m_r) == false
		  || associated_region->isInside(m_r) == false
		  || isInsideForbidenRegion == true)
	{
		m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
				   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));

		isInsideForbidenRegion = false;
		for(Region* forbiden_rgn : forbidden_regions)
		{
			isInsideForbidenRegion = isInsideForbidenRegion || forbiden_rgn->isInside(m_r);
		}

	}

	m_isImmobile = uniformDistributiion(0.0f, 1.0f) <= specie->getImmobileFraction();
}


float Particle::getD()
{
	return m_D;
}


bool Particle::isTrapped()
{
	return m_trapped;
}

bool Particle::isInside(Region* rgn)
{
	if(rgn == m_mother_rgn) return true; //fastest way to know

    Tower& tower = m_towers[rgn];
	if(tower.isSet == true)
	{
		return tower.isInsideRgn;  //second fastest way to know
	}
	else
	{
		return rgn->isInside(m_r); //slower way to know...
	}
}

Region* Particle::getMotherRgn()
{
	return m_mother_rgn;
}

Region* Particle::getDiffRgn()
{
	return m_child_rgn;
}

void Particle::setDiffRgn(Region* diff_rgn)
{
	m_child_rgn = diff_rgn;
}

void Particle::setR(vec2 r)
{
	m_r = r;
}

vec2 Particle::getR()
{
	return m_r;
}

vec4 Particle::getColor()
{
	switch(m_color_mode)
	{
		case UNIQUE_COLOR :
		{
			return m_color; //->
		}
		break;

		case SPECIES_COLOR :
		{
			return m_specie->getColor(); //->
		}
		break;

		case TRAPPING_STATE_COLOR :
		{
			if(isTrapped()) return vec4(1,0,0,1); //->
			else return vec4(0,1,0,1); //->
		}
		break;
	}
}

void Particle::setPositionRandomly()
{
	vec2 bary_r = m_mother_rgn->getBarycenter();
	float radius = std::sqrt(m_mother_rgn->getRadiusSquared());

	m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
			   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));
	while(!m_mother_rgn->isInside(m_r))
	{
		m_r = vec2(uniformDistributiion(bary_r.x - radius, bary_r.x + radius),
				   uniformDistributiion(bary_r.y - radius, bary_r.y + radius));
	}
}

void Particle::setColorMode(COLOR_MODE mode)
{
	m_color_mode = mode;
}

Particle::COLOR_MODE Particle::getColorMode()
{
	return m_color_mode;
}

void Particle::setUniqueColor(glm::vec4 color)
{
	m_color = color;
}

void Particle::changeUniqueColor()
{
	m_color = vec4(uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   uniformDistributiion(0.0f, 1.0f),
				   1.0f);
}

bool Particle::isImmobile()
{
	return m_isImmobile;
}


void Particle::setIsImmobile(bool isImmobile)
{
	m_isImmobile = isImmobile;
}



void Particle::updatePosition(float delta_t, bool pre_calc, RandomNumberGenerator& randomFactory)
{
	if(m_isImmobile == true) return;

	vec2 d_r = random_gaussian(std::sqrt(2*m_D*delta_t));
	vec2 r_test = m_r + d_r;

	if(isTrapped())
	{
		while((!(m_mother_rgn->isInside(r_test))) || (!(m_child_rgn->isInside(r_test))))
		{
			d_r = random_gaussian(std::sqrt(2*m_D*delta_t));
			r_test = m_r + d_r;
		}
		m_r = r_test;
	}
	else
	{
//		if(m_towers[m_mother_rgn].isInsideScope(r_test) == true)
//		{
//			m_r = r_test;
//			return; //->
//		}

		int edge_to_avoid =-1;
		float dr_length = length(d_r);

		while(dr_length>0.0)
		{
			m_mother_rgn->reflect(m_r, d_r, edge_to_avoid, m_r, d_r, edge_to_avoid, randomFactory);
			dr_length = length(d_r);
		}

//		m_towers[m_mother_rgn].r = m_r;
//		m_towers[m_mother_rgn].radius_squared = m_mother_rgn->getMaximumRadiusSquared(m_r);

	}

}

void Particle::updatePosition(float delta_t, list<Region>& rgns_l, RandomNumberGenerator& factory)
{
	if(m_isImmobile == true) return;

	vec2 d_r = std::sqrt(2*m_D*delta_t)*vec2(factory.gaussianRandomNumber(false),
											 factory.gaussianRandomNumber(false));

//	vec2 d_r = std::sqrt(2*m_D*delta_t)*vec2(factory.gaussianRandomNumber(true),
//											 factory.gaussianRandomNumber(true));

	vec2 r_test;
	float dr_length = length(d_r);
	map<Region*, int> rgn_to_avoid;
	list<Region*> close_rgns_l;



	CROSSING_DIRECTION cross_dir;

	while(dr_length)
	{
		r_test = m_r + d_r;
		close_rgns_l.clear();
//		dr_length = length(d_r);

		if(isTrapped() == true)
		{
			if(m_towers[m_mother_rgn].isInsideScope(r_test) == false) close_rgns_l.push_back(m_mother_rgn);
			if(m_child_rgn != m_mother_rgn &&
			   m_towers[m_child_rgn].isInsideScope(r_test) == false) close_rgns_l.push_back(m_child_rgn);

			list<Region*> close_trapped_rgns_l;
			for(Region& rgn : rgns_l)
			{
				if(m_towers[&rgn].isInsideScope(r_test) == false)
				{
					close_trapped_rgns_l.push_back(&rgn);
				}
			}

			reflectParticle(*this, d_r, rgn_to_avoid, d_r, rgn_to_avoid, close_rgns_l, cross_dir, factory);
			dr_length = length(d_r);

			for(Region* rgn : close_trapped_rgns_l)
			{
				m_towers[rgn].r = m_r;
                m_towers[rgn].radius_squared = (1-0.005)*rgn->getMaximumRadiusSquared(m_r, m_towers[rgn].isInsideRgn);//0.005 to avoid particle to be too close to the region
			}


		}
		else
		{
			for(Region& rgn : rgns_l)
			{
				if(m_towers[&rgn].isInsideScope(r_test) == false)
				{
					close_rgns_l.push_back(&rgn);
				}
			}

			reflectParticle(*this, d_r, rgn_to_avoid, d_r, rgn_to_avoid, close_rgns_l, cross_dir, factory);
			dr_length = length(d_r);

			for(Region* rgn : close_rgns_l)
			{
				m_towers[rgn].r = m_r;
                m_towers[rgn].radius_squared = (1-0.005)*rgn->getMaximumRadiusSquared(m_r, m_towers[rgn].isInsideRgn);//0.005 to avoid particle to be too close from the region
			}
		}

	}
}

void Particle::updateTrappingState(float delta_t, list<Region>& regions, RandomNumberGenerator& factory)
{
	if(m_isImmobile == true) return;

	if(isTrapped() == true)
	{
		if(factory.uniformRandomNumber() <= m_child_rgn->getKoff(m_specie)* delta_t)
		{
			m_trapped = false;
			m_child_rgn->deleteATrappedPrtlToNb(m_specie);
			m_trappingStateChanged_flag = true;
		}
		return; //->
	}
	else
	{
		Region* old_rgn = m_child_rgn;
		auto rgn = regions.rbegin();
		while(rgn != regions.rend() && &(*rgn) != m_mother_rgn)
		{
			if(rgn->isACompartment(m_specie) == true && isInside(&*rgn) == true)
			{
				m_child_rgn = &(*rgn);
				break; //->
			}
			rgn++;
		}

		if(&(*rgn) == m_mother_rgn) m_child_rgn = m_mother_rgn;
		if(old_rgn != m_child_rgn) m_trappingStateChanged_flag = true;

		if(m_child_rgn->isTrappingActivated(m_specie))
		{
			float k_on;
			if(m_child_rgn->areSitesAbundant(m_specie))
			{
				k_on = m_child_rgn->getKonAbundant(m_specie);
			}
			else
			{
				k_on = m_child_rgn->getKonNAbundant(m_specie) * m_child_rgn->getCurrentSiteDensity(m_specie);
			}


			if(factory.uniformRandomNumber() <= k_on* delta_t)
			{
				m_trapped = 1;
				m_trappingStateChanged_flag = true;
				m_child_rgn->addTrappedPrtlToNb(m_specie);
			}
		}
	}
}


void Particle::updateD(RandomNumberGenerator& factory)
{
	if(m_isImmobile == true) return;

	if(m_trappingStateChanged_flag == true)
	{
		if(m_trapped == true)
		{
			if(m_child_rgn->isDDistributed(m_specie) == true)
			{
				float poisson_mean = m_child_rgn->getPoissonMean(m_specie);
				factory.setPoissonMean(poisson_mean);
				m_D = m_child_rgn->getDtrapped(m_specie)*factory.poissonRandomNumber()/poisson_mean;
			}

			else m_D = m_child_rgn->getDtrapped(m_specie);

		}
		else
		{
			m_D = m_child_rgn->getD(m_specie);
		}

		m_trappingStateChanged_flag = false;
	}
}

void Particle::updatePhotophysicState(float delta_t, RandomNumberGenerator& factory)
{
	m_fluorophore.updatePhotophysicState(delta_t, factory);
}

ChemicalSpecies* Particle::getSpecie()
{
	return m_specie;
}


const FluorophoreSpecies* Particle::getFluoSpecie()
{
	return m_fluorophore.getFluoSpecie();
}

float Particle::getIntensity()
{
	return m_fluorophore.getIntensity();
}

Fluorophore* Particle::getFluorophore()
{
	return &m_fluorophore;
}

void Particle::addTower(Region* rgn)
{
    Tower& tower = m_towers[rgn];
	tower.r = m_r;
	tower.radius_squared = rgn->getMaximumRadiusSquared(m_r, tower.isInsideRgn);
	tower.isSet = true;
}

void Particle::removeTower(Region* rgn)
{
	m_towers.erase(rgn);
}

void reflectParticle(Particle& ptcl, vec2 dr_start, map<Region*, int> rgns_to_avoid_start,
					 vec2& dr_end, map<Region*, int>& rgns_to_avoid_end, list<Region*> rgns_l,
					 CROSSING_DIRECTION& cross_dir_end, RandomNumberGenerator& factory)
{
	rgns_to_avoid_end.clear();

	vec2 r_start = ptcl.getR();
	vec2 r_end;

	ChemicalSpecies* specie = ptcl.getSpecie();

	CROSSING_DIRECTION temp_cross_dir;
	CROSSING_DIRECTION cross_dir;
	int edge_to_avoid =-1;
	float t_end = std::numeric_limits<float>::max();
	float temp_t_end = -1;
	int intersected_edge = -1;
	int temp_intersected_edge = -1;
	Region* intersected_rgn = 0;
	bool hasIntersected = false;

	for(Region* rgn : rgns_l)
	{
		float temp_t_end = -1;

		map<Region*, int>::iterator rgn_to_avoid = rgns_to_avoid_start.find(rgn);
		if(rgn_to_avoid != rgns_to_avoid_start.end()) edge_to_avoid = rgns_to_avoid_start[rgn];
		else edge_to_avoid = -1;

		if(rgn->isACompartment(specie) &&
		   rgn->intersect(r_start, dr_start, edge_to_avoid, temp_t_end, temp_intersected_edge, temp_cross_dir) == true)
		{
//			if(temp_t_end == t_end)
//			{
//				rgns_to_avoid_end[rgn] = temp_intersected_edge;
//			}
			if(temp_t_end < t_end)
			{
				rgns_to_avoid_end.clear();

				t_end = temp_t_end;
				hasIntersected = true;
				intersected_rgn = rgn;
				intersected_edge = temp_intersected_edge;
				cross_dir = temp_cross_dir;
				cross_dir_end = temp_cross_dir;

				rgns_to_avoid_end[rgn] = temp_intersected_edge;
			}
		}
	}


	if(hasIntersected == true)
	{
		ptcl.m_nbReflection++;

		map<Region*, int>::iterator rgn_to_avoid = rgns_to_avoid_start.find(intersected_rgn);
		if(rgn_to_avoid != rgns_to_avoid_start.end()) edge_to_avoid = rgns_to_avoid_start[intersected_rgn];
		else edge_to_avoid = -1;

		float p_crossing = intersected_rgn->getCrossing(specie, cross_dir); //particle not trapped
		if(ptcl.isTrapped() == true) p_crossing = 0.0f; //particle is trapped
		intersected_rgn->reflect(r_start, dr_start, edge_to_avoid, r_end, dr_end, edge_to_avoid, factory, p_crossing);
	}

	else
	{
        r_end = r_start + (1-0.005f)*dr_start;//0.005f to avoid new position to be too close to the region
		dr_end = {0,0};
	}

	ptcl.setR(r_end);
}





