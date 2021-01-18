
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



#include "Probe.h"

using namespace std;
using namespace glm;



Probe::Probe()
{


}

Probe::Probe(BiologicalWorld* bio_world)
{
	m_bio_world = bio_world;

	m_region1 = NULL;
	m_region2 = NULL;

	m_specie1 = NULL;
	m_specie2 = NULL;
}

void Probe::setBiologicalWorld(BiologicalWorld* bio_world)
{
	m_bio_world = bio_world;
}

void Probe::setRegion1(Region* rgn)
{
	m_region1 = rgn;
}

void Probe::setRegion2(Region* rgn)
{
	m_region2 = rgn;
}

void Probe::setChemicalSpecie1(ChemicalSpecies* spc)
{
	m_specie1 = spc;
}

void Probe::setChemicalSpecie2(ChemicalSpecies* spc)
{
	m_specie2 = spc;
}

void Probe::setMeasureType(measureType measure_type)
{
	m_measure_type = measure_type;
}

void Probe::setGaussianBeamParam(myGaussianBeamParams &gaussianBeam_params)
{
	m_gaussianBeam_params = gaussianBeam_params;
}


float Probe::measure(int plane, float current_time, float dt)
{
	switch(m_measure_type)
	{
		case NONE:
		{
		}
		break;

		case INTENSITY:
		{
			if(m_region1)
			{
				float intensity = 0.0;

				for(Particle& ptcl : m_bio_world->m_particles)
				{
					if(ptcl.getSpecie() == m_specie1 &&
					   (ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)))
					{
						intensity += ptcl.getIntensity();
					}
				}

				m_signal.addValue(vec2(current_time, intensity));
				return intensity;
			}

		}
		break;

		case AVERAGE_INTENSITY:
		{
			if(m_region1 && m_region1->getSurface()>=0)
			{
				float intensity = 0.0;
				float average_intensity = 0.0;
				float surface = m_region1->getSurface();

				for(Particle& ptcl : m_bio_world->m_particles)
				{
					if(ptcl.getSpecie() == m_specie1 &&
					   (ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)))
					{
							intensity += ptcl.getIntensity();
					}
				}

				average_intensity = intensity / surface;
				m_signal.addValue(vec2(current_time, average_intensity));
				return average_intensity;
			}
		}
		break;

		case RELATIVE_INTENSITY:
		{

			return 0.0;
		}
		break;

		case RELATIVE_AVERAGE_INTENSITY:
		{
			{
				float intensity1 = 0.0;
				float average_intensity1 = 0.0;
				float surface1 = m_region1->getSurface();

				float intensity2 = 0.0;
				float average_intensity2 = 0.0;
				float surface2 = m_region2->getSurface();

				for(Particle& ptcl : m_bio_world->m_particles)
				{
					if(ptcl.getSpecie() == m_specie1 &&
					   (ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)))
					{
						intensity1 += ptcl.getIntensity();
					}

					if(ptcl.getSpecie() == m_specie2 &&
					   (ptcl.m_mother_rgn == m_region2 || ptcl.m_child_rgn == m_region2 || ptcl.isInside(m_region2)))

					{
						intensity2 += ptcl.getIntensity();
					}
				}

				average_intensity1 = intensity1 / surface1;
				average_intensity2 = intensity2 / surface2;

				m_signal.addValue(vec2(current_time, average_intensity1/average_intensity2));
//				m_signal.addValue(vec2(plane, average_intensity1/average_intensity2));
				return average_intensity1/average_intensity2;
			}
        }
		break;

        case RELATIVE_AVERAGE_INTENSITY_TWO_POP: // spc1 and spc2 intensities are meseared in rgn1 and compared to the intensity of the same spcs in rgn2
		{
			{
				float intensity1 = 0.0;
				float average_intensity1 = 0.0;
				float surface1 = m_region1->getSurface();

				float intensity2 = 0.0;
				float average_intensity2 = 0.0;
				float surface2 = m_region2->getSurface();

				for(Particle& ptcl : m_bio_world->m_particles)
				{
					if(ptcl.getSpecie() == m_specie1 &&
					   (ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)))
					{
						intensity1 += ptcl.getIntensity();
					}

					if(ptcl.getSpecie() == m_specie2 &&
					   (ptcl.m_mother_rgn == m_region2 || ptcl.m_child_rgn == m_region2 || ptcl.isInside(m_region2)))
					{
						intensity2 += ptcl.getIntensity();
					}
				}

				average_intensity1 = intensity1 / surface1;
				average_intensity2 = intensity2 / surface2;

				m_signal.addValue(vec2(current_time, average_intensity1/average_intensity2));
//				m_signal.addValue(vec2(plane, average_intensity1/average_intensity2));
				return average_intensity1/average_intensity2;
			}
		}
		break;

        case INTENSITY_IN_GAUSSIAN_BEAM: // spc1 and spc2 intensities are meseared in rgn1 and compared to the intensity of the same spcs in rgn2
		{
				float intensity1 = 0.0f;
				glm::vec2 dr;
				float dr_square;

				for(Particle& prtl : m_bio_world->m_particles)
				{
					if(prtl.getSpecie() == m_specie1 && prtl.isInside(m_region1))
					{
						dr = prtl.getR() - m_gaussianBeam_params.center;
						dr_square = dr.x*dr.x + dr.y*dr.y;

						if(m_gaussianBeam_params.noise_cutOff == 0 || dr_square <= 2*log(100.0f/m_gaussianBeam_params.noise_cutOff)*pow(m_gaussianBeam_params.sigma, 2.0f))
						{
							float gauss = exp(-dr_square/(2*pow(m_gaussianBeam_params.sigma,2.0)));
							intensity1 += prtl.getIntensity()*m_gaussianBeam_params.maxIntensity*gauss;

							if(m_gaussianBeam_params.koff >= 0 && dt >= 0)
							{
								prtl.getFluorophore()->bleach(gauss*m_gaussianBeam_params.koff, dt, m_bio_world->m_randomNumberFactory);
							}
						}
					}
				}

				m_signal.addValue(vec2(current_time, intensity1));
				return intensity1;
		}
		break;

        case AVERAGE_INTENSITY_IN_GAUSSIAN_BEAM: // spc1 and spc2 intensities are meseared in rgn1 and compared to the intensity of the same spcs in rgn2
		{
				float intensity1 = 0.0f;
				float average_intensity1 = 0.0f;
				float surface1 = m_region1->getSurface();
				glm::vec2 dr;
				float dr_square;

				for(auto& prtl : m_bio_world->m_particles)
				{
					if(prtl.getSpecie() == m_specie1 && prtl.isInside(m_region1))
					{
						dr = prtl.getR() - m_gaussianBeam_params.center;
						dr_square = dr.x*dr.x + dr.y*dr.y;
						if(m_gaussianBeam_params.noise_cutOff == 0 || dr_square <= 2*log(100.0f/m_gaussianBeam_params.noise_cutOff)*pow(m_gaussianBeam_params.sigma, 2.0f))
						{
							float gauss = exp(-dr_square/(2*pow(m_gaussianBeam_params.sigma,2.0)));
							intensity1 += prtl.getIntensity()*m_gaussianBeam_params.maxIntensity*gauss;

							if(m_gaussianBeam_params.koff >= 0 && dt >= 0)
							{
								prtl.getFluorophore()->bleach(gauss*m_gaussianBeam_params.koff, dt, m_bio_world->m_randomNumberFactory);
							}
						}
					}
				}

				average_intensity1 = intensity1 / surface1;


				m_signal.addValue(vec2(current_time, average_intensity1));
				return average_intensity1;
		}
		break;

		case TRACE_TRACKER:
		{

			map<Particle*, Trace>::iterator it_trc;
			FluoEvent fluo_event;



			for(Particle &ptcl : m_bio_world->m_particles)
			{
				//is the particle of the right specie? And is the particle visible?
				if(ptcl.getSpecie() == m_specie1)
				{
					//has the particle started a trace
					it_trc = m_runningTraces.find(&ptcl);
                    if(it_trc != m_runningTraces.end()) //the trace exists
					{
						if((ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)) &&
							ptcl.getIntensity())
						{
							fluo_event = FluoEvent{0, plane, ptcl.getR().x, ptcl.getR().y, 1, -1, 1};
							it_trc->second.addFluoEvent(fluo_event);
						}
						else
						{
							m_mesauredTraces.push_back((*it_trc).second);
							m_runningTraces.erase(it_trc);
						}
					}

					else
					{
						if((ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)) &&
							ptcl.getIntensity())
						{
							fluo_event = FluoEvent{0, plane, ptcl.getR().x, ptcl.getR().y, 1, -1, 1};
							m_runningTraces[&ptcl] = Trace();
							m_runningTraces[&ptcl].addFluoEvent(fluo_event);
						}

						else
						{
                            //the trace
						}
					}
				}
			}
		}
		break;

		case LOCALISATION :
		{
			for(Particle &ptcl : m_bio_world->m_particles)
			{
				//is the particle of the right specie? And is the particle visible?
				if(ptcl.getSpecie() == m_specie1)
				{
					//has the particle started a trace
					if((ptcl.m_mother_rgn == m_region1 || ptcl.m_child_rgn == m_region1 || ptcl.isInside(m_region1)) &&
						ptcl.getIntensity())
					{
						FluoEvent fluo_event = {0, plane, ptcl.getR().x, ptcl.getR().y, 1, -1, 1};
                        m_localisations_v.push_back(fluo_event);

					}
				}
			}
		}
		break;
	}
}

BiologicalWorld* Probe::getBiologicalWorld()
{
	return m_bio_world;
}

Region* Probe::getRegion1()
{
	return m_region1;
}

Region* Probe::getRegion2()
{
	return m_region2;
}

ChemicalSpecies* Probe::getChemicalSpecie1()
{
	return m_specie1;
}

ChemicalSpecies* Probe::getChemicalSpecie2()
{
	return m_specie2;
}

Probe::measureType Probe::getMeasureType()
{
	return m_measure_type;
}

myGaussianBeamParams Probe::getGaussianBeamParams()
{
	return m_gaussianBeam_params;
}


Signal& Probe::getSignalRef()
{
	return m_signal;
}

vector<Trace> Probe::getAllTraces()
{
	vector<Trace> traces = m_mesauredTraces;

	for(map<Particle*, Trace>::iterator it_trc = m_runningTraces.begin(); it_trc!= m_runningTraces.end(); ++it_trc)
	{
		traces.push_back((*it_trc).second);
	}

	return traces;
}

vector<FluoEvent> Probe::getAllLocalisations()
{
	return m_localisations_v;
}

void Probe::resetProbe()
{
    resetProbeMeasure();
    m_bio_world = 0;
    m_region1 = 0;
    m_region2 = 0;
    m_specie1 = 0;
    m_specie2 = 0;
    m_gaussianBeam_params = myGaussianBeamParams();
}

void Probe::resetProbeMeasure()
{
	m_runningTraces.clear();
	m_mesauredTraces.clear();
	m_signal.clearValues();
	m_localisations_v.clear();
}


