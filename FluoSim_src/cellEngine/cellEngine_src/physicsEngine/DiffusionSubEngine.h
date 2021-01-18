
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

#ifndef DIFFUSIONSUBENGINE_H
#define DIFFUSIONSUBENGINE_H


#include "chrono"
#include "random"
#include "thread"

#include "cellEngine_library_global.h"
    #include "biologicalWorld/Particle.h"
    #include "biologicalWorld/BiologicalWorld.h"
    #include "RandomNumberGenerator.h"

#include "toolBox_src/developmentTools/myClock.h"


class CELLENGINE_LIBRARYSHARED_EXPORT DiffusionSubEngine
{
public :

	enum ENGINE_MODE {SINGLETHREADED_MODE, MULTITHREADED_MODE, AUTOMATIC_SELECTION_MODE};
	enum ENGINE_SELECTED_MODE {SINGLETHREADED_SELECTED_MODE, MULTITHREADED_SELECTED_MODE};

    DiffusionSubEngine(BiologicalWorld* bio_wolrd);
	void updateSubSystem(float delta_t, int particle_idx_beg, int particle_idx_end,  int subSystem_idx);
	void updateSystem(float delta_t);
	void setEngineMode(ENGINE_MODE engine_mode);
	ENGINE_MODE getEngineMode();

	int getNbThreads();

	void updateSelectedMode();

	float getSingeThreadTime() const;
	float getMultiThreadTime() const;

private:

    BiologicalWorld* m_bio_world;

	ENGINE_MODE m_engine_mode;
	ENGINE_SELECTED_MODE m_engine_selected_mode;
	bool m_isUpdatingSelectedMode;
	uint m_currentNbMeasureInSingleThread;
	uint m_currentNbMeasureInMultiThread;

	uint m_nbThreadsMulti_perIt;
	vector<thread> m_threads_v;
    vector<RandomNumberGenerator> m_randomFactories_v;

	myChrono m_singleThreadLoop_clock;
	myChrono m_multiThreadLoop_clock;

};





#endif // DIFFUSIONSUBENGINE_H
