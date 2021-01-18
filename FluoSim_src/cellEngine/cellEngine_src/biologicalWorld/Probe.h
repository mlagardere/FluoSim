
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

#ifndef PROBE_H
#define PROBE_H

#include "list"

#include "cellEngine_library_global.h"
    #include "Measure/Trace.h"
    #include "Measure/Signal.h"
    #include "Region_gpu.h"
    #include "ChemicalSpecies.h"
    #include "BiologicalWorld.h"

struct CELLENGINE_LIBRARYSHARED_EXPORT myGaussianBeamParams
{
	float maxIntensity = -1;
	float sigma = -1;
	float noise_cutOff = -1;
	glm::vec2 center = {0,0};
	float koff = -1.0f;
};

class CELLENGINE_LIBRARYSHARED_EXPORT Probe
{
public :

    enum measureType{NONE, INTENSITY, AVERAGE_INTENSITY, RELATIVE_INTENSITY,
                     RELATIVE_AVERAGE_INTENSITY, RELATIVE_AVERAGE_INTENSITY_TWO_POP,
					 AVERAGE_INTENSITY_IN_GAUSSIAN_BEAM, INTENSITY_IN_GAUSSIAN_BEAM,
					 TRACE_TRACKER, LOCALISATION};

    Probe(BiologicalWorld* bio_world);
    Probe();
    void setBiologicalWorld(BiologicalWorld* bio_world);
    void setRegion1(Region* rgn);
    void setRegion2(Region* rgn);
    void setChemicalSpecie1(ChemicalSpecies* spc);
    void setChemicalSpecie2(ChemicalSpecies* spc);
    void setMeasureType(measureType measure_type);
	void setGaussianBeamParam(myGaussianBeamParams& gaussianBeam_params);

    BiologicalWorld* getBiologicalWorld();
	Region* getRegion1();
	Region* getRegion2();
    ChemicalSpecies* getChemicalSpecie1();
    ChemicalSpecies* getChemicalSpecie2();
	measureType getMeasureType();
	myGaussianBeamParams getGaussianBeamParams();
    Signal& getSignalRef();
    vector<Trace> getAllTraces();
    vector<FluoEvent>getAllLocalisations();

    void resetProbe();
	void resetProbeMeasure();
	float measure(int plane =-1, float current_time = -10, float dt = -1.0f);

private :

//probe data
	measureType m_measure_type;
	myGaussianBeamParams m_gaussianBeam_params;

//measurement data
    BiologicalWorld* m_bio_world;
        Region* m_region1;
		Region* m_region2;
        ChemicalSpecies* m_specie1;
        ChemicalSpecies* m_specie2;

//signal data
    Signal m_signal;
    vector<Trace> m_mesauredTraces;
    map<Particle*, Trace> m_runningTraces;

    vector<FluoEvent> m_localisations_v;
};



#endif // PROBE_H
