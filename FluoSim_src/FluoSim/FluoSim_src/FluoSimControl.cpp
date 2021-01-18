
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

int getRegionIdx(QString rgn_name)
{
	QString index_qstr = rgn_name.remove(0, 6);
	int idx = index_qstr.toInt();
	return idx;
}






FluoSim::FluoSim(QApplication* main_app) :

	FluoSimModel(main_app)
{
	initializeBioWorld();
	connect(&m_scrn, SIGNAL(rgnHasBeenDrawn_signal(std::vector<glm::vec2>)),
			this, SLOT(rgnHasBeenDrawn_slot(std::vector<glm::vec2>)));

	setViewsFromModel();

	m_tracePlayer.setProgressWdgt(&m_progressWdgt);
	m_tracePlayer.setMainWindow(&m_mainWindow);
}

void FluoSim::loadGeometry(string path)
{
	FluoSimModel::loadGeometry(path);
	setViewsFromModel();
}

void FluoSim::loadProject(string project_path)
{
	if(project_path.size() == 0)
	{
		vector<pair<string, string> > filteredTypes { {"FluoSim Projects Files", "pro"} };
		bool isFile_selected =  getFileLocationFiltered(project_path, filteredTypes, "", &m_mainWindow);

		if(isFile_selected == false) return; //->
	}

	FluoSimModel::loadProject(project_path);
    setViewsFromModel();
	FluoSimModel::evaluateSteadyState();
}

void FluoSim::saveProject(string project_path)
{
	if(project_path.size() == 0)
	{
		vector<pair<string, string> > filteredTypes { {"FluoSim Project Files", "pro"} };
		bool file_selected = getSaveLocationFiltered(project_path, filteredTypes,"", "", &m_mainWindow);
		if(file_selected == false) return; //->
	}

	FluoSimModel::saveProject(project_path);
}

void FluoSim::renderBioWorld()
{
	FluoSimModel::renderBioWorld();
}

void FluoSim::renderBioWorldCamera()
{
	FluoSimModel::renderBioWorldCamera();
}

void FluoSim::updateBioWorld()
{
//model
	FluoSimModel::updateBioWorld();

//views
	updateSimulationProgressViews();
}

void FluoSim::measureBioWorld()
{
	FluoSimModel::measureBioWorld();
}

void FluoSim::renderGraphicCurves()
{
	FluoSimModel::renderGraphicCurves();
}

void FluoSim::startSimulation()
{
//views
	if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
	{
		FluoSimView::enableExperimentParameters(false);
	}

//model
	FluoSimModel::startSimulation();
}

void FluoSim::pauseSimulation()
{
//views


//model
	FluoSimModel::pauseSimulation();
}

void FluoSim::stopSimulation()
{
//model
	FluoSimModel::stopSimulation();
//views
	if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
	{
		FluoSimView::enableExperimentParameters(true);
	}
	updateSimulationProgressViews();
}

void FluoSim::setSimulatorMode(SIMULATOR_MODE simulator_mode)
{
//model
	FluoSimModel::setSimulatorMode(simulator_mode);

//views
	if(simulator_mode == GEOMETRY_MODE || simulator_mode == LIVE_MODE ||
	   simulator_mode == EXPERIMENT_MODE)
	{
		m_centralLayout.removeWidget(m_tracePlayer.getPlaneSlider());
		m_tracePlayer.getPlaneSlider()->hide();

		m_ui_VLayout.removeWidget(&m_parameters_wdgt);
		m_ui_VLayout.removeWidget(m_tracePlayer.getParamtersWidget());
		m_ui_VLayout.addWidget(&m_parameters_wdgt);

		m_parameters_wdgt.show();
		m_tracePlayer.getParamtersWidget()->hide();
	}
	else //ANALYSIS_MODE
	{
		m_centralLayout.addWidget(m_tracePlayer.getPlaneSlider());
		m_tracePlayer.getPlaneSlider()->show();

		m_ui_VLayout.removeWidget(&m_parameters_wdgt);
		m_ui_VLayout.removeWidget(m_tracePlayer.getParamtersWidget());
		m_ui_VLayout.addWidget(m_tracePlayer.getParamtersWidget());

		m_parameters_wdgt.hide();
		m_tracePlayer.getParamtersWidget()->show();
	}

	map<bool,string> state_styleSheets;
	state_styleSheets[true] = string("QPushButton {background : rgb(72,72,72); color :rgb(240,188,23);\n}"
									 "QPushButton {font-weight : bold;}");
	state_styleSheets[false] = string("QPushButton {background : rgb(49,49,49); color: rgb(139,139,139);\n}"
									  "QPushButton:hover {background : rgb(49,49,49); color: rgb(220,220,220);\n}");
	map<QPushButton*, bool> ongletButtons_state;
	ongletButtons_state[&m_geometryOnglet_pushButton] = (simulator_mode == GEOMETRY_MODE);
	ongletButtons_state[&m_liveOnglet_pushButton] = (simulator_mode == LIVE_MODE);
	ongletButtons_state[&m_experimentOnglet_pushButton] = (simulator_mode == EXPERIMENT_MODE);
	ongletButtons_state[&m_analysisOnglet_pushButton] = (simulator_mode == ANALYSIS_MODE);

	for(auto& onglet : ongletButtons_state)
	{
		onglet.first->setStyleSheet(state_styleSheets[onglet.second].data());
	}

	EXPERIMENT_TYPE exp_type = m_experiment_params.experimentType;
	bool bckgdImage_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool simulation_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool allRegions_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool simulationStates_dropMenu_visible = (simulator_mode == LIVE_MODE || simulator_mode == EXPERIMENT_MODE);
	bool particleSystem_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool dynamic_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool PhotoPhysics_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool rendering_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool liveExperiment_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool wProtExperiment_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE);
	bool FRAPExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == FRAP_EXPERIMENT);
	bool PAFExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == PAF_EXPERIMENT);
	bool FCSExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == FCS_EXPERIMENT);
	bool SRIExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == SRI_EXPERIMENT);
	bool drugExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == DRUG_EXPERIMENT);
	bool fileDestination_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE);
		bool exportStack_visibility = m_experiment_params.experimentType != SRI_EXPERIMENT;
	bool wProtMeasuredRegions_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && !(exp_type == SRI_EXPERIMENT);
	bool wProtBleachedRegions_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && ((exp_type == FRAP_EXPERIMENT) ||
												  (exp_type == PAF_EXPERIMENT));


	m_backgroundImage_dropMenu.setRolled(!bckgdImage_dropMenu_visible);
	m_backgroundImage_dropMenu.setVisible(bckgdImage_dropMenu_visible);

	m_simulation_dropMenu.setRolled(!simulation_dropMenu_visible);
	m_simulation_dropMenu.setVisible(simulation_dropMenu_visible);

	m_allRegions_dropMenu.setRolled(!allRegions_dropMenu_visible);
	m_allRegions_dropMenu.setVisible(allRegions_dropMenu_visible);
    m_allRegions_tableWidget.clearSelection(); //it also resets the highlighted regions thx to signals being triggered during the process

	m_simulationStates_dropMenu.setRolled(!simulationStates_dropMenu_visible);
	m_simulationStates_dropMenu.setVisible(simulationStates_dropMenu_visible);

	if(simulator_mode == LIVE_MODE)
	{
		m_simulationMaxPlane_dSpinBx.setEnabled(true);
		connect(&m_simulationMaxPlane_dSpinBx, SIGNAL(valueChanged(double)), this , SLOT(NplanesGetChanged(double)));
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes);

		m_simulation_progressBar.setTextVisible(false);
		enableSimulationStateButtons(true);
        m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(240,188,23);width:1px;\n}");
	}

	if(simulator_mode == EXPERIMENT_MODE)
	{
		m_simulationMaxPlane_dSpinBx.setEnabled(false);
		disconnect(&m_simulationMaxPlane_dSpinBx, SIGNAL(valueChanged(double)), this , SLOT(NplanesGetChanged(double)));
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes + m_experiment_params.N_presequ);

		if((EXPERIMENT_TYPE)m_experiment_params.experimentType == SRI_EXPERIMENT)
		{
			if(m_backgroundImage == 0)
			{
				m_simulation_progressBar.setTextVisible(true);
				m_simulation_progressBar.setStyleSheet("QProgressBar {text-align : center; color : rgb(255,0,0); background-color : rgb(49,49,49);}"
                                                       "QProgressBar::chunk {background-color : rgb(49,49,49); color : rgb(255,0,0); width:1px;}"
													   "QProgressBar {font-weight : bold;}");
				m_simulation_progressBar.setFormat("- No Background Image -");
				enableSimulationStateButtons(false);
			}
			else
			{
                m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(200,0,0);width:1px;\n}");
			}
		}
		else
		{
			enableSimulationStateButtons(true);
			m_simulation_progressBar.setTextVisible(false);
            m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(200,0,0);width:1px;\n}");
		}
	}

	m_particleSystem_dropMenu.setRolled(!particleSystem_dropMenu_visible);
	m_particleSystem_dropMenu.setVisible(particleSystem_dropMenu_visible);

	m_dynamic_dropMenu.setRolled(!dynamic_dropMenu_visible);
	m_dynamic_dropMenu.setVisible(dynamic_dropMenu_visible);

	m_PhotoPhysics_dropMenu.setRolled(!PhotoPhysics_dropMenu_visible);
	m_PhotoPhysics_dropMenu.setVisible(PhotoPhysics_dropMenu_visible);

	m_rendering_dropMenu.setRolled(!rendering_dropMenu_visible);
	m_rendering_dropMenu.setVisible(rendering_dropMenu_visible);

	m_liveExperiment_dropMenu.setRolled(!liveExperiment_dropMenu_visible);
	m_liveExperiment_dropMenu.setVisible(liveExperiment_dropMenu_visible);

	m_wProtocoleExperiment_dropMenu.setRolled(!wProtExperiment_dropMenu_visible);
	m_wProtocoleExperiment_dropMenu.setVisible(wProtExperiment_dropMenu_visible);

	m_wProtocoleExperimentMeasuredRegions_dropMenu.setRolled(!wProtExperiment_dropMenu_visible);
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setVisible(wProtExperiment_dropMenu_visible);

	m_FRAPExperimentParameters_dropMenu.setRolled(!FRAPExpParam_dropMenu_visible);
	m_FRAPExperimentParameters_dropMenu.setVisible(FRAPExpParam_dropMenu_visible);

	m_PAFExperimentParameters_dropMenu.setRolled(!PAFExpParam_dropMenu_visible);
	m_PAFExperimentParameters_dropMenu.setVisible(PAFExpParam_dropMenu_visible);

	m_FCSExperimentParameters_dropMenu.setRolled(!FCSExpParam_dropMenu_visible);
	m_FCSExperimentParameters_dropMenu.setVisible(FCSExpParam_dropMenu_visible);

	m_SRIParameters_dropMenu.setRolled(!SRIExpParam_dropMenu_visible);
	m_SRIParameters_dropMenu.setVisible(SRIExpParam_dropMenu_visible);

	m_drugExperimentParameters_dropMenu.setRolled(!drugExpParam_dropMenu_visible);
	m_drugExperimentParameters_dropMenu.setVisible(drugExpParam_dropMenu_visible);

	m_fileDestination_dropMenu.setRolled(!fileDestination_dropMenu_visible);
	m_fileDestination_dropMenu.setVisible(fileDestination_dropMenu_visible);
        FluoSimView::showExportStackParameters(exportStack_visibility);

	m_wProtocoleExperimentMeasuredRegions_dropMenu.setRolled(!wProtMeasuredRegions_dropMenu_visible);
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setVisible(wProtMeasuredRegions_dropMenu_visible);

	m_wProtocoleExperimentBleachedRegions_dropMenu.setRolled(!wProtBleachedRegions_dropMenu_visible);
	m_wProtocoleExperimentBleachedRegions_dropMenu.setVisible(wProtBleachedRegions_dropMenu_visible);

	m_ui_dockWdgt.updateGeometry();
}

// COMMUNICATION FUNCTIONS

void FluoSim::sliderValueGetChanged(SLIDER_ID slider_id, float value)
{
	switch(slider_id)
	{
		case imageIntensity_slider :
		{
			m_rendering_params.image_intensity = value;
			updateRenderingParams();
		}
		break;

		case insideD_slider :
		{
            if(m_insideD_dSpinBxedSlider.getComboBoxIndex() == DIFFUSION_UNIT_COMBOBOX_PERCENTAGE) \
                m_dynamic_params.D_insideContact = value/100.0f*m_dynamic_params.D_outsideContact; //value is the Diffusion ratio in %
            else if (m_insideD_dSpinBxedSlider.getComboBoxIndex() == DIFFUSION_UNIT_COMBOBOX_UM2PERSEC) \
                m_dynamic_params.D_insideContact = value; //value is the Diffusion inside

			updateDynamicParams();
		}
		break;

		case outsideD_slider :
		{
			m_dynamic_params.D_outsideContact = value;
            if(m_insideD_dSpinBxedSlider.getComboBoxIndex() == DIFFUSION_UNIT_COMBOBOX_PERCENTAGE) \
                m_dynamic_params.D_insideContact = m_insideD_dSpinBxedSlider.value()/100.0f*m_dynamic_params.D_outsideContact; //value is the Diffusion ratio in %

			updateDynamicParams();

        }
		break;

		case trappedD_slider :
		{
			m_dynamic_params.D_trapped = value;
			updateDynamicParams();
		}
		break;

		case kon_slider :
		{
			m_dynamic_params.k_on = value;
			updateDynamicParams();
		}
		break;


		case koff_slider :
		{
			m_dynamic_params.k_off = value;
			updateDynamicParams();
		}
		break;

		case immobileFraction_slider :
		{
			m_dynamic_params.immobileFraction = value;
			updateDynamicParams();
		}
		break;

		case crossingProbability_slider :
		{
			m_dynamic_params.crossingProbability_outIn = value;
			updateDynamicParams();
		}
		break;

		case nbParticles_slider :
		{
			m_particleSystem_params.N_particles = value;
			updateParticleSystemParams();
		}
		break;

		case spotSize_slider :
		{
			m_rendering_params.spot_size = value;
			m_cameraRendering_params.spot_size = value;
			updateRenderingParams();
		}
		break;

		case spotIntensity_slider :
		{
            //we only store the value and we don't care about the unit, formatting will be performed in the updateRenderingParam() method.
            m_rendering_params.spot_intensity = value;
            m_cameraRendering_params.spot_intensity = value;

            updateRenderingParams();
		}
		break;

		case SRISpotSize_slider :
		{
			m_stormRendering_params.spot_size = value;
			updateRenderingParams();
        }
		break;

		case SRISpotIntensity_slider :
		{
//			m_stormRendering_params.spot_intensity = value;
//			updateRenderingParams();
		}
		break;

		case blinkingKon_slider :
		{
            m_photoPhysics_params.k_on_fluo = value;
			updatePhotoPhysicsParams();
		}
		break;

		case blinkingKoff_slider :
		{
            m_photoPhysics_params.k_off_fluo = value;
			updatePhotoPhysicsParams();
		}
		break;
	}
}

void FluoSim::actionGetTriggered(ACTION_ID action_id)
{
	switch(action_id)
	{
		case resetGeometry_action :
		{
			//model
			resetGeometry();
			//view
			setViewsFromModel();

		}
		break;

		case loadGeometry_action :
		{
			loadGeometry();
		}
		break;

		case saveGeometry_action :
		{
			saveGeometry();
		}
        break;

        case newProject_action :
        {
            stopSimulation();
            resetProject();
            setViewsFromModel();

        }
        break;

		case loadProject_action :
		{
			string path;
			loadProject(path);

		}
		break;

		case saveProject_action :
		{
			saveProject("");
		}
		break;

		case loadBackgroundImage_action :
		{
			//model		
			vector<pair<string, string> > filteredTypes;
				filteredTypes = {{"Image Files", "bmp jpg jpeg jpe  gif tif tiff png"} };

			string imageFile_path;
			bool file_selected = getFileLocationFiltered(imageFile_path, filteredTypes, "", &m_mainWindow);

			if(file_selected == false) return; //->


			bool isImageLoaded = loadBackgroundImage(imageFile_path);
			if(isImageLoaded == true)
			{
				//model
				updateRenderingParams();
				//view
				if(QDir(m_rendering_params.backgroundImage_path.data()).isRelative())
				{
					string lineEdit_text = string("\"FluoSimFolder\"/") + m_rendering_params.backgroundImage_path.data();
					m_loadBackgroundImage_lineEdit.setText(lineEdit_text.data());
				}
				else
				{
					m_loadBackgroundImage_lineEdit.setText(m_rendering_params.backgroundImage_path.data());
				}

                enableExportStackCheckBox(true);
                m_exportStack_noBackgroundImage_label.hide();
                enableExportStackParameters(m_experiment_params.isExportingStack);

			}
		}
		break;

		case captureScreen_action :
		{
			string file_path;
			vector<pair<string, string> > typeGroupName_filteredTypes_v;
			typeGroupName_filteredTypes_v.push_back({"BMP Image", "bmp"});
			bool isFile_selected = getSaveLocationFiltered(file_path, typeGroupName_filteredTypes_v,"screencapture.bmp", "SimulationOutputs", &m_mainWindow);

			if(isFile_selected == true) captureScreen(file_path);
		}
		break;

		case evaluateSteadyState_action :
		{
			evaluateSteadyState();
			updateRenderingParams();
		}
		break;

		case openUserManual_action :
		{
			QString appDir_path = m_main_app->applicationDirPath();
			QDesktopServices::openUrl(QUrl::fromLocalFile(appDir_path +
                                                          QString("/Resources/Documentation/FluoSim - UserManual.pdf")));
		}
		break;


		case openAbout_action :
		{
			displayAboutFluoSim();


		}
		break;

		case exit_action :
		{
			m_app_running = false;
		}
		break;

		case loadSPT_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/SPT.pro");

			loadProject(file_path.toLocal8Bit().data());

		}
		break;

		case loadFRAPIn_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/FRAP-in.pro");

			loadProject(file_path.toLocal8Bit().data());
		}
		break;

		case loadFRAPOut_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/FRAP-out.pro");

			loadProject(file_path.toLocal8Bit().data());
		}
		break;

		case loadFCSIn_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/FCS-in.pro");

			loadProject(file_path.toLocal8Bit().data());

		}
		break;

		case loadFCSOut_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/FCS-out.pro");

			loadProject(file_path.toLocal8Bit().data());

		}
		break;

		case loadSRI_action :
		{
			QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/SRI.pro");

			loadProject(file_path.toLocal8Bit().data());

		}
		break;

        case loadPAFIn_action :
        {
            QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/PAF-in.pro");

            loadProject(file_path.toLocal8Bit().data());
        }
        break;

        case loadPAFOut_action :
        {
            QString file_path = m_main_app->applicationDirPath();
            file_path += QString("/Resources/Examples/PAF-out.pro");

            loadProject(file_path.toLocal8Bit().data());
        }
        break;

		case showAndHideSimParametersWdgt_action :
		{
			//model
			bool isParameterVisible = m_showAndHideSimParametersWdgt_action.isChecked();
			m_simulation_states.isParameterWidgetVisible = isParameterVisible;

			//view
			m_ui_dockWdgt.setVisible(isParameterVisible);
		}
		break;

		case showAndHideGraphicWdgt_action :
		{
			//model
			bool isGraphicVisible = m_showAndHideGraphicWdgt_action.isChecked();
			m_simulation_states.isGraphicVisible = isGraphicVisible;

			//view
			m_graphic_dockWdgt.setVisible(isGraphicVisible);
			QPoint graphic_pos = m_centralWidget.mapToGlobal(QPoint(20,0+90));

			m_graphic_dockWdgt.setGeometry(graphic_pos.x(), graphic_pos.y(),250,150);

		}
		break;

		case drawRect_action :
		{
			m_scrn.drawRegion(RECTANGLE);
		}
		break;

		case drawCircle_action :
		{
			 m_scrn.drawRegion(CIRCLE);
		}
		break;

		case drawPolygon_action :
		{
			m_scrn.drawRegion(POLYGON);
		}
		break;
	}
}


void FluoSim::renderingModeGetChanged(int new_mode)
{
	switch(new_mode)
	{
		case RENDERING_COMBOBOX_POINTS :
		{
			m_rendering_params.rendering_mode = RENDERING_POINTS;
			FluoSimView::showGaussianParameters(false);

			QString old_text = m_particleColor_comboBx.currentText();
			m_particleColor_comboBx.clear();
				m_particleColor_comboBx.addItem("RED");
				m_particleColor_comboBx.addItem("GREEN");
				m_particleColor_comboBx.addItem("BLUE");
				m_particleColor_comboBx.addItem("WHITE");
				m_particleColor_comboBx.addItem("RANDOM");
				m_particleColor_comboBx.addItem("TRAPPING");

			if(old_text == "LOOKUP TABLE")
			{
				m_particleColor_comboBx.setCurrentText("WHITE");
				m_rendering_params.particle_color = PARTICLE_WHITE;
			}
			else
			{
				m_particleColor_comboBx.setCurrentText(old_text);
			}

			updateRenderingParams();
		}
		break;

		case RENDERING_COMBOBOX_GAUSSIANS :
		{
			m_rendering_params.rendering_mode = RENDERING_GAUSSIANS;
			FluoSimView::showGaussianParameters(true);

			int item_idx = m_particleColor_comboBx.currentIndex();
			m_particleColor_comboBx.clear();
				m_particleColor_comboBx.addItem("RED");
				m_particleColor_comboBx.addItem("GREEN");
				m_particleColor_comboBx.addItem("BLUE");
				m_particleColor_comboBx.addItem("WHITE");
				m_particleColor_comboBx.addItem("RANDOM");
				m_particleColor_comboBx.addItem("TRAPPING");
				m_particleColor_comboBx.addItem("LOOKUP TABLE");
			m_particleColor_comboBx.setCurrentIndex(item_idx);

			updateRenderingParams();
		}
		break;
	}
}

void FluoSim::SRIParticleColorGetChanged(int new_color)
{
	switch(new_color)
	{
		case PARTICLECOLOR_COMBOBOX_RED :
		{
			m_stormRendering_params.particle_color = PARTICLE_RED;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_GREEN :
		{
			m_stormRendering_params.particle_color = PARTICLE_GREEN;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_BLUE :
		{
			m_stormRendering_params.particle_color = PARTICLE_BLUE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_WHITE :
		{
			m_stormRendering_params.particle_color = PARTICLE_WHITE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_PSEUDOCOLOR :
		{
			m_stormRendering_params.particle_color = PARTICLE_LOOKUP_TABLE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_RANDOMCOLOR :
		{
			m_stormRendering_params.particle_color = PARTICLE_RANDOMCOLOR;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_TRAPPINGSTATECOLOR :
		{
			m_stormRendering_params.particle_color = PARTICLE_TRAPPING_STATE_COLOR;
			updateRenderingParams();
		}
		break;
	}
}

void FluoSim::particleColorGetChanged(int new_color)
{
	switch(new_color)
	{
		case PARTICLECOLOR_COMBOBOX_RED :
		{
			m_rendering_params.particle_color = PARTICLE_RED;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_GREEN :
		{
			m_rendering_params.particle_color = PARTICLE_GREEN;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_BLUE :
		{
			m_rendering_params.particle_color = PARTICLE_BLUE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_WHITE :
		{
			m_rendering_params.particle_color = PARTICLE_WHITE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_PSEUDOCOLOR :
		{
			m_rendering_params.particle_color = PARTICLE_LOOKUP_TABLE;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_RANDOMCOLOR :
		{
			m_rendering_params.particle_color = PARTICLE_RANDOMCOLOR;
			updateRenderingParams();
		}
		break;

		case PARTICLECOLOR_COMBOBOX_TRAPPINGSTATECOLOR :
		{
			m_rendering_params.particle_color = PARTICLE_TRAPPING_STATE_COLOR;
			updateRenderingParams();
		}
		break;
	}
}


void FluoSim::pixelSizeGetChanged(double new_pixelSize)
{
	//model
	m_simulation_params.pixel_size = new_pixelSize;
	m_tracePlayer.setPixelSize(new_pixelSize);

	//model/api
    updateSimulationParams(); //from model->bioWorld!
}

void FluoSim::timeStepGetChanged(double new_timeStep)
{
	//view
    disconnect(&m_timelapse_period_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(acquisitionPeriodGetChanged(double)));

    double timelapse_period = m_experiment_params.acquisitionPeriod;
    m_timelapse_period_dSpinBox.setSingleStep(new_timeStep);
    m_timelapse_period_dSpinBox.setMinimum(new_timeStep);

    if(modf(timelapse_period/new_timeStep,0) <= 0.0001) m_timelapse_period_dSpinBox.setValue(timelapse_period);
    else {
        m_timelapse_period_dSpinBox.setValue(round(timelapse_period/new_timeStep)*new_timeStep);
    }

    connect(&m_timelapse_period_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(acquisitionPeriodGetChanged(double)));

	//model
	m_simulation_params.dt_sim = new_timeStep;
    m_experiment_params.acquisitionPeriod = m_timelapse_period_dSpinBox.value();

	//model/api
	updateSimulationParams();
    if(m_rendering_params.isSpotIntensity_inPhotonsPerSec == true) updateRenderingParams(); //since the spotIntensity has changed !
}

void FluoSim::onOngletGetChanged(int new_onglet)
{
	ONGLET_ID onglet = (ONGLET_ID) new_onglet;

	switch(onglet)
	{
		case GEOMETRY_ONGLET : setSimulatorMode(GEOMETRY_MODE);
		break;

		case LIVE_ONGLET : setSimulatorMode(LIVE_MODE);
		break;

		case EXPERIMENT_ONGLET : setSimulatorMode(EXPERIMENT_MODE);
		break;

		case ANALYSIS_ONGLET : setSimulatorMode(ANALYSIS_MODE);
		break;
	}
}

void FluoSim::deleteRegionAllRegionsGetClicked()
{
//model

	auto items_ql = m_allRegions_tableWidget.selectedItems();
	auto items_l = items_ql.toStdList();
	list<int> all_rows_l;
	list<int> rows_l;

	for(auto item : items_l) all_rows_l.push_back(item->row());
	for(auto row : all_rows_l)
	{
		//potentially more than one item per row...
		if(std::find(rows_l.begin(), rows_l.end(), row) == rows_l.end())
		{
			rows_l.push_back(row);
		}
	}

	vector<string> rgn_names_v;
	for(auto row : rows_l)
	{
		string rgn_name = m_allRegions_tableWidget.item(row, 0)->text().toLocal8Bit().data();
		rgn_names_v.push_back(rgn_name);
	}
	deleteGeometries(rgn_names_v);

//view
		setViewsFromModel();
}

void FluoSim::regionCellGetEntered(int row_entered, int column_entered)
{
//View

	auto items = m_allRegions_tableWidget.selectedItems();
	if(items.size() != 0) return;

	int nb_row = m_allRegions_tableWidget.rowCount();
	int nb_column = m_allRegions_tableWidget.columnCount();
	for(int row = 0; row <= nb_row-1; row++)
	{
		for(int column = 0; column <= nb_column-1; column++)
		{
			if(row == row_entered)
			{
				m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(100,100,100));
				m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,240,240));
			}
			else
			{
				m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(72,72,72));
				m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,240,240));
			}
		}
	}
//Model

	m_rendering_params.highlightedRegion_names.clear();
	m_rendering_params.highlightedRegion_names.push_back(m_allRegions_tableWidget.item(row_entered, 0)->text().toLocal8Bit().data());
	updateRenderingParams();
}

void FluoSim::regionCellSelectionGetChanged()
{
    cout<<"inside regionSelectionGetChanged()";
	auto items_ql = m_allRegions_tableWidget.selectedItems();
	auto items_l = items_ql.toStdList();
	list<int> itemRows_l;
	for(auto item : items_l)
	{
		itemRows_l.push_back(item->row());
	}

	//view

	int nb_row = m_allRegions_tableWidget.rowCount();
	int nb_column = m_allRegions_tableWidget.columnCount();
	for(int row = 0; row <= nb_row-1; row++)
	{
		for(int column = 0; column <= nb_column-1; column++)
		{
			if(std::find(itemRows_l.begin(), itemRows_l.end(), row) != itemRows_l.end())
			{
				m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(100,100,100));
				m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,188,23));
			}
			else
			{
				m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(72,72,72));
				m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,240,240));
			}
		}
	}

	//model

	m_rendering_params.highlightedRegion_names.clear();
	for(auto itemRow : itemRows_l)
	{
		string rgn_name = m_allRegions_tableWidget.item(itemRow, 0)->text().toLocal8Bit().data();
		m_rendering_params.highlightedRegion_names.push_back(rgn_name);
	}
	updateRenderingParams();

}

void FluoSim::regionCellGetDoubleClicked(int row, int column)
{
	auto item = m_allRegions_tableWidget.item(row, column);
	auto text = item->text();
	auto spc = m_bioWorld->getSpecieAdr(0);

//view
	if(column == 2 && row != 0)
	{
		if(text == QString("T")) item->setText("");
		else item->setText("T");
	}

//model
	string rgn_name = m_allRegions_tableWidget.item(row, 0)->text().toLocal8Bit().data();
	int rgn_idx =  m_bioWorld->getRegionIdx(rgn_name);
	if(rgn_idx < 0) return; //->
	auto rgn = &(m_bioWorld->getRegionRef(rgn_idx));

	if(column == 2 && row != 0)
	{
		if(text == QString("T")) m_bioWorld->setIsACompartment(rgn_idx, 0, false); //old_state was trapping...
		else m_bioWorld->setIsACompartment(rgn_idx, 0, true); //old_state was untrapping...
	}
}

void FluoSim::allRegionsTableGetLeft()
{
    //model
    if(
       m_allRegions_tableWidget.selectedItems().empty())
    {
        m_rendering_params.highlightedRegion_names.clear();
        updateRenderingParams();
    }
    //view
    auto items_l = m_allRegions_tableWidget.selectedItems().toStdList();
    list<int> itemRows_l;
    for(auto item : items_l)
        itemRows_l.push_back(item->row());

    int nb_row = m_allRegions_tableWidget.rowCount();
    int nb_column = m_allRegions_tableWidget.columnCount();
    for(int row = 0; row <= nb_row-1; row++)
    {
        for(int column = 0; column <= nb_column-1; column++)
        {
            if(std::find(itemRows_l.begin(), itemRows_l.end(), row) != itemRows_l.end())
            {
                m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(100,100,100));
                m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,188,23));
            }
            else
            {
                m_allRegions_tableWidget.item(row, column)->setBackgroundColor(QColor(72,72,72));
                m_allRegions_tableWidget.item(row, column)->setTextColor(QColor(240,240,240));
            }
        }
    }
}



void FluoSim::playButtonHasBeenClicked(bool)
{
	startSimulation();
}

void FluoSim::pauseButtonHasBeenClicked(bool)
{
	pauseSimulation();
}


void FluoSim::stopButtonHasBeenClicked(bool)
{
	stopSimulation();
}

void FluoSim::NplanesGetChanged(double new_value)
{
	//model
	m_experiment_params.N_planes = new_value;

	//view
	disconnect(&m_duration_spinBox, SIGNAL(valueChanged(int)), this, SLOT(acquisitionNPlanesGetChanged(int)));
	m_duration_spinBox.setValue(m_experiment_params.N_planes);
	connect(&m_duration_spinBox, SIGNAL(valueChanged(int)), this, SLOT(acquisitionNPlanesGetChanged(int)));
}

void FluoSim::fixationGetChanged(bool isFixated)
{
	//model
	m_dynamic_params.isFixed = isFixated;
	updateDynamicParams();

	//view
	enableDynamicParameters(!isFixated);
}

void FluoSim::DinsideUnitGetChanged(int new_unit)
{
    //view
    switch(m_insideD_dSpinBxedSlider.getComboBoxIndex())
    {
        case DIFFUSION_UNIT_COMBOBOX_PERCENTAGE :
        {
            float Dinside_ratio;
            if(m_dynamic_params.D_outsideContact == 0) Dinside_ratio = 1;
            else Dinside_ratio = m_dynamic_params.D_insideContact/m_dynamic_params.D_outsideContact;
            m_insideD_dSpinBxedSlider.setMin(0);
            m_insideD_dSpinBxedSlider.setMax(100);
            m_insideD_dSpinBxedSlider.setStep(0.1);
            m_insideD_dSpinBxedSlider.setDecimals(1);
            m_insideD_dSpinBxedSlider.setValue(Dinside_ratio*100);
        }
        break;

        case DIFFUSION_UNIT_COMBOBOX_UM2PERSEC :
        {
            float D_inside = m_dynamic_params.D_insideContact; //need to save the value before changing slider properties since an event could be fired in the process
            m_insideD_dSpinBxedSlider.setMin(m_DSlider_minValue);
            m_insideD_dSpinBxedSlider.setMax(m_DSlider_maxValue);
            m_insideD_dSpinBxedSlider.setStep(0.0001);
            m_insideD_dSpinBxedSlider.setDecimals(4);
            m_insideD_dSpinBxedSlider.setValue(D_inside);
        }
        break;
    }
}

void FluoSim::immobileFractionGetChanged(double new_value)
{
	//model
	m_dynamic_params.immobileFraction = new_value/100.0; //in % in the view
	updateDynamicParams();

	//view
}


void FluoSim::updateSimulationProgressViews()
{
	int value;
	switch(m_simulation_states.simulator_mode)
	{
		case LIVE_MODE :
		{
			//Progress Bar
			value = (float(m_simulation_params.current_plane)/
					 (m_experiment_params.N_planes))*100;
			m_simulation_progressBar.setValue(value);

			//Counter
            QString currentPlane_qstr;
				currentPlane_qstr = QString("%1 / ").arg(int(m_simulation_params.current_plane));
			m_simulationMaxPlane_dSpinBx.setPrefix(currentPlane_qstr);
		}
		break;

		case EXPERIMENT_MODE :
		{
			//Progress Bar
			value = (float(m_simulation_params.current_plane + m_experiment_params.N_presequ)/
					 (m_experiment_params.N_planes + m_experiment_params.N_presequ))*100;
			m_simulation_progressBar.setValue(value);

			//Counter
            QString currentPlane_qstr;
				currentPlane_qstr = QString("%1 / ").arg(int(m_simulation_params.current_plane + m_experiment_params.N_presequ));
			m_simulationMaxPlane_dSpinBx.setPrefix(currentPlane_qstr);

		}
		break;
	}
}


void FluoSim::setViewsFromModel()
{
	int value ;
	float f_value;

//SOFTWARE_STATES
	bool isParameterVisible = m_simulation_states.isParameterWidgetVisible;
		m_ui_dockWdgt.setVisible(isParameterVisible);
		m_showAndHideSimParametersWdgt_action.setChecked(isParameterVisible);

	bool isGraphicVisible = m_simulation_states.isGraphicVisible;
		m_graphic_dockWdgt.setVisible(isGraphicVisible);
		if(isGraphicVisible == true)
		{
			QPoint graphic_pos = m_centralWidget.mapToGlobal(QPoint(20,0+90));
			m_graphic_dockWdgt.setGeometry(graphic_pos.x(), graphic_pos.y(),250,150);
		}
		m_showAndHideGraphicWdgt_action.setChecked(isGraphicVisible);

//SIMULATION MODE
	SIMULATOR_MODE simulator_mode = m_simulation_states.simulator_mode;

	if(simulator_mode == GEOMETRY_MODE || simulator_mode == LIVE_MODE ||
	   simulator_mode == EXPERIMENT_MODE)
	{
		m_centralLayout.removeWidget(m_tracePlayer.getPlaneSlider());
		m_tracePlayer.getPlaneSlider()->hide();

		m_ui_VLayout.removeWidget(&m_parameters_wdgt);
		m_ui_VLayout.removeWidget(m_tracePlayer.getParamtersWidget());
		m_ui_VLayout.addWidget(&m_parameters_wdgt);

		m_parameters_wdgt.show();
		m_tracePlayer.getParamtersWidget()->hide();
	}
	else //ANALYSIS_MODE
	{
		m_centralLayout.addWidget(m_tracePlayer.getPlaneSlider());
		m_tracePlayer.getPlaneSlider()->show();

		m_ui_VLayout.removeWidget(&m_parameters_wdgt);
		m_ui_VLayout.removeWidget(m_tracePlayer.getParamtersWidget());
		m_ui_VLayout.addWidget(m_tracePlayer.getParamtersWidget());

		m_parameters_wdgt.hide();
		m_tracePlayer.getParamtersWidget()->show();
	}

	map<bool,string> state_styleSheets;
	state_styleSheets[true] = string("QPushButton {background : rgb(72,72,72); color :rgb(240,188,23);\n}"
									 "QPushButton {font-weight : bold;}");
	state_styleSheets[false] = string("QPushButton {background : rgb(49,49,49); color: rgb(139,139,139);\n}"
									  "QPushButton:hover {background : rgb(49,49,49); color: rgb(220,220,220);\n}");
	map<QPushButton*, bool> ongletButtons_state;
	ongletButtons_state[&m_geometryOnglet_pushButton] = (simulator_mode == GEOMETRY_MODE);
	ongletButtons_state[&m_liveOnglet_pushButton] = (simulator_mode == LIVE_MODE);
	ongletButtons_state[&m_experimentOnglet_pushButton] = (simulator_mode == EXPERIMENT_MODE);
	ongletButtons_state[&m_analysisOnglet_pushButton] = (simulator_mode == ANALYSIS_MODE);

	for(auto& onglet : ongletButtons_state)
	{
		onglet.first->setStyleSheet(state_styleSheets[onglet.second].data());
	}

	EXPERIMENT_TYPE exp_type = m_experiment_params.experimentType;
	bool sri_renderingMode_enable = (simulator_mode == EXPERIMENT_MODE) && (exp_type == SRI_EXPERIMENT);
	bool bckgdImage_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool simulation_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool allRegions_dropMenu_visible = (simulator_mode == GEOMETRY_MODE);
	bool simulationStates_dropMenu_visible = (simulator_mode == LIVE_MODE || simulator_mode == EXPERIMENT_MODE);
	bool particleSystem_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool dynamic_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool PhotoPhysics_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool rendering_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool liveExperiment_dropMenu_visible = (simulator_mode == LIVE_MODE);
	bool wProtExperiment_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE);
	bool FRAPExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == FRAP_EXPERIMENT);
	bool PAFExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == PAF_EXPERIMENT);
	bool FCSExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == FCS_EXPERIMENT);
	bool SRIExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == SRI_EXPERIMENT);
	bool drugExpParam_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && (exp_type == DRUG_EXPERIMENT);
	bool fileDestination_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE);
	bool wProtMeasuredRegions_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && !(exp_type == SRI_EXPERIMENT);
	bool wProtBleachedRegions_dropMenu_visible = (simulator_mode == EXPERIMENT_MODE) && ((exp_type == FRAP_EXPERIMENT) ||
												  (exp_type == PAF_EXPERIMENT));

	if(sri_renderingMode_enable == true) \
		FluoSimModel::setRenderingParams(& (FluoSimModel::m_stormRendering_params));
	else FluoSimModel::setRenderingParams(&m_rendering_params);

	m_backgroundImage_dropMenu.setRolled(!bckgdImage_dropMenu_visible);
	m_backgroundImage_dropMenu.setVisible(bckgdImage_dropMenu_visible);

	m_simulation_dropMenu.setRolled(!simulation_dropMenu_visible);
	m_simulation_dropMenu.setVisible(simulation_dropMenu_visible);

	m_allRegions_dropMenu.setRolled(!allRegions_dropMenu_visible);
	m_allRegions_dropMenu.setVisible(allRegions_dropMenu_visible);

	m_simulationStates_dropMenu.setRolled(!simulationStates_dropMenu_visible);
	m_simulationStates_dropMenu.setVisible(simulationStates_dropMenu_visible);

	if(simulator_mode == LIVE_MODE)
	{
		m_simulationMaxPlane_dSpinBx.setEnabled(true);
		connect(&m_simulationMaxPlane_dSpinBx, SIGNAL(valueChanged(double)), this , SLOT(NplanesGetChanged(double)));
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes);

		enableSimulationStateButtons(true);
		m_simulation_progressBar.setTextVisible(false);
        m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(240,188,23);width:1px;\n}");
	}

	if(simulator_mode == EXPERIMENT_MODE)
	{
		m_simulationMaxPlane_dSpinBx.setEnabled(false);
		disconnect(&m_simulationMaxPlane_dSpinBx, SIGNAL(valueChanged(double)), this , SLOT(NplanesGetChanged(double)));
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes + m_experiment_params.N_presequ);

		if((EXPERIMENT_TYPE)m_experiment_params.experimentType == SRI_EXPERIMENT)
		{
			if(m_backgroundImage == 0)
			{
				m_simulation_progressBar.setTextVisible(true);
				m_simulation_progressBar.setStyleSheet("QProgressBar {text-align : center; color : rgb(255,0,0); background-color : rgb(49,49,49);}"
                                                       "QProgressBar::chunk {background-color : rgb(49,49,49); color : rgb(255,0,0);width:1px;}"
													   "QProgressBar {font-weight : bold;}");
				m_simulation_progressBar.setFormat("- No Background Image -");
				enableSimulationStateButtons(false);
			}
			else
			{
                m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(200,0,0);width:1px;\n}");
			}
		}
		else
		{
			enableSimulationStateButtons(true);
			m_simulation_progressBar.setTextVisible(false);
            m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(200,0,0);width:1px;\n}");
		}
	}

	m_particleSystem_dropMenu.setRolled(!particleSystem_dropMenu_visible);
	m_particleSystem_dropMenu.setVisible(particleSystem_dropMenu_visible);

	m_dynamic_dropMenu.setRolled(!dynamic_dropMenu_visible);
	m_dynamic_dropMenu.setVisible(dynamic_dropMenu_visible);

	m_PhotoPhysics_dropMenu.setRolled(!PhotoPhysics_dropMenu_visible);
	m_PhotoPhysics_dropMenu.setVisible(PhotoPhysics_dropMenu_visible);

	m_rendering_dropMenu.setRolled(!rendering_dropMenu_visible);
	m_rendering_dropMenu.setVisible(rendering_dropMenu_visible);

	m_liveExperiment_dropMenu.setRolled(!liveExperiment_dropMenu_visible);
	m_liveExperiment_dropMenu.setVisible(liveExperiment_dropMenu_visible);

	m_wProtocoleExperiment_dropMenu.setRolled(!wProtExperiment_dropMenu_visible);
	m_wProtocoleExperiment_dropMenu.setVisible(wProtExperiment_dropMenu_visible);

	m_wProtocoleExperimentMeasuredRegions_dropMenu.setRolled(!wProtExperiment_dropMenu_visible);
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setVisible(wProtExperiment_dropMenu_visible);

	m_FRAPExperimentParameters_dropMenu.setRolled(!FRAPExpParam_dropMenu_visible);
	m_FRAPExperimentParameters_dropMenu.setVisible(FRAPExpParam_dropMenu_visible);

	m_PAFExperimentParameters_dropMenu.setRolled(!PAFExpParam_dropMenu_visible);
	m_PAFExperimentParameters_dropMenu.setVisible(PAFExpParam_dropMenu_visible);

	m_FCSExperimentParameters_dropMenu.setRolled(!FCSExpParam_dropMenu_visible);
	m_FCSExperimentParameters_dropMenu.setVisible(FCSExpParam_dropMenu_visible);

	m_SRIParameters_dropMenu.setRolled(!SRIExpParam_dropMenu_visible);
	m_SRIParameters_dropMenu.setVisible(SRIExpParam_dropMenu_visible);

	m_drugExperimentParameters_dropMenu.setRolled(!drugExpParam_dropMenu_visible);
	m_drugExperimentParameters_dropMenu.setVisible(drugExpParam_dropMenu_visible);

	m_fileDestination_dropMenu.setRolled(!fileDestination_dropMenu_visible);
	m_fileDestination_dropMenu.setVisible(fileDestination_dropMenu_visible);

	m_wProtocoleExperimentMeasuredRegions_dropMenu.setRolled(!wProtMeasuredRegions_dropMenu_visible);
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setVisible(wProtMeasuredRegions_dropMenu_visible);

	m_wProtocoleExperimentBleachedRegions_dropMenu.setRolled(!wProtBleachedRegions_dropMenu_visible);
	m_wProtocoleExperimentBleachedRegions_dropMenu.setVisible(wProtBleachedRegions_dropMenu_visible);
    m_wProtocoleExperimentBleachedRegions_dropMenu.setText(exp_type == FRAP_EXPERIMENT ? "Photobleached Regions" : "Photoactivated Regions");

	m_ui_dockWdgt.updateGeometry();

//******GEOMETRY ONGLET******
		if(m_rendering_params.backgroundImage_path.empty() == false)
		{
			if(QDir(m_rendering_params.backgroundImage_path.data()).isRelative())
			{
				string lineEdit_text = string("\"FluoSimFolder\"/") + m_rendering_params.backgroundImage_path.data();
				m_loadBackgroundImage_lineEdit.setText(lineEdit_text.data());
			}
			else
			{
				m_loadBackgroundImage_lineEdit.setText(m_rendering_params.backgroundImage_path.data());
			}
		}
        else m_loadBackgroundImage_lineEdit.clear();

		m_backgroundVisibleInGeom_checkBox.setChecked(m_rendering_params.isBackgroundImageVisible_inGeom);
		m_imageSmoothing_checkBox.setChecked(m_rendering_params.isBackgroundImageSmoothed);

		auto spc = m_bioWorld->getSpecieAdr(0);
		int nb_rgns = m_bioWorld->getNbRegions();
		for(int row = m_allRegions_tableWidget.rowCount()-1; row>= 0; row--)
		{
			m_allRegions_tableWidget.removeRow(row);
		}

		for(int rgn_idx = 0; rgn_idx <= nb_rgns-1; rgn_idx++)
		{
			m_allRegions_tableWidget.insertRow(rgn_idx);
			QTableWidgetItem* name_item = new QTableWidgetItem(m_bioWorld->getRegionRef(rgn_idx).getName().data());
				name_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				name_item->setTextColor(QColor(240,240,240));
			QTableWidgetItem* contour_item = new QTableWidgetItem("");
				if(rgn_idx == 0) contour_item->setText("C");
				contour_item->setTextAlignment(Qt::AlignCenter);
				contour_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				contour_item->setTextColor(QColor(240,240,240));
			QTableWidgetItem* trapping_item = new QTableWidgetItem("");
				if(rgn_idx != 0 && m_bioWorld->getRegionRef(rgn_idx).isACompartment(spc)) trapping_item->setText("T");
				trapping_item->setTextAlignment(Qt::AlignCenter);
				trapping_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				trapping_item->setTextColor(QColor(240,240,240));

			m_allRegions_tableWidget.setItem(rgn_idx, 0, name_item);
			m_allRegions_tableWidget.setItem(rgn_idx, 1, contour_item);
			m_allRegions_tableWidget.setItem(rgn_idx, 2, trapping_item);
		}

//*******LIVE MODE*******
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes);
		m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes);
		updateSimulationProgressViews();


	//PARTICLE SYSTEM
        m_nbParticles_spinBxedSlider.setValue(m_particleSystem_params.N_particles);

	//SIMULATION
		m_pixelSize_dSpinBx.valueChanged(m_simulation_params.pixel_size);
		m_pixelSize_dSpinBx.setValue(m_simulation_params.pixel_size);

		m_timeStep_dSpinBx.valueChanged(m_simulation_params.dt_sim);
		m_timeStep_dSpinBx.setValue(m_simulation_params.dt_sim);

	//DYNAMIC PARAMETERS
        if(m_dynamic_params.D_insideContact > m_dynamic_params.D_outsideContact)
        {
            m_insideD_dSpinBxedSlider.setComboBoxIndex(DIFFUSION_UNIT_COMBOBOX_UM2PERSEC);
        }
        switch(m_insideD_dSpinBxedSlider.getComboBoxIndex())
        {
            case DIFFUSION_UNIT_COMBOBOX_PERCENTAGE :
            {
                float Dinside_ratio;
                if(m_dynamic_params.D_outsideContact == 0) Dinside_ratio = 1;
                else Dinside_ratio = m_dynamic_params.D_insideContact/m_dynamic_params.D_outsideContact;
                m_insideD_dSpinBxedSlider.setValue(Dinside_ratio*100);

            }
            break;

            case DIFFUSION_UNIT_COMBOBOX_UM2PERSEC : m_insideD_dSpinBxedSlider.setValue(m_dynamic_params.D_insideContact);
            break;
        }
        m_outsideD_dSpinBxedSlider.setValue(m_dynamic_params.D_outsideContact);
        m_trappedD_dSpinBxedSlider.setValue(m_dynamic_params.D_trapped);
        m_kon_dSpinBxedSlider.setValue(m_dynamic_params.k_on);
        m_koff_dSpinBxedSlider.setValue(m_dynamic_params.k_off);

		double value_d = m_dynamic_params.immobileFraction;
		m_immobileFraction_dSpinBx.setValue(value_d*100.0); //in % in the view

		FluoSimView::enableDynamicParameters(!m_dynamic_params.isFixed);
		m_fixation_checkBx.setChecked(m_dynamic_params.isFixed);

		m_initialEnrichment_spinBox.setValue(m_experiment_params.initial_enrichment);
        m_crossingProbability_dSpinBxedSlider.setValue(m_dynamic_params.crossingProbability_outIn);

	//PHOTOPHYSICS
        m_blinkingKon_dSpinBxedSlider.setValue(m_photoPhysics_params.k_on_fluo);
        m_blinkingKoff_dSpinBxedSlider.setValue(m_photoPhysics_params.k_off_fluo);

	//RENDERING
		m_regionsVisible_checkBx.setChecked(m_rendering_params.areRegionsVisible);
		m_backgroundVisibleInLiveAndExp_checkBx.setChecked(m_rendering_params.isBackgroundImageVisible_inLiveAndExp);

		m_particleColor_comboBx.setCurrentIndex(m_rendering_params.particle_color);
		m_renderingMode_comboBx.setCurrentIndex(m_rendering_params.rendering_mode);

		value = round(((m_rendering_params.image_intensity - m_imageIntensity_slider_minValue) /(m_imageIntensity_slider_maxValue - m_imageIntensity_slider_minValue))*m_imageIntensity_slider.maximum());
		m_imageIntensity_slider.setValue(value);
		updateSliderLabelValue(imageIntensity_slider);

        m_spotSize_dSpinBxedSlider.setValue(m_rendering_params.spot_size);

        float spot_intensity = m_rendering_params.spot_intensity; //since may be modificed by the unit change
        int unit = (m_rendering_params.isSpotIntensity_inPhotonsPerSec ? SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC : SPOTINTENSITY_UNIT_COMBOBOX_ADCOUNTS);
        m_spotIntensity_spinBxedSlider.setComboBoxIndex(unit); // triggers an event which when processed change the range of the slider, so no need to take care of it right here
        m_spotIntensity_spinBxedSlider.setValue(spot_intensity); //since the value stored in m_rendering_params is a float between 0 and 1

        m_photonConversion_label.setEnabled(m_spotIntensity_spinBxedSlider.getComboBoxIndex()== SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
        m_photonConversion_dSpinBx.setEnabled(m_spotIntensity_spinBxedSlider.getComboBoxIndex()==SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
        m_photonConversion_dSpinBx.setValue(m_rendering_params.ADCounts_perPhoton);
        m_autoscale_checkBx.setChecked(m_rendering_params.isAutoscale);

		if(m_current_rendering_params->rendering_mode == RENDERING_POINTS) showGaussianParameters(false);
		else showGaussianParameters(true);


	//LIVE EXPERIMENTAL PARAMETERS
	m_liveExperimentType_comboBx.setCurrentIndex(m_liveExperiment_params.experimentType);
	if(m_liveExperiment_params.experimentType == FCS_LIVE_EXPERIMENT)
	{
		m_signal.show();
		m_probe.getSignalRef().hide();

		showLivePhotoBleachingParameters(false);
		showLivePhotoActivationParameters(false);
	}

	if(m_liveExperiment_params.experimentType == PHOTOBLEACHING_LIVE_EXPERIMENT)
	{
		m_signal.hide();
		m_probe.getSignalRef().show();

		showLivePhotoBleachingParameters(true);
		showLivePhotoActivationParameters(false);
	}
	if(m_liveExperiment_params.experimentType == PHOTOACTIVATION_LIVE_EXPERIMENT)
	{
		m_signal.hide();
		m_probe.getSignalRef().show();

		showLivePhotoBleachingParameters(false);
		showLivePhotoActivationParameters(true);
	}

//******EXPERIMENTAL MODE******
	//Protocole Parameters
		m_experimentType_comboBx.setCurrentIndex(m_experiment_params.experimentType);
        m_acquisitionMode_comboBox.setCurrentIndex(m_experiment_params.acquisitionType);
			if(m_experiment_params.acquisitionType == STREAM_ACQUISITION)
			{
				FluoSimView::showTimeLapseParameters(false);
			}
            else if(m_experiment_params.acquisitionType == TIMELAPSE_ACQUISITION)
			{
                FluoSimView::showTimeLapseParameters(true);
			}

        m_timelapse_period_dSpinBox.setValue(m_experiment_params.acquisitionPeriod);
        m_presequ_spinBox.setValue(m_experiment_params.N_presequ);
		m_duration_spinBox.setValue(m_experiment_params.N_planes);
		m_frapDPlane_spinBox.setValue(m_experiment_params.dN_frap);
		m_frapPlane_spinBox.setValue(m_experiment_params.N_frap);
		m_frapKoff_dspinBox.setValue(m_experiment_params.k_off_frap);
		m_photoActivationDPlane_spinBox.setValue(m_experiment_params.dN_photoActivation);
		m_photoActivationPlane_spinBox.setValue(m_experiment_params.N_photoActivation);
		m_photoActivationKon_dspinBox.setValue(m_experiment_params.k_on_photoActivation);

		m_FCSBeamIsEnabled_checkBox.setChecked(m_experiment_params.is_fcsUsingGaussianBeam);
			m_FCSBeamIsEnabled_checkBox.clicked(m_experiment_params.is_fcsUsingGaussianBeam);
		m_FCSBeamKoff_dSpinBox.setValue(m_experiment_params.fcs_beamKoff);
		m_FCSBeamMaxIntensity_dSpinBox.setValue(m_experiment_params.fcs_maxIntensity);
		m_FCSBeamSigma_dSpinBox.setValue(m_experiment_params.fcs_beamSigma);

		float new_value = m_experiment_params.drug_newValue; //needed, if not m_experiment_params.drug_newValue get modified by next code line...
		m_drugAdminParamChanged_comboBx.setCurrentIndex(m_experiment_params.drug_affectedParam);
		m_drugAdminNewValue_dspinBox.setValue(new_value);
		m_drugAdminPlane_spinBox.setValue(m_experiment_params.N_drug);
		m_repetitioon_spinBox.setValue(m_experiment_params.N_repetition);
		m_exportStack_checkBx.setChecked(m_experiment_params.isExportingStack);
            enableExportStackParameters(m_experiment_params.isExportingStack);
            enableExportStackCheckBox(m_rendering_params.backgroundImage_path.empty() == false);
            m_exportStack_noBackgroundImage_label.setVisible(m_rendering_params.backgroundImage_path.empty() == true);
            bool exportStack_visibility = m_experiment_params.experimentType != SRI_EXPERIMENT;
            FluoSimView::showExportStackParameters(exportStack_visibility);
        m_photonBackground_spinBx.setValue(m_cameraRendering_params.photonBackground);
        m_stackPoissonN_checkBx.setChecked(m_cameraRendering_params.isUsingPoissonNoise);
        m_cameraOffset_spinBx.setValue(m_cameraRendering_params.cameraOffset);
        m_cameraReadoutNoise_dSpinBx.setValue(m_cameraRendering_params.readoutNoise_sigma);


		m_SRIIntensity_spinBx.setValue(round(m_stormRendering_params.point_intensity*(256*256-1)));
		m_SRIPointingAccuracy_dSpinBx.setValue(m_experiment_params.SRI_pointingAccuracy);
		m_SRIZoomLevel_spinBx.setValue(m_experiment_params.SRI_zoom);

	//File Destination
		string folder_directory = m_experiment_params.file_destination.data();
		if(QDir(folder_directory.data()).isRelative())
		{
			folder_directory = string("\"FluoSimFolder\"/") + m_experiment_params.file_destination;
			m_fileDestination_lineEdit.setText(folder_directory.data());
		}
		else m_fileDestination_lineEdit.setText(folder_directory.data());

	//Measured Regions
		nb_rgns = m_bioWorld->getNbRegions();
		if(m_bioWorld == 0 || nb_rgns == 0)
		{
			m_allMeasuredRegions_comboBx.clear();
			m_allBleachedRegions_comboBx.clear();

			m_measuredRegion_comboBx.clear();
			m_measuredRegion_comboBx.addItem("No Regions");

			m_bleachedRegion_comboBx.clear();
			m_bleachedRegion_comboBx.addItem("No Regions");
			m_photoActivatedRegion_comboBx.clear();
			m_photoActivatedRegion_comboBx.addItem("No Regions");

			m_measuredRegions_listWidget.clear();
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
				m_measuredRegions_listWidget.addItem(rgnName.data());
			}

			m_bleachedRegions_listWidget.clear();
			for(string& bleached_rgnName : m_experiment_params.bleachedRegionNames_v)
			{
				m_bleachedRegions_listWidget.addItem(bleached_rgnName.data());
			}
		}
		else
		{
            //disconnection needed : if not undesired index changes are detected
			disconnect(&m_measuredRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(measuredRegiongetChanged(QString)));
			disconnect(&m_bleachedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(bleachedRegionGetChanged(QString)));
			disconnect(&m_photoActivatedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(photoActivatedRegionGetChanged(QString)));

				m_allMeasuredRegions_comboBx.clear();
				m_allBleachedRegions_comboBx.clear();
				//need to be disconnected...
				m_measuredRegion_comboBx.clear();
				m_bleachedRegion_comboBx.clear();
				m_photoActivatedRegion_comboBx.clear();
				QStringList regions;
				for(int rgn_idx = 0; rgn_idx <= nb_rgns-1; rgn_idx++)
				{
					regions.push_back(m_bioWorld->getRegionRef(rgn_idx).getName().data());
					string rgn = m_bioWorld->getRegionRef(rgn_idx).getName().data();
				}

				//live
				m_measuredRegion_comboBx.addItems(regions);
				m_bleachedRegion_comboBx.addItems(regions);
				m_photoActivatedRegion_comboBx.addItems(regions);

			connect(&m_measuredRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(measuredRegiongetChanged(QString)));
			connect(&m_bleachedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(bleachedRegionGetChanged(QString)));
			connect(&m_photoActivatedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(photoActivatedRegionGetChanged(QString)));

			string measured_rgnName = m_liveExperiment_params.measured_RgnName;
			int measured_rgnIdx = m_bioWorld->getRegionIdx(measured_rgnName);
			if(measured_rgnIdx != -1) m_measuredRegion_comboBx.setCurrentText(measured_rgnName.data());

			string bleached_rgnName = m_liveExperiment_params.bleached_RgnName;
			int bleached_rgnIdx = m_bioWorld->getRegionIdx(bleached_rgnName);
			if(bleached_rgnIdx != -1) m_bleachedRegion_comboBx.setCurrentText(bleached_rgnName.data());

			string photoActivated_rgnName = m_liveExperiment_params.photoActivated_RgnName;
			int photoActivated_rgnIdx = m_bioWorld->getRegionIdx(photoActivated_rgnName);
			if(photoActivated_rgnIdx != -1) m_photoActivatedRegion_comboBx.setCurrentText(photoActivated_rgnName.data());

			//protocole
			m_allMeasuredRegions_comboBx.addItems(regions);
			m_allBleachedRegions_comboBx.addItems(regions);

			m_measuredRegions_listWidget.clear();
			for(string& rgnName : m_experiment_params.measuredRegionNames_v)
			{
				m_measuredRegions_listWidget.addItem(rgnName.data());
			}

			m_bleachedRegions_listWidget.clear();
			for(string& bleached_rgnName : m_experiment_params.bleachedRegionNames_v)
			{
				m_bleachedRegions_listWidget.addItem(bleached_rgnName.data());
			}
		}
//ANALYSIS MODE : is managed through the tracePlayer object
}


void FluoSim::regionVisibilityGetChanged(bool value)
{
	m_rendering_params.areRegionsVisible = value;
}

void FluoSim::backgroundVisiblityInGeomGetChanged(bool value)
{
	if(m_backgroundImage != 0)
	{
		//model
		m_rendering_params.isBackgroundImageVisible_inGeom = value;
	}

}

void FluoSim::backgroundVisiblityInLiveAndExpGetChanged(bool value)
{
	//model
	if(m_backgroundImage != 0)
	{
		//model
		m_rendering_params.isBackgroundImageVisible_inLiveAndExp = value;
	}
	updateRenderingParams();

}

void FluoSim::spotIntensityUnitGetChanged(int new_unit)
{
    //view
    m_photonConversion_label.setEnabled(new_unit == SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
    m_photonConversion_dSpinBx.setEnabled(new_unit == SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
    enableExportStackParameters(m_experiment_params.isExportingStack == true); //to update the illumination state

    switch(new_unit)
    {
        case SPOTINTENSITY_UNIT_COMBOBOX_ADCOUNTS :
        {
            //we need to save the value since it might be modified by the range change
            float spotIntensity_photonsPerSec = m_rendering_params.spot_intensity;
            m_spotIntensity_spinBxedSlider.setMin(m_spotIntensitySlider_minValue);
            m_spotIntensity_spinBxedSlider.setMax(m_spotIntensitySlider_maxValue);
            m_spotIntensity_spinBxedSlider.setValue(spotIntensity_photonsPerSec*m_rendering_params.ADCounts_perPhoton*m_simulation_params.dt_sim);
        }
        break;

        case SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC :
        {
            float spotIntensity_ADCouts = m_rendering_params.spot_intensity;
            m_spotIntensity_spinBxedSlider.setMin(m_spotIntensitySlider_photonPerSec_minValue);
            m_spotIntensity_spinBxedSlider.setMax(m_spotIntensitySlider_photonPerSec_maxValue);
            m_spotIntensity_spinBxedSlider.setValue(spotIntensity_ADCouts/(m_rendering_params.ADCounts_perPhoton*m_simulation_params.dt_sim));
        }
        break;
    }

    //model
    m_rendering_params.isSpotIntensity_inPhotonsPerSec = (new_unit == SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
    m_cameraRendering_params.isSpotIntensity_inPhotonsPerSec =(new_unit == SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
    updateRenderingParams();

}

void FluoSim::ADCountsPerPhotonGetChanged(double value)
{
    //view

    //model
    m_rendering_params.ADCounts_perPhoton = value;
    m_cameraRendering_params.ADCounts_perPhoton = value;

    if(m_rendering_params.isSpotIntensity_inPhotonsPerSec == true)
    {// the actual spotIntensity has changed (since depends on the conversion rate), then we need to update the renderer (myScreen_point) state
        updateRenderingParams();
    }
}

void FluoSim::autoscaleGetChanged(bool value)
{
    //model
    m_rendering_params.isAutoscale = value;
    updateRenderingParams();
}

void FluoSim::imageSmoothingGetChanged(bool isSmoothed)
{
	if(m_backgroundImage != 0)
	{
		//model
		m_rendering_params.isBackgroundImageSmoothed = isSmoothed;
		if(isSmoothed == true)
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
}


void FluoSim::measuredRegiongetChanged(QString rgn_name_qstr)
{
	string rgn_name = rgn_name_qstr.toLocal8Bit().data();
	int rgn_idx = m_bioWorld->getRegionIdx(rgn_name_qstr.toLocal8Bit().data());
	Region* rgn = &(m_bioWorld->getRegionRef(rgn_idx));

	if(m_bioWorld == 0 ||
	   rgn_idx == -1 ||
	   m_bioWorld->getNbRegions() == 0) return; //->

	m_probe.resetProbeMeasure();

	int nb_rgn = m_bioWorld->getNbRegions();
	for(int temp_rgn_idx = 0; temp_rgn_idx <= nb_rgn-1; temp_rgn_idx++)
	{
		if(temp_rgn_idx == rgn_idx)
		m_bioWorld->getRegionRef(temp_rgn_idx).setColor(glm::vec4(0,1,0,1));
		else
		m_bioWorld->getRegionRef(temp_rgn_idx).setColor(glm::vec4(0,0,1,1));
	}

	//model

	m_liveExperiment_params.measured_RgnName = rgn_name;
	m_probe.setRegion1(rgn);
	m_probe.setRegion2(rgn);
}

void FluoSim::regionComboBoxFocusGetLost()
{
    //view
    m_allRegions_tableWidget.clearSelection();

    //model
    FluoSimModel::m_rendering_params.highlightedRegion_names.clear();
    FluoSimModel::updateRenderingParams();
}

void FluoSim::regionGetHighlighted(QString region_name)
{
	//view
	m_rendering_params.highlightedRegion_names.clear();
	m_rendering_params.highlightedRegion_names.push_back(region_name.toLocal8Bit().data());

	updateRenderingParams();
}

void FluoSim::liveExperimentTypeGetChanged(int idx)
{
	//view

	LIVE_EXPERIMENT_TYPE exp_type = (LIVE_EXPERIMENT_TYPE) idx;

	if(exp_type == FCS_LIVE_EXPERIMENT)
	{
		m_signal.show();
		m_probe.getSignalRef().hide();

		FluoSimView::showLivePhotoBleachingParameters(false);
		FluoSimView::showLivePhotoActivationParameters(false);
	}

	if(exp_type == PHOTOBLEACHING_LIVE_EXPERIMENT)
	{
		m_signal.hide();
		m_probe.getSignalRef().show();

		FluoSimView::showLivePhotoBleachingParameters(true);
		FluoSimView::showLivePhotoActivationParameters(false);
	}

	if(exp_type == PHOTOACTIVATION_LIVE_EXPERIMENT)
	{
		m_signal.hide();
		m_probe.getSignalRef().show();

		FluoSimView::showLivePhotoBleachingParameters(false);
		FluoSimView::showLivePhotoActivationParameters(true);
	}

	//model

	m_liveExperiment_params.experimentType = exp_type;

}

void FluoSim::bleachedRegionGetChanged(QString rgn_name_qstr)
{
	string rgn_name = rgn_name_qstr.toLocal8Bit().data();
	int rgn_idx = m_bioWorld->getRegionIdx(rgn_name);

	if(m_bioWorld == 0 ||
	   rgn_idx == -1 ||
	   m_bioWorld->getNbRegions() == 0) return; //->

	//view

	m_photoActivatedRegion_comboBx.setCurrentText(rgn_name_qstr);

//	model

	m_liveExperiment_params.bleached_RgnName = rgn_name;
	m_liveExperiment_params.photoActivated_RgnName = rgn_name;
	m_frapHead->setRegion(&(m_bioWorld->getRegionRef(rgn_idx)));
}


void FluoSim::photoActivatedRegionGetChanged(QString rgn_name_qstr)
{
	string rgn_name = rgn_name_qstr.toLocal8Bit().data();
	int rgn_idx = m_bioWorld->getRegionIdx(rgn_name);

	if(m_bioWorld == 0 ||
	   rgn_idx == -1 ||
	   m_bioWorld->getNbRegions() == 0) return; //->

	//view

	m_bleachedRegion_comboBx.setCurrentText(rgn_name_qstr);

	//model

	m_liveExperiment_params.bleached_RgnName = rgn_name;
	m_liveExperiment_params.photoActivated_RgnName = rgn_name;
	m_frapHead->setRegion(&(m_bioWorld->getRegionRef(rgn_idx)));

}

void FluoSim::regionGetBleached()
{
	if((m_bioWorld == 0) || (m_bleachedRegion_comboBx.currentText() == "No Regions"))  return; //->

	m_frapHead->bleachRegion();
}

void FluoSim::regionGetPhotoActivated()
{
	if((m_bioWorld == 0) || (m_bleachedRegion_comboBx.currentText() == "No Regions"))  return; //->
	m_frapHead->photoActivateRegion();
}

void FluoSim::experimentTypeGetChanged(int type)
{
//**views**
	if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
	{
		bool FRAPExpParam_dropMenu_visible = (type == FRAP_EXPERIMENT);
		bool PAFExpParam_dropMenu_visible = (type == PAF_EXPERIMENT);
		bool FCSExpParam_dropMenu_visible = (type == FCS_EXPERIMENT);
		bool SRIExpParam_dropMenu_visible = (type == SRI_EXPERIMENT);
		bool drugExpParam_dropMenu_visible = (type == DRUG_EXPERIMENT);
		bool wProtMeasuredRegions_dropMenu_visible = !(type == SRI_EXPERIMENT);
		bool wProtBleachedRegions_dropMenu_visible = ((type == FRAP_EXPERIMENT) ||
													  (type == PAF_EXPERIMENT));
		bool exportStack_visibility = type != SRI_EXPERIMENT;

		m_FRAPExperimentParameters_dropMenu.setRolled(!FRAPExpParam_dropMenu_visible);
		m_FRAPExperimentParameters_dropMenu.setVisible(FRAPExpParam_dropMenu_visible);

		m_PAFExperimentParameters_dropMenu.setRolled(!PAFExpParam_dropMenu_visible);
		m_PAFExperimentParameters_dropMenu.setVisible(PAFExpParam_dropMenu_visible);

		m_FCSExperimentParameters_dropMenu.setRolled(!FCSExpParam_dropMenu_visible);
		m_FCSExperimentParameters_dropMenu.setVisible(FCSExpParam_dropMenu_visible);

		m_SRIParameters_dropMenu.setRolled(!SRIExpParam_dropMenu_visible);
		m_SRIParameters_dropMenu.setVisible(SRIExpParam_dropMenu_visible);

		m_drugExperimentParameters_dropMenu.setRolled(!drugExpParam_dropMenu_visible);
		m_drugExperimentParameters_dropMenu.setVisible(drugExpParam_dropMenu_visible);
        if(type == DRUG_EXPERIMENT) updateDrugNewValueSpinBox();

		m_wProtocoleExperimentMeasuredRegions_dropMenu.setRolled(!wProtMeasuredRegions_dropMenu_visible);
		m_wProtocoleExperimentMeasuredRegions_dropMenu.setVisible(wProtMeasuredRegions_dropMenu_visible);

		m_wProtocoleExperimentBleachedRegions_dropMenu.setRolled(!wProtBleachedRegions_dropMenu_visible);
		m_wProtocoleExperimentBleachedRegions_dropMenu.setVisible(wProtBleachedRegions_dropMenu_visible);
        m_wProtocoleExperimentBleachedRegions_dropMenu.setText(type == FRAP_EXPERIMENT ? "Photobleached Regions" : "Photoactivated Regions");

        FluoSimView::showExportStackParameters(exportStack_visibility);
        m_exportStack_noBackgroundImage_label.setVisible(m_rendering_params.backgroundImage_path.empty() == true
                                                         && type != SRI_EXPERIMENT);


		if((EXPERIMENT_TYPE)type == SRI_EXPERIMENT)
		{
			if(m_backgroundImage == 0)
			{
				m_simulation_progressBar.setTextVisible(true);
				m_simulation_progressBar.setStyleSheet("QProgressBar {text-align : center; color : rgb(255,0,0); background-color : rgb(49,49,49);}"
													   "QProgressBar::chunk {background-color : rgb(49,49,49); color : rgb(255,0,0);}"
													   "QProgressBar {font-weight : bold;}"
													   );
				m_simulation_progressBar.setFormat("- No Background Image -");

				enableSimulationStateButtons(false);
			}
		}
		else
		{
			enableSimulationStateButtons(true);
			m_simulation_progressBar.setTextVisible(false);
            m_simulation_progressBar.setStyleSheet("QProgressBar::chunk {background : rgb(200,0,0);width:1px;\n}");
		}
	}

//**model**
	FluoSimModel::setExperimentType((EXPERIMENT_TYPE) type);

}




void FluoSim::acquisitionTypeGetChanged(int type)
{
//model

	FluoSimModel::m_experiment_params.acquisitionType = (ACQUISITION_TYPE) type;

//views

	if((ACQUISITION_TYPE) type == TIMELAPSE_ACQUISITION)
	{
		FluoSimView::showTimeLapseParameters(true);
	}
	else
	{
		FluoSimView::showTimeLapseParameters(false);
	}

}

void FluoSim::acquisitionPeriodGetChanged(double new_period)
{
    //view
    disconnect(&m_timelapse_period_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(acquisitionPeriodGetChanged(double)));

    double timeStep = m_simulation_params.dt_sim;

    if(modf(new_period/timeStep,0) <= 0.0001) m_timelapse_period_dSpinBox.setValue(new_period);
    else {
        m_timelapse_period_dSpinBox.setValue(ceil(new_period/timeStep)*timeStep);
    }

    connect(&m_timelapse_period_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(acquisitionPeriodGetChanged(double)));

    //model
    m_experiment_params.acquisitionPeriod = m_timelapse_period_dSpinBox.value();

}

void FluoSim::acquisitionPresequenceGetChanged(int presequ)
{
//***model***
	FluoSimModel::m_experiment_params.N_presequ = presequ;

//***views***
    if(m_simulation_states.simulator_mode == EXPERIMENT_MODE)
    {
        m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes +
                                              m_experiment_params.N_presequ);

        m_simulation_params.current_plane = -m_experiment_params.N_presequ;
        m_simulation_params.current_time = -m_experiment_params.N_presequ * m_simulation_params.dt_sim;
    }
    else
    {
        m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes);
        m_simulation_params.current_plane = 0;
        m_simulation_params.current_time = 0.0f;
    }

	updateSimulationProgressViews();
}

void FluoSim::acquisitionNPlanesGetChanged(int Nplanes)
{
//model
	FluoSimModel::m_experiment_params.N_planes = Nplanes;
//views

	m_simulationMaxPlane_dSpinBx.setValue(m_experiment_params.N_planes +
										  m_experiment_params.N_presequ);
	FluoSimModel::stopSimulation();
	updateSimulationProgressViews();
}

void FluoSim::frapPlaneIdxGetChanged(int frap_idx)
{
	FluoSimModel::m_experiment_params.N_frap = frap_idx;
}

void FluoSim::frapDPlaneGetChanged(int dN_frap)
{
	FluoSimModel::m_experiment_params.dN_frap = dN_frap;
}

void FluoSim::frapKoffGetChanged(double k_off)
{
	FluoSimModel::m_experiment_params.k_off_frap = k_off;
}

void FluoSim::photoActivationPlaneIdxGetChanged(int photoActivation_idx)
{
	FluoSimModel::m_experiment_params.N_photoActivation = photoActivation_idx;
}

void FluoSim::photoActivationDPlaneGetChanged(int dN_photoActivation)
{
	FluoSimModel::m_experiment_params.dN_photoActivation = dN_photoActivation;
}

void FluoSim::photoActivationKonGetChanged(double k_on)
{
	FluoSimModel::m_experiment_params.k_on_photoActivation = k_on;
}

void FluoSim::FCSBeamIsEnabledGetChanged(bool isEnabled)
{
	//view

	m_FCSBeamMaxIntensity_label.setEnabled(isEnabled);
	m_FCSBeamMaxIntensity_dSpinBox.setEnabled(isEnabled);
	m_FCSBeamSigma_label.setEnabled(isEnabled);
	m_FCSBeamSigma_dSpinBox.setEnabled(isEnabled);
	m_FCSBeamKoff_label.setEnabled(isEnabled);
	m_FCSBeamKoff_dSpinBox.setEnabled(isEnabled);

	//model
	FluoSimModel::m_experiment_params.is_fcsUsingGaussianBeam = isEnabled;
}

void FluoSim::FCSBeamSigmaGetChanged(double sigma)
{
	//view

	//model
	m_experiment_params.fcs_beamSigma = sigma;
	FluoSimModel::updateExperimentParams();
}

void FluoSim::FCSBeamMaxIntensityGetChanged(double intensity)
{
	//view

	//model
	m_experiment_params.fcs_maxIntensity = intensity;
	FluoSimModel::updateExperimentParams();

}

void FluoSim::FCSBeamKoffGetChanged(double koff)
{
	//view

	//model
	m_experiment_params.fcs_beamKoff = koff;
	FluoSimModel::updateExperimentParams();

}

void FluoSim::SRIPointingAccuracyGetChanged(double pointingAccuracy)
{
	m_experiment_params.SRI_pointingAccuracy = pointingAccuracy;
}

void FluoSim::SRIIntensityGetChanged(int intensity)
{
	m_stormRendering_params.point_intensity = float(intensity)/(256*256-1);
	updateRenderingParams();
}

void FluoSim::SRIZoomLevelGetChanged(int zoom)
{
	m_experiment_params.SRI_zoom = zoom;
	updateRenderingParams();
}

void FluoSim::drugAdminPlaneChanged(int plane)
{
	m_experiment_params.N_drug = plane;
}
void FluoSim::drugAdminParamChanged(int afftected_param)
{
	FluoSimModel::m_experiment_params.drug_affectedParam = (DRUG_AFFECTED_PARAM) afftected_param;
	FluoSimModel::m_dynamic_params.drug_affectedParam = (DRUG_AFFECTED_PARAM) afftected_param;

	FluoSimView::updateDrugNewValueSpinBox();
}
void FluoSim::drugAdminNewValueChanged(double newValue)
{
	FluoSimModel::m_experiment_params.drug_newValue = newValue;
	FluoSimModel::m_dynamic_params.drug_newValue = newValue;
}

void FluoSim::NbRepetitionGetChanged(int nb_repetition)
{
	FluoSimModel::m_experiment_params.N_repetition = nb_repetition;
}

void FluoSim::initialEnrichmentGetChanged(double initial_enrichment)
{
	m_experiment_params.initial_enrichment = initial_enrichment;
}

void FluoSim::fileDestinationButtonHasBeenClicked()
{
	string folder_directory;
	bool isAFolderSelected = getFolderLocation(folder_directory, m_experiment_params.file_destination, false, &m_mainWindow); //->
	if(isAFolderSelected == false) return; //->

	//model part
	FluoSimModel::setDestinationPath(folder_directory);

	//view part
	folder_directory = m_experiment_params.file_destination.data(); //might have been changed by previous line
	if(QDir(folder_directory.data()).isRelative())
	{
		folder_directory = string("\"FluoSimFolder\"/") + m_experiment_params.file_destination;
		m_fileDestination_lineEdit.setText(folder_directory.data());
	}
	else
	{
		m_fileDestination_lineEdit.setText(folder_directory.data());
	}
}

void FluoSim::isExportingStackGetChanged(bool new_state)
{
//model
	m_experiment_params.isExportingStack = new_state;

//view:
    //disable stack offset and poisson noise if stack is not exported
    FluoSimView::enableExportStackParameters(new_state);
}

void FluoSim::photonBackgroundHasBeenChanged(int intensity)
{
	//model
	m_cameraRendering_params.photonBackground = intensity;
	updateRenderingParams();

	//view
}
void FluoSim::isStackUsingPoissonNoiseGetChanged(bool isUsingPoissonNoise)
{
	//model
    m_cameraRendering_params.isUsingPoissonNoise = isUsingPoissonNoise;
	updateRenderingParams();

	//view
}

void FluoSim::cameraOffsetHasBeenChanged(int value)
{
    //model
    m_cameraRendering_params.cameraOffset = value;
    updateRenderingParams();
}

void FluoSim::cameraReadoutNoiseHasBeenChanged(double value)
{
    //model
    m_cameraRendering_params.readoutNoise_sigma = value;
    updateRenderingParams();
}

void FluoSim::addRegionMeasuredRegions(string rgn_name)
{
	vector<string>& measuredRgn_names = m_experiment_params.measuredRegionNames_v;
	if(std::find(measuredRgn_names.begin(),
				 measuredRgn_names.end(),
				 rgn_name) != measuredRgn_names.end()) return; //->

//view part
	m_measuredRegions_listWidget.addItem(rgn_name.data());

//model part
	m_experiment_params.measuredRegionNames_v.push_back(rgn_name);
	updateExperimentParams();
}

void FluoSim::removeRegionMeasuredRegions(string rgn_name)
{
	if(rgn_name.size() <= 0) return; //->

	vector<string>& measuredRgn_names = m_experiment_params.measuredRegionNames_v;
	vector<string>::iterator toBeErasedRgn = std::find(measuredRgn_names.begin(),
													   measuredRgn_names.end(),
													   rgn_name);
	if(toBeErasedRgn == measuredRgn_names.end()) return; //->

//view part
	//is treated in the view part of the object

//model part
	m_experiment_params.measuredRegionNames_v.erase(toBeErasedRgn);
	updateExperimentParams();
}


void FluoSim::addRegionBleachedRegions(string rgn_name)
{
	vector<string>& bleachedRgn_names = m_experiment_params.bleachedRegionNames_v;
	if(std::find(bleachedRgn_names.begin(),
				 bleachedRgn_names.end(),
				 rgn_name) != bleachedRgn_names.end()) return; //->

//view part
	m_bleachedRegions_listWidget.addItem(rgn_name.data());

//model part
	m_experiment_params.bleachedRegionNames_v.push_back(rgn_name);
	updateExperimentParams();
}

void FluoSim::removeRegionBleachedRegions(string rgn_name)
{
	if(rgn_name.size() <= 0) return; //->

	vector<string>& bleachedRgn_names = m_experiment_params.bleachedRegionNames_v;
	vector<string>::iterator toBeErasedRgn = std::find(bleachedRgn_names.begin(),
													   bleachedRgn_names.end(),
													   rgn_name);
	if(toBeErasedRgn == bleachedRgn_names.end()) return; //->

//view part
	//is treated in the view part of the object

//model part
	m_experiment_params.bleachedRegionNames_v.erase(toBeErasedRgn);
	updateExperimentParams();
}



void FluoSim::rgnHasBeenDrawn_slot(std::vector<glm::vec2> r_v)
{
	addGeometry(r_v);
	setViewsFromModel();
}

void FluoSim::graphicDockWidgetClosed()
{
	//model
	m_simulation_states.isGraphicVisible = false;

	//view
	m_showAndHideGraphicWdgt_action.setChecked(false);
}

void FluoSim::treatEvent(QEvent* event)
{
	/*________________________________________
	 *
	 *			Drag and Drop events
	 * _______________________________________*/

	if(event->type() == QEvent::DragEnter)
	{
		QDragEnterEvent* drag_event = (QDragEnterEvent*) event;

		if (!drag_event->mimeData()->hasUrls())
		{
			return;
		}

		const QMimeData* data = drag_event->mimeData();
			QList<QUrl> urls = data->urls();
			QUrl url = urls.back();
			QString path_str = url.path();

		if(path_str.endsWith(".trc") ||
		   path_str.endsWith(".tiff") || path_str.endsWith(".tif") || path_str.endsWith(".stk") ||
		   path_str.endsWith(".lut") || path_str.endsWith(".LUT") ||
		   path_str.endsWith(".rgb") || path_str.endsWith(".RGB") ||
		   path_str.endsWith(".rgn"))
		{
			drag_event->acceptProposedAction();
		}
	}

	if(event->type() == QEvent::Drop)
	{
		QDropEvent* drop_event = (QDropEvent*) event;

		if(!drop_event->mimeData()->hasUrls()) return;

		const QMimeData* data = drop_event->mimeData();
		QList<QUrl> urls = data->urls();
		QUrl url = urls.back();
		QString path_str = url.path();
			path_str.remove(0,1);
		QString host_str = url.host();
			cout<<path_str.toLocal8Bit().data()<<"\n";
			if(host_str.isEmpty() == false) path_str.insert(0, QString("//")+host_str+QString('/'));
			cout<<path_str.toLocal8Bit().data()<<"\n";

		if(path_str.endsWith(".trc"))
		{
			//model
			m_tracePlayer.setTracesFromStringFile(path_str.toLocal8Bit().data());
			//view
			m_tracePlayer.m_fileLoader_lineEdit.setText(path_str); //accessibleTrough friendship
		}

		if(path_str.endsWith(".tiff") ||
		   path_str.endsWith(".tif") ||
		   path_str.endsWith(".stk"))
		{
			//model
			m_tracePlayer.setMovieFromStringFile(path_str.toLocal8Bit().data());
			//view
			m_tracePlayer.m_movieLoader_lineEdit.setText(path_str);
		}

		if(path_str.endsWith(".lut"))
		{
			m_tracePlayer.setLUT(path_str.toLocal8Bit().data());
		}

		if(path_str.endsWith(".rgn"))
		{
			FluoSim::loadGeometry(path_str.toLocal8Bit().data());

		}
	}
}

