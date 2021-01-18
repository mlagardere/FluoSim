
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



#include "DiffusionSubEngine.h"


using namespace std;
using namespace glm;

DiffusionSubEngine::DiffusionSubEngine(BiologicalWorld * bio_wolrd)
{
	m_bio_world = bio_wolrd;

	m_singleThreadLoop_clock.setNbRecordedTours(20);
	m_multiThreadLoop_clock.setNbRecordedTours(20);

	m_nbThreadsMulti_perIt = std::thread::hardware_concurrency();
	cout<<"Hint multiThread : "<<std::thread::hardware_concurrency()<<"\n";

	for(int t_idx= 0; t_idx <= m_nbThreadsMulti_perIt-1; t_idx++)
	{
		m_threads_v.push_back(thread());
        m_randomFactories_v.push_back(RandomNumberGenerator());
	}

}

void DiffusionSubEngine::updateSubSystem(float delta_t, int particle_idx_beg, int particle_idx_end, int thread_idx)
{
	if(particle_idx_end - particle_idx_beg == 0) return;

	auto particle_beg = m_bio_world->m_particles.begin();
	auto particle_end = m_bio_world->m_particles.begin();
	advance(particle_beg, particle_idx_beg);
	advance(particle_end, particle_idx_end);

	if(m_bio_world->isFixed() == true)
	{
		for(auto particle = particle_beg; particle != particle_end; particle++)
		{
			particle->updatePhotophysicState(delta_t, m_bio_world->m_randomNumberFactory);
		}
	}
	else
	{
		int particle_idx = 0;
		for(auto particle = particle_beg; particle != particle_end; particle++)
		{
			particle->updatePosition(delta_t,m_bio_world->m_regions,  m_randomFactories_v[thread_idx]);
			m_bio_world->updateTrappingState(delta_t, *particle, m_randomFactories_v[thread_idx]);
			m_bio_world->updateD(*particle);
			particle->updatePhotophysicState(delta_t, m_randomFactories_v[thread_idx]);
			particle_idx++;
		}
	}
}


void DiffusionSubEngine::updateSystem(float delta_t)
{
	switch(m_engine_mode)
	{
		case SINGLETHREADED_MODE :
		{
			if(m_bio_world->isFixed() == true)
			{
				for(Particle& part : m_bio_world->m_particles)
				{
					part.updatePhotophysicState(delta_t, m_bio_world->m_randomNumberFactory);
				}
			}
			else
			{
				for(Particle& part : m_bio_world->m_particles)
				{
					part.updatePosition(delta_t, m_bio_world->m_regions, m_bio_world->m_randomNumberFactory);
					m_bio_world->updateTrappingState(delta_t, part, m_bio_world->m_randomNumberFactory);
					m_bio_world->updateD(part);
					part.updatePhotophysicState(delta_t, m_bio_world->m_randomNumberFactory);
				}
			}
		}
		break;

		case MULTITHREADED_MODE :
		{
			m_multiThreadLoop_clock.startTour();

			int nbThreads = m_nbThreadsMulti_perIt;

			int nb_particles = m_bio_world->m_particles.size();
			int nb_particle_per_thread;
			int nb_particle_last_thread;
			if(nb_particles%nbThreads == 0)
			{
				nb_particle_per_thread = nb_particles/nbThreads;
				nb_particle_last_thread = nb_particle_per_thread;
			}
			else
			{
				nb_particle_per_thread = nb_particles/(nbThreads); //n = n/a + n%a
				nb_particle_last_thread= nb_particles/(nbThreads) + nb_particles%(nbThreads);
			}

			for(int it_idx = 0; it_idx <= int(nbThreads)-2; it_idx++)
			{
                m_threads_v[it_idx] = thread(&DiffusionSubEngine::updateSubSystem, this,
										   delta_t,
										   it_idx*nb_particle_per_thread,
										   (it_idx+1)*nb_particle_per_thread,
											 it_idx); //no -1 since the end is not reached
			}

            m_threads_v[nbThreads-1] = thread(&DiffusionSubEngine::updateSubSystem, this,
													  delta_t,
													  (nbThreads-1)*nb_particle_per_thread,
													  (nbThreads-1)*nb_particle_per_thread+nb_particle_last_thread,
													nbThreads-1);  //no -1 since the end is not reached

			for(uint thread_idx = 0; thread_idx < nbThreads; thread_idx++)
			{
				m_threads_v[thread_idx].join();
			}

			m_multiThreadLoop_clock.endTour();
		}
		break;

		case AUTOMATIC_SELECTION_MODE :
		{
			if(m_isUpdatingSelectedMode == true)
			{
				if(m_currentNbMeasureInSingleThread !=
				   m_singleThreadLoop_clock.getNbRecordedTours())
				{
					m_engine_selected_mode = SINGLETHREADED_SELECTED_MODE;
					m_currentNbMeasureInSingleThread++;
				}
				else
				{
					if(m_currentNbMeasureInMultiThread !=
					   m_multiThreadLoop_clock.getNbRecordedTours())
					{
						m_engine_selected_mode = MULTITHREADED_SELECTED_MODE;
						m_currentNbMeasureInMultiThread++;
					}
					else //we hava finish both evaluation in single and multi mode
					{
						if(m_singleThreadLoop_clock.getAveragedTimeInTours() <=
						   m_multiThreadLoop_clock.getAveragedTimeInTours())
						{
							m_engine_selected_mode = SINGLETHREADED_SELECTED_MODE;
						}
						else
						{
							m_engine_selected_mode = MULTITHREADED_SELECTED_MODE;
						}

						m_isUpdatingSelectedMode = false;
					}
				}
			}

			switch(m_engine_selected_mode)
			{
				case SINGLETHREADED_SELECTED_MODE :
				{
					m_singleThreadLoop_clock.startTour();
					if(m_bio_world->isFixed() == true)
					{
						for(Particle& part : m_bio_world->m_particles)
						{
							part.updatePhotophysicState(delta_t, m_bio_world->m_randomNumberFactory);
						}
					}
					else
					{
						for(Particle& part : m_bio_world->m_particles)
						{
							part.updatePosition(delta_t, m_bio_world->m_regions, m_bio_world->m_randomNumberFactory);
							m_bio_world->updateTrappingState(delta_t, part, m_bio_world->m_randomNumberFactory);
							m_bio_world->updateD(part);
							part.updatePhotophysicState(delta_t, m_bio_world->m_randomNumberFactory);
						}
					}
					m_singleThreadLoop_clock.endTour();
				}
				break;

				case MULTITHREADED_SELECTED_MODE :
				{
					m_multiThreadLoop_clock.startTour();

					int nbThreads = m_nbThreadsMulti_perIt;

					int nb_particles = m_bio_world->m_particles.size();
					int nb_particle_per_thread;
					int nb_particle_last_thread;
					if(nb_particles%nbThreads == 0)
					{
						nb_particle_per_thread = nb_particles/nbThreads;
						nb_particle_last_thread = nb_particle_per_thread;
					}
					else
					{
						nb_particle_per_thread = nb_particles/(nbThreads); //n = n/a + n%a
						nb_particle_last_thread= nb_particles/(nbThreads) + nb_particles%(nbThreads);
					}

					for(int it_idx = 0; it_idx <= int(nbThreads)-2; it_idx++)
					{
                        m_threads_v[it_idx] = thread(&DiffusionSubEngine::updateSubSystem, this,
												   delta_t,
												   it_idx*nb_particle_per_thread,
												   (it_idx+1)*nb_particle_per_thread,
													 it_idx); //no -1 since the end is not reached
					}

                    m_threads_v[nbThreads-1] = thread(&DiffusionSubEngine::updateSubSystem, this,
															  delta_t,
															  (nbThreads-1)*nb_particle_per_thread,
															  (nbThreads-1)*nb_particle_per_thread+nb_particle_last_thread,
															nbThreads-1);  //no -1 since the end is not reached

					for(uint thread_idx = 0; thread_idx < nbThreads; thread_idx++)
					{
						m_threads_v[thread_idx].join();
					}

					m_multiThreadLoop_clock.endTour();
				}
				break;
			}
		}
		break;
	}
}

int DiffusionSubEngine::getNbThreads()
{
	if(m_engine_mode == SINGLETHREADED_MODE || (m_engine_mode == AUTOMATIC_SELECTION_MODE &&
												m_engine_selected_mode == SINGLETHREADED_SELECTED_MODE))
	{
		return 1;
	}
	else return m_nbThreadsMulti_perIt;
}

void DiffusionSubEngine::setEngineMode(ENGINE_MODE engine_mode)
{
	m_engine_mode = engine_mode;
}


DiffusionSubEngine::ENGINE_MODE DiffusionSubEngine::getEngineMode()
{
	return m_engine_mode;
}


void DiffusionSubEngine::updateSelectedMode()
{
	m_isUpdatingSelectedMode = true;
	m_currentNbMeasureInSingleThread = 0;
	m_currentNbMeasureInMultiThread = 0;
}

float DiffusionSubEngine::getSingeThreadTime()  const
{
	return m_singleThreadLoop_clock.getAveragedTimeInTours();
}

float DiffusionSubEngine::getMultiThreadTime()  const
{
	return m_multiThreadLoop_clock.getAveragedTimeInTours();
}

