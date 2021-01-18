
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



#include "FluoSim.h"
#include "TracePlayer.h"

using namespace std;
using namespace glm;


double inter_corelation(int j, vec2* r_1, vec2* r_2, int n)
{
	double I=0;
	if(j>=n){I=0;}
	else
	{
		float t_mean=0;
		float f_mean=0;
		float ft_mean = 0;

		for (int i=0; i<=n-j-1; i++)
		{
			t_mean=t_mean+ (*(r_2+i+j)).y; //la deuxième fonction est translatée
			f_mean=f_mean+ (*(r_1+i)).y;
		}

		t_mean /= n-j;
		f_mean /= n-j;

		for (int i=0; i<=n-j-1; i++)
		{
			ft_mean = ft_mean + ((*(r_1+i)).y-f_mean)*((*(r_2+i+j)).y-t_mean);
		}

		ft_mean /= n-j;
		I = ft_mean / (t_mean * f_mean);

	}
	return I;
}

FluoSimModel::FluoSimModel(QApplication* main_app) :
	FluoSimView(main_app),
    m_font1("Resources/Fonts/font_glWord.png", test_desc),
	m_simulationFPS_glWord(&m_font1),
	m_singleThreadedTime_glWord(&m_font1),
	m_multiThreadedTime_glWord(&m_font1),
	m_nbThreads_glWord(&m_font1),
	m_diffTime_glWord(&m_font1),
	m_trappTime_glWord(&m_font1),
	m_photoTime_glWord(&m_font1),
	m_tracePlayer(m_scrn.getRenderWindow())

{
	m_bioWorld = 0;
	m_engine = 0;
	m_backgroundImage = 0;
	m_cameraImage = 0;
	m_frapHead = 0;
	m_stack_tiffHdl = 0;

	m_main_app = main_app;
	m_app_running = true;
    m_frame_rate = 30.0f;//targetted rendering frame rate

//SOFTWARE_STATES
	m_showAndHideSimParametersWdgt_action.setChecked(true);
	m_showAndHideGraphicWdgt_action.setChecked(true);

//SIMULATION MODE
    initializeModelParameters();
	m_scrn.getRenderWindow()->makeCurrent();
}


FluoSimModel::~FluoSimModel()
{
    for(Probe* &probe : m_experimental_probes_v)
	{
        if(probe != 0) delete probe;
        probe = 0;
	}
	m_experimental_probes_v.clear();

	if(m_bioWorld != 0) delete m_bioWorld;
	if(m_engine != 0) delete m_engine;
}




void FluoSimModel::initializeBioWorld()
{
	if(m_bioWorld != 0) return; //->

    m_scrn.setLUT("Resources/Luts/defaultLUT.rgb");
	m_bioWorld = new BiologicalWorld();
	m_engine = new DiffusionSubEngine(m_bioWorld);
    m_engine->setEngineMode(DiffusionSubEngine::AUTOMATIC_SELECTION_MODE);
    //m_engine->setEngineMode(DiffusionSubEngine::MULTITHREADED_MODE);

	ChemicalSpecies spc1("spc1", vec4(0.2,1,0.2,1));
		m_bioWorld->addChemicalSpecie(spc1);
		m_bioWorld->addFluorophoreSpecie(0,0); //will be set later on...

	updateParticleSystemParams();
	updateDynamicParams();
	updatePhotoPhysicsParams();
	updateExperimentParams();
}

void FluoSimModel::initializeModelParameters()
{
    m_simulation_states.simulator_mode = LIVE_MODE;
    m_simulation_states.simulationStarted = false;
    m_simulation_states.simulationEnded = true;
    m_simulation_states.simulationPaused = false;
    m_simulation_states.isSingleShot = false;
    m_simulation_states.isRenderingEnabled = true;

    m_simulation_params.current_plane = 0;
    m_simulation_params.current_time = 0.0f;

    m_simulation_states.isGraphicVisible = false;
    m_simulation_states.isParameterWidgetVisible = true;

    //PARTICLE SYSTEM
    m_particleSystem_params.N_particles = 0;

    //SIMULATION
    m_simulation_params.pixel_size = 0.160;
    m_tracePlayer.setPixelSize(m_simulation_params.pixel_size);


    m_simulation_params.dt_sim = 0.020;
    m_tracePlayer.setTimeStep(m_simulation_params.dt_sim);

    //DYNAMIC PARAMETERSu
    m_dynamic_params.D_insideContact = 0.0;
    m_dynamic_params.D_outsideContact = 0.0;
    m_dynamic_params.D_trapped = 0.0;
    m_dynamic_params.k_off = 0;
    m_dynamic_params.k_on = 0;
    m_dynamic_params.crossingProbability_outIn = 1.0f;
    m_dynamic_params.immobileFraction = 0.0f;
    m_dynamic_params.isDrugAffected = false;
    m_dynamic_params.drug_affectedParam = DOUT_DRUGPARAM;
    m_dynamic_params.drug_newValue = 0.0;
    m_dynamic_params.isFixed = false;

    //PHOTOPHYSICS PARAMETERS

    m_photoPhysics_params.k_off_fluo = 0;
    m_photoPhysics_params.k_on_fluo = 0;

    //RENDERING PARAMETERS

    //casual rendering
    m_rendering_params.image_intensity = 1.0f;
    m_rendering_params.highlightedRegion_names = {};
    m_rendering_params.areRegionsVisible = true;
    m_rendering_params.isBackgroundImageVisible_inGeom = true;
    m_rendering_params.isBackgroundImageVisible_inLiveAndExp = true;
    m_rendering_params.isBackgroundImageSmoothed = false;
    m_rendering_params.spot_intensity = 0.5*65535;
    m_rendering_params.isSpotIntensity_inPhotonsPerSec = false;
    m_rendering_params.ADCounts_perPhoton = 100;
    m_rendering_params.spot_size = 0.250; //[µm]
    m_rendering_params.point_size = 3;
    m_rendering_params.point_intensity = 1.0f;
    m_rendering_params.isAutoscale = true;
    m_rendering_params.autoscale_factor = 1.0f;
    m_rendering_params.rendering_mode = RENDERING_POINTS;
    m_rendering_params.particle_color = PARTICLE_GREEN;
    m_rendering_params.areWordsVisible = true;
    m_rendering_params.clear_color = vec4(0,0,0,0);
    m_rendering_params.backgroundImage_path = "";
    m_rendering_params.photonBackground = 0;
    m_rendering_params.isUsingPoissonNoise = false;
    m_rendering_params.cameraOffset = 0;
    m_rendering_params.readoutNoise_sigma = 0.0f;
    m_rendering_params.rendering_target = Screen::SCREEN_FRAMEBUFFER;

    //storm rendering
    m_stormRendering_params.areRegionsVisible = false;
    m_stormRendering_params.isBackgroundImageSmoothed = true;
    m_stormRendering_params.spot_intensity = 1.0f;
    m_stormRendering_params.spot_size = 0.250; //[µm]
    m_stormRendering_params.point_size = 1;
    m_stormRendering_params.point_intensity = 1.0f;
    m_stormRendering_params.rendering_mode = RENDERING_ACCUMULATION_GAUSSIANS;
    m_stormRendering_params.particle_color = PARTICLE_WHITE;
    m_stormRendering_params.clear_color = vec4(0,0,0,0);
    m_stormRendering_params.photonBackground = 0;
    m_stormRendering_params.isUsingPoissonNoise = false;
    m_stormRendering_params.cameraOffset = 0;
    m_stormRendering_params.readoutNoise_sigma = 0.0f;
    m_stormRendering_params.rendering_target = Screen::CAMERA_FRAMEBUFFER;

    //camera rendering
    m_cameraRendering_params.areRegionsVisible = false;
    m_cameraRendering_params.isBackgroundImageVisible_inLiveAndExp = false;
    m_cameraRendering_params.isBackgroundImageSmoothed = true;
    m_cameraRendering_params.spot_intensity = 1.0f;
    m_cameraRendering_params.isSpotIntensity_inPhotonsPerSec = false;
    m_cameraRendering_params.ADCounts_perPhoton = 100;
    m_cameraRendering_params.spot_size = 0.250; //[µm]
    m_cameraRendering_params.rendering_mode = RENDERING_GAUSSIANS;
    m_cameraRendering_params.particle_color = PARTICLE_WHITE;
    m_cameraRendering_params.clear_color = vec4(0,0,0,0);
    m_cameraRendering_params.photonBackground = 0;
    m_cameraRendering_params.isUsingPoissonNoise = false;
    m_cameraRendering_params.cameraOffset = 0;
    m_cameraRendering_params.readoutNoise_sigma = 0.0f;
    m_cameraRendering_params.rendering_target = Screen::CAMERA_FRAMEBUFFER;

    m_current_rendering_params = &m_rendering_params;

    //LIVE EXPERIMENT PARAMTERS :

    m_liveExperiment_params.measured_RgnName = "";
    m_liveExperiment_params.bleached_RgnName = "";
    m_liveExperiment_params.photoActivated_RgnName = "";
    m_liveExperiment_params.experimentType = PHOTOBLEACHING_LIVE_EXPERIMENT;

//EXPERIMENT MODE
    m_experiment_params.experimentType = SPT_EXPERIMENT;
    m_experiment_params.acquisitionType = STREAM_ACQUISITION;

    m_experiment_params.N_planes = 1000;
    m_experiment_params.N_frap = 100;
    m_experiment_params.dN_frap = 100;
    m_experiment_params.k_off_frap = 100;
    m_experiment_params.initial_enrichment = -1.0;

    m_experiment_params.is_fcsUsingGaussianBeam = false;
    m_experiment_params.fcs_beamSigma = 0.0f;
    m_experiment_params.fcs_maxIntensity = 0.0f;
    m_experiment_params.fcs_beamKoff = 0.0f;

    m_experiment_params.N_photoActivation = 100;
    m_experiment_params.dN_photoActivation = 100;
    m_experiment_params.k_on_photoActivation = 100;


    m_experiment_params.N_drug = 100;
    m_experiment_params.drug_affectedParam = DOUT_DRUGPARAM;
    m_experiment_params.drug_newValue = 0.0;

    m_experiment_params.SRI_pointingAccuracy = 0.1f;
    m_experiment_params.SRI_zoom = 1;

    m_experiment_params.N_presequ = 1000;
    m_experiment_params.N_repetition = 1;
    m_experiment_params.index_repetion = 0;
    m_experiment_params.acquisitionPeriod = m_simulation_params.dt_sim;

    m_experiment_params.measuredRegionNames_v = {};
    m_experiment_params.bleachedRegionNames_v = {};

    m_experiment_params.recording_mode = ALL_IN_ONE_RECORDING;
    m_experiment_params.file_destination = string(m_main_app->applicationDirPath().toLocal8Bit().data());
        setDestinationPath("SimulationOutputs"); //relative path
    m_experiment_params.isExportingStack = false;

}

void FluoSimModel::evaluateSteadyState()
{
	int N_rgn = m_bioWorld->getNbRegions();
	if(m_bioWorld == 0 || N_rgn <= 0) return; //->

	m_bioWorld->deleteAllParticles();

	float k_on = m_dynamic_params.k_on;
	float k_off = m_dynamic_params.k_off;
    float D_insideContact = m_dynamic_params.D_insideContact;
    float D_outsideContact = m_dynamic_params.D_outsideContact;
	float p_crossing_outIn = m_dynamic_params.crossingProbability_outIn;

	if(m_dynamic_params.k_on == 0) k_on = std::pow(2,-126);
	if(m_dynamic_params.k_off == 0) k_off = std::pow(2,-126);
	if(m_dynamic_params.D_outsideContact == 0) D_outsideContact = std::pow(2,-126);
	if(D_insideContact == 0) D_insideContact = std::pow(2,-126);
	if(m_dynamic_params.crossingProbability_outIn == 0) p_crossing_outIn = std::pow(2,-126);

	vector<int> forbidden_rgns_v;
	float S2 = 0;
	for(int rgn_idx = 1; rgn_idx < N_rgn; rgn_idx++)
	{
		vector<vec2> r_v;
		Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
		if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == false) continue; //<
		getIntersectionRegion(m_bioWorld->getRegionRef(0), rgn, r_v);
		S2 += computeSurface(r_v);
		forbidden_rgns_v.push_back(rgn_idx);
	}
	float S1 = m_bioWorld->getRegionRef(0).getSurface()-S2;

	float eps_T = 1 + k_on / k_off;
	float eps_D = D_outsideContact*p_crossing_outIn/D_insideContact;

	float sigma1 = 1.0f/(S1+S2*eps_D*eps_T);
	float sigma2_D = (eps_D)/(S1+S2*eps_D*eps_T);
	float sigma2_T = eps_D*(eps_T-1)/(S1+S2*eps_D*eps_T);

    struct particleState
	{
		int creation_rgn_idx;
		vector<int> forbidden_rgns_v;
		bool is_trapped;
		float interval_min, interval_max; //if rand in [0; INTERVAL_MAX[ is in [interval_min; interval_max] then  a particle is created in the associated state
		int nb_particles;
	};

    vector<particleState> particle_states;
	particle_states.push_back({0, forbidden_rgns_v, false, 0.0f, S1*sigma1, 0});
	for(int rgn_idx = 1; rgn_idx < N_rgn; rgn_idx++)
	{
        Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
        if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == false) continue; //<

        vector<vec2> r_v;
        getIntersectionRegion(m_bioWorld->getRegionRef(0), rgn, r_v);
		float S = computeSurface(r_v);

		particle_states.push_back({rgn_idx, {}, false, particle_states.back().interval_max,
												 particle_states.back().interval_max +S*sigma2_D, 0});
		particle_states.push_back({rgn_idx, {}, true, particle_states.back().interval_max,
												 particle_states.back().interval_max +S*sigma2_T, 0});
	}

	float interval_MAX = particle_states.back().interval_max;
	int N_particles = m_particleSystem_params.N_particles;
	for(int part_idx = 0; part_idx < N_particles; part_idx++)
	{
		float value = uniformDistributiion(0.0f, interval_MAX);
        for(particleState& part_state : particle_states)
		{
			if(value >= part_state.interval_min &&
			   value < part_state.interval_max)
			{
				part_state.nb_particles++;
			}
		}
	}

    for(particleState& part_state : particle_states)
	{
		m_bioWorld->addParticles(part_state.nb_particles,
								 0,
								 part_state.creation_rgn_idx,
								 part_state.forbidden_rgns_v,
								 0,0,
								 part_state.is_trapped);
	}
}


void FluoSimModel::imposeSteadyState()
{
	int N_rgn = m_bioWorld->getNbRegions();
	if(m_bioWorld == 0 || N_rgn <= 0) return; //->

	m_bioWorld->deleteAllParticles();

	float k_on = m_dynamic_params.k_on;
	float k_off = m_dynamic_params.k_off;
	float initial_enrichment = m_experiment_params.initial_enrichment;

	if(m_dynamic_params.k_on == 0) k_on = std::pow(2,-126);
	if(m_dynamic_params.k_off == 0) k_off = std::pow(2,-126);

	vector<int> forbidden_rgns_v;
	float S2 = 0;
	for(int rgn_idx = 1; rgn_idx < N_rgn; rgn_idx++)
	{
		vector<vec2> r_v;
		Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
		if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == false) continue; //<
		getIntersectionRegion(m_bioWorld->getRegionRef(0), rgn, r_v);
		S2 += computeSurface(r_v);
		forbidden_rgns_v.push_back(rgn_idx);
	}
	float S1 = m_bioWorld->getRegionRef(0).getSurface()-S2;

	float eps_T = 1 + k_on / k_off;
	float sigma1 = 1.0f/(S1+S2*initial_enrichment);
	float sigma2_D = initial_enrichment/(eps_T*(S1+S2*initial_enrichment));
	float sigma2_T = initial_enrichment*(eps_T-1)/(eps_T*(S1+S2*initial_enrichment));

    struct particleState
	{
		int creation_rgn_idx;
		vector<int> forbidden_rgns_v;
		bool is_trapped;
		float interval_min, interval_max; //if rand in [0; INTERVAL_MAX[ is in [interval_min; interval_max] then  a particle is created in the associated state
		int nb_particles;
	};

    vector<particleState> particle_states;
	particle_states.push_back({0, forbidden_rgns_v, false, 0.0f, S1*sigma1, 0});
	for(int rgn_idx = 1; rgn_idx < N_rgn; rgn_idx++)
	{
        Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
        if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == false) continue; //<

        vector<vec2> r_v;
        getIntersectionRegion(m_bioWorld->getRegionRef(0), rgn, r_v);
		float S = computeSurface(r_v);

		particle_states.push_back({rgn_idx, {}, false, particle_states.back().interval_max,
												 particle_states.back().interval_max +S*sigma2_D, 0});
		particle_states.push_back({rgn_idx, {}, true, particle_states.back().interval_max,
												 particle_states.back().interval_max +S*sigma2_T, 0});
	}

	float interval_MAX = particle_states.back().interval_max;
	int N_particles = m_particleSystem_params.N_particles;
	for(int part_idx = 0; part_idx < N_particles; part_idx++)
	{
		float value = uniformDistributiion(0.0f, interval_MAX);
        for(particleState& part_state : particle_states)
		{
			if(value >= part_state.interval_min &&
			   value < part_state.interval_max)
			{
				part_state.nb_particles++;
			}
		}
	}

    for(particleState& part_state : particle_states)
	{
		m_bioWorld->addParticles(part_state.nb_particles,
								 0,
								 part_state.creation_rgn_idx,
								 part_state.forbidden_rgns_v,
								 0,0,
								 part_state.is_trapped);
	}
}


void FluoSimModel::evaluateAutoscaleFactor()
{
    if(m_bioWorld == 0 || m_bioWorld->getNbRegions() == 0) return; //->

    //maximum density evaluation
    float density_max = 0.0f;
    for(int rgn_idx=0; rgn_idx < m_bioWorld->getNbRegions(); rgn_idx++)
    {
        float density = m_bioWorld->getParticleDensity(rgn_idx, 0, true);
        if(m_bioWorld->getRegionRef(rgn_idx).isACompartment(m_bioWorld->getSpecieAdr(0)) == true &&
           density >= density_max) density_max = density;
    }

    //overlapping factor ~ mean intensity of cutoff(ed) overlapping gaussians
    float overlapping_factor = 6*3.14*(density_max*std::pow(m_rendering_params.spot_size/m_simulation_params.pixel_size,2));

    //(peak) intensity of a gaussian, depends on its unit
    float spot_intensity = (m_rendering_params.isSpotIntensity_inPhotonsPerSec == false) ? m_rendering_params.spot_intensity/65535 : \
                                             m_rendering_params.spot_intensity*m_simulation_params.dt_sim/65535;
    float autoscale_factor;
    if(overlapping_factor >= 1)
    {
        float delta = 0.5/255.0f; //do deal with the cutoff while writting to the colorbuffer which is 8bit depth
        autoscale_factor = 1.1f*(1/overlapping_factor + delta)/spot_intensity; //the second term is obtained by considering the mean intenity of overlapping gaussians with a cutoff (gaussian intensities lower than delta are discarded(erf(g-1(delta))
    }
    else autoscale_factor = 1.0f/spot_intensity;

    m_rendering_params.autoscale_factor = autoscale_factor;
    m_scrn.setAutoscaleFactor(autoscale_factor);


}

void FluoSimModel::resetGeometry()
{
	list<Region*> rgn_adr_v;
	for(int rgn_idx = 0; rgn_idx <= m_bioWorld->getNbRegions()-1; rgn_idx++)
	{
		rgn_adr_v.push_back(&(m_bioWorld->getRegionRef(rgn_idx)));
	}

	for(auto rgn_adr : rgn_adr_v)
	{
		m_bioWorld->deleteRegion(rgn_adr);
	}

	m_experiment_params.bleachedRegionNames_v.clear();
	m_experiment_params.measuredRegionNames_v.clear();

	updateParticleSystemParams();
	updateDynamicParams();
	updatePhotoPhysicsParams();
	updateExperimentParams();
}

void FluoSimModel::deleteGeometry(string rgn_name)
{
	int rgn_idx = -1;
	rgn_idx = m_bioWorld->getRegionIdx(rgn_name);
	if(rgn_idx == -1) return; //->

	Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
	m_bioWorld->deleteRegion(rgn);

	vector<string>::iterator toBeErased_name = std::find(m_experiment_params.measuredRegionNames_v.begin(),
													 m_experiment_params.measuredRegionNames_v.end(),
													 rgn_name);

	if(toBeErased_name != m_experiment_params.measuredRegionNames_v.end())
	{
		m_experiment_params.measuredRegionNames_v.erase(toBeErased_name);
	}

	toBeErased_name = std::find(m_experiment_params.bleachedRegionNames_v.begin(),
							   m_experiment_params.bleachedRegionNames_v.end(),
							   rgn_name);
	if(toBeErased_name != m_experiment_params.bleachedRegionNames_v.end())
	{
		m_experiment_params.bleachedRegionNames_v.erase(toBeErased_name);
	}

	updateParticleSystemParams();
	updateDynamicParams();
	updatePhotoPhysicsParams();
	updateExperimentParams();
}

void FluoSimModel::deleteGeometries(vector<string>& rgn_names_v)
{
	for(string& rgn_name : rgn_names_v)
	{
		int rgn_idx = -1;
		rgn_idx = m_bioWorld->getRegionIdx(rgn_name);
		if(rgn_idx == -1) return; //->

		Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
		m_bioWorld->deleteRegion(rgn);

		vector<string>::iterator toBeErased_name = std::find(m_experiment_params.measuredRegionNames_v.begin(),
														 m_experiment_params.measuredRegionNames_v.end(),
														 rgn_name);

		if(toBeErased_name != m_experiment_params.measuredRegionNames_v.end())
		{
			m_experiment_params.measuredRegionNames_v.erase(toBeErased_name);
		}

		toBeErased_name = std::find(m_experiment_params.bleachedRegionNames_v.begin(),
								   m_experiment_params.bleachedRegionNames_v.end(),
								   rgn_name);
		if(toBeErased_name != m_experiment_params.bleachedRegionNames_v.end())
		{
			m_experiment_params.bleachedRegionNames_v.erase(toBeErased_name);
		}

        if(std::find(rgn_names_v.begin(), rgn_names_v.end(), m_liveExperiment_params.measured_RgnName) != rgn_names_v.end()) {
            if(m_bioWorld->getNbRegions() >= 1) m_liveExperiment_params.measured_RgnName = m_bioWorld->getRegionRef(0).getName();
            else m_liveExperiment_params.measured_RgnName = "";
        }
        if(std::find(rgn_names_v.begin(), rgn_names_v.end(), m_liveExperiment_params.bleached_RgnName) != rgn_names_v.end()) {
            if(m_bioWorld->getNbRegions() >= 1) m_liveExperiment_params.bleached_RgnName = m_bioWorld->getRegionRef(0).getName();
            else m_liveExperiment_params.bleached_RgnName = "";
        }
        if(std::find(rgn_names_v.begin(), rgn_names_v.end(), m_liveExperiment_params.photoActivated_RgnName) != rgn_names_v.end()) {
            if(m_bioWorld->getNbRegions() >= 1) m_liveExperiment_params.photoActivated_RgnName = m_bioWorld->getRegionRef(0).getName();
            else m_liveExperiment_params.photoActivated_RgnName = "";
        }
	}


	updateParticleSystemParams();
	updateDynamicParams();
	updatePhotoPhysicsParams();
	updateExperimentParams();
    updateLiveExperimentParams();

}

void FluoSimModel::resetProject()
{
    //reset bioWorld
    if(m_bioWorld != 0)
    {
        delete m_bioWorld;
        m_bioWorld = new BiologicalWorld();
    }
    if(m_engine != 0)
    {
        delete m_engine;
        m_engine = new DiffusionSubEngine(m_bioWorld);
    }
        m_engine->setEngineMode(DiffusionSubEngine::AUTOMATIC_SELECTION_MODE);

    ChemicalSpecies spc1("spc1", vec4(0.2,1,0.2,1));
        m_bioWorld->addChemicalSpecie(spc1);
        m_bioWorld->addFluorophoreSpecie(0,0); //will be set later on...

    //reset measurements
    m_probe.resetProbe();
    for(Probe* &probe : m_experimental_probes_v)
    {
        if(probe != 0) delete probe;
        probe = 0;
    }
    m_experimental_probes_v.clear();

    if(m_frapHead != 0)
    {
        delete m_frapHead;
        m_frapHead = 0;
    }
    m_experimental_frapHead_v.clear();
    m_recordedSignalValues_perRep_perProbe.clear();

    //reset images
    if(m_backgroundImage != 0)
    {
        delete m_backgroundImage;
        m_backgroundImage = 0;
    }
    if(m_cameraImage != 0)
    {
        delete m_cameraImage;
        m_cameraImage = 0;
    }

    //reset model parameters
    initializeModelParameters();

    //apply changes to the cell engine
    updateParticleSystemParams();
    updateLiveExperimentParams();
    updateDynamicParams();
    updatePhotoPhysicsParams();
    updateExperimentParams();

}

void FluoSimModel::loadProject(string project_path)
{
    stopSimulation();

	enum projectProperties
	{
		rgn_Number,
		rgn_Geometries,
		rgn_areCompartments,
		particleSystemParams_N_particles,
		particleSystemParams_particle_color,
		dynamicParams_D_outsideContact,
		dynamicParams_D_insideContact_ratio,
        dynamicParams_D_insideContact,
        dynamicParams_k_on,
		dynamicParams_k_off,
		dynamicParams_D_trapped,
		dynamicParams_crossingProbability_outIn,
		dynamicParams_immobileFraction,
		dynamicParams_isFixed,
		photoPhysicsParams_k_on_fluo,
		photoPhysicsParams_k_off_fluo,
		renderingParams_image_intensity,
		renderingParams_areRegionsVisible,
		renderingParams_isBackgroundImageVisible_inGeom,
		renderingParams_isBackgroundImageVisible_inLiveAndExp,
		renderingParams_spot_size,
		renderingParams_spot_intensity,
        renderingParams_isSpotIntensity_inPhotonsPerSec,
        renderingParams_ADCounts_perPhoton,
		renderingParams_rendering_mode,
		renderingParams_particle_color,
		renderingParams_backgroundImage_path,
        renderingParams_stackIntensityOffset, //depraciated, replaced with renderingParams.photonBackground
        renderingParams_photonBackground,
		renderingParams_isStackWPoissonNoise,
        renderingParams_cameraOffset,
        renderingParams_readoutNoise_sigma,
        simulationParams_dt_sim,
		simulationParams_current_plane,
		simulationParams_current_time,
		simulationParams_pixel_size,
		liveExperimentParams_experimentType,
		liveExperimentParams_measured_RgnName,
		liveExperimentParams_bleached_RgnName,
		liveExperimentParams_photoActivated_RgnName,
		experimentParams_experimentType,
		experimentParams_acquisitionType,
		experimentParams_N_planes,
		experimentParams_N_frap,
		experimentParams_dN_frap,
		experimentParams_k_off_frap,
		experimentParams_N_photoActivation,
		experimentParams_dN_photoActivation,
		experimentParams_k_on_photoActivation,
		experimentParams_N_presequ,
		experimentParams_N_repetition,
		experimentParams_acquisitionPeriod,
		experimentParams_measuredRegionIdxs_v,
		experimentParams_bleachedRegionIdxs_v,
		experimentParams_file_destination,
		experimentParams_recording_mode,
		experimentParams_isExportingStack,
		experimentParams_is_fcsUsingGaussianBeam,
		experimentParams_fcs_beamSigma,
		experimentParams_fcs_beamKoff,
		experimentParams_fcs_maxIntensity,
		experimentParams_N_drug,
		experimentParams_drug_affectedParam,
		experimentParams_drug_newValue,

		experimentParams_SRI_pointingAccuracy,
		experimentParams_SRI_detectionIntensity,
		experimentParams_SRI_zoom,

		simulationState_simulator_mode,
		simulationState_simulationStarted,
		simulationState_simulationEnded,
		simulationState_simulationPaused,
		simulationState_isSingleShot,
		simulationState_isRendering,
		simulationState_isGraphicVisible,
		simulationState_isParameterWidgetVisible
	};

	vector<string> projectPropertieNames_v
	{
		"region.Number",
		"region.Geometries",
		"region.areCompartments",
		"particleSystemParams.N_particles",
		"particleSystemParams.particle_color",
		"dynamicParams.D_outsideContact",
		"dynamicParams.D_insideContact_ratio",
        "dynamicParams.D_insideContact",
        "dynamicParams.k_on",
		"dynamicParams.k_off",
		"dynamicParams.D_trapped",
		"dynamicParams.crossingProbability_outIn",
		"dynamicParams.immobileFraction",
		"dynamicParams.isFixed",
		"photoPhysicsParams.k_on_fluo",
		"photoPhysicsParams.k_off_fluo",
		"renderingParams.image_intensity",
		"renderingParams.areRegionsVisible",
		"renderingParams.isBackgroundImageVisible_inGeom",
		"renderingParams.isBackgroundImageVisible_inLiveAndExp",
		"renderingParams.spot_size",
		"renderingParams.spot_intensity",
        "renderingParams.isSpotIntensity_inPhotonsPerSec",
        "renderingParams.ADCounts_perPhoton",
		"renderingParams.rendering_mode",
		"renderingParams.particle_color",
		"renderingParams.backgroundImage_path",
		"renderingParams.stackIntensityOffset",
        "renderingParams.photonBackground",
		"renderingParams.isStackWPoissonNoise",
        "renderingParams.cameraOffset",
        "renderingParams.readoutNoise_sigma",
		"simulationParams.dt_sim",
		"simulationParams.current_plane",
		"simulationParams.current_time",
		"simulationParams.pixel_size",
		"liveExperimentParams.experimentType",
		"liveExperimentParams.measured_RgnName",
		"liveExperimentParams.bleached_RgnName",
		"liveExperimentParams.photoActivated_RgnName",
		"experimentParams.experimentType",
		"experimentParams.acquisitionType",
		"experimentParams.N_planes",
		"experimentParams.N_frap",
		"experimentParams.dN_frap",
		"experimentParams.k_off_frap",
		"experimentParams.N_photoActivation",
		"experimentParams.dN_photoActivation",
		"experimentParams.k_on_photoActivation",
		"experimentParams.N_presequ",
		"experimentParams.N_repetition",
		"experimentParams.acquisitionPeriod",
		"experimentParams.measuredRegionIdxs_v",
		"experimentParams.bleachedRegionIdxs_v",
		"experimentParams.file_destination",
		"experimentParams.recording_mode",
		"experimentParams.isExportingStack",
		"experimentParams.is_fcsUsingGaussianBeam",
		"experimentParams.fcs_beamSigma",
		"experimentParams.fcs_maxIntensity",
		"experimentParams.fcs_beamKoff",
		"experimentParams.N_drug",
		"experimentParams.drug_affectedParam",
		"experimentParams.drug_newValue",

		"experimentParams.SRI_pointingAccuracy",
		"experimentParams.SRI_detectionIntensity",
		"experimentParams.SRI_zoom",

		"simulationState.simulator_mode",
		"simulationState.simulationStarted",
		"simulationState.simulationEnded",
		"simulationState.simulationPaused",
		"simulationState.isSingleShot",
		"simulationState.isRendering",
		"simulationState.isGraphicVisible",
		"simulationState.isParameterWidgetVisible"
	};


	map<string, projectProperties> projectProperties_map
	{
		{"region.Number", rgn_Number},
		{"region.Geometries", rgn_Geometries},
		{"region.areCompartments", rgn_areCompartments},
		{"particleSystemParams.N_particles", particleSystemParams_N_particles},
		{"particleSystemParams.particle_color", particleSystemParams_particle_color},
		{"dynamicParams.D_outsideContact", dynamicParams_D_outsideContact},
        {"dynamicParams.D_insideContact_ratio", dynamicParams_D_insideContact_ratio},
        {"dynamicParams.D_insideContact", dynamicParams_D_insideContact},
        {"dynamicParams.k_on", dynamicParams_k_on},
		{"dynamicParams.k_off", dynamicParams_k_off},
		{"dynamicParams.D_trapped", dynamicParams_D_trapped},
		{"dynamicParams.crossingProbability_outIn", dynamicParams_crossingProbability_outIn},
		{"dynamicParams.immobileFraction", dynamicParams_immobileFraction},
		{"dynamicParams.isFixed", dynamicParams_isFixed},
		{"photoPhysicsParams.k_on_fluo", photoPhysicsParams_k_on_fluo},
		{"photoPhysicsParams.k_off_fluo", photoPhysicsParams_k_off_fluo},
		{"renderingParams.image_intensity", renderingParams_image_intensity},
		{"renderingParams.areRegionsVisible", renderingParams_areRegionsVisible},
		{"renderingParams.isBackgroundImageVisible_inGeom", renderingParams_isBackgroundImageVisible_inGeom},
		{"renderingParams.isBackgroundImageVisible_inLiveAndExp", renderingParams_isBackgroundImageVisible_inLiveAndExp},
		{"renderingParams.spot_size", renderingParams_spot_size},
		{"renderingParams.spot_intensity",renderingParams_spot_intensity},
        {"renderingParams.isSpotIntensity_inPhotonsPerSec", renderingParams_isSpotIntensity_inPhotonsPerSec},
        {"renderingParams.ADCounts_perPhoton", renderingParams_ADCounts_perPhoton},
		{"renderingParams.rendering_mode",renderingParams_rendering_mode},
		{"renderingParams.particle_color",renderingParams_particle_color},
		{"renderingParams.backgroundImage_path",renderingParams_backgroundImage_path},

		{"renderingParams.stackIntensityOffset",renderingParams_stackIntensityOffset},
        {"renderingParams.photonBackground", renderingParams_photonBackground},
		{"renderingParams.isStackWPoissonNoise",renderingParams_isStackWPoissonNoise},
        {"renderingParams.cameraOffset", renderingParams_cameraOffset},
        {"renderingParams.readoutNoise_sigma", renderingParams_readoutNoise_sigma},

		{"simulationParams.dt_sim",simulationParams_dt_sim},
		{"simulationParams.current_plane",simulationParams_current_plane},
		{"simulationParams.current_time",simulationParams_current_time},
		{"simulationParams.pixel_size",simulationParams_pixel_size},
		{"liveExperimentParams.experimentType",liveExperimentParams_experimentType},
		{"liveExperimentParams.measured_RgnName",liveExperimentParams_measured_RgnName},
		{"liveExperimentParams.bleached_RgnName",liveExperimentParams_bleached_RgnName},
		{"liveExperimentParams.photoActivated_RgnName",liveExperimentParams_photoActivated_RgnName},
		{"experimentParams.experimentType",experimentParams_experimentType},
		{"experimentParams.acquisitionType",experimentParams_acquisitionType},
		{"experimentParams.N_planes",experimentParams_N_planes},
		{"experimentParams.N_frap",experimentParams_N_frap},
		{"experimentParams.dN_frap",experimentParams_dN_frap},
		{"experimentParams.k_off_frap",experimentParams_k_off_frap},
		{"experimentParams.N_photoActivation",experimentParams_N_photoActivation},
		{"experimentParams.dN_photoActivation",experimentParams_dN_photoActivation},
		{"experimentParams.k_on_photoActivation",experimentParams_k_on_photoActivation},
		{"experimentParams.N_presequ",experimentParams_N_presequ},
		{"experimentParams.N_repetition",experimentParams_N_repetition},
		{"experimentParams.acquisitionPeriod",experimentParams_acquisitionPeriod},
		{"experimentParams.measuredRegionIdxs_v",experimentParams_measuredRegionIdxs_v},
		{"experimentParams.bleachedRegionIdxs_v",experimentParams_bleachedRegionIdxs_v},
		{"experimentParams.file_destination",experimentParams_file_destination},
		{"experimentParams.recording_mode",experimentParams_recording_mode},
		{"experimentParams.isExportingStack",experimentParams_isExportingStack},
		{"experimentParams.is_fcsUsingGaussianBeam",experimentParams_is_fcsUsingGaussianBeam},
		{"experimentParams.fcs_beamSigma",experimentParams_fcs_beamSigma},
		{"experimentParams.fcs_maxIntensity",experimentParams_fcs_maxIntensity},
		{"experimentParams.fcs_beamKoff",experimentParams_fcs_beamKoff},
		{"experimentParams.N_drug",experimentParams_N_drug},
		{"experimentParams.drug_affectedParam",experimentParams_drug_affectedParam},
		{"experimentParams.drug_newValue",experimentParams_drug_newValue},

		{"experimentParams.SRI_pointingAccuracy", experimentParams_SRI_pointingAccuracy},
		{"experimentParams.SRI_detectionIntensity", experimentParams_SRI_detectionIntensity},
		{"experimentParams.SRI_zoom", experimentParams_SRI_zoom},

		{"simulationState.simulator_mode",simulationState_simulator_mode},
		{"simulationState.simulationStarted",simulationState_simulationStarted},
		{"simulationState.simulationEnded",simulationState_simulationEnded},
		{"simulationState.simulationPaused",simulationState_simulationPaused},
		{"simulationState.isSingleShot",simulationState_isSingleShot},
		{"simulationState.isRendering",simulationState_isRendering},

		{"simulationState.isGraphicVisible", simulationState_isGraphicVisible},
		{"simulationState.isParameterWidgetVisible", simulationState_isParameterWidgetVisible}

	};

	string project_str;
	if(getFileContent(project_path, project_str) == false) return; //->

    //we get the different field names
    vector<projectProperties> projectProperties_v;
    int line_idx = 0;
    int line_pos = 0;
    string line;
    while(getLine(project_str, line, line_pos, line_pos))
    {
        int bra_pos = line.find('[');
        int ket_pos = line.find(']');
        if(bra_pos >= ket_pos) continue; //<

        int word_pos = 0;
        string subLine = line.substr(bra_pos+1, ket_pos-(bra_pos+1));
        string word;
        getWord(subLine, word, 0, word_pos);

        auto propertieName_it = std::find(projectPropertieNames_v.begin(), projectPropertieNames_v.end(), word);
        if(propertieName_it != projectPropertieNames_v.end())
        {
            projectProperties_v.push_back(projectProperties_map[*propertieName_it]);
        }
    }


    line_idx = 0;
    line_pos = 0;
    line = "";
	while(getLine(project_str, line, line_pos, line_pos))
	{
		int bra_pos = line.find('[');
		int ket_pos = line.find(']');
		if(bra_pos >= ket_pos) continue; //<

		int word_pos = 0;
		string subLine = line.substr(bra_pos+1, ket_pos-(bra_pos+1));
		string word;
		getWord(subLine, word, 0, word_pos);

		int nb_rgn = -1;

		auto propertieName_it = std::find(projectPropertieNames_v.begin(), projectPropertieNames_v.end(), word);
		if(propertieName_it != projectPropertieNames_v.end())
		{
			switch(projectProperties_map[*propertieName_it])
			{
				case rgn_Number :
				{
					getWord(line, word, ket_pos+1, word_pos);
					nb_rgn = stoi(word);
				}
				break;

				case rgn_Geometries :
				{
					vector<Region*> rgns_v;
					for(int rgn_idx = 0; rgn_idx<= m_bioWorld->getNbRegions()-1; rgn_idx++)
					{
						rgns_v.push_back(&(m_bioWorld->getRegionRef(rgn_idx)));
					}

					for(Region* rgn : rgns_v)
					{
						m_bioWorld->deleteRegion(rgn);
					}

					vector<vector<vec2> > m_geometries_v;
					vector<vec2> geometry;

					word_pos = ket_pos+1;
					while(getWord(line, word, word_pos, word_pos))
					{
						if(word.size() == 0) continue;//<
						geometry.clear();
						int vert_nb = stoi(word);
						int vert_idx = 0;
						while(vert_idx != vert_nb)
						{
							vec2 r;
							getWord(line, word, word_pos, word_pos);
							r.x = stof(word);
							getWord(line, word, word_pos, word_pos);
							r.y = stof(word);

							geometry.push_back(r);
							vert_idx++;
						}
						m_geometries_v.push_back(geometry);
					}

					for(auto geom : m_geometries_v)
					{
						addGeometry(geom);
					}

				}
				break;

				case rgn_areCompartments :
				{
					vector<bool> areCompartments;

					word_pos = ket_pos+1;
					while(getWord(line, word, word_pos, word_pos))
					{
						if(word == "TRUE") areCompartments.push_back(true);
						else if(word == "FALSE") areCompartments.push_back(false);

						if(m_bioWorld->getNbRegions() == areCompartments.size())
						{
							for(uint rgn_idx = 0; rgn_idx < m_bioWorld->getNbRegions(); rgn_idx++)
							{
								m_bioWorld->getRegionRef(rgn_idx).setIsACompartment(m_bioWorld->getSpecieAdr(0),
																					areCompartments[rgn_idx]);
							}
						}
					}
				}
				break;

				case particleSystemParams_N_particles :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_particleSystem_params.N_particles = stof(word);

				}
				break;

//				case particleSystemParams_particle_color :
//				{

//				}
//				break;

				case dynamicParams_D_outsideContact :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.D_outsideContact = stof(word);
                    cout<<"Load Project : Doutside_contact : \n";
				}
				break;

				case dynamicParams_D_insideContact_ratio :
				{
					getWord(line, word, ket_pos+1, word_pos);
                    m_dynamic_params.D_insideContact = stof(word)*m_dynamic_params.D_outsideContact;
                    cout<<"Load Project : Dinside_contact : "<<m_dynamic_params.D_insideContact<<"\n";
//					m_dynamic_params.D_insideContact_ratio = stof(word);

				}
                break;

                case dynamicParams_D_insideContact :
                {
                    getWord(line, word, ket_pos+1, word_pos);
                    m_dynamic_params.D_insideContact = stof(word);
//					m_dynamic_params.D_insideContact_ratio = stof(word);

                }
                break;

				case dynamicParams_k_on :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.k_on = stof(word);
				}
				break;

				case dynamicParams_k_off :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.k_off = stof(word);
				}
				break;

				case dynamicParams_D_trapped :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.D_trapped = stof(word);
				}
				break;

				case dynamicParams_crossingProbability_outIn :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.crossingProbability_outIn = stof(word);
				}
				break;

				case dynamicParams_immobileFraction :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_dynamic_params.immobileFraction = stof(word);
				}
				break;

				case dynamicParams_isFixed :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_dynamic_params.isFixed = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_dynamic_params.isFixed = false;
					}
				}
				break;

				case photoPhysicsParams_k_on_fluo :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_photoPhysics_params.k_on_fluo = stof(word);
				}
				break;

				case photoPhysicsParams_k_off_fluo :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_photoPhysics_params.k_off_fluo = stof(word);
				}
				break;

				case renderingParams_image_intensity :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_rendering_params.image_intensity = stof(word);
				}
				break;

				case renderingParams_areRegionsVisible :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_rendering_params.areRegionsVisible = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_rendering_params.areRegionsVisible = false;
					}
				}
				break;

				case renderingParams_isBackgroundImageVisible_inGeom :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_rendering_params.isBackgroundImageVisible_inGeom = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_rendering_params.isBackgroundImageVisible_inGeom = false;
					}
				}
				break;

				case renderingParams_isBackgroundImageVisible_inLiveAndExp :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_rendering_params.isBackgroundImageVisible_inLiveAndExp = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_rendering_params.isBackgroundImageVisible_inLiveAndExp = false;
					}
				}
				break;

				case renderingParams_spot_size :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_rendering_params.spot_size = stof(word);
					m_cameraRendering_params.spot_size = stof(word);
				}
				break;

                case renderingParams_isSpotIntensity_inPhotonsPerSec :
                {
                    cout<<"inside isSpotIntensity_inPhotonsPerSec...";
                    getWord(line, word, ket_pos+1, word_pos);
                    if(word == "true" || word == "TRUE")
                    {
                        m_rendering_params.isSpotIntensity_inPhotonsPerSec = true;
                        m_cameraRendering_params.isSpotIntensity_inPhotonsPerSec = true;
                    }
                    if(word == "false" || word == "FALSE")
                    {
                        m_rendering_params.isSpotIntensity_inPhotonsPerSec = false;
                        m_cameraRendering_params.isSpotIntensity_inPhotonsPerSec = false;
                    }
                }
                break;

				case renderingParams_spot_intensity :
				{
                    getWord(line, word, ket_pos+1, word_pos);

                    bool hasPhotonsPerSecOption = \
                            std::find(projectProperties_v.begin(), projectProperties_v.end(), renderingParams_isSpotIntensity_inPhotonsPerSec) != projectProperties_v.end();
                    float spot_intensity = stof(word);
                    if(hasPhotonsPerSecOption == false) \
                        spot_intensity = 65535*spot_intensity; //since in this old version the spotIntensity was stored normalized between 0 and 1
                    m_rendering_params.spot_intensity = spot_intensity;
                    m_cameraRendering_params.spot_intensity = spot_intensity;
				}
				break;

                case renderingParams_ADCounts_perPhoton :
                {
                    getWord(line, word, ket_pos+1, word_pos);
                    m_rendering_params.ADCounts_perPhoton = stof(word);
                    m_cameraRendering_params.ADCounts_perPhoton = stof(word);
                }
                break;

				case renderingParams_rendering_mode :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "RENDERING_POINTS")
					{
					   m_rendering_params.rendering_mode = RENDERING_POINTS;
					}
					if(word == "RENDERING_GAUSSIANS")
					{
					   m_rendering_params.rendering_mode = RENDERING_GAUSSIANS;
					}
				}
				break;

				case renderingParams_particle_color :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "PARTICLE_RED") m_rendering_params.particle_color = PARTICLE_RED;
					if(word == "PARTICLE_GREEN") m_rendering_params.particle_color = PARTICLE_GREEN;
					if(word == "PARTICLE_BLUE") m_rendering_params.particle_color = PARTICLE_BLUE;
					if(word == "PARTICLE_WHITE") m_rendering_params.particle_color = PARTICLE_WHITE;
					if(word == "PARTICLE_RANDOMCOLOR") m_rendering_params.particle_color = PARTICLE_RANDOMCOLOR;
					if(word == "PARTICLE_TRAPPING_STATE_COLOR") m_rendering_params.particle_color = PARTICLE_TRAPPING_STATE_COLOR;
					if(word == "PARTICLE_PSEUDOCOLOR") m_rendering_params.particle_color = PARTICLE_LOOKUP_TABLE;
				}
				break;

				case renderingParams_backgroundImage_path :
				{
					getLine(line, word, ket_pos+1, word_pos);
					std::size_t pos = word.find_first_not_of(' ');
					string sub_line;
					if(pos != word.npos) sub_line = word.substr(pos);
					else sub_line = word;

					loadBackgroundImage(sub_line);
				}
				break;

				case renderingParams_stackIntensityOffset :
                case renderingParams_photonBackground :
                {
					getWord(line, word, ket_pos+1, word_pos);
					m_cameraRendering_params.photonBackground = stof(word);
				}
				break;


				case renderingParams_isStackWPoissonNoise :
				{
					getWord(line, word, ket_pos+1, word_pos);
                    if(word == "true" || word == "TRUE") m_cameraRendering_params.isUsingPoissonNoise = true;
                    if(word == "false" || word == "FALSE") m_cameraRendering_params.isUsingPoissonNoise = false;
				}
				break;

                case renderingParams_cameraOffset :
                {
                    getWord(line, word, ket_pos+1, word_pos);
                    m_cameraRendering_params.cameraOffset = stof(word);
                }

                case renderingParams_readoutNoise_sigma :
                {
                    getWord(line, word, ket_pos+1, word_pos);
                    m_cameraRendering_params.readoutNoise_sigma = stof(word);
                }

				case simulationParams_dt_sim :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_params.dt_sim = stof(word);
				}
				break;

				case simulationParams_current_plane :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_params.current_plane = stof(word);
				}
				break;

				case simulationParams_current_time :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_params.current_time = stof(word);
				}
				break;

				case simulationParams_pixel_size :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_params.pixel_size = stof(word);
				}
				break;

				case liveExperimentParams_experimentType :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "PHOTOBLEACHING_LIVE_EXPERIMENT")
					{
						m_liveExperiment_params.experimentType = PHOTOBLEACHING_LIVE_EXPERIMENT;
					}
					if(word == "PHOTOACTIVATION_LIVE_EXPERIMENT")
					{
						m_liveExperiment_params.experimentType = PHOTOACTIVATION_LIVE_EXPERIMENT;
					}
					if(word == "FCS_LIVE_EXPERIMENT")
					{
						m_liveExperiment_params.experimentType = FCS_LIVE_EXPERIMENT;
					}
				}
				break;

				case liveExperimentParams_measured_RgnName :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_liveExperiment_params.measured_RgnName = word;
				}
				break;


				case liveExperimentParams_bleached_RgnName :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_liveExperiment_params.bleached_RgnName = word;
				}
				break;


				case liveExperimentParams_photoActivated_RgnName :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_liveExperiment_params.photoActivated_RgnName = word;
				}
				break;


				case experimentParams_experimentType :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "SPT_EXPERIMENT")
					{
						m_experiment_params.experimentType = SPT_EXPERIMENT;
					}
					if(word == "FRAP_EXPERIMENT")
					{
						m_experiment_params.experimentType = FRAP_EXPERIMENT;
					}
					if(word == "PAF_EXPERIMENT")
					{
						m_experiment_params.experimentType = PAF_EXPERIMENT;
					}
					if(word == "FCS_EXPERIMENT")
					{
						m_experiment_params.experimentType = FCS_EXPERIMENT;
					}
					if(word == "SRI_EXPERIMENT")
					{
						m_experiment_params.experimentType = SRI_EXPERIMENT;
					}

					if(word == "DRUG_EXPERIMENT")
					{
						m_experiment_params.experimentType = DRUG_EXPERIMENT;
					}

				}
				break;

				case experimentParams_acquisitionType :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "STREAM_ACQUISITION")
					{
						m_experiment_params.acquisitionType = STREAM_ACQUISITION;
					}
					if(word == "TIMELAPSE_ACQUISITION")
					{
						m_experiment_params.acquisitionType = TIMELAPSE_ACQUISITION;
					}
				}
				break;

				case experimentParams_N_planes :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_planes = stof(word);
				}
				break;

				case experimentParams_N_frap :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_frap = stof(word);

				}
				break;

				case experimentParams_dN_frap :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.dN_frap = stof(word);

				}
				break;

				case experimentParams_k_off_frap :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.k_off_frap = stof(word);

				}
				break;

				case experimentParams_N_photoActivation :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_photoActivation = stof(word);

				}
				break;

				case experimentParams_dN_photoActivation :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.dN_photoActivation = stof(word);

				}
				break;

				case experimentParams_k_on_photoActivation :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.k_on_photoActivation = stof(word);
				}
				break;

				case experimentParams_N_presequ :
				{
                    getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_presequ = stof(word);
				}
				break;

				case experimentParams_N_repetition :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_repetition = stof(word);
				}
				break;

				case experimentParams_acquisitionPeriod :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.acquisitionPeriod = stof(word);
				}
				break;

				case experimentParams_measuredRegionIdxs_v :	
				{
					m_experiment_params.measuredRegionNames_v.clear();
					word_pos = ket_pos+1;
					while(getWord(line, word, word_pos, word_pos))
					{
						if(m_bioWorld->getRegionIdx(word) == -1) continue;//<
						m_experiment_params.measuredRegionNames_v.push_back(word);
					}
				}
				break;

				case experimentParams_bleachedRegionIdxs_v :
				{
					m_experiment_params.bleachedRegionNames_v.clear();
					word_pos = ket_pos+1;
					while(getWord(line, word, word_pos, word_pos))
					{
						if(m_bioWorld->getRegionIdx(word) == -1) continue;//<
						m_experiment_params.bleachedRegionNames_v.push_back(word);
					}
				}
				break;

				case experimentParams_file_destination :
				{
					int end_pos;
					string file_destination;
					getLine(line, file_destination, ket_pos+1+1,end_pos);
					setDestinationPath(file_destination);
				}
				break;

				case experimentParams_recording_mode :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "SEQUENTIAL_RECORDING")
					{
						m_experiment_params.recording_mode = SEQUENTIAL_RECORDING;
					}
					if(word == "ALL_IN_ONE_RECORDING")
					{
						m_experiment_params.recording_mode = ALL_IN_ONE_RECORDING;
					}
				}
				break;

				case experimentParams_isExportingStack :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "TRUE")
					{
						m_experiment_params.isExportingStack = true;
					}
					if(word == "FALSE")
					{
						m_experiment_params.isExportingStack = false;
					}
				}
				break;

				case experimentParams_is_fcsUsingGaussianBeam :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "TRUE")
					{
						m_experiment_params.is_fcsUsingGaussianBeam = true;
					}
					if(word == "FALSE")
					{
						m_experiment_params.is_fcsUsingGaussianBeam = false;
					}
				}
				break;

				case experimentParams_fcs_beamSigma :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.fcs_beamSigma = stof(word);
				}
				break;

				case experimentParams_fcs_beamKoff :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.fcs_beamKoff = stof(word);
				}
				break;

				case experimentParams_fcs_maxIntensity :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.fcs_maxIntensity = stof(word);
				}
				break;

				case experimentParams_N_drug :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.N_drug = stof(word);
				}
				break;

				case experimentParams_drug_affectedParam :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "DOUT_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = DOUT_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = DOUT_DRUGPARAM;
					}
					if(word == "DIN_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = DIN_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = DIN_DRUGPARAM;
					}
					if(word == "DTRAP_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = DTRAP_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = DTRAP_DRUGPARAM;
					}
					if(word == "KON_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = KON_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = KON_DRUGPARAM;
					}
					if(word == "KOFF_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = KOFF_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = KOFF_DRUGPARAM;
					}
					if(word == "POROS_DRUGPARAM")
					{
						m_experiment_params.drug_affectedParam = POROS_DRUGPARAM;
						m_dynamic_params.drug_affectedParam = POROS_DRUGPARAM;
					}
				}
				break;

				case experimentParams_drug_newValue :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.drug_newValue = stof(word);
					m_dynamic_params.drug_newValue = stof(word);
				}
				break;

				case experimentParams_SRI_pointingAccuracy :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.SRI_pointingAccuracy = stof(word);
				}
				break;

				case experimentParams_SRI_detectionIntensity :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_stormRendering_params.point_intensity = stof(word);
				}
				break;

				case experimentParams_SRI_zoom :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_experiment_params.SRI_zoom = std::stoi(word);
				}
				break;

				case simulationState_simulator_mode :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "GEOMETRY_MODE")
					{
						m_simulation_states.simulator_mode = GEOMETRY_MODE;
					}
					if(word == "LIVE_MODE")
					{
						m_simulation_states.simulator_mode = LIVE_MODE;
					}
					if(word == "EXPERIMENT_MODE")
					{
						m_simulation_states.simulator_mode = EXPERIMENT_MODE;
					}
					if(word == "ANALYSIS_MODE")
					{
						m_simulation_states.simulator_mode = ANALYSIS_MODE;
					}
				}
				break;

				case simulationState_simulationStarted :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_simulation_states.simulationStarted = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_simulation_states.simulationStarted = false;
					}

				}
				break;
				case simulationState_simulationEnded :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_simulation_states.simulationEnded = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_simulation_states.simulationEnded = false;
					}
				}
				break;

				case simulationState_simulationPaused :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_simulation_states.simulationPaused = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_simulation_states.simulationPaused = false;
					}
				}
				break;

				case simulationState_isSingleShot :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_simulation_states.isSingleShot = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_simulation_states.isSingleShot = false;
					}
				}
				break;

				case simulationState_isRendering :
				{
					getWord(line, word, ket_pos+1, word_pos);
					if(word == "true" || word == "TRUE")
					{
						m_simulation_states.isRenderingEnabled = true;
					}
					if(word == "false" || word == "FALSE")
					{
						m_simulation_states.isRenderingEnabled = false;
					}
				}
				break;

				case simulationState_isParameterWidgetVisible :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_states.isParameterWidgetVisible = \
								(word == "true" || word == "TRUE") ? true : false;
				}
				break;

				case simulationState_isGraphicVisible :
				{
					getWord(line, word, ket_pos+1, word_pos);
					m_simulation_states.isGraphicVisible = \
								(word == "true" || word == "TRUE") ? true : false;
				}
				break;
			}
		}
		else
		{
			cout<<word<<" : was not found...\n";
		}
		line_idx++;
	}

	updateExperimentParams();
    updateLiveExperimentParams();
}

void FluoSimModel::saveProject(string project_filePath)
{
	ofstream myfile;
	myfile.open(project_filePath);

	if(myfile.is_open() == false) return;//->

	myfile<<"[region.Number] "<<m_bioWorld->getNbRegions()<<"\n";

	myfile<<"[region.Geometries] ";
	for(int rgn_idx = 0; rgn_idx<=m_bioWorld->getNbRegions()-1; rgn_idx++)
	{
		Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
		int size = rgn.getSize();
		myfile<<size<<" ";
		for(int pt_idx = 0; pt_idx <= size-1; pt_idx++)
		{
			myfile<<rgn.getPoint(pt_idx).x<<" "<<rgn.getPoint(pt_idx).y<<" ";
		}
	}
	myfile<<"\n";

	myfile<<"[region.areCompartments] ";
	for(int rgn_idx = 0; rgn_idx<=m_bioWorld->getNbRegions()-1; rgn_idx++)
	{
		Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
		if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == true)
		{
			myfile<<"TRUE"<<" ";
		}
		if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == false)
		{
			myfile<<"FALSE"<<" ";
		}
	}
	myfile<<"\n";

	myfile<<"[particleSystemParams.N_particles] "<<m_particleSystem_params.N_particles<<"\n";

	myfile<<"[dynamicParams.D_outsideContact] "<<m_dynamic_params.D_outsideContact<<"\n";

//	myfile<<"[dynamicParams.D_insideContact_ratio] "<<m_dynamic_params.D_insideContact_ratio<<"\n";

    myfile<<"[dynamicParams.D_insideContact] "<<m_dynamic_params.D_insideContact<<"\n";

	myfile<<"[dynamicParams.k_on] "<<m_dynamic_params.k_on<<"\n";

	myfile<<"[dynamicParams.k_off] "<<m_dynamic_params.k_off<<"\n";

	myfile<<"[dynamicParams.D_trapped] "<<m_dynamic_params.D_trapped<<"\n";

	myfile<<"[dynamicParams.crossingProbability_outIn] "<<m_dynamic_params.crossingProbability_outIn<<"\n";

	myfile<<"[dynamicParams.immobileFraction] "<<m_dynamic_params.immobileFraction<<"\n";

	string isFixed_str = m_dynamic_params.isFixed ? "TRUE" : "FALSE";
	myfile<<"[dynamicParams.isFixed] "<<isFixed_str<<"\n";

	myfile<<"[photoPhysicsParams.k_on_fluo] "<<m_photoPhysics_params.k_on_fluo<<"\n";

	myfile<<"[photoPhysicsParams.k_off_fluo] "<<m_photoPhysics_params.k_off_fluo<<"\n";

	myfile<<"[renderingParams.image_intensity] "<<m_rendering_params.image_intensity<<"\n";

	string areRegion_visible_str;
	if(m_rendering_params.areRegionsVisible == true) areRegion_visible_str = "TRUE";
	if(m_rendering_params.areRegionsVisible == false) areRegion_visible_str = "FALSE";

	myfile<<"[renderingParams.areRegionsVisible] "<<areRegion_visible_str<<"\n";

	//to be completed!!!
    string isBackgroundVisible_inGeom_str;
    string isBackgroundVisible_inLiveAndExp_str;
    if(m_rendering_params.isBackgroundImageVisible_inGeom == true) isBackgroundVisible_inGeom_str = "TRUE";
    else isBackgroundVisible_inGeom_str = "FALSE";
    if(m_rendering_params.isBackgroundImageVisible_inLiveAndExp == true) isBackgroundVisible_inLiveAndExp_str = "TRUE";
    else isBackgroundVisible_inLiveAndExp_str = "FALSE";

    myfile<<"[renderingParams.isBackgroundImageInGeomVisible] "<<isBackgroundVisible_inGeom_str<<"\n";
    myfile<<"[renderingParams.isBackgroundImageInLiveAndExpVisible] "<<isBackgroundVisible_inLiveAndExp_str<<"\n";

	myfile<<"[renderingParams.spot_size] "<<m_rendering_params.spot_size<<"\n";

    string isSpotIntensity_inPhotonsPerSec = m_rendering_params.isSpotIntensity_inPhotonsPerSec ? "TRUE" : "FALSE";
    myfile<<"[renderingParams.isSpotIntensity_inPhotonsPerSec] "<<isSpotIntensity_inPhotonsPerSec<<"\n";
	myfile<<"[renderingParams.spot_intensity] "<<m_rendering_params.spot_intensity<<"\n";
    myfile<<"[renderingParams.ADCounts_perPhoton] "<<m_rendering_params.ADCounts_perPhoton<<"\n";

	string renderingMode_str;
	if(m_rendering_params.rendering_mode == RENDERING_POINTS) renderingMode_str = "RENDERING_POINTS";
	if(m_rendering_params.rendering_mode == RENDERING_GAUSSIANS) renderingMode_str = "RENDERING_GAUSSIANS";
	myfile<<"[renderingParams.rendering_mode] "<<renderingMode_str<<"\n";

	string particleColor_str;
	if(m_rendering_params.particle_color == PARTICLE_RED) particleColor_str = "PARTICLE_RED";
	if(m_rendering_params.particle_color == PARTICLE_GREEN) particleColor_str = "PARTICLE_GREEN";
	if(m_rendering_params.particle_color == PARTICLE_BLUE) particleColor_str = "PARTICLE_BLUE";
	if(m_rendering_params.particle_color == PARTICLE_WHITE) particleColor_str = "PARTICLE_WHITE";
	if(m_rendering_params.particle_color == PARTICLE_RANDOMCOLOR) particleColor_str = "PARTICLE_RANDOMCOLOR";
	if(m_rendering_params.particle_color == PARTICLE_TRAPPING_STATE_COLOR) particleColor_str = "PARTICLE_TRAPPING_STATE_COLOR";
	if(m_rendering_params.particle_color == PARTICLE_LOOKUP_TABLE) particleColor_str = "PARTICLE_PSEUDOCOLOR";
	myfile<<"[renderingParams.particle_color] "<<particleColor_str<<"\n";

	//backgroundImage_path
	string backgroundImage_path = m_rendering_params.backgroundImage_path;
	std::size_t pos = backgroundImage_path.find(m_main_app->applicationDirPath().toStdString());
	if(pos != backgroundImage_path.npos)
	{
		backgroundImage_path = backgroundImage_path.substr(pos + m_main_app->applicationDirPath().size());
		std::size_t pos = backgroundImage_path.find_first_not_of('/');
		if(pos == backgroundImage_path.npos) backgroundImage_path =  backgroundImage_path.substr(pos);
    }
	myfile<<"[renderingParams.backgroundImage_path] "<<backgroundImage_path<<"\n";
    myfile<<"[renderingParams.photonBackground] "<<m_cameraRendering_params.photonBackground<<"\n";
    myfile<<"[renderingParams.isStackWPoissonNoise] "<<(m_cameraRendering_params.isUsingPoissonNoise ? "TRUE\n" : "FALSE\n");
    myfile<<"[renderingParams.cameraOffset] "<<m_cameraRendering_params.cameraOffset<<"\n";
    myfile<<"[renderingParams.readoutNoise_sigma] "<<m_cameraRendering_params.readoutNoise_sigma<<"\n";

	myfile<<"[simulationParams.dt_sim] "<<m_simulation_params.dt_sim<<"\n";

	myfile<<"[simulationParams.current_plane] "<<m_simulation_params.current_plane<<"\n";

	myfile<<"[simulationParams.current_time] "<<m_simulation_params.current_time<<"\n";

	myfile<<"[simulationParams.pixel_size] "<<m_simulation_params.pixel_size<<"\n";

	string experimentType_str;
	if(m_liveExperiment_params.experimentType == PHOTOBLEACHING_LIVE_EXPERIMENT)
		experimentType_str = "PHOTOBLEACHING_LIVE_EXPERIMENT";
	if(m_liveExperiment_params.experimentType == PHOTOACTIVATION_LIVE_EXPERIMENT)
		experimentType_str = "PHOTOACTIVATION_LIVE_EXPERIMENT";
	if(m_liveExperiment_params.experimentType == FCS_LIVE_EXPERIMENT)
		experimentType_str = "FCS_LIVE_EXPERIMENT";
	myfile<<"[liveExperimentParams.experimentType] "<<experimentType_str<<"\n";

	QString saved_measured_RgnName;
	int measured_rgnIdx = m_bioWorld->getRegionIdx(m_liveExperiment_params.measured_RgnName);
	if(measured_rgnIdx != -1)
	{
		saved_measured_RgnName = "Region" + QString("test").setNum(measured_rgnIdx,10);
		myfile<<"[liveExperimentParams.measured_RgnName] "<<saved_measured_RgnName.toLocal8Bit().data()<<"\n";
	}

	QString saved_bleached_RgnName;
	int bleached_rgnIdx = m_bioWorld->getRegionIdx(m_liveExperiment_params.bleached_RgnName);
	if(bleached_rgnIdx != -1)
	{
		saved_bleached_RgnName = "Region" + QString("test").setNum(bleached_rgnIdx,10);
		myfile<<"[liveExperimentParams.bleached_RgnName] "<<saved_bleached_RgnName.toLocal8Bit().data()<<"\n";
	}

	QString saved_photoActivated_RgnName;
	int photoActivated_rgnIdx = m_bioWorld->getRegionIdx(m_liveExperiment_params.photoActivated_RgnName);
	if(photoActivated_rgnIdx != -1)
	{
		saved_photoActivated_RgnName = "Region" + QString("test").setNum(photoActivated_rgnIdx,10);
		myfile<<"[liveExperimentParams.photoActivated_RgnName] "<<saved_photoActivated_RgnName.toLocal8Bit().data()<<"\n";
	}

	experimentType_str = "";
	if(m_experiment_params.experimentType == SPT_EXPERIMENT)
		experimentType_str = "SPT_EXPERIMENT";
	if(m_experiment_params.experimentType == FRAP_EXPERIMENT)
		experimentType_str = "FRAP_EXPERIMENT";
	if(m_experiment_params.experimentType == PAF_EXPERIMENT)
		experimentType_str = "PAF_EXPERIMENT";
	if(m_experiment_params.experimentType == FCS_EXPERIMENT)
		experimentType_str = "FCS_EXPERIMENT";
	if(m_experiment_params.experimentType == SRI_EXPERIMENT)
		experimentType_str = "SRI_EXPERIMENT";
	if(m_experiment_params.experimentType == DRUG_EXPERIMENT)
		experimentType_str = "DRUG_EXPERIMENT";
	myfile<<"[experimentParams.experimentType] "<<experimentType_str<<"\n";

	string acquisitionType_str;
	if(m_experiment_params.acquisitionType == STREAM_ACQUISITION)
		acquisitionType_str = "STREAM_ACQUISITION";
	if(m_experiment_params.acquisitionType == TIMELAPSE_ACQUISITION)
		acquisitionType_str = "TIMELAPSE_ACQUISITION";
	myfile<<"[experimentParams.acquisitionType] "<<acquisitionType_str<<"\n";

	myfile<<"[experimentParams.N_planes] "<<m_experiment_params.N_planes<<"\n";

	myfile<<"[experimentParams.N_frap] "<<m_experiment_params.N_frap<<"\n";

	myfile<<"[experimentParams.dN_frap] "<<m_experiment_params.dN_frap<<"\n";

	myfile<<"[experimentParams.k_off_frap] "<<m_experiment_params.k_off_frap<<"\n";

	myfile<<"[experimentParams.N_photoActivation] "<<m_experiment_params.N_photoActivation<<"\n";

	myfile<<"[experimentParams.dN_photoActivation] "<<m_experiment_params.dN_photoActivation<<"\n";

	myfile<<"[experimentParams.k_on_photoActivation] "<<m_experiment_params.k_on_photoActivation<<"\n";

	myfile<<"[experimentParams.N_drug] "<<m_experiment_params.N_drug<<"\n";

	string drugAffectedParam_str;
	if(m_experiment_params.drug_affectedParam == DOUT_DRUGPARAM)
		drugAffectedParam_str = "DOUT_DRUGPARAM";
	if(m_experiment_params.drug_affectedParam == DIN_DRUGPARAM)
		drugAffectedParam_str = "DIN_DRUGPARAM";
	if(m_experiment_params.drug_affectedParam == DTRAP_DRUGPARAM)
		drugAffectedParam_str = "DTRAP_DRUGPARAM";
	if(m_experiment_params.drug_affectedParam == KON_DRUGPARAM)
		drugAffectedParam_str = "KON_DRUGPARAM";
	if(m_experiment_params.drug_affectedParam == KOFF_DRUGPARAM)
		drugAffectedParam_str = "KOFF_DRUGPARAM";
	if(m_experiment_params.drug_affectedParam == POROS_DRUGPARAM)
		drugAffectedParam_str = "POROS_DRUGPARAM";

	myfile<<"[experimentParams.drug_affectedParam] "<<drugAffectedParam_str<<"\n";

	myfile<<"[experimentParams.drug_newValue] "<<m_experiment_params.drug_newValue<<"\n";

	myfile<<"[experimentParams.SRI_pointingAccuracy] "<<m_experiment_params.SRI_pointingAccuracy<<"\n";

	myfile<<"[experimentParams.SRI_detectionIntensity] "<<m_stormRendering_params.point_intensity<<"\n";

	myfile<<"[experimentParams.SRI_zoom] "<<m_experiment_params.SRI_zoom<<"\n";

	myfile<<"[experimentParams.N_presequ] "<<m_experiment_params.N_presequ<<"\n";

	myfile<<"[experimentParams.N_repetition] "<<m_experiment_params.N_repetition<<"\n";

	myfile<<"[experimentParams.acquisitionPeriod] "<<m_experiment_params.acquisitionPeriod<<"\n";

	myfile<<"[experimentParams.measuredRegionIdxs_v] ";
	for(string rgn_name : m_experiment_params.measuredRegionNames_v)
	{
		QString saved_measured_RgnName;
		int measured_rgnIdx = m_bioWorld->getRegionIdx(rgn_name);
		if(measured_rgnIdx != -1)
		{
			saved_measured_RgnName = "Region" + QString("test").setNum(measured_rgnIdx,10);
			myfile<<saved_measured_RgnName.toLocal8Bit().data()<<" ";
		}
	}
	myfile<<"\n";

	myfile<<"[experimentParams.bleachedRegionIdxs_v] ";
	for(string rgn_name : m_experiment_params.bleachedRegionNames_v)
	{
		QString saved_bleached_RgnName;
		int bleached_rgnIdx = m_bioWorld->getRegionIdx(rgn_name);
		if(bleached_rgnIdx != -1)
		{
			saved_bleached_RgnName = "Region" + QString("test").setNum(bleached_rgnIdx,10);
			myfile<<saved_bleached_RgnName.toLocal8Bit().data()<<" ";
		}
	}
	myfile<<"\n";

	myfile<<"[experimentParams.file_destination] ";
	myfile<<m_experiment_params.file_destination<<"\n";

	myfile<<"[experimentParams.recording_mode] ";
	string recording_mode;
	if(m_experiment_params.recording_mode == SEQUENTIAL_RECORDING) recording_mode = "SEQUENTIAL_RECORDING";
	if(m_experiment_params.recording_mode == ALL_IN_ONE_RECORDING) recording_mode = "ALL_IN_ONE_RECORDING";
	myfile<<recording_mode<<"\n";

	myfile<<"[experimentParams.isExportingStack] ";
	string isExportingStack_str;
	if(m_experiment_params.isExportingStack == true) isExportingStack_str = "TRUE";
	if(m_experiment_params.isExportingStack == false) isExportingStack_str = "FALSE";
	myfile<<isExportingStack_str<<"\n";

	myfile<<"[experimentParams.is_fcsUsingGaussianBeam] ";
	string is_fcsUsingGaussianBeam_str;
	if(m_experiment_params.is_fcsUsingGaussianBeam == true) is_fcsUsingGaussianBeam_str = "TRUE";
	if(m_experiment_params.is_fcsUsingGaussianBeam == false) is_fcsUsingGaussianBeam_str = "FALSE";
	myfile<<is_fcsUsingGaussianBeam_str<<"\n";

	myfile<<"[experimentParams.fcs_beamSigma] "<<m_experiment_params.fcs_beamSigma<<"\n";
	myfile<<"[experimentParams.fcs_beamKoff] "<<m_experiment_params.fcs_beamKoff<<"\n";
	myfile<<"[experimentParams.fcs_maxIntensity] "<<m_experiment_params.fcs_maxIntensity<<"\n";

//	{"experimentParams.file_destination",experimentParams_file_destination},

	string simulatorMode_str;
	if(m_simulation_states.simulator_mode == GEOMETRY_MODE) simulatorMode_str = "GEOMETRY_MODE";
	if(m_simulation_states.simulator_mode == LIVE_MODE) simulatorMode_str = "LIVE_MODE";
	if(m_simulation_states.simulator_mode == EXPERIMENT_MODE) simulatorMode_str = "EXPERIMENT_MODE";
	if(m_simulation_states.simulator_mode == ANALYSIS_MODE) simulatorMode_str = "ANALYSIS_MODE";
	myfile<<"[simulationState.simulator_mode] "<<simulatorMode_str<<"\n";

	string hasStarted_str;
	if(m_simulation_states.simulationStarted == true) hasStarted_str = "TRUE";
	if(m_simulation_states.simulationStarted == false) hasStarted_str = "FALSE";
	myfile<<"[simulationState.simulationStarted] "<<hasStarted_str<<"\n";

	string hasPaused_str;
	if(m_simulation_states.simulationPaused == true) hasPaused_str = "TRUE";
	if(m_simulation_states.simulationPaused == false) hasPaused_str = "FALSE";
	myfile<<"[simulationState.simulationPaused] "<<hasPaused_str<<"\n";

	string hasEnded_str;
	if(m_simulation_states.simulationEnded == true) hasEnded_str = "TRUE";
	if(m_simulation_states.simulationEnded == false) hasEnded_str = "FALSE";
	myfile<<"[simulationState.simulationEnded] "<<hasEnded_str<<"\n";

	string isSingleShot_str;
	if(m_simulation_states.isSingleShot == true) isSingleShot_str = "TRUE";
	if(m_simulation_states.isSingleShot == false) isSingleShot_str = "FALSE";
	myfile<<"[simulationState.isSingleShot] "<<isSingleShot_str<<"\n";

	string isRendering_str;
	if(m_simulation_states.isRenderingEnabled == true) isRendering_str = "TRUE";
	if(m_simulation_states.isRenderingEnabled == false) isRendering_str = "FALSE";
	myfile<<"[simulationState.isRendering] "<<isRendering_str<<"\n";

	string isParameterWidgetVisible_str;
	isParameterWidgetVisible_str = ((m_simulation_states.isParameterWidgetVisible == true) ? "TRUE" :	"FALSE");
	myfile<<"[simulationState.isParameterWidgetVisible] "<<isParameterWidgetVisible_str<<"\n";

	string isGraphicVisible_str;
	isGraphicVisible_str = ((m_simulation_states.isGraphicVisible == true) ? "TRUE" : "FALSE");
	myfile<<"[simulationState.isGraphicVisible] "<<isGraphicVisible_str<<"\n";

	myfile.close();

}

void FluoSimModel::addGeometry(vector<glm::vec2>& rgn_r)
{
	int next_rgnIdx = m_bioWorld->getNextRegionUniqueId();

    m_bioWorld->addRegion(rgn_r);
    Region& rgn = m_bioWorld->getRegionRef(m_bioWorld->getNbRegions()-1);
    rgn.computeSurface();

    if(rgn.getSize() <= 2 || rgn.getSurface() == 0.0f) m_bioWorld->deleteRegion(&rgn);
    else
    {
        rgn.setName("Region" + to_string(next_rgnIdx));
        rgn.computeBarycenter();
        rgn.computeRadiusSquared();
        if(m_bioWorld->getNbRegions() > 1)
        {
            rgn.setIsACompartment(m_bioWorld->getSpecieAdr(0), false);
        }
    }

    if(m_liveExperiment_params.measured_RgnName == "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.measured_RgnName = m_bioWorld->getRegionRef(0).getName();
    }
    if(m_liveExperiment_params.bleached_RgnName== "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.bleached_RgnName = m_bioWorld->getRegionRef(0).getName();
    }
    if(m_liveExperiment_params.photoActivated_RgnName == "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.photoActivated_RgnName = m_bioWorld->getRegionRef(0).getName();
    }


	updateParticleSystemParams();
	updateDynamicParams();
	updatePhotoPhysicsParams();
    updateExperimentParams();
    updateLiveExperimentParams();

	m_bioWorld->fitBiologicalWorld(m_scrn);
}


void FluoSimModel::loadGeometry(string path)
{
	if(path.size() == 0)
	{
		vector<pair<string, string> > filteredTypes { {"Geometry Files", "rgn roi"} };

		bool isFile_selected =  getFileLocationFiltered(path, filteredTypes, "", &m_mainWindow);

		if(isFile_selected == false) return; //->
	}

	vector<vector<glm::vec2> > rgns_v;
	vector<glm::vec2> rgn_v;

	int file_type = -1;
	if(QString(path.data()).endsWith(".roi"))	file_type = 0;
	if(QString(path.data()).endsWith(".ROI"))	file_type = 0;
	if(QString(path.data()).endsWith(".rgn"))	file_type = 1;
	if(QString(path.data()).endsWith(".RGN"))	file_type = 1;

	if(file_type == -1) return; //->
	if(file_type == 0)
	{
		rgn_v = getPointsFromImageJRgnFile(path);
		rgns_v.push_back(rgn_v);
	}
	if(file_type == 1) 	rgns_v = getPointsFromMetaMorphRgnFile(path);

	int nb_rgns = rgns_v.size();
	for(int rgn_idx = 0; rgn_idx <= nb_rgns-1; rgn_idx++)
	{
		int next_rgnIdx = m_bioWorld->getNextRegionUniqueId();

		m_bioWorld->addRegion(rgns_v[rgn_idx]);
		Region& rgn = m_bioWorld->getRegionRef(m_bioWorld->getNbRegions()-1);
		rgn.computeSurface();

		if(rgn.getSize() <= 2 || rgn.getSurface() == 0.0f) m_bioWorld->deleteRegion(&rgn);
		else
		{
			rgn.setName("Region" + to_string(next_rgnIdx));
			rgn.computeBarycenter();
			rgn.computeRadiusSquared();
            if(m_bioWorld->getNbRegions() > 1)
            {
                rgn.setIsACompartment(m_bioWorld->getSpecieAdr(0), false);
            }
            next_rgnIdx++;
		}
	}

    if(m_liveExperiment_params.measured_RgnName == "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.measured_RgnName = m_bioWorld->getRegionRef(0).getName();
    }
    if(m_liveExperiment_params.bleached_RgnName== "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.bleached_RgnName = m_bioWorld->getRegionRef(0).getName();
    }
    if(m_liveExperiment_params.photoActivated_RgnName == "" && m_bioWorld->getNbRegions() >=1) {
        m_liveExperiment_params.photoActivated_RgnName = m_bioWorld->getRegionRef(0).getName();
    }

    updateParticleSystemParams();
    updateDynamicParams();
    updatePhotoPhysicsParams();
    updateExperimentParams();

	m_bioWorld->fitBiologicalWorld(m_scrn);
}

void FluoSimModel::saveGeometry()
{
	string geometry_filePath;

	vector<pair<string, string> > filteredTypes { {"MetaMorph Regions", "rgn"} ,
												  {"Adobe Illustrator Polygons", "svg"}};

	bool file_selected = getSaveLocationFiltered(geometry_filePath, filteredTypes,"", "", &m_mainWindow);

	if(file_selected == false) return; //->

	string file_type;
	file_type = geometry_filePath.substr(1+geometry_filePath.find_last_of("."), string::npos);

	if(file_type == "rgn")
	{
		m_bioWorld->saveRegions(geometry_filePath, METAMORPH_GEOMETRY_FILE_FORMAT);
	}
	if(file_type == "svg")
	{
		m_bioWorld->saveRegions(geometry_filePath, SVG_FILE_FORMAT);
	}

}

bool FluoSimModel::loadBackgroundImage(string& imageFile_path)
{
	QDir app_dir = m_main_app->applicationDirPath();
	QDir file_dir(imageFile_path.data());
	string file_absolutePath = imageFile_path;
	if(app_dir.exists(file_absolutePath.data()) == false) return false; //->

	if(file_dir.isRelative())
	{
		file_absolutePath = app_dir.absolutePath().toStdString() + '/' + imageFile_path;
		m_rendering_params.backgroundImage_path = imageFile_path;
	}
	else // not relative
	{
		std::size_t pos = file_absolutePath.find(app_dir.path().toStdString());
		if(pos !=  file_absolutePath.npos) //can be made relative
		{
			string file_relativePath;
			file_relativePath = file_absolutePath.substr(pos + m_main_app->applicationDirPath().size());
			std::size_t pos = file_relativePath.find_first_not_of('/');
			if(pos != file_relativePath.npos) file_relativePath =  file_relativePath.substr(pos);

			m_rendering_params.backgroundImage_path = file_relativePath;
		}
		else //can't be made relative
		{
			m_rendering_params.backgroundImage_path =  file_absolutePath;
		}
	}

	//image loading...
	if(m_backgroundImage == 0)
	{
		m_backgroundImage = new myGLImage(file_absolutePath, vec2(0,0), vec2(2,2), m_textureRect_coords_gMV);

	}

	m_scrn.getRenderWindow()->makeCurrent();
	m_backgroundImage->getTexture().clearTexture();
	m_backgroundImage->loadImage(file_absolutePath);
	m_backgroundImage->getRect().setDiag(vec2(0,0),	 vec2(0,0) + m_backgroundImage->getTexture().getSize());
	m_backgroundImage->setIsYInverted(false);
	m_backgroundImage->show();

//	m_backgroundImage->setRenderingMode(myGLImage::IMAGE_RENDERING1);

	//to center the image
	vec2 center = 0.5f*(m_backgroundImage->getRect().getTopRight() + m_backgroundImage->getRect().getBottomLeft());
	vec2 rect_size = m_backgroundImage->getRect().getTopRight() - m_backgroundImage->getRect().getBottomLeft();
	vec2 square_size(4/3.0f*std::max(rect_size.x, rect_size.y),
					 4/3.0f*std::max(rect_size.x, rect_size.y));

	m_scrn.setPixelView(center - 0.5f*square_size, square_size);

	return true;
}

void FluoSimModel::renderBioWorld()
{
	int renderedObjectTypes;
    if(m_simulation_states.simulator_mode == GEOMETRY_MODE/* || m_simulation_states.simulator_mode == ANALYSIS_MODE*/) \
		renderedObjectTypes = BiologicalWorld::REGION_BIT;
	else {
		renderedObjectTypes = BiologicalWorld::PARTICLE_BIT;
		if(m_current_rendering_params->areRegionsVisible) renderedObjectTypes += BiologicalWorld::REGION_BIT;
	}

	//***RENDERING***//
	switch(m_current_rendering_params->rendering_mode)
	{
		case RENDERING_POINTS :
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.removeRenderable();

            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
				m_scrn.clearScreen();
            if(m_current_rendering_params->rendering_target == Screen::CAMERA_FRAMEBUFFER) \
				m_scrn.clearCamera();



            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
                m_scrn.renderBeingDrawnRegion();

            if(m_current_rendering_params->areWordsVisible)
			{
				updateGLWords();
				m_simulationFPS_glWord.draw(m_scrn.getRenderWindow());
				m_nbThreads_glWord.draw(m_scrn.getRenderWindow());
//				m_singleThreadedTime_glWord.draw(m_scrn.getRenderWindow());
//				m_multiThreadedTime_glWord.draw(m_scrn.getRenderWindow());
			}

			if(m_backgroundImage != 0 && ((m_simulation_states.simulator_mode == GEOMETRY_MODE &&
												m_current_rendering_params->isBackgroundImageVisible_inGeom) ||
										 (m_simulation_states.simulator_mode == LIVE_MODE || m_simulation_states.simulator_mode == EXPERIMENT_MODE)&&
												m_current_rendering_params->isBackgroundImageVisible_inLiveAndExp))

			{
				m_scrn.getRenderWindow()->makeCurrent();
				m_backgroundImage->render(m_scrn.getRenderWindow());
			}

			if(m_bioWorld != 0)
			{
                m_scrn.setRenderingMode(Screen::POINTS);
				m_bioWorld->renderBiologicalWorld(m_scrn, renderedObjectTypes);
				m_scrn.drawBuffer(m_current_rendering_params->rendering_target);
			}

            if(m_current_rendering_params->isUsingPoissonNoise == true)
			{
				m_scrn.applyPoissonNoise();
			}

            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
				m_scrn.refreshScreen();
		}
		break;

		case RENDERING_GAUSSIANS :
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.removeRenderable();

            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
				m_scrn.clearScreen();
            if(m_current_rendering_params->rendering_target == Screen::CAMERA_FRAMEBUFFER) \
				m_scrn.clearCamera();

            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
                m_scrn.renderBeingDrawnRegion();

            if(m_current_rendering_params->areWordsVisible)
			{
				updateGLWords();
				m_simulationFPS_glWord.draw(m_scrn.getRenderWindow());
				m_nbThreads_glWord.draw(m_scrn.getRenderWindow());
//				m_singleThreadedTime_glWord.draw(m_scrn.getRenderWindow());
//				m_multiThreadedTime_glWord.draw(m_scrn.getRenderWindow());
			}

			if(m_backgroundImage != 0 && ((m_simulation_states.simulator_mode == GEOMETRY_MODE &&
												m_current_rendering_params->isBackgroundImageVisible_inGeom) ||
										 (m_simulation_states.simulator_mode == LIVE_MODE || m_simulation_states.simulator_mode == EXPERIMENT_MODE)&&
												m_current_rendering_params->isBackgroundImageVisible_inLiveAndExp))
			{
				m_backgroundImage->render(m_scrn.getRenderWindow());
			}

			if(m_bioWorld != 0)
			{
                evaluateAutoscaleFactor();
                m_scrn.setAutoscaleFactor(m_rendering_params.autoscale_factor);

				if(m_current_rendering_params->particle_color == PARTICLE_LOOKUP_TABLE) \
                    m_scrn.setRenderingMode(Screen::GAUSSIANS_LOOKUP_TABLE);
				else \
                    m_scrn.setRenderingMode(Screen::GAUSSIANS);

				m_bioWorld->renderBiologicalWorld(m_scrn, renderedObjectTypes);
				m_scrn.drawBuffer(m_current_rendering_params->rendering_target);
			}

            if(m_current_rendering_params == &m_cameraRendering_params) \
                m_scrn.applyOffsetsAndNoises();

            if(m_current_rendering_params->rendering_target == Screen::SCREEN_FRAMEBUFFER) \
				m_scrn.refreshScreen();
		}
		break;


		case RENDERING_ACCUMULATION_GAUSSIANS :
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.removeRenderable();

			if(m_bioWorld != 0)
			{
				if(m_current_rendering_params->particle_color == PARTICLE_LOOKUP_TABLE)
				{
                    m_scrn.setRenderingMode(Screen::GAUSSIANS_LOOKUP_TABLE);
				}
				else
				{
                    m_scrn.setRenderingMode(Screen::POINTS);
				}

				m_bioWorld->renderBiologicalWorld(m_scrn, BiologicalWorld::PARTICLE_BIT,
												  m_experiment_params.SRI_pointingAccuracy / m_simulation_params.pixel_size);

                m_scrn.drawBuffer(Screen::CAMERA_FRAMEBUFFER);
			}
		}
		break;
	}
}

void FluoSimModel::renderCamera()
{
	if(m_bioWorld != 0)
	{
		//Render SRImage inside texture
		if(m_backgroundImage != 0)
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.clearScreen();

			updateGLWords();
			m_simulationFPS_glWord.draw(m_scrn.getRenderWindow());
            m_scrn.renderBeingDrawnRegion();

			m_cameraImage->loadImage(m_scrn.getCameraTexture());
            m_cameraImage->scaleR16Image();
			m_cameraImage->render(m_scrn.getRenderWindow());

			m_scrn.refreshScreen();
		}
	}
}

void FluoSimModel::renderBioWorldCamera(bool wDrawing/*, bool wRemoveRenderable*/)
{
	int renderedObjectTypes;
	if(m_simulation_states.simulator_mode == GEOMETRY_MODE)
	{
		renderedObjectTypes = BiologicalWorld::REGION_BIT;
	}
	else
	{
		renderedObjectTypes = BiologicalWorld::PARTICLE_BIT;
		if(m_current_rendering_params->areRegionsVisible) renderedObjectTypes += BiologicalWorld::REGION_BIT;
	}

	//***RENDERING***//

	switch(m_current_rendering_params->rendering_mode)
	{
		case RENDERING_POINTS :
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.removeRenderable();
			m_scrn.clearCamera();

			if(m_bioWorld != 0)
			{
                m_scrn.setRenderingMode(Screen::POINTS);
				m_bioWorld->renderBiologicalWorld(m_scrn, renderedObjectTypes);
                m_scrn.drawBuffer(Screen::CAMERA_FRAMEBUFFER);

                if(m_current_rendering_params->isUsingPoissonNoise == true)
				{
					m_scrn.applyPoissonNoise();
				}
			}
		}
		break;

		case RENDERING_GAUSSIANS :
		{
			m_scrn.getRenderWindow()->makeCurrent();
			m_scrn.removeRenderable();
			m_scrn.clearCamera();

			if(m_bioWorld != 0)
			{
				if(m_current_rendering_params->particle_color == PARTICLE_LOOKUP_TABLE)
				{
                    m_scrn.setRenderingMode(Screen::GAUSSIANS_LOOKUP_TABLE);
				}
				else
				{
                    m_scrn.setRenderingMode(Screen::GAUSSIANS);
				}

				m_bioWorld->renderBiologicalWorld(m_scrn, renderedObjectTypes);
                m_scrn.drawBuffer(Screen::CAMERA_FRAMEBUFFER);

                if(m_current_rendering_params->isUsingPoissonNoise == true)
				{
					m_scrn.applyPoissonNoise();
				}

			}
		}
		break;

		case RENDERING_ACCUMULATION_GAUSSIANS :
		{
			//construct SRImage
			m_scrn.getRenderWindow()->makeCurrent();
//			if(wRemoveRenderable == true)
			m_scrn.removeRenderable();

            //can be ameliorate !
			m_scrn.setPointSize(1);
			m_scrn.setPointIntensity(m_stormRendering_params.spot_intensity);
            //can be ameliorate !

			if(m_bioWorld != 0)
			{
				if(m_current_rendering_params->particle_color == PARTICLE_LOOKUP_TABLE)
				{
                    m_scrn.setRenderingMode(Screen::GAUSSIANS_LOOKUP_TABLE);
				}
				else
				{
                    m_scrn.setRenderingMode(Screen::POINTS);
				}

				m_bioWorld->renderBiologicalWorld(m_scrn, BiologicalWorld::PARTICLE_BIT,
												  m_experiment_params.SRI_pointingAccuracy / m_simulation_params.pixel_size);

				if(wDrawing == true)
				{
                    m_scrn.drawBuffer(Screen::CAMERA_FRAMEBUFFER);
				}

			}
		}
		break;
	}
}


void FluoSimModel::captureScreen(string& file_path)
{
//RENDERING WITHOUT TEXT
	myGLScreen* window = m_scrn.getRenderWindow();
	window->makeCurrent();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_current_rendering_params->areWordsVisible = false;
	renderBioWorld();
	renderBioWorld();
	m_current_rendering_params->areWordsVisible = true;

//COPY FRAMEBUFFER IN TEXTURE
	gstd::gTexture screen_text(GL_TEXTURE_2D);
	window->captureScreenBackBuffer(screen_text);

	vector<uint8> text_data;
	screen_text.getTextureData(text_data);

//SAVE DATA IN A .BMP image file
    vec2 screen_size = screen_text.getSize();
	QImage screen_image(text_data.data(), screen_size.x, screen_size.y, QImage::Format_RGBA8888);
	screen_image = screen_image.mirrored(false, true);

	screen_image.save(file_path.data());
}

void FluoSimModel::captureCamera(string tiff_path, myTiff::OPENING_MODE mode)
{
//RENDERING WITHOUT TEXT
	myGLScreen* window = m_scrn.getRenderWindow();
	window->makeCurrent();
	renderBioWorldCamera();

//COPY FRAMEBUFFER IN TEXTURE
	vector<uint16> text_data_16;
	gstd::gTexture& texture = m_scrn.getCameraTexture();
	texture.getTextureData(text_data_16, GL_RED, GL_UNSIGNED_SHORT, 1);
	m_scrn.clearCamera();

//SAVE DATA IN A .BMP image file
	vec2 screen_size = texture.getSize();
	if(tiff_path.empty() == true) tiff_path = "screen_capture_camera.tiff";
	myTiff tiff;
	tiff.open(tiff_path, mode);
	tiff.setSamplesPerPixel(1);
	tiff.setBytesPerSample(2);
	tiff.setTiffSize(screen_size);
	tiff.addPage((uint8*) text_data_16.data());
}

void FluoSimModel::captureCamera(TinyTIFFFile* tiff_hdl, myTiff::OPENING_MODE mode)
{
    if(m_rendering_params.backgroundImage_path.empty() == true) return; //-> (no background image i.e. no camera field set...)

//RENDERING WITHOUT TEXT
	myGLScreen* window = m_scrn.getRenderWindow();
	window->makeCurrent();

	setRenderingParams(&m_cameraRendering_params);
	renderBioWorld();

// COPY FRAMEBUFFER IN TEXTURE
	vector<uint16> text_data_16;
	gstd::gTexture& texture = m_scrn.getCameraTexture();
	texture.getTextureData(text_data_16, GL_RED, GL_UNSIGNED_SHORT, 1);
	m_scrn.clearCamera();

	if(m_stack_tiffHdl == 0)
	{
		gstd::gTexture& texture = m_scrn.getCameraTexture();
		vec2 screen_size = texture.getSize();

		//create an absolute path from m_experiment_params.file_destination
		QDir app_dir = m_main_app->applicationDirPath();
		QDir destinationDir_dir(m_experiment_params.file_destination.data());

		string destinationDir_str = m_experiment_params.file_destination;
		if(destinationDir_dir.isRelative())
		{
			if(destinationDir_str.empty()) destinationDir_str = app_dir.absolutePath().toStdString();
			else
			{
				if(app_dir.exists(destinationDir_str.data())) \
								destinationDir_str = app_dir.absolutePath().toStdString() + '/' + destinationDir_str;
				else return; //->
			}
		}
		if(app_dir.exists(destinationDir_str.data()) == false) return; //->
		string tiffPath_str = destinationDir_str + string("/stack.tif");
		cout<<"inside : aptureCamera : tiffPath_str = "<<tiffPath_str;
		m_stack_tiffHdl = TinyTIFFWriter_open(tiffPath_str.data(), 16, screen_size.x, screen_size.y);
	}

	if (m_stack_tiffHdl == 0) return; //->

	TinyTIFFWriter_writeImage(m_stack_tiffHdl, text_data_16.data());
}



void FluoSimModel::renderGraphicCurves()
{
	if(m_graphic == 0 || m_bioWorld == 0) return; //->

	m_graphic->makeCurrent();
	glClearColor(0.1,0.1,0.1,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2.0);

	m_graphic->renderSignals();
	m_graphic->swapBuffers();
}


void FluoSimModel::runSimulator()
{
	m_fullLoop_clock.startChrono();
	m_rendering_clock.startChrono();
	m_rendering_clock.setNbRecordedTours(100);
	m_updatingBioWorld_clock.startChrono();
	m_updatingBioWorld_clock.setNbRecordedTours(20);
	m_measuringBioWorld_clock.setNbRecordedTours(20);
	m_fullLoop_clock2.setNbRecordedTours(200);

	int index = 0;
	while(m_app_running == true)
	{
		m_fullLoop_clock2.startTour();

		if(m_simulation_states.simulationStarted == true  &&
		   m_simulation_states.simulationPaused == false  &&
		   m_simulation_params.current_plane <= m_experiment_params.N_planes-1)
		{

            //steps : FRAP/PAF/DRUG -> measure -> bioWorld update
			//FRAP
			if(m_simulation_states.simulator_mode == EXPERIMENT_MODE &&
			   m_experiment_params.experimentType == FRAP_EXPERIMENT &&
			   m_simulation_params.current_plane >= m_experiment_params.N_frap &&
			   m_simulation_params.current_plane <= m_experiment_params.N_frap + m_experiment_params.dN_frap-1)
			{
				auto frapHead = m_experimental_frapHead_v.begin();
				while(frapHead != m_experimental_frapHead_v.end())
				{
					frapHead->bleachRegion(m_experiment_params.k_off_frap, m_simulation_params.dt_sim);
					frapHead++;
				}
			}

			//PAF
			if(m_simulation_states.simulator_mode == EXPERIMENT_MODE &&
			   m_experiment_params.experimentType == PAF_EXPERIMENT &&
			   m_simulation_params.current_plane >= m_experiment_params.N_photoActivation &&
			   m_simulation_params.current_plane <= m_experiment_params.N_photoActivation + m_experiment_params.dN_photoActivation-1)
			{
				auto frapHead = m_experimental_frapHead_v.begin();
				while(frapHead != m_experimental_frapHead_v.end())
				{
					frapHead->photoActivateRegion(m_experiment_params.k_on_photoActivation, m_simulation_params.dt_sim);
					frapHead++;
				}
			}

			//DRUG
			if(m_simulation_states.simulator_mode == EXPERIMENT_MODE &&
			   m_experiment_params.experimentType == DRUG_EXPERIMENT &&
			   m_simulation_params.current_plane == m_experiment_params.N_drug)
			{
				m_dynamic_params.isDrugAffected = true;
				updateDynamicParams();
				m_dynamic_params.isDrugAffected = false;
            }

            //measure
            if(m_simulation_params.current_plane >= 0)
            {
                    m_measuringBioWorld_clock.startTour();
                    measureBioWorld();
                    m_measuringBioWorld_clock.endTour();
            }

            //bioWorld update
            m_updatingBioWorld_clock.startTour();
            updateBioWorld();
            m_updatingBioWorld_clock.endTour();

			if(m_simulation_params.current_plane >= m_experiment_params.N_planes)
			{
				if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
				{
					saveSimulationProducts();

					if(m_experiment_params.index_repetion < m_experiment_params.N_repetition-1)
					{
						if(m_experiment_params.initial_enrichment < 0.0) evaluateSteadyState();
						else imposeSteadyState();

						m_experiment_params.index_repetion++;

						clearProbeSignals();
						m_scrn.clearCamera();

						m_simulation_params.current_plane = -m_experiment_params.N_presequ;
						m_simulation_params.current_time = -m_experiment_params.N_presequ * m_simulation_params.dt_sim;

						//reset molecule dynamic before drugAdmin
						m_dynamic_params.isDrugAffected = false;
						updateDynamicParams();
					}
					else
					{
						stopSimulation();
						if(m_simulation_states.isSingleShot == true) m_app_running = false;
					}
				}
				else
				{
					stopSimulation();
				}
			}
		}

		while(pollEvents());
		m_main_app->processEvents();

		if(m_fullLoop_clock.getCurrentTimeInTour() >= 1.0f/m_frame_rate)
		{
		   m_fullLoop_clock.endTour();
		   updateParameterWidgetSize();
		   m_allRegions_tableWidget.repaint();

		   if((m_simulation_states.simulator_mode == GEOMETRY_MODE ||
			   m_simulation_states.simulator_mode == LIVE_MODE ||
			   m_simulation_states.simulator_mode == EXPERIMENT_MODE)
									&
			   m_simulation_states.isRenderingEnabled == true)
			{
				m_rendering_clock.startTour();

				if(m_simulation_states.simulator_mode == EXPERIMENT_MODE &&
					(m_experiment_params.experimentType == SRI_EXPERIMENT ||
					m_experiment_params.isExportingStack == true))
				{
					if(m_experiment_params.experimentType != SRI_EXPERIMENT)
					{
						setRenderingParams(&m_cameraRendering_params);
                        renderBioWorld();
					}
					renderCamera();
				}
				else {
					setRenderingParams(&m_rendering_params);
					renderBioWorld();
				}

				renderGraphicCurves();
				m_rendering_clock.endTour();
			}


			if(m_simulation_states.simulator_mode == ANALYSIS_MODE)
			{
				m_tracePlayer.render(m_scrn.getRenderWindow());
                //uncomment to see the regions in the spt analysis panel
//                m_bioWorld->renderBiologicalWorld(m_scrn, BiologicalWorld::REGION_BIT);
//                  m_scrn.renderBeingDrawnRegion();
				m_scrn.getRenderWindow()->swapBuffers();
			}
		}

		index++;
		m_fullLoop_clock2.endTour();
	}
}


void FluoSimModel::startSimulation()
{
	if(m_simulation_states.simulationStarted == true &&
	   m_simulation_states.simulationPaused == false) return; //->

	m_simulation_states.simulationStarted = true;
	m_simulation_states.simulationPaused = false;
	m_simulation_states.simulationEnded = false;
}

void FluoSimModel::pauseSimulation()
{
	if(m_simulation_states.simulationEnded == true ||
	   m_simulation_states.simulationPaused == true) return; //->

	//	m_simulation_states.m_simulationStarted = true;
		m_simulation_states.simulationPaused = true;
	//	m_simulation_states.m_simulationEnded = false;
}

void FluoSimModel::stopSimulation()
{
	m_simulation_states.simulationStarted = false;
	m_simulation_states.simulationPaused = false;
	m_simulation_states.simulationEnded = true;

	//if simulation is interupted during drug exp
	m_dynamic_params.isDrugAffected = false;
    updateDynamicParams(); //to set the basal parameters (i.e. without drug)

	clearProbeSignals();
	clearRecordedProbeSignals();
	m_scrn.clearCamera();

	m_experiment_params.index_repetion = 0;

	if(m_stack_tiffHdl != 0)
	{
		TinyTIFFWriter_close(m_stack_tiffHdl);
		m_stack_tiffHdl = 0;
	}

	switch(m_simulation_states.simulator_mode)
	{
		case LIVE_MODE :
		{
			m_simulation_params.current_plane = 0;
			m_simulation_params.current_time = 0;

		}
		break;

		case EXPERIMENT_MODE :
		{
			m_simulation_params.current_plane = -m_experiment_params.N_presequ;
			m_simulation_params.current_time = -m_experiment_params.N_presequ * m_simulation_params.dt_sim;
		}
	}
}

void FluoSimModel::setDestinationPath(string path)
{
	if(QDir(path.data()).exists() == false) // path does not exist ?
	{
		path = string("SimulationOutputs");
		if(QDir(path.data()).exists() == false) //in case of the absence of the default destination folder
		{
			path = string("");
		}
	}
	else // path exists
	{
		std::size_t pos = path.find(m_main_app->applicationDirPath().toStdString());
		if(pos != path.npos) //can be made relative
		{
			path = path.substr(pos + m_main_app->applicationDirPath().size());
			std::size_t pos = path.find_first_not_of('/');
			if(pos != path.npos) path =  path.substr(pos);
		}
	}

	m_experiment_params.file_destination = path;
}

void FluoSimModel::saveSimulationProducts()
{
//create an absolute path from m_experiment_params.file_destination
	QDir app_dir = m_main_app->applicationDirPath();
	QDir destinationDir_dir(m_experiment_params.file_destination.data());

	string destinationDir_str = m_experiment_params.file_destination;
	if(destinationDir_dir.isRelative())
	{
		if(destinationDir_str.empty())
		{
			destinationDir_str = app_dir.absolutePath().toStdString();
		}
		else
		{
			if(app_dir.exists(destinationDir_str.data()))
			{
				destinationDir_str = app_dir.absolutePath().toStdString() + '/' + destinationDir_str;
			}
			else return; //->
		}
	}

	if(app_dir.exists(destinationDir_str.data()) == false) return; //->

//start saving
	int m_nb_experimental_probes = m_experimental_probes_v.size();

	switch (m_experiment_params.experimentType)
	{
		case SPT_EXPERIMENT :
		{
			for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
			{
                Probe* probe = m_experimental_probes_v[probe_idx];
                if(probe->getMeasureType() == Probe::TRACE_TRACKER)
				{
					auto traces = m_experimental_probes_v[probe_idx]->getAllTraces();
					m_experimental_probes_v[probe_idx]->resetProbeMeasure();

					string rgn_str = to_string(m_bioWorld->getRegionIdx(probe->getRegion1()));
					string file_dir = destinationDir_str + string("/traces_rgn") + rgn_str +
																			 string("_rep") + to_string(m_experiment_params.index_repetion) +
																			 string(".trc");
					QPoint center_pos = this->mapToGlobal(this->centralWidget()->pos() + QPoint(this->centralWidget()->size().width()/2.0f, this->centralWidget()->size().height()/2.0f)
										- QPoint(m_progressWdgt.size().width()/2.0f, m_progressWdgt.size().height()/2.0f));
					m_progressWdgt.setText("Exporting Trajectories...");
					m_progressWdgt.move(center_pos);
					m_progressWdgt.show();
					m_main_app->processEvents();

					saveTracesAsString(traces, file_dir, PALMTRACER_FORMAT, m_simulation_params.dt_sim, m_progressWdgt.getProgressBar());

					m_progressWdgt.hide();
				}

                else if(probe->getMeasureType() == Probe::LOCALISATION)
				{
					auto localisations_v = probe->getAllLocalisations();
					m_experimental_probes_v[probe_idx]->resetProbeMeasure();

					string rgn_str = to_string(m_bioWorld->getRegionIdx(probe->getRegion1()));
					string file_dir = destinationDir_str + string("/localisations_rgn") + rgn_str +
																			 string("_rep") + to_string(m_experiment_params.index_repetion) +
																			 string(".txt");

					saveLocalisationsAsString(localisations_v, file_dir, THUNDERSTORM_FORMAT, m_simulation_params.dt_sim, m_simulation_params.pixel_size);
				}
			}
		}
		break;

		case FRAP_EXPERIMENT :
		{
			if(m_experiment_params.recording_mode == ALL_IN_ONE_RECORDING)
			{
				//resizing
				if(m_recordedSignalValues_perRep_perProbe.size() != m_nb_experimental_probes)
				{
					m_recordedSignalValues_perRep_perProbe.resize(m_nb_experimental_probes);
				}

				//measuring
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
                    Signal& signal = m_experimental_probes_v[probe_idx]->getSignalRef();
					m_recordedSignalValues_perRep_perProbe[probe_idx].push_back(signal.getValues_v(0, signal.getSize()-1));
				}

				//saving
				if(m_experiment_params.index_repetion == m_experiment_params.N_repetition-1)
				{
					//saving
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();
						string file_path = destinationDir_str +
										   string("/averageIntensity_rgn") + rgn_str +
										   string(".txt");

						if(m_recordedSignalValues_perRep_perProbe[probe_idx].size() != 0)
						{
							ofstream myfile;
							myfile.open(file_path.data());

							uint nb_recorded_values = m_recordedSignalValues_perRep_perProbe[probe_idx][0].size();
							uint nb_recorded_rep = m_recordedSignalValues_perRep_perProbe[probe_idx].size();

							for(uint value_idx = 0;  value_idx < nb_recorded_values; value_idx++)
							{
								for(uint rep = 0; rep < nb_recorded_rep; rep++)
								{
									vec2 value = m_recordedSignalValues_perRep_perProbe[probe_idx][rep][value_idx];
									myfile<<value.x<<"\t"<<value.y<<"\t";
								}
								myfile<<"\n";
							}
							myfile.close();
						}
					}

					//reset probes
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						m_experimental_probes_v[probe_idx]->resetProbeMeasure();
					}
				}
			}

			if(m_experiment_params.recording_mode == SEQUENTIAL_RECORDING)
			{
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
					string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();

					m_experimental_probes_v[probe_idx]->getSignalRef().saveSignal(destinationDir_str +
																				  string("/averageIntensity_rgn") + rgn_str +
																				  string("_rep") + to_string(m_experiment_params.index_repetion) +
																				  string(".txt"));
					cout<<"\n\nPath = "<<destinationDir_str +
						  string("/averageIntensity_rgn") + rgn_str +
						  string("_rep") + to_string(m_experiment_params.index_repetion) +
						  string(".txt")<<"\n\n\n";

					m_experimental_probes_v[probe_idx]->resetProbeMeasure();
				}
			}
		}
		break;

        case PAF_EXPERIMENT :
        {
            if(m_experiment_params.recording_mode == ALL_IN_ONE_RECORDING)
            {
                //resizing
                if(m_recordedSignalValues_perRep_perProbe.size() != m_nb_experimental_probes)
                {
                    m_recordedSignalValues_perRep_perProbe.resize(m_nb_experimental_probes);
                }

                //measuring
                for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
                {
                    Signal& signal = m_experimental_probes_v[probe_idx]->getSignalRef();
                    m_recordedSignalValues_perRep_perProbe[probe_idx].push_back(signal.getValues_v(0, signal.getSize()-1));
                }

                //saving
                if(m_experiment_params.index_repetion == m_experiment_params.N_repetition-1)
                {
                    //saving
                    for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
                    {
                        string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();
                        string file_path = destinationDir_str +
                                           string("/averageIntensity_rgn") + rgn_str +
                                           string(".txt");

                        if(m_recordedSignalValues_perRep_perProbe[probe_idx].size() != 0)
                        {
                            ofstream myfile;
                            myfile.open(file_path.data());

                            uint nb_recorded_values = m_recordedSignalValues_perRep_perProbe[probe_idx][0].size();
                            uint nb_recorded_rep = m_recordedSignalValues_perRep_perProbe[probe_idx].size();

                            for(uint value_idx = 0;  value_idx < nb_recorded_values; value_idx++)
                            {
                                for(uint rep = 0; rep < nb_recorded_rep; rep++)
                                {
                                    vec2 value = m_recordedSignalValues_perRep_perProbe[probe_idx][rep][value_idx];
                                    myfile<<value.x<<"\t"<<value.y<<"\t";
                                }
                                myfile<<"\n";
                            }
                            myfile.close();
                        }
                    }

                    //reset probes
                    for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
                    {
                        m_experimental_probes_v[probe_idx]->resetProbeMeasure();
                    }
                }
            }

            if(m_experiment_params.recording_mode == SEQUENTIAL_RECORDING)
            {
                for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
                {
                    string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();

                    m_experimental_probes_v[probe_idx]->getSignalRef().saveSignal(destinationDir_str +
                                                                                  string("/averageIntensity_rgn") + rgn_str +
                                                                                  string("_rep") + to_string(m_experiment_params.index_repetion) +
                                                                                  string(".txt"));
                    cout<<"\n\nPath = "<<destinationDir_str +
                          string("/averageIntensity_rgn") + rgn_str +
                          string("_rep") + to_string(m_experiment_params.index_repetion) +
                          string(".txt")<<"\n\n\n";

                    m_experimental_probes_v[probe_idx]->resetProbeMeasure();
                }
            }
        }
        break;

        case FCS_EXPERIMENT :
		{
			if(m_experiment_params.recording_mode == ALL_IN_ONE_RECORDING)
			{
				//resizing
				if(m_recordedSignalValues_perRep_perProbe.size() != m_nb_experimental_probes)
				{
					m_recordedSignalValues_perRep_perProbe.resize(m_nb_experimental_probes);
				}

				//saving in memory
				int probe_idx = 0;
                for(Probe* probe : m_experimental_probes_v)
				{
					//correlogram calculation
                    Signal& raw_signal = probe->getSignalRef();
                    Signal correlated_signal;

					int N = 100;
					for(int value_idx = 0; value_idx <= N-1; value_idx++)
					{
						float dt_sim = m_simulation_params.dt_sim;
						float a = log10(dt_sim);
						float b = log10(raw_signal.getValue(raw_signal.getValuesRef_v().size()-1).x);
						float ab = pow(10.0, a +(b-a)*value_idx/(N-1));

						float correlation = inter_corelation(ab/dt_sim,
															 raw_signal.getValuesRef_v().data(),
															 raw_signal.getValuesRef_v().data(),
															 raw_signal.getValuesRef_v().size());
						correlated_signal.addValue(vec2(log10(ab), correlation));
					}

					m_recordedSignalValues_perRep_perProbe[probe_idx].push_back(correlated_signal.getValues_v(0, correlated_signal.getSize()-1));
					probe_idx++;
					cout<<"nb rep : "<<m_recordedSignalValues_perRep_perProbe[probe_idx].size()<<"\n";
				}

				//saving in file
				if(m_experiment_params.index_repetion == m_experiment_params.N_repetition-1)
				{
					//saving
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();
						string file_path = destinationDir_str +
										   string("/allCorrelations_rgn") + rgn_str +
										   string(".txt");

						if(m_recordedSignalValues_perRep_perProbe[probe_idx].size() != 0)
						{
							ofstream myfile;
							myfile.open(file_path.data());

							uint nb_recorded_values = m_recordedSignalValues_perRep_perProbe[probe_idx][0].size();
							uint nb_recorded_rep = m_recordedSignalValues_perRep_perProbe[probe_idx].size();

							for(uint value_idx = 0;  value_idx < nb_recorded_values; value_idx++)
							{
								for(uint rep = 0; rep < nb_recorded_rep; rep++)
								{
									vec2 value = m_recordedSignalValues_perRep_perProbe[probe_idx][rep][value_idx];
									myfile<<value.x<<"\t"<<value.y<<"\t";
								}
								myfile<<"\n";
							}
							myfile.close();
						}
					}

					//reset probes
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						m_experimental_probes_v[probe_idx]->resetProbeMeasure();
					}
				}
			}

			if(m_experiment_params.recording_mode == SEQUENTIAL_RECORDING)
			{
				int probe_idx = 0;
                for(Probe* probe : m_experimental_probes_v)
				{
					string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();

					//correlogram calculation
                    Signal& raw_signal = probe->getSignalRef();
                    Signal correlated_signal;

					int N = 100;
					for(int value_idx = 0; value_idx <= N-1; value_idx++)
					{
						float dt_sim = m_simulation_params.dt_sim;
						float a = log10(dt_sim);
						float b = log10(raw_signal.getValue(raw_signal.getValuesRef_v().size()-1).x);
						float ab = pow(10.0, a +(b-a)*value_idx/(N-1));

						float correlation = inter_corelation(ab/dt_sim,
															 raw_signal.getValuesRef_v().data(),
															 raw_signal.getValuesRef_v().data(),
															 raw_signal.getValuesRef_v().size());
						correlated_signal.addValue(vec2(log10(ab), correlation));
					}

					//savings
					raw_signal.saveSignal(destinationDir_str +
										  string("/averageIntensity_rgn") + rgn_str +
										  string("_rep") + to_string(m_experiment_params.index_repetion) +
										  string(".txt"));

					correlated_signal.saveSignal(destinationDir_str +
												 string("/correlation_rgn") + rgn_str +
												 string("_rep") + to_string(m_experiment_params.index_repetion) +
												 string(".txt"));

					//reset
					m_experimental_probes_v[probe_idx]->resetProbeMeasure();
					probe_idx++;
				}
			}
		}
		break;


		case DRUG_EXPERIMENT :
		{
			if(m_experiment_params.recording_mode == ALL_IN_ONE_RECORDING)
			{
				//resizing
				if(m_recordedSignalValues_perRep_perProbe.size() != m_nb_experimental_probes)
				{
					m_recordedSignalValues_perRep_perProbe.resize(m_nb_experimental_probes);
				}

				//measuring
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
                    Signal& signal = m_experimental_probes_v[probe_idx]->getSignalRef();
					m_recordedSignalValues_perRep_perProbe[probe_idx].push_back(signal.getValues_v(0, signal.getSize()-1));
					cout<<"nb rep : "<<m_recordedSignalValues_perRep_perProbe[probe_idx].size()<<"\n";
				}

				//saving
				if(m_experiment_params.index_repetion == m_experiment_params.N_repetition-1)
				{
					//saving
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();
						string file_path = destinationDir_str +
										   string("/averageIntensity_rgn") + rgn_str +
										   string(".txt");

						if(m_recordedSignalValues_perRep_perProbe[probe_idx].size() != 0)
						{
							ofstream myfile;
							myfile.open(file_path.data());

							uint nb_recorded_values = m_recordedSignalValues_perRep_perProbe[probe_idx][0].size();
							uint nb_recorded_rep = m_recordedSignalValues_perRep_perProbe[probe_idx].size();

							for(uint value_idx = 0;  value_idx < nb_recorded_values; value_idx++)
							{
								for(uint rep = 0; rep < nb_recorded_rep; rep++)
								{
									vec2 value = m_recordedSignalValues_perRep_perProbe[probe_idx][rep][value_idx];
									myfile<<value.x<<"\t"<<value.y<<"\t";
								}
								myfile<<"\n";
							}
							myfile.close();
						}
					}

					//reset probes
					for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
					{
						m_experimental_probes_v[probe_idx]->resetProbeMeasure();
					}
				}
			}

			if(m_experiment_params.recording_mode == SEQUENTIAL_RECORDING)
			{
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
					string rgn_str = m_measuredRegions_listWidget.item(probe_idx)->text().toLocal8Bit().data();

					m_experimental_probes_v[probe_idx]->getSignalRef().saveSignal(destinationDir_str +
																				  string("/averageIntensity_rgn") + rgn_str +
																				  string("_rep") + to_string(m_experiment_params.index_repetion) +
																				  string(".txt"));

					m_experimental_probes_v[probe_idx]->resetProbeMeasure();
				}
			}
		}
		break;

		case SRI_EXPERIMENT :
		{
			myGLScreen* window = m_scrn.getRenderWindow();
			window->makeCurrent();

            vector<uint16> data_text;
			gstd::gTexture& texture = m_scrn.getCameraTexture();
            texture.getTextureData(data_text, GL_RED, GL_UNSIGNED_SHORT, 1);
            vec2 screen_size = texture.getSize();

			myTiff tiff;
			string file_name =	destinationDir_str +
                                string("/SRI_screen_capture_rep") +
								to_string(m_experiment_params.index_repetion) +
								string(".tiff");

			tiff.open(file_name, myTiff::WRITE_MODE);
			tiff.setSamplesPerPixel(1);
			tiff.setBytesPerSample(2);
			tiff.setTiffSize(screen_size);
            tiff.addPage((uint8*) data_text.data());
        }
		break;
	}
}

void FluoSimModel::setSimulatorMode(SIMULATOR_MODE simulator_mode)
{
	m_simulation_states.simulator_mode = simulator_mode;

	switch(simulator_mode)
	{
		case GEOMETRY_MODE :
		{
			if(m_current_rendering_params != &m_rendering_params)
			{
				m_current_rendering_params = &m_rendering_params;
				updateRenderingParams();
			}
		}
		break;

		case LIVE_MODE :
		{
			if(m_current_rendering_params != &m_rendering_params)
			{
				m_current_rendering_params = &m_rendering_params;
				updateRenderingParams();
			}

		}
		break;

		case EXPERIMENT_MODE :
		{
			if(m_experiment_params.experimentType == SRI_EXPERIMENT)
			{
				if(m_current_rendering_params != &m_stormRendering_params)
				{
					m_current_rendering_params = &m_stormRendering_params;
					updateRenderingParams();
				}
			}
			else
			{
				if(m_current_rendering_params != &m_rendering_params)
				{
					m_current_rendering_params = &m_rendering_params;
					updateRenderingParams();
				}
			}
		}
		break;

		case ANALYSIS_MODE :
		{
			if(m_current_rendering_params != &m_rendering_params)
			{
				m_current_rendering_params = &m_rendering_params;
				updateRenderingParams();
			}
		}
		break;
	}

	stopSimulation();
}

SIMULATOR_MODE FluoSimModel::getSimulatorMode()
{
	return m_simulation_states.simulator_mode;
}

void FluoSimModel::setExperimentType(EXPERIMENT_TYPE experiment_type)
{
	m_experiment_params.experimentType = experiment_type;
		if(m_simulation_states.simulator_mode == EXPERIMENT_MODE) //since rendering mode should not be modified except in the experiment mode
		{
			if(m_experiment_params.experimentType == SRI_EXPERIMENT)
			{
				if(m_current_rendering_params != &m_stormRendering_params)
				{
					m_current_rendering_params = &m_stormRendering_params;
					updateRenderingParams();
				}
			}
			else
			{
				if(m_current_rendering_params != &m_rendering_params)
				{
					m_current_rendering_params = &m_rendering_params;
					updateRenderingParams();
				}
			}
		}


	updateExperimentParams();
}

void FluoSimModel::setRenderingParams(renderingParams* rendering_params)
{
	if(m_current_rendering_params == rendering_params) return; //<-

	m_current_rendering_params = rendering_params;
	updateRenderingParams();
}

void FluoSimModel::updateBioWorld()
{
	if(m_bioWorld == 0) return; //->

    m_engine->updateSystem(m_simulation_params.dt_sim);
	m_simulation_params.current_plane++;
    m_simulation_params.current_time = m_simulation_params.current_plane*m_simulation_params.dt_sim;
}

void FluoSimModel::measureBioWorld()
{
	if(m_bioWorld == 0) return; //->

	if(m_simulation_states.simulator_mode == LIVE_MODE ||
	   m_experiment_params.acquisitionType == STREAM_ACQUISITION)
	{
        m_probe.measure(m_simulation_params.current_plane, m_simulation_params.current_time, m_simulation_params.dt_sim);
	}

	if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
	{
		if(m_experiment_params.acquisitionType == STREAM_ACQUISITION)
		{
			if(m_experiment_params.experimentType == SRI_EXPERIMENT)
			{
                setRenderingParams(&m_stormRendering_params);
                renderBioWorld();
			}
			else
			{
				if(m_experiment_params.isExportingStack == true)
				{
					captureCamera(m_stack_tiffHdl, myTiff::WRITE_MODE);
				}

				int m_nb_experimental_probes = m_experimental_probes_v.size();
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
                    m_experimental_probes_v[probe_idx]->measure(m_simulation_params.current_plane,
                                                                m_simulation_params.current_time,
                                                                m_simulation_params.dt_sim);
				}
			}
		}

		if(m_experiment_params.acquisitionType == TIMELAPSE_ACQUISITION &&
           m_simulation_params.current_plane % lround(m_experiment_params.acquisitionPeriod/m_simulation_params.dt_sim) == 0)
		{
			if(m_experiment_params.experimentType == SRI_EXPERIMENT)
			{
//				renderBioWorldCamera();
                setRenderingParams(&m_stormRendering_params);
                renderBioWorld();
            }
			else
			{
                if(m_experiment_params.isExportingStack == true)
                {
                    captureCamera(m_stack_tiffHdl, myTiff::WRITE_MODE);
                }

                int m_nb_experimental_probes = m_experimental_probes_v.size();
				for(int probe_idx = 0; probe_idx <= m_nb_experimental_probes-1; probe_idx++)
				{
					m_experimental_probes_v[probe_idx]->measure(m_simulation_params.current_plane,
																m_simulation_params.current_time,
																m_simulation_params.dt_sim);
				}
			}
		}
	}
}

void FluoSimModel::computeAutoCorrelogram()
{
	if(m_bioWorld == 0) return; //->

	if(m_simulation_states.simulator_mode == LIVE_MODE ||
	   m_experiment_params.acquisitionType == STREAM_ACQUISITION)
	{
		auto& raw_signal = m_probe.getSignalRef();
		m_signal.clearValues();

		int N = 100;
		for(int value_idx = 0; value_idx <= N-1; value_idx++)
		{
			float dt_sim = m_simulation_params.dt_sim;
			float a = log10(dt_sim);
			float b = log10(raw_signal.getValue(raw_signal.getValuesRef_v().size()-1).x);
			float ab = pow(10.0, a +(b-a)*value_idx/(N-1));

			float correlation = inter_corelation(ab/dt_sim,
												 raw_signal.getValuesRef_v().data(),
												 raw_signal.getValuesRef_v().data(),
												 raw_signal.getValuesRef_v().size());
			m_signal.addValue(vec2(log10(ab), correlation));
		}
	}
}


void FluoSimModel::clearProbeSignals()
{
	m_probe.resetProbeMeasure();
	m_signal.clearValues();

    for(Probe* probe : m_experimental_probes_v)
	{
		probe->resetProbeMeasure();
	}
}

void FluoSimModel::clearRecordedProbeSignals()
{
	m_recordedSignalValues_perRep_perProbe.clear();
}




void FluoSimModel::updateParticleSystemParams()
{
	if(m_bioWorld == 0 || m_bioWorld->getNbRegions() == 0) return; //->

	int new_nb_particles = m_particleSystem_params.N_particles;
	int current_nb_particles = m_bioWorld->getParticlePositions().size();
	int Dnb_particles = new_nb_particles - current_nb_particles;

	if(Dnb_particles > 0) m_bioWorld->addParticles(Dnb_particles,0,0,0);
	if(Dnb_particles < 0) m_bioWorld->deleteParticlesWithMotherRgn(abs(Dnb_particles),0,0);


	m_engine->updateSelectedMode();
	updateRenderingParams(); //to update color;
}

void FluoSimModel::updateDynamicParams()
{
	if(m_bioWorld == 0 || m_bioWorld->getNbRegions() == 0) return; //->

	float inv_pxSquared = 1.0/(pow(m_simulation_params.pixel_size, 2.0));

	bool isFixed;
	float D_outside, D_inside, D_trapped;
	float kon,koff;
	float crossingProbability_outIn;
	float immobile_fraction;

	isFixed = m_dynamic_params.isFixed;
	D_outside = m_dynamic_params.D_outsideContact*inv_pxSquared;
    D_inside = m_dynamic_params.D_insideContact*inv_pxSquared;
    D_trapped = m_dynamic_params.D_trapped*inv_pxSquared;
	kon = m_dynamic_params.k_on;
	koff = m_dynamic_params.k_off;
	crossingProbability_outIn = m_dynamic_params.crossingProbability_outIn;
	immobile_fraction = m_dynamic_params.immobileFraction;

	if(m_dynamic_params.isDrugAffected == true)
	{
		switch(m_dynamic_params.drug_affectedParam)
		{
			case DOUT_DRUGPARAM :
			{
				D_outside = m_dynamic_params.drug_newValue*inv_pxSquared;
			}
			break;

			case DIN_DRUGPARAM :
			{
                D_inside = m_dynamic_params.drug_newValue*inv_pxSquared;
			}
			break;

			case DTRAP_DRUGPARAM :
			{
				D_trapped = m_dynamic_params.drug_newValue*inv_pxSquared;
			}
			break;

			case KON_DRUGPARAM :
			{
				kon = m_dynamic_params.drug_newValue;
			}
			break;

			case KOFF_DRUGPARAM :
			{
				koff = m_dynamic_params.drug_newValue;
			}
			break;

			case POROS_DRUGPARAM :
			{
				crossingProbability_outIn = m_dynamic_params.drug_newValue;
			}
			break;
		}
	}

	m_bioWorld->setFixation(isFixed);
	m_bioWorld->setD(0,0, D_outside);
	m_bioWorld->setCrossing(0,0,0,1.0);

	int nb_rgns = m_bioWorld->getNbRegions();
	for(int rgn_idx = 1; rgn_idx <= nb_rgns-1; rgn_idx++)
	{
		m_bioWorld->setD(rgn_idx,0,D_inside);
		m_bioWorld->setTrappingAbundant(rgn_idx,0,D_trapped,
										   kon,
										   koff);
		m_bioWorld->setCrossing(rgn_idx,0,1,crossingProbability_outIn);
	}

	m_bioWorld->setImmobileFraction(0, immobile_fraction);
	m_engine->updateSelectedMode();
}

void FluoSimModel::updatePhotoPhysicsParams()
{
	if(m_bioWorld == 0) return; //->

	m_bioWorld->getFluoSpecieAdr(0)->setKon(m_photoPhysics_params.k_on_fluo);
	m_bioWorld->getFluoSpecieAdr(0)->setKoff(m_photoPhysics_params.k_off_fluo);
	m_engine->updateSelectedMode();
}

void FluoSimModel::updateSimulationParams()
{
	if(m_bioWorld == 0) return; //->

	updateDynamicParams();
	updateRenderingParams();
	updateExperimentParams();
}

void FluoSimModel::updateExperimentParams()
{
//Probes
    for(Probe* &probe : m_experimental_probes_v)
	{
        delete probe;
        probe = 0;
	}
	m_experimental_probes_v.clear();

	switch (m_experiment_params.experimentType)
	{
		case SPT_EXPERIMENT :
		{
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
				int rgn_idx = m_bioWorld->getRegionIdx(rgnName);
				if(rgn_idx == -1) continue; //

				Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));

				//probe1 trajectories
				auto probe1 = new Probe(m_bioWorld);
				probe1->setRegion1(rgn);
				probe1->setRegion2(rgn);
				probe1->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
				probe1->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));
				probe1->setMeasureType(Probe::TRACE_TRACKER);
				m_experimental_probes_v.push_back(probe1);

				//probe2 : localisations
				//we should check that the stack exportation is enabled
				auto probe2 = new Probe(m_bioWorld);
				probe2->setRegion1(rgn);
				probe2->setRegion2(rgn);
				probe2->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
				probe2->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));
				probe2->setMeasureType(Probe::LOCALISATION);
				m_experimental_probes_v.push_back(probe2);
			}
		}
		break;

		case FRAP_EXPERIMENT :
		{
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
                auto probe1 = new Probe(m_bioWorld);
				int rgn_idx = m_bioWorld->getRegionIdx(rgnName);

				if(rgn_idx == -1) continue; //
				Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
                probe1->setRegion1(rgn);
                probe1->setRegion2(rgn);
                probe1->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
                probe1->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));
                probe1->setMeasureType(Probe::AVERAGE_INTENSITY);
                m_experimental_probes_v.push_back(probe1);
			}
		}
		break;

        case PAF_EXPERIMENT :
        {
            for(string& rgnName : m_experiment_params.measuredRegionNames_v)
            {
                auto probe1 = new Probe(m_bioWorld);
                int rgn_idx = m_bioWorld->getRegionIdx(rgnName);

                if(rgn_idx == -1) continue; //
                Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
                probe1->setRegion1(rgn);
                probe1->setRegion2(rgn);
                probe1->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
                probe1->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));
                probe1->setMeasureType(Probe::AVERAGE_INTENSITY);
                m_experimental_probes_v.push_back(probe1);
            }
        }
        break;

		case FCS_EXPERIMENT :
		{
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
                auto probe1 = new Probe(m_bioWorld);
				int rgn_idx = m_bioWorld->getRegionIdx(rgnName);

				if(rgn_idx == -1) continue; //
				Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
                probe1->setRegion1(rgn);
                probe1->setRegion2(rgn);
                probe1->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
                probe1->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));

				if(m_experiment_params.is_fcsUsingGaussianBeam == true)
				{
                    probe1->setMeasureType(Probe::AVERAGE_INTENSITY_IN_GAUSSIAN_BEAM);
					myGaussianBeamParams beam_param;
						beam_param.center = rgn->getBarycenter();
						beam_param.maxIntensity = m_experiment_params.fcs_maxIntensity;
						//since fcs_beamSigma is in µm and the proba beamSigma is in px, a conversion is needed
						beam_param.sigma = m_experiment_params.fcs_beamSigma/m_simulation_params.pixel_size;
						beam_param.noise_cutOff = 0.0f;
						beam_param.koff = m_experiment_params.fcs_beamKoff;
                    probe1->setGaussianBeamParam(beam_param);
				}
				else
				{
                    probe1->setMeasureType(Probe::AVERAGE_INTENSITY);
				}

                m_experimental_probes_v.push_back(probe1);
			}
		}
		break;

		case DRUG_EXPERIMENT :
		{
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
                auto probe1 = new Probe(m_bioWorld);
				int rgn_idx = m_bioWorld->getRegionIdx(rgnName);

				if(rgn_idx == -1) continue; //
				Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
                probe1->setRegion1(rgn);
                probe1->setRegion2(rgn);
                probe1->setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
                probe1->setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));
                probe1->setMeasureType(Probe::AVERAGE_INTENSITY);
                m_experimental_probes_v.push_back(probe1);
			}
		}
		break;
	}

//FRAP HEADS
	m_experimental_frapHead_v.clear();

	for(string& bleached_rgnName : m_experiment_params.bleachedRegionNames_v)
	{
		int rgn_idx = m_bioWorld->getRegionIdx(bleached_rgnName);
		if(rgn_idx == -1) continue; //<
		Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));
        FrapHead frapHead(rgn,
							m_bioWorld->getFluoSpecieAdr(0),
							m_bioWorld);

		m_experimental_frapHead_v.push_back(frapHead);
	}
}


void FluoSimModel::updateLiveExperimentParams()
{
    if(m_bioWorld == 0) return;

    //measure
	int rgn_idx = m_bioWorld->getRegionIdx(m_liveExperiment_params.measured_RgnName);
    if(rgn_idx != -1) {
        m_probe.setBiologicalWorld(m_bioWorld);
        m_probe.setChemicalSpecie1(m_bioWorld->getSpecieAdr(0));
        m_probe.setChemicalSpecie2(m_bioWorld->getSpecieAdr(0));

        m_probe.setRegion1(&(m_bioWorld->getRegionRef(rgn_idx)));
        m_probe.setRegion2(&(m_bioWorld->getRegionRef(rgn_idx)));

        m_probe.setMeasureType(Probe::AVERAGE_INTENSITY);
        m_probe.resetProbeMeasure();
        m_probe.resetProbeMeasure();

        m_graphic->clearSignals();
        m_graphic->addSignal(&m_probe.getSignalRef());

        m_signal.setColor(glm::vec4(0,1,0,1));
        m_graphic->addSignal(&m_signal);

        renderGraphicCurves();
    }

    //frap
    rgn_idx = m_bioWorld->getRegionIdx(m_liveExperiment_params.measured_RgnName);
    if(rgn_idx != -1) {
        if(m_frapHead != 0) delete m_frapHead;
        m_frapHead = new FrapHead(&(m_bioWorld->getRegionRef(rgn_idx)), m_bioWorld->getFluoSpecieAdr(0), m_bioWorld);
    }
}

void FluoSimModel::updateRenderingParams()
{
// Image Intensity
	if(m_backgroundImage != 0)
	{
		m_backgroundImage->setIntensity(m_current_rendering_params->image_intensity);
		if(m_current_rendering_params->isBackgroundImageSmoothed == true)
		{
			m_backgroundImage->getTexture().setMinFilter(GL_LINEAR);
			m_backgroundImage->getTexture().setMagFilter(GL_LINEAR);
		}
		else
		{
			m_backgroundImage->getTexture().setMinFilter(GL_NEAREST);
			m_backgroundImage->getTexture().setMagFilter(GL_NEAREST);
		}
	}

// highlighted Regions Color
	list<int> highlightedRgn_idxs;
	for(auto highlightedRgn_name : m_current_rendering_params->highlightedRegion_names)
	{
		int highlightedRgn_idx = m_bioWorld->getRegionIdx(highlightedRgn_name);
		highlightedRgn_idxs.push_back(highlightedRgn_idx);
	}

	int nb_rgn = m_bioWorld->getNbRegions();
	for(int rgn_idx = 0; rgn_idx <= nb_rgn-1; rgn_idx++)
	{
        Region& rgn = m_bioWorld->getRegionRef(rgn_idx);
        if(std::find(highlightedRgn_idxs.begin(), highlightedRgn_idxs.end(), rgn_idx) == highlightedRgn_idxs.end())
		{
            rgn.setIsHighlighted(false);
            if(rgn_idx == 0) //cellular contour
                rgn.setColor({0,0,1,1});
            else if(rgn.isACompartment(m_bioWorld->getSpecieAdr(0)) == true) //compartment
                rgn.setColor({1,0,0,1});
            else rgn.setColor({0,0,1,1}); //measure region
        }
		else
		{
            rgn.setIsHighlighted(true);
            rgn.setHighlightedColor({0,1,0,0});
		}
	}

// Background color
	m_scrn.getRenderWindow()->setClearColor(m_current_rendering_params->clear_color);

// Camera Offsets
    m_scrn.setCameraPrePoissonOffset(m_current_rendering_params->photonBackground*m_simulation_params.dt_sim);
    m_scrn.setCameraPostGainOffset(m_current_rendering_params->cameraOffset);

// Camera Poisson Noise
    m_scrn.setCameraIsUsingPoissonNoise(m_current_rendering_params->isUsingPoissonNoise);
    m_scrn.setCameraReadoutNoiseSigma(m_current_rendering_params->readoutNoise_sigma);

// Camera Gain
    m_scrn.setCameraGain(m_current_rendering_params->ADCounts_perPhoton);
    m_scrn.setCameraIsBypassingPoissonAndNoise(m_current_rendering_params->isSpotIntensity_inPhotonsPerSec == false);

// Camera Definition
	if(m_backgroundImage != 0)
	{
		if(m_cameraImage == 0)
		{
			m_cameraImage = new myGLImage(m_backgroundImage->getRect().getBottomLeft(),
										  m_backgroundImage->getRect().getTopRight(),
										  m_textureRect_coords_gMV);
		}

		m_scrn.getRenderWindow()->makeCurrent();
		m_cameraImage->getTexture().clearTexture();
		m_cameraImage->loadImage(m_scrn.getCameraTexture());
		m_cameraImage->getRect().setDiag(m_backgroundImage->getRect().getBottomLeft(),
										 m_backgroundImage->getRect().getTopRight());
		m_cameraImage->setIsYInverted(false);
		m_cameraImage->show();
		m_cameraImage->myRenderableObject::setRenderingMode(myRenderableObject::IMAGE_GS_R16_RENDERING);

		glm::vec2 bottomLeft = m_backgroundImage->getRect().getBottomLeft();
		glm::vec2 topRight = m_backgroundImage->getRect().getTopRight();
		glm::vec2 diag = topRight - bottomLeft;

		vec2 size = m_experiment_params.SRI_zoom*diag;
		m_scrn.setCameraDefinition(size);
		m_scrn.setCameraField(bottomLeft,
							  topRight);
	}


// Spot Properties
	m_scrn.getRenderWindow()->makeCurrent();

    m_scrn.setPointIntensity(m_current_rendering_params->point_intensity);
    m_scrn.setPointSize(m_current_rendering_params->point_size);

    if(m_current_rendering_params->isSpotIntensity_inPhotonsPerSec == true) \
        m_scrn.setSpotIntensity(m_current_rendering_params->spot_intensity*m_simulation_params.dt_sim/65535); //normalized number of photons
    else \
        m_scrn.setSpotIntensity(m_current_rendering_params->spot_intensity/65535);
	m_scrn.setSpotSize(vec2(2*m_current_rendering_params->spot_size/(m_simulation_params.pixel_size*0.2),
							2*m_current_rendering_params->spot_size/(m_simulation_params.pixel_size*0.2)));
    m_scrn.setIsAutoscale(m_rendering_params.isAutoscale);
    m_scrn.setAutoscaleFactor(m_rendering_params.autoscale_factor);

// Particles Color
	if(m_bioWorld == 0) return; //->

	switch(m_current_rendering_params->particle_color)
	{
		case PARTICLE_RED :
		{
			m_bioWorld->setParticleColorMode(Particle::SPECIES_COLOR);
			m_bioWorld->getSpecieAdr(0)->setColor({1,0,0,1});
		}
		break;
		case PARTICLE_GREEN :
		{
			m_bioWorld->setParticleColorMode(Particle::SPECIES_COLOR);
			m_bioWorld->getSpecieAdr(0)->setColor({0,1,0,1});
		}
		break;
		case PARTICLE_BLUE :
		{
			m_bioWorld->setParticleColorMode(Particle::SPECIES_COLOR);
			m_bioWorld->getSpecieAdr(0)->setColor({0,0,1,1});
		}
		break;
		case PARTICLE_WHITE :
		{
			m_bioWorld->setParticleColorMode(Particle::SPECIES_COLOR);
			m_bioWorld->getSpecieAdr(0)->setColor({1,1,1,1});
		}
		break;
		case PARTICLE_LOOKUP_TABLE :
		{
			m_bioWorld->setParticleColorMode(Particle::SPECIES_COLOR);
			m_bioWorld->getSpecieAdr(0)->setColor({1,1,1,1});
		}
		break;

		case PARTICLE_RANDOMCOLOR :
		{
			m_bioWorld->setParticleColorMode(Particle::UNIQUE_COLOR);
		}
		break;

		case PARTICLE_TRAPPING_STATE_COLOR :
		{
			m_bioWorld->setParticleColorMode(Particle::TRAPPING_STATE_COLOR);
		}
		break;
	}
}

void FluoSimModel::updateGLWords()
{
	vec2 scrn_size = m_scrn.getRenderWindow()->size();

//	string fullLoop_time_str = "CURRENT_TIME_:";
//	fullLoop_time_str += floatToString(m_simulation_params.current_time);
//	fullLoop_time_str += 's';
//	m_fullLoopTime_glWord.setText(fullLoop_time_str);
//	m_fullLoopTime_glWord.setDirection(glm::vec2(1,0));
//	m_fullLoopTime_glWord.setColor(glm::vec4(0,1,0,1));
//	m_fullLoopTime_glWord.setLetterSizePxl(7);
//	m_fullLoopTime_glWord.setPositionPxl(vec2(10, scrn_size.y - 20));

	string m_simulationFPS_str = "FPS:";
	if(m_simulation_states.simulationStarted == true &&
	   m_simulation_states.simulationPaused == false)
	{
		m_simulationFPS_str += floatToString(int(1.0f/m_fullLoop_clock2.getAveragedTimeInTours()));
	}
	else m_simulationFPS_str += "-1";
	m_simulationFPS_glWord.setText(m_simulationFPS_str);
	m_simulationFPS_glWord.setDirection(glm::vec2(1,0));
	m_simulationFPS_glWord.setColor(glm::vec4(1,240.0f/188.0f,23.0f/255.0f,1));
	m_simulationFPS_glWord.setLetterSizePxl(8);
	m_simulationFPS_glWord.setPositionPxl(vec2(10, scrn_size.y - 20));

	string m_measuringTime_str = "NB_THREADS:";
	m_measuringTime_str += to_string(m_engine->getNbThreads());
	m_nbThreads_glWord.setText(m_measuringTime_str);
	m_nbThreads_glWord.setDirection(glm::vec2(1,0));
	m_nbThreads_glWord.setColor(glm::vec4(0,1,0,1));
	m_nbThreads_glWord.setLetterSizePxl(7);
	m_nbThreads_glWord.setPositionPxl(m_simulationFPS_glWord.getPositionPxl()-vec2(0,10+10));

	string renderingTime_str = "SINGLE THREAD:";
	renderingTime_str+= floatToString(m_engine->getSingeThreadTime());
	renderingTime_str += 's';
	m_singleThreadedTime_glWord.setText(renderingTime_str);
	m_singleThreadedTime_glWord.setDirection(glm::vec2(1,0));
	m_singleThreadedTime_glWord.setColor(glm::vec4(0,1,0,1));
	m_singleThreadedTime_glWord.setLetterSizePxl(7);
	m_singleThreadedTime_glWord.setPositionPxl(m_nbThreads_glWord.getPositionPxl()-vec2(0,10+5));

	string updatingBioWorldTime_str = "MULTI THREAD_TIME:";
	updatingBioWorldTime_str += floatToString(m_engine->getMultiThreadTime());
	updatingBioWorldTime_str += 's';
	m_multiThreadedTime_glWord.setText(updatingBioWorldTime_str);
	m_multiThreadedTime_glWord.setDirection(glm::vec2(1,0));
	m_multiThreadedTime_glWord.setColor(glm::vec4(0,1,0,1));
	m_multiThreadedTime_glWord.setLetterSizePxl(7);
	m_multiThreadedTime_glWord.setPositionPxl(m_singleThreadedTime_glWord.getPositionPxl()-vec2(0,10+5));



//	string m_diffTime_str = "____DIFF_TIME_:";
//	m_diffTime_str += floatToString(m_engine->getAverageDiffTime());
//	m_diffTime_str += 's';
//	m_diffTime_glWord.setText(m_diffTime_str);
//	m_diffTime_glWord.setDirection(glm::vec2(1,0));
//	m_diffTime_glWord.setColor(glm::vec4(0,1,0,1));
//	m_diffTime_glWord.setLetterSizePxl(7);
//	m_diffTime_glWord.setPositionPxl(m_updatingBioWorldTime_glWord.getPositionPxl()-vec2(0,10+5));

//	string m_trappTime_str = "____TRAPPING_TIME_:";
//	m_trappTime_str += floatToString(m_engine->getAverageTrappingTime());
//	m_trappTime_str += 's';
//	m_trappTime_glWord.setText(m_trappTime_str);
//	m_trappTime_glWord.setDirection(glm::vec2(1,0));
//	m_trappTime_glWord.setColor(glm::vec4(0,1,0,1));
//	m_trappTime_glWord.setLetterSizePxl(7);
//	m_trappTime_glWord.setPositionPxl(m_diffTime_glWord.getPositionPxl()-vec2(0,10+5));

//	string m_photoTime_str = "____PHOTOPHYSICS_TIME_:";
//	m_photoTime_str += floatToString(m_engine->getAveragetPhotoPhysicsTime());
//	m_photoTime_str += 's';
//	m_photoTime_glWord.setText(m_photoTime_str);
//	m_photoTime_glWord.setDirection(glm::vec2(1,0));
//	m_photoTime_glWord.setColor(glm::vec4(0,1,0,1));
//	m_photoTime_glWord.setLetterSizePxl(7);
//	m_photoTime_glWord.setPositionPxl(m_trappTime_glWord.getPositionPxl()-vec2(0,10+5));

}



