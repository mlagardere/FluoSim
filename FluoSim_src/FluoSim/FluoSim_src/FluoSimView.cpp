
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

//reference for stylesheets  :http://www.trinitydesktop.org/docs/qt4/stylesheet-examples.html#customizing-qcombobox

using namespace std;



FluoSimView::FluoSimView(QApplication* main_app) :

	myMainWindow(),
	m_mainWindow(*(myMainWindow*) this),
	m_scrn(main_app),
	m_load_geometry_action(&m_mainWindow),
    m_save_geometry_action(&m_mainWindow),
    m_new_project_action(&m_mainWindow),
	m_load_project_action(&m_mainWindow),
	m_save_project_action(&m_mainWindow),
	m_exit_action(main_app),
	m_captureScreen_action(&m_mainWindow),
	m_load_SPT_parameters_action(&m_mainWindow),
	m_load_FRAPOut_parameters_action(&m_mainWindow),
	m_load_FRAPIn_parameters_action(&m_mainWindow),
	m_load_FCSOut_parameters_action(&m_mainWindow),
	m_load_FCSIn_parameters_action(&m_mainWindow),
	m_load_SRI_parameters_action(&m_mainWindow),
    m_load_PAFOut_parameters_action(&m_mainWindow),
    m_load_PAFIn_parameters_action(&m_mainWindow),
	m_showAndHideSimParametersWdgt_action(&m_mainWindow),
	m_showAndHideGraphicWdgt_action(&m_mainWindow),
	m_openUserManual_action(&m_mainWindow),
	m_openAbout_action(&m_mainWindow),
	m_reset_geometry_action(&m_mainWindow),
	m_drawRect_action(&m_mainWindow),
	m_drawCircle_action(&m_mainWindow),
	m_drawPolygon_action(&m_mainWindow),
	m_drawing_toolBar(&m_mainWindow),
	m_drawing_actionGroup(&m_mainWindow),
	m_progressWdgt(&m_mainWindow)

{

	m_DSlider_minValue = 0.00; m_DSlider_maxValue = 2.00;
	m_KSlider_minValue = 0.00; m_KSlider_maxValue = 10.00;
	m_blinkingKSlider_minValue = 0.00; m_blinkingKSlider_maxValue = 10.00;
	m_spotSizeSlider_minValue = 0; m_spotSizeSlider_maxValue = 1;
    m_spotIntensitySlider_minValue = 0; m_spotIntensitySlider_maxValue = 65535;
    m_spotIntensitySlider_photonPerSec_minValue = 0; m_spotIntensitySlider_photonPerSec_maxValue = 9999;
    m_photonConversion_minValue = 0; m_photonConversion_maxValue = 9999;

	m_mainWindow.setAcceptDrops(true);
	m_main_app = main_app;

	m_mainWindow.setWindowState(Qt::WindowMaximized);
	buildMenuBar();
	buildToolBar();

	m_centralLayout.addWidget(m_scrn.getRenderWindow());
	m_mainWindow.setWindowTitle("FluoSim");
	m_mainWindow.setCentralWidget(&m_centralWidget);
	m_mainWindow.centralWidget()->setLayout(&m_centralLayout);

	m_scrn.initializeScreen();
	m_scrn.setPixelView(glm::vec2(-128,-128), glm::vec2(640, 640));
	updateParameterWidgetSize();

	buildParameterWidget();
	buildGraphicWidget();
	m_graphic_dockWdgt.setVisible(true);

	m_main_app->processEvents();
    m_mainWindow.show();

	connectSignals();
	setStyleSheets();
	setStatusTip();
}

FluoSimView::~FluoSimView()
{
	m_centralLayout.removeWidget(m_scrn.getRenderWindow());
	m_scrn.getRenderWindow()->QWidget::setParent(0);
//	delete m_graphic;
}

void FluoSimView::connectSignals()
{

	//slider signalMapper
	m_sliders_signalMapper.setMapping(&m_imageIntensity_slider, imageIntensity_slider);
    m_sliders_signalMapper.setMapping(&m_nbParticles_spinBxedSlider, nbParticles_slider);

	connect(&m_sliders_signalMapper, SIGNAL(mapped(int)), this, SLOT(onSliderValueGetChanged(int)));
			connect(&m_imageIntensity_slider, SIGNAL(valueChanged(int)), &m_sliders_signalMapper, SLOT(map()));
            connect(&m_nbParticles_spinBxedSlider, &mySpinBoxedSlider::valueChanged,
                    [=](int value) {this->onSliderValueGetChanged(nbParticles_slider, (double)value);});
            connect(&m_outsideD_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(outsideD_slider, value);});
            m_insideD_dSpinBxedSlider_connection = connect(&m_insideD_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(insideD_slider, value);});
            connect(&m_trappedD_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(trappedD_slider, value);});
            connect(&m_crossingProbability_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(crossingProbability_slider, value);});
            connect(&m_kon_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(kon_slider, value);});
            connect(&m_koff_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(koff_slider, value);});
            connect(&m_blinkingKon_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(blinkingKon_slider, value);});
            connect(&m_blinkingKoff_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(blinkingKoff_slider, value);});
            connect(&m_spotSize_dSpinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(spotSize_slider, value);});
            connect(&m_spotIntensity_spinBxedSlider, &myDoubleSpinBoxedSlider::valueChanged,
                    [=](double value) {this->onSliderValueGetChanged(spotIntensity_slider, (double) value);});

	//action signalMapper
	m_action_signalMapper.setMapping(&m_deleteAllRegionAllRegion_pushButton, resetGeometry_action);
	m_action_signalMapper.setMapping(&m_load_geometry_action, loadGeometry_action);
	m_action_signalMapper.setMapping(&m_save_geometry_action, saveGeometry_action);
    m_action_signalMapper.setMapping(&m_new_project_action, newProject_action);
	m_action_signalMapper.setMapping(&m_load_project_action, loadProject_action);
	m_action_signalMapper.setMapping(&m_save_project_action, saveProject_action);
	m_action_signalMapper.setMapping(&m_importGeometry_pushButton, loadGeometry_action);
	m_action_signalMapper.setMapping(&m_exportGeometry_pushButton, saveGeometry_action);
	m_action_signalMapper.setMapping(&m_captureScreen_action, captureScreen_action);
	m_action_signalMapper.setMapping(&m_loadBackgroundImage_pushButton, loadBackgroundImage_action);
	m_action_signalMapper.setMapping(&m_evaluateSteadyState_pushBut, evaluateSteadyState_action);
	m_action_signalMapper.setMapping(&m_exit_action, exit_action);
	m_action_signalMapper.setMapping(&m_openUserManual_action, openUserManual_action);
	m_action_signalMapper.setMapping(&m_openAbout_action, openAbout_action);
	m_action_signalMapper.setMapping(&m_mainWindow, exit_action);
	m_action_signalMapper.setMapping(&m_load_SPT_parameters_action, loadSPT_action);
	m_action_signalMapper.setMapping(&m_load_FRAPIn_parameters_action, loadFRAPIn_action);
	m_action_signalMapper.setMapping(&m_load_FRAPOut_parameters_action, loadFRAPOut_action);
	m_action_signalMapper.setMapping(&m_load_FCSIn_parameters_action, loadFCSIn_action);
	m_action_signalMapper.setMapping(&m_load_FCSOut_parameters_action, loadFCSOut_action);
	m_action_signalMapper.setMapping(&m_load_SRI_parameters_action, loadSRI_action);
    m_action_signalMapper.setMapping(&m_load_PAFIn_parameters_action, loadPAFIn_action);
    m_action_signalMapper.setMapping(&m_load_PAFOut_parameters_action, loadPAFOut_action);
    m_action_signalMapper.setMapping(&m_showAndHideSimParametersWdgt_action, showAndHideSimParametersWdgt_action);
	m_action_signalMapper.setMapping(&m_showAndHideGraphicWdgt_action, showAndHideGraphicWdgt_action);
	m_action_signalMapper.setMapping(&m_drawRect_action, drawRect_action);
	m_action_signalMapper.setMapping(&m_drawCircle_action, drawCircle_action);
	m_action_signalMapper.setMapping(&m_drawPolygon_action, drawPolygon_action);

	connect(&m_action_signalMapper, SIGNAL(mapped(int)), this,  SLOT(onActionTriggered(int)));
		connect(&m_deleteAllRegionAllRegion_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_geometry_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_save_geometry_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
        connect(&m_new_project_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_project_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_save_project_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_importGeometry_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_exportGeometry_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_captureScreen_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_loadBackgroundImage_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_evaluateSteadyState_pushBut, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_openUserManual_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_openAbout_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_exit_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_mainWindow, SIGNAL(widgetGetClosed()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_SPT_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_FRAPIn_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_FRAPOut_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_FCSIn_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_FCSOut_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_load_SRI_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
        connect(&m_load_PAFIn_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
        connect(&m_load_PAFOut_parameters_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
        connect(&m_showAndHideSimParametersWdgt_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_showAndHideGraphicWdgt_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_drawRect_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_drawCircle_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));
		connect(&m_drawPolygon_action, SIGNAL(triggered()), &m_action_signalMapper, SLOT(map()));

	m_ongletsPushButton_signalMapper.setMapping(&m_geometryOnglet_pushButton, GEOMETRY_ONGLET);
	m_ongletsPushButton_signalMapper.setMapping(&m_liveOnglet_pushButton, LIVE_ONGLET);
	m_ongletsPushButton_signalMapper.setMapping(&m_experimentOnglet_pushButton, EXPERIMENT_ONGLET);
	m_ongletsPushButton_signalMapper.setMapping(&m_analysisOnglet_pushButton, ANALYSIS_ONGLET);

	connect(&m_ongletsPushButton_signalMapper, SIGNAL(mapped(int)), this, SLOT(onOngletGetChanged(int)));
		connect(&m_geometryOnglet_pushButton, SIGNAL(clicked()), &m_ongletsPushButton_signalMapper, SLOT(map()));
		connect(&m_liveOnglet_pushButton, SIGNAL(clicked()), &m_ongletsPushButton_signalMapper, SLOT(map()));
		connect(&m_experimentOnglet_pushButton, SIGNAL(clicked()), &m_ongletsPushButton_signalMapper, SLOT(map()));
		connect(&m_analysisOnglet_pushButton, SIGNAL(clicked()), &m_ongletsPushButton_signalMapper, SLOT(map()));

	connect(&m_graphic_dockWdgt, SIGNAL(widgetGetClosed_sig()), this, SLOT(graphicDockWidgetClosed()));

	connect(&m_allRegions_tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(regionCellGetDoubleClicked(int,int)));
	connect(&m_allRegions_tableWidget, SIGNAL(cellEntered(int,int)), this, SLOT(regionCellGetEntered(int,int)));
	connect(&m_allRegions_tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(regionCellSelectionGetChanged()));
    connect(&m_allRegions_tableWidget, SIGNAL(widgetGetLeft_sig()), this, SLOT(allRegionsTableGetLeft()));
	connect(&m_deleteRegionAllRegion_pushButton, SIGNAL(clicked()), this, SLOT(deleteRegionAllRegionsGetClicked()));

	connect(&m_play_pushBut, SIGNAL(clicked(bool)), this , SLOT(playButtonHasBeenClicked(bool)));
	connect(&m_pause_pushBut, SIGNAL(clicked(bool)), this , SLOT(pauseButtonHasBeenClicked(bool)));
	connect(&m_stop_pushBut, SIGNAL(clicked(bool)), this , SLOT(stopButtonHasBeenClicked(bool)));

	connect(&m_simulationMaxPlane_dSpinBx, SIGNAL(valueChanged(double)), this , SLOT(NplanesGetChanged(double)));

	connect(&m_renderingMode_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(renderingModeGetChanged(int)));
	connect(&m_particleColor_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(particleColorGetChanged(int)));
	connect(&m_pixelSize_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(pixelSizeGetChanged(double)));
	connect(&m_timeStep_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(timeStepGetChanged(double)));

	connect(&m_mainWindow, SIGNAL(widgetGetMoved_sig(QMoveEvent*)), this, SLOT(mainWindowGetMoved(QMoveEvent*)));
	connect(&m_mainWindow, SIGNAL(widgetGetResized_sig(QResizeEvent*)), this, SLOT(mainWindowGetResized(QResizeEvent*)));
	connect(&m_mainWindow, SIGNAL(widgetGetLowered_sig()), this, SLOT(mainWindowGetLowered()));
	connect(&m_mainWindow, SIGNAL(widgetGetRaised_sig()), this, SLOT(mainWindowGetRaised()));
	connect(m_main_app, SIGNAL(applicationStateChanged(Qt::ApplicationState))   ,this, SLOT(applicationStateGetChanged(Qt::ApplicationState)));

	connect(&m_fixation_checkBx, SIGNAL(clicked(bool)), this, SLOT(fixationGetChanged(bool)));
    connect(&m_immobileFraction_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(immobileFractionGetChanged(double)));
    connect(&m_insideD_dSpinBxedSlider, SIGNAL(prefixIndexGetChanged(int)), this, SLOT(DinsideUnitGetChanged(int)));


	connect(&m_regionsVisible_checkBx, SIGNAL(clicked(bool)), this, SLOT(regionVisibilityGetChanged(bool)));
	connect(&m_backgroundVisibleInLiveAndExp_checkBx, SIGNAL(clicked(bool)), this, SLOT(backgroundVisiblityInLiveAndExpGetChanged(bool)));
	connect(&m_backgroundVisibleInGeom_checkBox, SIGNAL(clicked(bool)), this, SLOT(backgroundVisiblityInGeomGetChanged(bool)));
	connect(&m_imageSmoothing_checkBox, SIGNAL(clicked(bool)), this, SLOT(imageSmoothingGetChanged(bool)));
    connect(&m_spotIntensity_spinBxedSlider, SIGNAL(prefixIndexGetChanged(int)), this, SLOT(spotIntensityUnitGetChanged(int)));
    connect(&m_photonConversion_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(ADCountsPerPhotonGetChanged(double)));
    connect(&m_autoscale_checkBx, SIGNAL(clicked(bool)), this, SLOT(autoscaleGetChanged(bool)));

	connect(&m_measuredRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(measuredRegiongetChanged(QString)));
	connect(&m_measuredRegion_comboBx, SIGNAL(highlighted(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_measuredRegion_comboBx, SIGNAL(popUpGetShown(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_measuredRegion_comboBx, SIGNAL(focusGetLost()), this, SLOT(regionComboBoxFocusGetLost()));
	connect(&m_liveExperimentType_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(liveExperimentTypeGetChanged(int)));
	connect(&m_bleachedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(bleachedRegionGetChanged(QString)));
	connect(&m_bleachedRegion_comboBx, SIGNAL(highlighted(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_bleachedRegion_comboBx, SIGNAL(focusGetLost()), this, SLOT(regionComboBoxFocusGetLost()));
    connect(&m_photoActivatedRegion_comboBx, SIGNAL(currentIndexChanged(QString)), this, SLOT(photoActivatedRegionGetChanged(QString)));
	connect(&m_photoActivatedRegion_comboBx, SIGNAL(highlighted(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_photoActivatedRegion_comboBx, SIGNAL(focusGetLost()), this, SLOT(regionComboBoxFocusGetLost()));

	connect(&m_startBleaching_pushButton, SIGNAL(clicked()), this, SLOT(regionGetBleached()));
	connect(&m_startPhotoActivation_pushButton, SIGNAL(clicked()), this, SLOT(regionGetPhotoActivated()));

	connect(&m_experimentType_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(experimentTypeGetChanged(int)));
	connect(&m_acquisitionMode_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(acquisitionTypeGetChanged(int)));
	connect(&m_timelapse_period_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(acquisitionPeriodGetChanged(double)));
	connect(&m_presequ_spinBox, SIGNAL(valueChanged(int)), this, SLOT(acquisitionPresequenceGetChanged(int)));
	connect(&m_repetitioon_spinBox, SIGNAL(valueChanged(int)), this, SLOT(NbRepetitionGetChanged(int)));
	connect(&m_initialEnrichment_spinBox, SIGNAL(valueChanged(double)), this, SLOT(initialEnrichmentGetChanged(double)));
	connect(&m_frapPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(frapPlaneIdxGetChanged(int)));
	connect(&m_frapDPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(frapDPlaneGetChanged(int)));
	connect(&m_frapKoff_dspinBox, SIGNAL(valueChanged(double)), this, SLOT(frapKoffGetChanged(double)));
	connect(&m_photoActivationPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(photoActivationPlaneIdxGetChanged(int)));
	connect(&m_photoActivationDPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(photoActivationDPlaneGetChanged(int)));
	connect(&m_photoActivationKon_dspinBox, SIGNAL(valueChanged(double)), this, SLOT(photoActivationKonGetChanged(double)));
	connect(&m_FCSBeamIsEnabled_checkBox, SIGNAL(clicked(bool)), this, SLOT(FCSBeamIsEnabledGetChanged(bool)));
	connect(&m_FCSBeamSigma_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(FCSBeamSigmaGetChanged(double)));
	connect(&m_FCSBeamMaxIntensity_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(FCSBeamMaxIntensityGetChanged(double)));
	connect(&m_FCSBeamKoff_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(FCSBeamKoffGetChanged(double)));

	connect(&m_SRIPointingAccuracy_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(SRIPointingAccuracyGetChanged(double)));
	connect(&m_SRIIntensity_spinBx, SIGNAL(valueChanged(int)), this, SLOT(SRIIntensityGetChanged(int)));
	connect(&m_SRIZoomLevel_spinBx, SIGNAL(valueChanged(int)), this, SLOT(SRIZoomLevelGetChanged(int)));

	connect(&m_drugAdminParamChanged_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(drugAdminParamChanged(int)));
	connect(&m_drugAdminPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(drugAdminPlaneChanged(int)));
	connect(&m_drugAdminNewValue_dspinBox, SIGNAL(valueChanged(double)), this, SLOT(drugAdminNewValueChanged(double)));

	connect(&m_duration_spinBox, SIGNAL(valueChanged(int)), this, SLOT(acquisitionNPlanesGetChanged(int)));
	connect(&m_fileDestination_pushButton, SIGNAL(clicked()), this, SLOT(fileDestinationButtonHasBeenClicked()));
	connect(&m_exportStack_checkBx, SIGNAL(clicked(bool)), this, SLOT(isExportingStackGetChanged(bool)));
    connect(&m_photonBackground_spinBx, SIGNAL(valueChanged(int)), this, SLOT(photonBackgroundHasBeenChanged(int)));
	connect(&m_stackPoissonN_checkBx, SIGNAL(clicked(bool)), this, SLOT(isStackUsingPoissonNoiseGetChanged(bool)));
    connect(&m_cameraOffset_spinBx, SIGNAL(valueChanged(int)), this, SLOT(cameraOffsetHasBeenChanged(int)));
    connect(&m_cameraReadoutNoise_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(cameraReadoutNoiseHasBeenChanged(double)));

	connect(&m_addMeasuredRegion_pushButton, SIGNAL(clicked()), this, SLOT(addRegionMeasuredRegionsGetClicked()));
	connect(&m_allMeasuredRegions_comboBx, SIGNAL(highlighted(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_allMeasuredRegions_comboBx, SIGNAL(focusGetLost()), this, SLOT(regionComboBoxFocusGetLost()));
    connect(&m_removeMeasuredRegion_pushButton, SIGNAL(clicked()), this, SLOT(removeRegionMeasuredRegionsGetClicked()));
	connect(&m_addBleachedRegion_pushButton, SIGNAL(clicked()), this, SLOT(addRegionBleachedRegionsGetClicked()));
	connect(&m_allBleachedRegions_comboBx, SIGNAL(highlighted(QString)), this, SLOT(regionGetHighlighted(QString)));
    connect(&m_allBleachedRegions_comboBx, SIGNAL(focusGetLost()), this, SLOT(regionComboBoxFocusGetLost()));
    connect(&m_removeBleachedRegion_pushButton, SIGNAL(clicked()), this, SLOT(removeRegionBleachedRegionsGetClicked()));
}

void FluoSimView::onSliderValueGetChanged(int slider_id)
{
	//VIEW
	float value = updateSliderLabelValue((SLIDER_ID) slider_id);

    //->CONTROL
	sliderValueGetChanged((SLIDER_ID) slider_id, value);
}

void FluoSimView::onSliderValueGetChanged(int slider_id, double value)
{
    //VIEW
    //->CONTROL
    sliderValueGetChanged((SLIDER_ID) slider_id, value);
}



void FluoSimView::onActionTriggered(int action_id)
{
	//->CONTROL
	actionGetTriggered((ACTION_ID) action_id);
}

float FluoSimView::updateSliderLabelValue(SLIDER_ID slider_id)
{
	float value;

	float D_value;
	float K_value;
	float crossing_value;

	switch(slider_id)
	{
		case imageIntensity_slider :
        {
			float ratio = float(m_imageIntensity_slider.value() - m_imageIntensity_slider.minimum())/
						  (m_imageIntensity_slider.maximum() - m_imageIntensity_slider.minimum());

			value = m_imageIntensity_slider_minValue +
					ratio*(m_imageIntensity_slider_maxValue - m_imageIntensity_slider_minValue);

			char* data_cstr = new char[5+1+1];
			sprintf(data_cstr, "%5.1f", value);

			m_imageIntensityValue_label.setText("X" + QString(data_cstr));
			delete[] data_cstr;

		}
		break;
    }
	return value;
}

void FluoSimView::updateDrugNewValueSpinBox()
{
	switch(m_drugAdminParamChanged_comboBx.currentIndex())
	{
		case DOUT_DRUGPARAM :
		{
            m_drugAdminNewValue_dspinBox.setPrefix("[µm²/s] ");
			m_drugAdminNewValue_dspinBox.setMinimum(m_DSlider_minValue);
			m_drugAdminNewValue_dspinBox.setMaximum(m_DSlider_maxValue);
			m_drugAdminNewValue_dspinBox.setDecimals(3);
			m_drugAdminNewValue_dspinBox.setSingleStep(0.1);

            string str = m_outsideD_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();

            float value;
            value = atof(str.data());

			m_drugAdminNewValue_dspinBox.setValue(value);
		}
		break;

		case DIN_DRUGPARAM :
		{
            m_drugAdminNewValue_dspinBox.setPrefix("[µm²/s] ");
			m_drugAdminNewValue_dspinBox.setMinimum(0);
            m_drugAdminNewValue_dspinBox.setMaximum(2);
            m_drugAdminNewValue_dspinBox.setDecimals(3);
            m_drugAdminNewValue_dspinBox.setSingleStep(0.001);

            string str = m_insideD_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
            float value;
            if(m_insideD_dSpinBxedSlider.getComboBoxIndex() == DIFFUSION_UNIT_COMBOBOX_UM2PERSEC) \
                value = atof(str.data());
            else //DIFFUSION_UNIT_COMBOX_PERCENT
            {
                string str2 = m_outsideD_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
                float DOutside = atof(str.data()), DInsidePercent = atof(str2.data());
                value = DOutside*DInsidePercent/100.0f;
            }

			m_drugAdminNewValue_dspinBox.setValue(value);

		}
		break;

		case DTRAP_DRUGPARAM :
		{
            m_drugAdminNewValue_dspinBox.setPrefix("[µm²/s] ");
			m_drugAdminNewValue_dspinBox.setMinimum(m_DSlider_minValue);
			m_drugAdminNewValue_dspinBox.setMaximum(m_DSlider_maxValue);
			m_drugAdminNewValue_dspinBox.setDecimals(3);
			m_drugAdminNewValue_dspinBox.setSingleStep(0.1);

            string str = m_trappedD_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
            float value;
            value = atof(str.data());

			m_drugAdminNewValue_dspinBox.setValue(value);
		}
		break;

		case KON_DRUGPARAM :
		{
			m_drugAdminNewValue_dspinBox.setPrefix("[1/s] ");
			m_drugAdminNewValue_dspinBox.setMinimum(m_KSlider_minValue);
			m_drugAdminNewValue_dspinBox.setMaximum(m_KSlider_maxValue);
			m_drugAdminNewValue_dspinBox.setDecimals(3);
			m_drugAdminNewValue_dspinBox.setSingleStep(0.1);

            string str = m_kon_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
            float value;
            value = atof(str.data());

			m_drugAdminNewValue_dspinBox.setValue(value);
		}
		break;

		case KOFF_DRUGPARAM :
		{
			m_drugAdminNewValue_dspinBox.setPrefix("[1/s] ");
			m_drugAdminNewValue_dspinBox.setMinimum(m_KSlider_minValue);
			m_drugAdminNewValue_dspinBox.setMaximum(m_KSlider_maxValue);
			m_drugAdminNewValue_dspinBox.setDecimals(3);
			m_drugAdminNewValue_dspinBox.setSingleStep(0.1);

            string str = m_koff_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
            float value;
            value = atof(str.data());

			m_drugAdminNewValue_dspinBox.setValue(value);
		}
		break;

		case POROS_DRUGPARAM :
		{
			m_drugAdminNewValue_dspinBox.setPrefix("[ ] ");
			m_drugAdminNewValue_dspinBox.setMinimum(0);
			m_drugAdminNewValue_dspinBox.setMaximum(1.0);
			m_drugAdminNewValue_dspinBox.setDecimals(2);
			m_drugAdminNewValue_dspinBox.setSingleStep(0.1);

            string str = m_crossingProbability_dSpinBxedSlider.getSpinBoxText().toLocal8Bit().data();
            float value;
            value = atof(str.data());

			m_drugAdminNewValue_dspinBox.setValue(value);
		}
		break;
	}
}


bool FluoSimView::pollEvents()
{
    //not used...
	return false;//m_scrn.pollEvents();
}

void FluoSimView::buildParameterWidget()
{
	string app_dir = m_main_app->applicationDirPath().toLocal8Bit().data();

		m_geometryOnglet_pushButton.setText("GEOMETRY");
		m_liveOnglet_pushButton.setText("LIVE");
		m_experimentOnglet_pushButton.setText("EXPERIMENT");
		m_analysisOnglet_pushButton.setText("SPT ANALYSIS");
	m_onglets_layout.addWidget(&m_geometryOnglet_pushButton);
	m_onglets_layout.addWidget(&m_liveOnglet_pushButton);
	m_onglets_layout.addWidget(&m_experimentOnglet_pushButton);
	m_onglets_layout.addWidget(&m_analysisOnglet_pushButton);
	m_onglets_layout.setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_onglets_layout.setSpacing(0);

//*******************************
//*	   1st Panel : GEOMETRY	*
//*******************************

//	Background Image drop-menu
				m_loadBackgroundImage_lineEdit.setReadOnly(true);
				m_loadBackgroundImage_lineEdit.setText("");
				m_loadBackgroundImage_pushButton.setText("Browse...");
				m_loadBackgroundImage_Hlayout.addWidget(&m_loadBackgroundImage_lineEdit);
				m_loadBackgroundImage_Hlayout.addWidget(&m_loadBackgroundImage_pushButton);

				m_imageIntensity_label.setText("Image Intensity:");
				m_imageIntensity_slider.setOrientation(Qt::Horizontal);
				m_imageIntensity_slider_minValue = 0;
				m_imageIntensity_slider_maxValue = 10;
				m_imageIntensity_slider.setMinimum(0);
				m_imageIntensity_slider.setMaximum(1000);
				m_imageIntensity_slider.setValue(100);
				m_imageIntensity_Hlayout.addWidget(&m_imageIntensity_label);
				m_imageIntensity_Hlayout.addWidget(&m_imageIntensity_slider);
				m_imageIntensity_Hlayout.addWidget(&m_imageIntensityValue_label);

				m_imageVisibility_label.setText("Display Image: ");
				m_backgroundVisibleInGeom_checkBox.setChecked(true);
				m_imageSmoothing_label.setText("Smooth Image: ");
				m_imageSmoothing_checkBox.setChecked(false);
				m_imageVisibilityAndSmoothing_Hlayout.addStretch(1);
				m_imageVisibilityAndSmoothing_Hlayout.addWidget(&m_imageVisibility_label);
				m_imageVisibilityAndSmoothing_Hlayout.addWidget(&m_backgroundVisibleInGeom_checkBox);
				m_imageVisibilityAndSmoothing_Hlayout.addStretch(1);
				m_imageVisibilityAndSmoothing_Hlayout.addWidget(&m_imageSmoothing_label);
				m_imageVisibilityAndSmoothing_Hlayout.addWidget(&m_imageSmoothing_checkBox);
				m_imageVisibilityAndSmoothing_Hlayout.addStretch(1);
				m_imageVisibilityAndSmoothing_Hlayout.setAlignment(Qt::AlignHCenter);

			m_backgroundImage_vlayout.setMargin(3);
			m_backgroundImage_vlayout.setContentsMargins(7,5,7,6);
			m_backgroundImage_vlayout.setSpacing(2);
			m_backgroundImage_vlayout.addLayout(&m_loadBackgroundImage_Hlayout);
			m_backgroundImage_vlayout.addLayout(&m_imageIntensity_Hlayout);
			m_backgroundImage_vlayout.addLayout(&m_imageVisibilityAndSmoothing_Hlayout);

		m_backgroundImageFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
		m_backgroundImageFrame.setLayout(&m_backgroundImage_vlayout);

	m_backgroundImage_dropMenu.setText("Background Image");
	m_backgroundImage_dropMenu.setWidget(&m_backgroundImageFrame);

//	Simulation Length Scale
		m_pixelSize_label.setText("Geometry and Image Pixel Size: [µm] ");
		m_pixelSize_dSpinBx.setMinimum(0.001);
		m_pixelSize_dSpinBx.setMaximum(1.000);
		m_pixelSize_dSpinBx.setValue(0);
		m_pixelSize_dSpinBx.setDecimals(3);
		m_pixelSize_dSpinBx.setSingleStep(0.005);
		m_pixelSize_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);

	m_pixelSize_layout.addWidget(&m_pixelSize_label);
	m_pixelSize_layout.addWidget(&m_pixelSize_dSpinBx);
	m_pixelSize_layout.addStretch(10);

	m_simulation_layout.setMargin(3);
	m_simulation_layout.setContentsMargins(7,5,7,6);
	m_simulation_layout.setSpacing(2);
	m_simulation_layout.addLayout(&m_pixelSize_layout);

	m_simulationFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_simulationFrame.setLayout(&m_simulation_layout);
	m_simulation_dropMenu.setText("Simulation Length Scale");
	m_simulation_dropMenu.setWidget(&m_simulationFrame);


//	AllRegion drop-menu
				m_importGeometry_pushButton.setText("Import Geometries...");
				m_exportGeometry_pushButton.setText("Export Geometries...");
			m_importExportGeometry_layout.addWidget(&m_importGeometry_pushButton);
			m_importExportGeometry_layout.addWidget(&m_exportGeometry_pushButton);

				m_deleteRegionAllRegion_pushButton.setText("Delete Region");
				m_deleteAllRegionAllRegion_pushButton.setText("Reset Geometry");
			m_deleteRegions_hLayout.addWidget(&m_deleteRegionAllRegion_pushButton);
			m_deleteRegions_hLayout.addWidget(&m_deleteAllRegionAllRegion_pushButton);

			m_allRegions_tableWidget.setColumnCount(3);
			m_allRegions_tableWidget.verticalHeader()->setDefaultSectionSize(m_allRegions_tableWidget.verticalHeader()->minimumSectionSize());
			m_allRegions_tableWidget.verticalHeader()->setVisible(false);
			m_allRegions_tableWidget.horizontalHeader()->setStretchLastSection(true);
			m_allRegions_tableWidget.setMinimumHeight(160);
			m_allRegions_tableWidget.setMouseTracking(true);
			m_allRegions_tableWidget.setHorizontalHeaderLabels({"Name", "Contour", "Trapping"});
			m_allRegions_tableWidget.horizontalHeader()->setSectionsClickable(false);
            m_allRegions_tableWidget.setStyleSheet("QTableWidget  {font-size : 11px; background-color : rgb(72,72,72);}"
                                                   "QHeaderView::section {font-size : 11px; color : rgb(240,240,240); background-color : rgb(72,72,72);}"
												   "QTableWidget::item:selected {background-color : rgb(100,100,100);}"
												   "QTableWidget::item:selected {color : rgb(240,188,23);}");
		m_allRegions_vlayout.setMargin(3);
		m_allRegions_vlayout.setContentsMargins(7,5,7,6);
		m_allRegions_vlayout.setSpacing(2);
		m_allRegions_vlayout.addLayout(&m_importExportGeometry_layout);
		m_allRegions_vlayout.addWidget(&m_allRegions_tableWidget);
		m_allRegions_vlayout.addLayout(&m_deleteRegions_hLayout);

		m_allRegionsFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
		m_allRegionsFrame.setLayout(&m_allRegions_vlayout);

	m_allRegions_dropMenu.setText("Regions");
	m_allRegions_dropMenu.setWidget(&m_allRegionsFrame);

//*******************************
//*	   2nd Panel: LIVE      	*
//*******************************
		m_play_pushBut.setFixedSize(15,15);
		m_pause_pushBut.setFixedSize(15,15);
		m_stop_pushBut.setFixedSize(15,15);
		m_simulation_progressBar.setOrientation(Qt::Horizontal);
		m_simulation_progressBar.setMinimum(0);
		m_simulation_progressBar.setMaximum(100);
		m_simulation_progressBar.setValue(0);
		m_simulation_progressBar.setTextVisible(0);
		m_simulationMaxPlane_dSpinBx.setMinimum(0);
		m_simulationMaxPlane_dSpinBx.setMaximum(1000000);
		m_simulationMaxPlane_dSpinBx.setValue(10000);
		m_simulationMaxPlane_dSpinBx.setDecimals(0);
		m_simulationMaxPlane_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);

	m_simulationStatesButtons_layout.setMargin(3);
	m_simulationStatesButtons_layout.setContentsMargins(7,0,7,1);
	m_simulationStatesButtons_layout.setSpacing(2);
	m_simulationStatesButtons_layout.addWidget(&m_play_pushBut);
	m_simulationStatesButtons_layout.addWidget(&m_pause_pushBut);
	m_simulationStatesButtons_layout.addWidget(&m_stop_pushBut);
	m_simulationStatesButtons_layout.addWidget(&m_simulation_progressBar);
	m_simulationStatesButtons_layout.addWidget(&m_simulationMaxPlane_dSpinBx);

		m_timeStep_label.setText("Simulation Time Step: [s]");
		m_timeStep_dSpinBx.setMinimum(0);
		m_timeStep_dSpinBx.setMaximum(1);
		m_timeStep_dSpinBx.setValue(0);
		m_timeStep_dSpinBx.setDecimals(6);
		m_timeStep_dSpinBx.setSingleStep(0.002);
		m_timeStep_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);

	m_timeStep_layout.addStretch(10);
	m_timeStep_layout.addWidget(&m_timeStep_label);
	m_timeStep_layout.addWidget(&m_timeStep_dSpinBx);
	m_timeStep_layout.addStretch(10);

	m_simulationStates_vlayout.addLayout(&m_simulationStatesButtons_layout);
	m_simulationStates_vlayout.addLayout(&m_timeStep_layout);
	m_simulationStatesFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_simulationStatesFrame.setLayout(&m_simulationStates_vlayout);
	m_simulationStates_dropMenu.setText("Simulation Progress");
	m_simulationStates_dropMenu.setWidget(&m_simulationStatesFrame);

//First Frame

	//first line
    m_outsideD_dSpinBxedSlider.setText("Diffusion Outside:");
    m_outsideD_dSpinBxedSlider.setSpinBoxPrefix("[µm²/s]");
    m_outsideD_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
    m_outsideD_dSpinBxedSlider.setMin(m_DSlider_minValue);
    m_outsideD_dSpinBxedSlider.setMax(m_DSlider_maxValue);
    m_outsideD_dSpinBxedSlider.setValue(0);
    m_outsideD_dSpinBxedSlider.setStep(0.001);
    m_outsideD_dSpinBxedSlider.setDecimals(3);

	//second line
    m_insideD_dSpinBxedSlider.setText("Diffusion Inside:");
    m_insideD_dSpinBxedSlider.setSpinBoxPrefixMode(myDoubleSpinBoxedSlider::QCOMBOBX_PREFIX_MODE);
    m_insideD_dSpinBxedSlider.setSpinBoxPrefixes(QStringList({"[%Dout]   ","[µm²/s]"}));
    m_insideD_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
    m_insideD_dSpinBxedSlider.setMin(0);
    m_insideD_dSpinBxedSlider.setMax(100);
    m_insideD_dSpinBxedSlider.setValue(0);
    m_insideD_dSpinBxedSlider.setStep(0.1);
    m_insideD_dSpinBxedSlider.setDecimals(1);

	//third line
        m_trappedD_dSpinBxedSlider.setText("Diffusion Trapped:");
        m_trappedD_dSpinBxedSlider.setSpinBoxPrefix("[µm²/s]");
        m_trappedD_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_trappedD_dSpinBxedSlider.setMin(m_DSlider_minValue);
        m_trappedD_dSpinBxedSlider.setMax(m_DSlider_maxValue);
        m_trappedD_dSpinBxedSlider.setValue(0);
        m_trappedD_dSpinBxedSlider.setStep(0.001);
        m_trappedD_dSpinBxedSlider.setDecimals(3);

	//fourth line
        m_kon_dSpinBxedSlider.setText("Binding Rate:");
        m_kon_dSpinBxedSlider.setSpinBoxPrefix("[1/s]");
        m_kon_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_kon_dSpinBxedSlider.setMin(m_KSlider_minValue);
        m_kon_dSpinBxedSlider.setMax(m_KSlider_maxValue);
        m_kon_dSpinBxedSlider.setValue(0);
        m_kon_dSpinBxedSlider.setStep(0.001);
        m_kon_dSpinBxedSlider.setDecimals(3);

	//fifth line
        m_koff_dSpinBxedSlider.setText("Unbinding Rate:");
        m_koff_dSpinBxedSlider.setSpinBoxPrefix("[1/s]");
        m_koff_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_koff_dSpinBxedSlider.setMin(m_KSlider_minValue);
        m_koff_dSpinBxedSlider.setMax(m_KSlider_maxValue);
        m_koff_dSpinBxedSlider.setValue(0);
        m_koff_dSpinBxedSlider.setStep(0.001);
        m_koff_dSpinBxedSlider.setDecimals(3);

	//sixth line
        m_crossingProbability_dSpinBxedSlider.setText("Crossing Probability:");
        m_crossingProbability_dSpinBxedSlider.setSpinBoxPrefix("");
        m_crossingProbability_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_crossingProbability_dSpinBxedSlider.setMin(0);
        m_crossingProbability_dSpinBxedSlider.setMax(1);
        m_crossingProbability_dSpinBxedSlider.setValue(0);
        m_crossingProbability_dSpinBxedSlider.setStep(0.01);
        m_crossingProbability_dSpinBxedSlider.setDecimals(2);

        m_immobileFraction_label.setText("Immobile Fraction: [%]");
		m_immobileFraction_dSpinBx.setMinimum(0.00f);
		m_immobileFraction_dSpinBx.setMaximum(100.00f);
		m_immobileFraction_dSpinBx.setDecimals(2);
		m_immobileFraction_dSpinBx.setSingleStep(1);
		m_immobileFraction_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_fixation_label.setText("Fixation:");
		m_fixation_checkBx.setChecked(false);
	m_immobileFraction_layout.addWidget(&m_immobileFraction_label);
	m_immobileFraction_layout.addWidget(&m_immobileFraction_dSpinBx);
	m_immobileFraction_layout.addStretch(5);
	m_immobileFraction_layout.addWidget(&m_fixation_label);
	m_immobileFraction_layout.addWidget(&m_fixation_checkBx);
	m_immobileFraction_layout.addStretch(5);

	m_rows_layout.setMargin(3);
	m_rows_layout.setContentsMargins(7,5,7,6);
	m_rows_layout.setSpacing(2);
    m_rows_layout.addWidget(&m_outsideD_dSpinBxedSlider);
    m_rows_layout.addWidget(&m_insideD_dSpinBxedSlider);
    m_rows_layout.addWidget(&m_crossingProbability_dSpinBxedSlider);
    m_rows_layout.addWidget(&m_kon_dSpinBxedSlider);
    m_rows_layout.addWidget(&m_koff_dSpinBxedSlider);
    m_rows_layout.addWidget(&m_trappedD_dSpinBxedSlider);
    m_rows_layout.addLayout(&m_immobileFraction_layout);

	m_dynamicFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_dynamicFrame.setLayout(&m_rows_layout);

	m_dynamic_dropMenu.setText("Kinetic Parameters");
	m_dynamic_dropMenu.setWidget(&m_dynamicFrame);


//third frame

	//first line
    m_nbParticles_spinBxedSlider.setText("Copy Number:");
    m_nbParticles_spinBxedSlider.setSpinBoxPrefix("");
    m_nbParticles_spinBxedSlider.setMin(0);
    m_nbParticles_spinBxedSlider.setMax(150000);
    m_nbParticles_spinBxedSlider.setValue(0);
    m_nbParticles_spinBxedSlider.setSpinBoxButtonSymbols(QAbstractSpinBox::NoButtons);
		m_evaluateSteadyState_pushBut.setText("Evaluate Steady State...");
	m_evaluateSteadyState_layout.addWidget(&m_evaluateSteadyState_pushBut);

	m_particleSystemFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_particleSystemProperties_Vlayout.setMargin(3);
	m_particleSystemProperties_Vlayout.setContentsMargins(7,5,7,6);
	m_particleSystemProperties_Vlayout.setSpacing(2);
    m_particleSystemProperties_Vlayout.addWidget(&m_nbParticles_spinBxedSlider);
	m_particleSystemProperties_Vlayout.addLayout(&m_evaluateSteadyState_layout);
	m_particleSystemFrame.setLayout(&m_particleSystemProperties_Vlayout);

	m_particleSystem_dropMenu.setText("Molecules");
	m_particleSystem_dropMenu.setWidget(&m_particleSystemFrame);

//fourth frame

	//second line
        m_blinkingKon_dSpinBxedSlider.setText("Switch-on Rate: ");
        m_blinkingKon_dSpinBxedSlider.setSpinBoxPrefix("[1/s]");
        m_blinkingKon_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_blinkingKon_dSpinBxedSlider.setMin(m_blinkingKSlider_minValue);
        m_blinkingKon_dSpinBxedSlider.setMax(m_blinkingKSlider_maxValue);
        m_blinkingKon_dSpinBxedSlider.setValue(0);
        m_blinkingKon_dSpinBxedSlider.setStep(0.001);
        m_blinkingKon_dSpinBxedSlider.setDecimals(3);

	//third line
        m_blinkingKoff_dSpinBxedSlider.setText("Switch-off Rate: ");
        m_blinkingKoff_dSpinBxedSlider.setSpinBoxPrefix("[1/s]");
        m_blinkingKoff_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_blinkingKoff_dSpinBxedSlider.setMin(m_blinkingKSlider_minValue);
        m_blinkingKoff_dSpinBxedSlider.setMax(m_blinkingKSlider_maxValue);
        m_blinkingKoff_dSpinBxedSlider.setValue(0);
        m_blinkingKoff_dSpinBxedSlider.setStep(0.001);
        m_blinkingKoff_dSpinBxedSlider.setDecimals(3);

	m_PhotoPhysics_Vlayout.setMargin(3);
	m_PhotoPhysics_Vlayout.setContentsMargins(7,5,7,6);
	m_PhotoPhysics_Vlayout.setSpacing(2);
    m_PhotoPhysics_Vlayout.addWidget(&m_blinkingKon_dSpinBxedSlider);
    m_PhotoPhysics_Vlayout.addWidget(&m_blinkingKoff_dSpinBxedSlider);

	m_PhotoPhysicsFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_PhotoPhysicsFrame.setLayout(&m_PhotoPhysics_Vlayout);

	m_PhotoPhysics_dropMenu.setText("PhotoPhysics Parameters");
	m_PhotoPhysics_dropMenu.setWidget(&m_PhotoPhysicsFrame);


//fifth frame

		//first line
			m_regionsVisible_label.setText("Display Regions: ");
			m_regionsVisible_checkBx.setFixedSize(9,9);
			m_regionsVisible_checkBx.setCheckState(Qt::Checked);
		m_regionsVisible_layout.addWidget(&m_regionsVisible_label);
		m_regionsVisible_layout.addWidget(&m_regionsVisible_checkBx);

		//second line
			m_backgroundVisible_label.setText("Display Background Image: ");
			m_backgroundVisibleInLiveAndExp_checkBx.setGeometry(0,0, 20,20);
			m_backgroundVisibleInLiveAndExp_checkBx.setCheckState(Qt::Checked);
		m_backgroundVisible_layout.addWidget(&m_backgroundVisible_label);
		m_backgroundVisible_layout.addWidget(&m_backgroundVisibleInLiveAndExp_checkBx);

	m_propertiesVisibility_layout.addLayout(&m_regionsVisible_layout);
	m_propertiesVisibility_layout.addStretch(10);
	m_propertiesVisibility_layout.addLayout(&m_backgroundVisible_layout);

		m_particleColor_label.setText("Color: ");
		m_particleColor_comboBx.addItem("RED");
		m_particleColor_comboBx.addItem("GREEN");
		m_particleColor_comboBx.addItem("BLUE");
		m_particleColor_comboBx.addItem("WHITE");
		m_particleColor_comboBx.addItem("RANDOM");
		m_particleColor_comboBx.addItem("TRAPPING");
		m_particleColor_comboBx.addItem("LOOKUP TABLE");

		m_renderingMode_label.setText("Motif: ");
		m_renderingMode_comboBx.addItem("POINT");
		m_renderingMode_comboBx.addItem("GAUSSIAN");

	m_renderingParticle_layout.addWidget(&m_renderingMode_label);
	m_renderingParticle_layout.addWidget(&m_renderingMode_comboBx);
	m_renderingParticle_layout.addStretch(3);
	m_renderingParticle_layout.addWidget(&m_particleColor_label);
	m_renderingParticle_layout.addWidget(&m_particleColor_comboBx);


	//third line
        m_spotSize_dSpinBxedSlider.setText("Spot Size (SD) : ");
        m_spotSize_dSpinBxedSlider.setSpinBoxPrefix("[µm]");
        m_spotSize_dSpinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_spotSize_dSpinBxedSlider.setMin(m_spotSizeSlider_minValue);
        m_spotSize_dSpinBxedSlider.setMax(m_spotSizeSlider_maxValue);
        m_spotSize_dSpinBxedSlider.setValue(0);
        m_spotSize_dSpinBxedSlider.setStep(0.001);
        m_spotSize_dSpinBxedSlider.setDecimals(3);


	//fourth line
        m_spotIntensity_spinBxedSlider.setText("Spot Intensity : ");
        m_spotIntensity_spinBxedSlider.setSpinBoxPrefixMode(myDoubleSpinBoxedSlider::QCOMBOBX_PREFIX_MODE);
        m_spotIntensity_spinBxedSlider.setSpinBoxPrefixes({"A/D counts   ", "photons/s   "});
        m_spotIntensity_spinBxedSlider.setSpinBoxButtonSymbols(QSpinBox::NoButtons);
        m_spotIntensity_spinBxedSlider.setMin(m_spotIntensitySlider_minValue); //range for A/D counts
        m_spotIntensity_spinBxedSlider.setMax(m_spotIntensitySlider_maxValue);
        m_spotIntensity_spinBxedSlider.setValue(0);
        m_spotIntensity_spinBxedSlider.setStep(1);
    m_renderingPropertiesSliders_Vlayout.addWidget(&m_spotSize_dSpinBxedSlider);
    m_renderingPropertiesSliders_Vlayout.addWidget(&m_spotIntensity_spinBxedSlider);

    //fifth line
        m_autoscale_label.setText("Autoscale : ");
        m_autoscale_checkBx.setChecked(true);
        m_photonConversion_label.setText("Gain [A/D counts/photon]:");
        m_photonConversion_dSpinBx.setDecimals(2);
        m_photonConversion_dSpinBx.setMinimum(m_photonConversion_minValue);
        m_photonConversion_dSpinBx.setMaximum(m_photonConversion_maxValue);
        m_photonConversion_dSpinBx.setAlignment(Qt::AlignRight);
        m_photonConversion_dSpinBx.setButtonSymbols(QDoubleSpinBox::NoButtons);
        m_photonConversion_dSpinBx.setValue(1);

        m_autoscale_hLayout.addWidget(&m_photonConversion_label);
        m_autoscale_hLayout.addWidget(&m_photonConversion_dSpinBx);
        m_autoscale_hLayout.addStretch(10);
        m_autoscale_hLayout.addWidget(&m_autoscale_label);
        m_autoscale_hLayout.addWidget(&m_autoscale_checkBx);
            m_autoscale_hLayout.setAlignment(&m_autoscale_label, Qt::AlignRight);
            m_autoscale_hLayout.setAlignment(&m_autoscale_checkBx, Qt::AlignRight);
            m_autoscale_wLayout.setLayout(&m_autoscale_hLayout);
            m_autoscale_hLayout.setMargin(0);
            m_autoscale_hLayout.setSpacing(1);

	m_renderingProperties_Vlayout.addLayout(&m_propertiesVisibility_layout);
	m_renderingProperties_Vlayout.addLayout(&m_renderingParticle_layout);
	m_renderingProperties_Vlayout.addLayout(&m_renderingPropertiesSliders_Vlayout);
    m_renderingProperties_Vlayout.addWidget(&m_autoscale_wLayout);
        m_renderingProperties_Vlayout.setMargin(3);
        m_renderingProperties_Vlayout.setContentsMargins(7,5,7,6);
        m_renderingProperties_Vlayout.setSpacing(2);

    m_renderingFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    m_renderingFrame.setLayout(&m_renderingProperties_Vlayout);

	m_rendering_dropMenu.setText("Rendering Parameters");
	m_rendering_dropMenu.setWidget(&m_renderingFrame);

		//first line
			m_liveExperimentType_label.setText("Illumination Type: ");
			m_liveExperimentType_comboBx.addItems({"Photobleaching", "Photoactivation"/*, "FCS EXPERIMENT"*/});
		m_liveExperimentType_layout.addWidget(&m_liveExperimentType_label);
		m_liveExperimentType_layout.addWidget(&m_liveExperimentType_comboBx);

		//second line
			m_measuredRegion_label.setText("Measured Rgn: ");
			m_measuredRegion_comboBx.addItem("No Regions");
		m_measuredRegion_layout.addWidget(&m_measuredRegion_label);
		m_measuredRegion_layout.addWidget(&m_measuredRegion_comboBx);


    //BLEACHING
		//third line
			m_bleachedRegion_label.setText("Photobleached Rgn: ");
			m_bleachedRegion_comboBx.addItem("No Regions");
		m_bleachedRegion_layout.addWidget(&m_bleachedRegion_label);
		m_bleachedRegion_layout.addWidget(&m_bleachedRegion_comboBx);

		//fourth line
			m_startBleaching_pushButton.setText("Start Photobleaching");
		m_startBleaching_layout.addWidget(&m_startBleaching_pushButton);

    //PHOTOACTIVATION
		//third line
			m_photoActivatedRegion_label.setText("PhotoActivated Rgn: ");
			m_photoActivatedRegion_comboBx.addItem("No Regions");
		m_photoActivatedRegion_layout.addWidget(&m_photoActivatedRegion_label);
		m_photoActivatedRegion_layout.addWidget(&m_photoActivatedRegion_comboBx);

		//fourth line
			m_startPhotoActivation_pushButton.setText("Start PhotoActivation");
		m_startPhotoActivation_layout.addWidget(&m_startPhotoActivation_pushButton);

	m_liveExperimentFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_liveExperimentProperties_Vlayout.setMargin(3);
	m_liveExperimentProperties_Vlayout.setContentsMargins(7,5,7,6);
	m_liveExperimentProperties_Vlayout.setSpacing(2);
	m_liveExperimentProperties_Vlayout.addLayout(&m_liveExperimentType_layout);
	m_liveExperimentProperties_Vlayout.addLayout(&m_bleachedRegion_layout);
	m_liveExperimentProperties_Vlayout.addLayout(&m_startBleaching_layout);
	m_liveExperimentProperties_Vlayout.addLayout(&m_photoActivatedRegion_layout);
	m_liveExperimentProperties_Vlayout.addLayout(&m_startPhotoActivation_layout);

	m_liveExperimentFrame.setLayout(&m_liveExperimentProperties_Vlayout);

	m_liveExperiment_dropMenu.setText("Live Measurements");
	m_liveExperiment_dropMenu.setWidget(&m_liveExperimentFrame);


//*******************************
//*	   3rd Panel: EXPERIMENT    *
//*******************************


		m_experimentType_label.setText("Experiment: ");
        QStringList experimentType_items({"SPT","FRAP","PAF","FCS", "SRI","DRUG"});
		m_experimentType_comboBx.addItems(experimentType_items);
    m_experimentType_layout.addWidget(&m_experimentType_label);
	m_experimentType_layout.addWidget(&m_experimentType_comboBx);

		m_acquisitionMode_label.setText("Acquisition Mode: ");
		QStringList acquisitionMode_items({"STREAM", "TIMELAPSE"});
		m_acquisitionMode_comboBox.addItems(acquisitionMode_items);
	m_acquisitionMode_layout.addWidget(&m_acquisitionMode_label);
	m_acquisitionMode_layout.addWidget(&m_acquisitionMode_comboBox);


		m_timelapse_period_label.setText("Sampling Period: ");
		m_timelapse_period_dSpinBox.setPrefix("[s] ");
		m_timelapse_period_dSpinBox.setMinimum(0);
		m_timelapse_period_dSpinBox.setMaximum(100);
		m_timelapse_period_dSpinBox.setValue(0);
		m_timelapse_period_dSpinBox.setDecimals(3);
		m_timelapse_period_dSpinBox.setSingleStep(m_timeStep_dSpinBx.value());
		m_timelapse_period_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_timelapse_period_layout.addWidget(&m_timelapse_period_label);
	m_timelapse_period_layout.addWidget(&m_timelapse_period_dSpinBox);


		m_presequ_label.setText("Presequence: ");
		m_presequ_spinBox.setPrefix("[frame] ");
		m_presequ_spinBox.setMinimum(0);
		m_presequ_spinBox.setMaximum(100000);
		m_presequ_spinBox.setValue(1000);
		m_presequ_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_presequ_layout.addWidget(&m_presequ_label);
	m_presequ_layout.addWidget(&m_presequ_spinBox);

		m_duration_label.setText("Duration: ");
		m_duration_spinBox.setPrefix("[frame] ");
		m_duration_spinBox.setMinimum(0);
		m_duration_spinBox.setMaximum(1000000);
		m_duration_spinBox.setValue(1000);
		m_duration_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_duration_layout.addWidget(&m_duration_label);
	m_duration_layout.addWidget(&m_duration_spinBox);

		m_repetition_label.setText("Repetition: ");
        m_repetitioon_spinBox.setMinimum(1);
        m_repetitioon_spinBox.setMaximum(1000);
		m_repetitioon_spinBox.setValue(1);
		m_repetitioon_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_repetition_layout.addWidget(&m_repetition_label);
	m_repetition_layout.addWidget(&m_repetitioon_spinBox);

		m_initialEnrichment_label.setText("Initial Enrichment: ");
		m_initialEnrichment_spinBox.setMinimum(-1.0f);
		m_initialEnrichment_spinBox.setMaximum(100.0f);
		m_initialEnrichment_spinBox.setDecimals(2);
		m_initialEnrichment_spinBox.setValue(1);
		m_initialEnrichment_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_initialEnrichment_layout.addWidget(&m_initialEnrichment_label);
	m_initialEnrichment_layout.addWidget(&m_initialEnrichment_spinBox);

	m_wProtocoleExperimentProperties_Vlayout.setMargin(3);
	m_wProtocoleExperimentProperties_Vlayout.setContentsMargins(7,5,7,6);
	m_wProtocoleExperimentProperties_Vlayout.setSpacing(2);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_experimentType_layout);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_acquisitionMode_layout);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_timelapse_period_layout);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_presequ_layout);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_duration_layout);
	m_wProtocoleExperimentProperties_Vlayout.addLayout(&m_repetition_layout);

	m_wProtocoleExperimentFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_wProtocoleExperimentFrame.setLayout(&m_wProtocoleExperimentProperties_Vlayout);

	m_wProtocoleExperiment_dropMenu.setText("General Parameters");
	m_wProtocoleExperiment_dropMenu.setWidget(&m_wProtocoleExperimentFrame);


	//*** FRAP experiments : ***
		m_frapPlane_label.setText("Bleaching Starting Frame: ");
		m_frapPlane_spinBox.setPrefix("[frame_idx] ");
		m_frapPlane_spinBox.setMinimum(0);
		m_frapPlane_spinBox.setMaximum(10000000);
		m_frapPlane_spinBox.setValue(100);
		m_frapPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_frapPlane_layout.addWidget(&m_frapPlane_label);
	m_frapPlane_layout.addWidget(&m_frapPlane_spinBox);

		m_frapDPlane_label.setText("Bleaching Number Frames: ");
		m_frapDPlane_spinBox.setPrefix("[frame] ");
		m_frapDPlane_spinBox.setMinimum(0);
		m_frapDPlane_spinBox.setMaximum(1000000);
		m_frapDPlane_spinBox.setValue(100);
		m_frapDPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_frapDPlane_layout.addWidget(&m_frapDPlane_label);
	m_frapDPlane_layout.addWidget(&m_frapDPlane_spinBox);

		m_frapKoff_label.setText("Bleaching Koff: ");
		m_frapKoff_dspinBox.setPrefix("[1/s] ");
		m_frapKoff_dspinBox.setMinimum(0);
		m_frapKoff_dspinBox.setMaximum(1000);
		m_frapKoff_dspinBox.setValue(100);
		m_frapKoff_dspinBox.setDecimals(3);
		m_frapKoff_dspinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_frapKoff_layout.addWidget(&m_frapKoff_label);
	m_frapKoff_layout.addWidget(&m_frapKoff_dspinBox);

	m_FRAPExperimentParameters_Vlayout.setMargin(3);
	m_FRAPExperimentParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_FRAPExperimentParameters_Vlayout.setSpacing(2);
	m_FRAPExperimentParameters_Vlayout.addLayout(&m_frapPlane_layout);
	m_FRAPExperimentParameters_Vlayout.addLayout(&m_frapDPlane_layout);
	m_FRAPExperimentParameters_Vlayout.addLayout(&m_frapKoff_layout);


	m_FRAPExperimentParameters_Frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_FRAPExperimentParameters_Frame.setLayout(&m_FRAPExperimentParameters_Vlayout);

	m_FRAPExperimentParameters_dropMenu.setText("FRAP Parameters");
	m_FRAPExperimentParameters_dropMenu.setWidget(&m_FRAPExperimentParameters_Frame);

	//*** PAF experiments : ***
		m_photoActivationPlane_label.setText("Activation Starting Frame: ");
		m_photoActivationPlane_spinBox.setPrefix("[frame_idx] ");
		m_photoActivationPlane_spinBox.setMinimum(0);
		m_photoActivationPlane_spinBox.setMaximum(1000000);
		m_photoActivationPlane_spinBox.setValue(100);
		m_photoActivationPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_photoActivationPlane_layout.addWidget(&m_photoActivationPlane_label);
	m_photoActivationPlane_layout.addWidget(&m_photoActivationPlane_spinBox);

		m_photoActivationDPlane_label.setText("Activation Number Frames: ");
		m_photoActivationDPlane_spinBox.setPrefix("[frame] ");
		m_photoActivationDPlane_spinBox.setMinimum(0);
		m_photoActivationDPlane_spinBox.setMaximum(1000000);
		m_photoActivationDPlane_spinBox.setValue(100);
		m_photoActivationDPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_photoActivationDPlane_layout.addWidget(&m_photoActivationDPlane_label);
	m_photoActivationDPlane_layout.addWidget(&m_photoActivationDPlane_spinBox);

		m_photoActivationKon_label.setText("Activation Rate: ");
		m_photoActivationKon_dspinBox.setPrefix("[1/s] ");
		m_photoActivationKon_dspinBox.setMinimum(0);
		m_photoActivationKon_dspinBox.setMaximum(1000);
		m_photoActivationKon_dspinBox.setValue(100);
		m_photoActivationKon_dspinBox.setDecimals(3);
		m_photoActivationKon_dspinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_photoActivationKon_layout.addWidget(&m_photoActivationKon_label);
	m_photoActivationKon_layout.addWidget(&m_photoActivationKon_dspinBox);


	m_PAFExperimentParameters_Vlayout.setMargin(3);
	m_PAFExperimentParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_PAFExperimentParameters_Vlayout.setSpacing(2);
	m_PAFExperimentParameters_Vlayout.addLayout(&m_photoActivationPlane_layout);
	m_PAFExperimentParameters_Vlayout.addLayout(&m_photoActivationDPlane_layout);
	m_PAFExperimentParameters_Vlayout.addLayout(&m_photoActivationKon_layout);

	m_PAFExperimentParameters_Frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_PAFExperimentParameters_Frame.setLayout(&m_PAFExperimentParameters_Vlayout);

	m_PAFExperimentParameters_dropMenu.setText("PAF Parameters");
	m_PAFExperimentParameters_dropMenu.setWidget(&m_PAFExperimentParameters_Frame);


	//*** FCS experiments : ***
		m_FCSBeamIsEnabled_label.setText("Enable Gaussian Beam:");
		m_FCSBeamIsEnabled_checkBox.setChecked(true);
	m_FCSBeamIsEnabled_layout.addWidget(&m_FCSBeamIsEnabled_label);
	m_FCSBeamIsEnabled_layout.addWidget(&m_FCSBeamIsEnabled_checkBox);
	m_FCSBeamIsEnabled_layout.setAlignment(&m_FCSBeamIsEnabled_checkBox, Qt::AlignLeft);

		m_FCSBeamSigma_label.setText("Beam Sigma: ");
		m_FCSBeamSigma_dSpinBox.setPrefix("[µm] ");
		m_FCSBeamSigma_dSpinBox.setMinimum(0.0f);
		m_FCSBeamSigma_dSpinBox.setMaximum(10.0f);
		m_FCSBeamSigma_dSpinBox.setValue(1.0f);
		m_FCSBeamSigma_dSpinBox.setDecimals(3);
		m_FCSBeamSigma_dSpinBox.setSingleStep(0.005);
		m_FCSBeamSigma_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_FCSBeamSigma_layout.addWidget(&m_FCSBeamSigma_label);
	m_FCSBeamSigma_layout.addWidget(&m_FCSBeamSigma_dSpinBox);

		m_FCSBeamMaxIntensity_label.setText("Beam Max Intensity: ");
		m_FCSBeamMaxIntensity_dSpinBox.setPrefix("[ ] ");
		m_FCSBeamMaxIntensity_dSpinBox.setMinimum(0.0f);
		m_FCSBeamMaxIntensity_dSpinBox.setMaximum(1.0f);
		m_FCSBeamMaxIntensity_dSpinBox.setValue(1.0f);
		m_FCSBeamMaxIntensity_dSpinBox.setDecimals(3);
		m_FCSBeamMaxIntensity_dSpinBox.setSingleStep(0.001);
		m_FCSBeamMaxIntensity_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_FCSBeamMaxIntensity_layout.addWidget(&m_FCSBeamMaxIntensity_label);
	m_FCSBeamMaxIntensity_layout.addWidget(&m_FCSBeamMaxIntensity_dSpinBox);

		m_FCSBeamKoff_label.setText("Beam Bleaching Rate: ");
		m_FCSBeamKoff_dSpinBox.setPrefix("[1/s] ");
		m_FCSBeamKoff_dSpinBox.setMinimum(0.0f);
		m_FCSBeamKoff_dSpinBox.setMaximum(1000.0f);
		m_FCSBeamKoff_dSpinBox.setValue(1.0f);
		m_FCSBeamKoff_dSpinBox.setDecimals(3);
		m_FCSBeamKoff_dSpinBox.setSingleStep(0.001);
		m_FCSBeamKoff_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_FCSBeamKoff_layout.addWidget(&m_FCSBeamKoff_label);
	m_FCSBeamKoff_layout.addWidget(&m_FCSBeamKoff_dSpinBox);

	m_FCSExperimentParameters_Vlayout.setMargin(3);
	m_FCSExperimentParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_FCSExperimentParameters_Vlayout.setSpacing(2);
	m_FCSExperimentParameters_Vlayout.addLayout(&m_FCSBeamIsEnabled_layout);
	m_FCSExperimentParameters_Vlayout.addLayout(&m_FCSBeamSigma_layout);
	m_FCSExperimentParameters_Vlayout.addLayout(&m_FCSBeamMaxIntensity_layout);
	m_FCSExperimentParameters_Vlayout.addLayout(&m_FCSBeamKoff_layout);

	m_FCSExperimentParameters_Frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_FCSExperimentParameters_Frame.setLayout(&m_FCSExperimentParameters_Vlayout);

	m_FCSExperimentParameters_dropMenu.setText("FCS Parameters");
	m_FCSExperimentParameters_dropMenu.setWidget(&m_FCSExperimentParameters_Frame);

	//SRIExperiment
		m_SRIPointingAccuracy_label.setText("Localization Precision: ");
		m_SRIPointingAccuracy_dSpinBx.setPrefix("[µm] ");
		m_SRIPointingAccuracy_dSpinBx.setMinimum(0);
		m_SRIPointingAccuracy_dSpinBx.setMaximum(10);
		m_SRIPointingAccuracy_dSpinBx.setValue(0.100);
		m_SRIPointingAccuracy_dSpinBx.setDecimals(3);
		m_SRIPointingAccuracy_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_SRIPointingAccuracy_layout.addWidget(&m_SRIPointingAccuracy_label);
	m_SRIPointingAccuracy_layout.addWidget(&m_SRIPointingAccuracy_dSpinBx);


	//third line
		m_SRIIntensity_label.setText("Intensity: ");
		m_SRIIntensity_spinBx.setPrefix("[ ]");
		m_SRIIntensity_spinBx.setMinimum(1);
		m_SRIIntensity_spinBx.setMaximum(1000);
		m_SRIIntensity_spinBx.setValue(1);
		m_SRIIntensity_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_SRIIntensity_layout.addWidget(&m_SRIIntensity_label);
	m_SRIIntensity_layout.addWidget(&m_SRIIntensity_spinBx);

	//fourth line
		m_SRIZoomLevel_label.setText("Zoom: ");
		m_SRIZoomLevel_spinBx.setPrefix("[ ]");
		m_SRIZoomLevel_spinBx.setMinimum(1);
		m_SRIZoomLevel_spinBx.setMaximum(10);
		m_SRIZoomLevel_spinBx.setValue(1);
		m_SRIZoomLevel_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_SRIZoomLevel_layout.addWidget(&m_SRIZoomLevel_label);
	m_SRIZoomLevel_layout.addWidget(&m_SRIZoomLevel_spinBx);

	m_SRIParametersFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_SRIParameters_Vlayout.setMargin(3);
	m_SRIParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_SRIParameters_Vlayout.setSpacing(2);
	m_SRIParameters_Vlayout.addLayout(&m_SRIPointingAccuracy_layout);
	m_SRIParameters_Vlayout.addLayout(&m_SRIIntensity_layout);
	m_SRIParameters_Vlayout.addLayout(&m_SRIZoomLevel_layout);
	m_SRIParametersFrame.setLayout(&m_SRIParameters_Vlayout);

	m_SRIParameters_dropMenu.setText("SRI Parameters");
	m_SRIParameters_dropMenu.setWidget(&m_SRIParametersFrame);


	//*** DRUG experiments : ***
		m_drugAdminParamChanged_label.setText("Affected Parameter: ");
		QStringList paramChanged_items = {"OutsideContact D","InsideContact D","TrappedContact D","Binding Rate","Unbinding Rate", "Crossing Probability"};
		m_drugAdminParamChanged_comboBx.addItems(paramChanged_items);
	m_drugAdminParamChanged_layout.addWidget(&m_drugAdminParamChanged_label);
	m_drugAdminParamChanged_layout.addWidget(&m_drugAdminParamChanged_comboBx);

		m_drugAdminPlane_label.setText("Drug Administration Frame: ");
		m_drugAdminPlane_spinBox.setPrefix("[frame_idx] ");
		m_drugAdminPlane_spinBox.setMinimum(0);
		m_drugAdminPlane_spinBox.setMaximum(1000000);
		m_drugAdminPlane_spinBox.setValue(100);
		m_drugAdminPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_drugAdminPlane_layout.addWidget(&m_drugAdminPlane_label);
	m_drugAdminPlane_layout.addWidget(&m_drugAdminPlane_spinBox);

		m_drugAdminNewValue_label.setText("New Value: ");
        m_drugAdminNewValue_dspinBox.setPrefix("[µm²/s ] ");
		m_drugAdminNewValue_dspinBox.setMinimum(m_DSlider_minValue);
		m_drugAdminNewValue_dspinBox.setMaximum(m_DSlider_maxValue);
		m_drugAdminNewValue_dspinBox.setValue(0.0);
		m_drugAdminNewValue_dspinBox.setDecimals(3);
		m_drugAdminNewValue_dspinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_drugAdminNewValue_layout.addWidget(&m_drugAdminNewValue_label);
	m_drugAdminNewValue_layout.addWidget(&m_drugAdminNewValue_dspinBox);

	m_drugExperimentParameters_Vlayout.setMargin(3);
	m_drugExperimentParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_drugExperimentParameters_Vlayout.setSpacing(2);
	m_drugExperimentParameters_Vlayout.addLayout(&m_drugAdminParamChanged_layout);
	m_drugExperimentParameters_Vlayout.addLayout(&m_drugAdminPlane_layout);
	m_drugExperimentParameters_Vlayout.addLayout(&m_drugAdminNewValue_layout);

	m_drugExperimentParameters_Frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_drugExperimentParameters_Frame.setLayout(&m_drugExperimentParameters_Vlayout);

	m_drugExperimentParameters_dropMenu.setText("Drug Parameters");
	m_drugExperimentParameters_dropMenu.setWidget(&m_drugExperimentParameters_Frame);

//*** Experiment Measured Regions ***

	m_addMeasuredRegion_pushButton.setText("Add region...");
	m_addMeasuredRegion_pushButton.setFixedSize(90,22);
	m_addMeasuredRegion_Hlayout.addWidget(&m_allMeasuredRegions_comboBx);
	m_addMeasuredRegion_Hlayout.addWidget(&m_addMeasuredRegion_pushButton);

	m_removeMeasuredRegion_pushButton.setText("Remove Region...");

	m_wProtocoleExperimentMeasuredRegions_Vlayout.setMargin(3);
	m_wProtocoleExperimentMeasuredRegions_Vlayout.setContentsMargins(7,5,7,6);
	m_wProtocoleExperimentMeasuredRegions_Vlayout.setSpacing(2);
	m_wProtocoleExperimentMeasuredRegions_Vlayout.addLayout(&m_addMeasuredRegion_Hlayout);
	m_wProtocoleExperimentMeasuredRegions_Vlayout.addWidget(&m_measuredRegions_listWidget);
	m_wProtocoleExperimentMeasuredRegions_Vlayout.addWidget(&m_removeMeasuredRegion_pushButton);

	m_wProtocoleExperimentMeasuredRegionsFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_wProtocoleExperimentMeasuredRegionsFrame.setLineWidth(1);
	m_wProtocoleExperimentMeasuredRegionsFrame.setMidLineWidth(1);
	m_wProtocoleExperimentMeasuredRegionsFrame.setStyleSheet("QFrame {border : rgb(0,0,0);}");
	m_wProtocoleExperimentMeasuredRegionsFrame.setLayout(&m_wProtocoleExperimentMeasuredRegions_Vlayout);

	m_wProtocoleExperimentMeasuredRegions_dropMenu.setText("Measured Regions");
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setWidget(&m_wProtocoleExperimentMeasuredRegionsFrame);

//*** Experiment Bleached Regions ***

	m_addBleachedRegion_pushButton.setText("Add Region...");
	m_addBleachedRegion_pushButton.setFixedSize(90,22);
	m_addBleachedRegion_Hlayout.addWidget(&m_allBleachedRegions_comboBx);
	m_addBleachedRegion_Hlayout.addWidget(&m_addBleachedRegion_pushButton);
	m_removeBleachedRegion_pushButton.setText("Remove Region...");

	m_wProtocoleExperimentBleachedRegions_Vlayout.setMargin(3);
	m_wProtocoleExperimentBleachedRegions_Vlayout.setContentsMargins(7,5,7,6);
	m_wProtocoleExperimentBleachedRegions_Vlayout.setSpacing(2);
	m_wProtocoleExperimentBleachedRegions_Vlayout.addLayout(&m_addBleachedRegion_Hlayout);
	m_wProtocoleExperimentBleachedRegions_Vlayout.addWidget(&m_bleachedRegions_listWidget);
	m_wProtocoleExperimentBleachedRegions_Vlayout.addWidget(&m_removeBleachedRegion_pushButton);

	m_wProtocoleExperimentBleachedRegionsFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_wProtocoleExperimentBleachedRegionsFrame.setLayout(&m_wProtocoleExperimentBleachedRegions_Vlayout);

    m_wProtocoleExperimentBleachedRegions_dropMenu.setText("Photobleached Regions");
	m_wProtocoleExperimentBleachedRegions_dropMenu.setWidget(&m_wProtocoleExperimentBleachedRegionsFrame);

//*** File Destination ***

	m_fileDestination_lineEdit.setReadOnly(true);
	m_fileDestination_lineEdit.setText(m_main_app->applicationDirPath());
	m_fileDestination_pushButton.setText("Browse...");
    m_exportStack_label.setText("Export Stack:");
    m_exportStack_noBackgroundImage_label.setText(" - No Background Image Loaded - ");
	m_exportStack_checkBx.setChecked(false);	
    m_photonBackground_label.setText("Photon Background\n   [photons/s]:");
	m_photonBackground_spinBx.setRange(0, 65535);//16 bits intensity
	m_photonBackground_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
	m_stackPoissonN_label.setText("Poisson Noise:");
	m_fileDestination_Hlayout.addWidget(&m_fileDestination_lineEdit);
	m_fileDestination_Hlayout.addWidget(&m_fileDestination_pushButton);
	m_exportStack_Hlayout.addWidget(&m_exportStack_label);
	m_exportStack_Hlayout.addWidget(&m_exportStack_checkBx);
    m_exportStack_Hlayout.addWidget(&m_exportStack_noBackgroundImage_label);
	m_exportStack_Hlayout.addStretch(100);
	m_exportStackIlluminationProperty_Hlayout.addStretch(10);
	m_exportStackIlluminationProperty_Hlayout.addWidget(&m_photonBackground_label);
	m_exportStackIlluminationProperty_Hlayout.addWidget(&m_photonBackground_spinBx);
	m_exportStackIlluminationProperty_Hlayout.addStretch(5);
	m_exportStackIlluminationProperty_Hlayout.addWidget(&m_stackPoissonN_label);
	m_exportStackIlluminationProperty_Hlayout.addWidget(&m_stackPoissonN_checkBx);
	m_exportStackIlluminationProperty_Hlayout.addStretch(10);

    m_cameraOffset_label.setText("Camera Offset\n [A/D Counts]: ");
    m_cameraOffset_spinBx.setMinimum(0);
    m_cameraOffset_spinBx.setMaximum(65535);
    m_cameraOffset_spinBx.setValue(0);
    m_cameraOffset_spinBx.setSingleStep(1);
    m_cameraOffset_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_cameraReadoutNoise_label.setText("Readout Noise (SD)\n [A/D Counts]: ");
    m_cameraReadoutNoise_dSpinBx.setValue(0);
    m_cameraReadoutNoise_dSpinBx.setMinimum(0);
    m_cameraReadoutNoise_dSpinBx.setMaximum(65535);
    m_cameraReadoutNoise_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_exportStackCameraProperty_Hlayout.addStretch(10);
    m_exportStackCameraProperty_Hlayout.addWidget(&m_cameraOffset_label);
    m_exportStackCameraProperty_Hlayout.addWidget(&m_cameraOffset_spinBx);
    m_exportStackCameraProperty_Hlayout.addStretch(5);
    m_exportStackCameraProperty_Hlayout.addWidget(&m_cameraReadoutNoise_label);
    m_exportStackCameraProperty_Hlayout.addWidget(&m_cameraReadoutNoise_dSpinBx);
    m_exportStackCameraProperty_Hlayout.addStretch(10);

	m_fileDestination_Vlayout.addLayout(&m_fileDestination_Hlayout);
	m_fileDestination_Vlayout.addLayout(&m_exportStack_Hlayout);
	m_fileDestination_Vlayout.addLayout(&m_exportStackIlluminationProperty_Hlayout);
    m_fileDestination_Vlayout.addLayout(&m_exportStackCameraProperty_Hlayout);

	m_fileDestinationFrame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_fileDestinationFrame.setLineWidth(1);
	m_fileDestinationFrame.setMidLineWidth(1);
	m_fileDestinationFrame.setStyleSheet("QFrame {border : rgb(0,0,0);}");
	m_fileDestinationFrame.setLayout(&m_fileDestination_Vlayout);

	m_fileDestination_dropMenu.setText("Destination Folder");
	m_fileDestination_dropMenu.setWidget(&m_fileDestinationFrame);

	m_parameters_VLayout.addWidget(&m_backgroundImage_dropMenu);
	m_parameters_VLayout.addWidget(&m_simulation_dropMenu);
	m_parameters_VLayout.addWidget(&m_allRegions_dropMenu);
	m_parameters_VLayout.addWidget(&m_simulationStates_dropMenu);
	m_parameters_VLayout.addWidget(&m_particleSystem_dropMenu);
	m_parameters_VLayout.addWidget(&m_dynamic_dropMenu);
	m_parameters_VLayout.addWidget(&m_PhotoPhysics_dropMenu);
	m_parameters_VLayout.addWidget(&m_wProtocoleExperiment_dropMenu);
	m_parameters_VLayout.addWidget(&m_FRAPExperimentParameters_dropMenu);
	m_parameters_VLayout.addWidget(&m_PAFExperimentParameters_dropMenu);
	m_parameters_VLayout.addWidget(&m_FCSExperimentParameters_dropMenu);
	m_parameters_VLayout.addWidget(&m_SRIParameters_dropMenu);
	m_parameters_VLayout.addWidget(&m_drugExperimentParameters_dropMenu);
	m_parameters_VLayout.addWidget(&m_fileDestination_dropMenu);
	m_parameters_VLayout.addWidget(&m_rendering_dropMenu);
	m_parameters_VLayout.addWidget(&m_liveExperiment_dropMenu);
	m_parameters_VLayout.addWidget(&m_wProtocoleExperimentMeasuredRegions_dropMenu);
	m_parameters_VLayout.addWidget(&m_wProtocoleExperimentBleachedRegions_dropMenu);

	m_parameters_wdgt.setLayout(&m_parameters_VLayout);

	m_parameters_VLayout.setMargin(0);
	m_parameters_VLayout.setSpacing(0);

	m_ui_VLayout.addLayout(&m_onglets_layout);
	m_ui_VLayout.addLayout(&m_parameters_VLayout);
    m_ui_VLayout.setMargin(10);
	m_ui_VLayout.setSpacing(10);

	m_ui_wdgt.setLayout(&m_ui_VLayout);
	m_ui_wdgt.adjustSize();
	m_ui_wdgt.setMinimumSize(m_ui_VLayout.minimumSize());
	m_ui_dockWdgt.setWidget(&m_ui_wdgt);

	QWidget* title_wdgt = new QWidget();
	m_ui_dockWdgt.setTitleBarWidget(title_wdgt);
	title_wdgt->hide();

	m_mainWindow.addDockWidget(Qt::BottomDockWidgetArea, &m_ui_dockWdgt, Qt::Vertical);
	m_ui_dockWdgt.setAllowedAreas(Qt::NoDockWidgetArea);
    m_ui_dockWdgt.setVisible(true);
	m_ui_dockWdgt.setFloating(true);

	m_ui_dockWdgt.adjustSize();
	m_ui_dockWdgt.setMinimumSize(m_ui_VLayout.minimumSize());

}

void FluoSimView::setStatusTip()
{
    //not used...
	return; //->

	string insideD_toolTip = "Coefficient of diffusion inside the membrane compartments.\n"
							 "Is expressed as a percentage of the coefficient of diffusion outside the compartments.";
//	m_insideD_label.setToolTip(insideD_toolTip.data());

	string outsideD_toolTip = "Coefficient of diffusion of molecules outside of any membrane compartments.\n"
							  "Is expressed in µm^2/s.";
}

void FluoSimView::buildMenuBar()
{
	//File Menu
    m_file_menu.setTitle("File");
        m_new_project_action.setText("New project...");
		m_load_project_action.setText("Load project...");
		m_save_project_action.setText("Save project...");
		m_captureScreen_action.setText("Capture Screen...");
		m_exit_action.setText("Exit");

    m_file_menu.addAction(&m_new_project_action);
	m_file_menu.addAction(&m_load_project_action);
	m_file_menu.addAction(&m_save_project_action);
	m_file_menu.addAction(&m_captureScreen_action);
	m_file_menu.addAction(&m_exit_action);

	//Examples Menu
	m_example_menu.setTitle("Examples");
		m_load_SPT_parameters_action.setText("SPT Experiment");
		m_FRAPExample_menu.setTitle("FRAP Experiment");
			m_load_FRAPOut_parameters_action.setText("NoContact");
			m_load_FRAPIn_parameters_action.setText("Contact");
			m_FRAPExample_menu.addAction(&m_load_FRAPOut_parameters_action);
			m_FRAPExample_menu.addAction(&m_load_FRAPIn_parameters_action);
		m_FCSExample_menu.setTitle("FCS Experiment");
			m_load_FCSOut_parameters_action.setText("NoContact");
			m_load_FCSIn_parameters_action.setText("Contact");
			m_FCSExample_menu.addAction(&m_load_FCSOut_parameters_action);
			m_FCSExample_menu.addAction(&m_load_FCSIn_parameters_action);
		m_load_SRI_parameters_action.setText("SRI Experiment");
        m_PAFExample_menu.setTitle("PAF Experiment");
            m_load_PAFOut_parameters_action.setText("NoContact");
            m_load_PAFIn_parameters_action.setText("Contact");
            m_PAFExample_menu.addAction(&m_load_PAFOut_parameters_action);
            m_PAFExample_menu.addAction(&m_load_PAFIn_parameters_action);

	m_example_menu.addAction(&m_load_SPT_parameters_action);
	m_example_menu.addMenu(&m_FRAPExample_menu);
	m_example_menu.addMenu(&m_FCSExample_menu);
	m_example_menu.addAction(&m_load_SRI_parameters_action);
    m_example_menu.addMenu(&m_PAFExample_menu);

	m_example_menu.setDisabled(false);

	//Window Menu
	m_window_menu.setTitle("Window");
		m_showAndHideSimParametersWdgt_action.setText("Show/Hide Parameters");
		m_showAndHideSimParametersWdgt_action.setCheckable(true);
		m_showAndHideGraphicWdgt_action.setText("Show/Hide Plot");
		m_showAndHideGraphicWdgt_action.setCheckable(true);
	m_window_menu.addAction(&m_showAndHideSimParametersWdgt_action);
	m_window_menu.addAction(&m_showAndHideGraphicWdgt_action);


	//Help Menu
	m_help_menu.setTitle("Help");
		m_openUserManual_action.setText("Manual");
		m_openAbout_action.setText("About");
	m_help_menu.addAction(&m_openUserManual_action);
	m_help_menu.addAction(&m_openAbout_action);
        m_openAbout_action.setDisabled(false);

	m_mainWindow.menuBar()->addMenu(&m_file_menu);
	m_mainWindow.menuBar()->addMenu(&m_example_menu);
	m_mainWindow.menuBar()->addMenu(&m_window_menu);
	m_mainWindow.menuBar()->addMenu(&m_help_menu);

}

void FluoSimView::buildToolBar()
{
	QString app_dir = m_main_app->applicationDirPath();

    m_drawRect_action.setIcon(QIcon(app_dir + "/Resources/Icons/drawRect-yellow_icon.png"));
    m_drawCircle_action.setIcon(QIcon(app_dir + "/Resources/Icons/drawCircle-yellow_icon.png"));
    m_drawPolygon_action.setIcon(QIcon(app_dir + "/Resources/Icons/drawPolygon-yellow_icon.png"));

	m_drawing_actionGroup.addAction(&m_drawRect_action);
	m_drawing_actionGroup.addAction(&m_drawCircle_action);
	m_drawing_actionGroup.addAction(&m_drawPolygon_action);

	m_drawing_toolBar.addAction(&m_drawRect_action);
	m_drawing_toolBar.addAction(&m_drawCircle_action);
	m_drawing_toolBar.addAction(&m_drawPolygon_action);

	m_drawing_toolBar.show();

	m_mainWindow.addToolBar(&m_drawing_toolBar);
}

void FluoSimView::showGaussianParameters(bool visible)
{
	if(visible == true)
	{
        m_spotSize_dSpinBxedSlider.show();
        m_spotIntensity_spinBxedSlider.show();
        m_autoscale_wLayout.show();
    }
	else
	{
        m_spotSize_dSpinBxedSlider.hide();
        m_spotIntensity_spinBxedSlider.hide();
        m_autoscale_wLayout.hide();


        m_rendering_dropMenu.unroll();

		int size_x = m_ui_dockWdgt.width();
		m_ui_dockWdgt.setMinimumSize(size_x, m_ui_VLayout.minimumSize().height());
			m_ui_dockWdgt.adjustSize();
		m_ui_dockWdgt.setMinimumSize(m_ui_VLayout.minimumSize());
	}
}

void FluoSimView::showLivePhotoBleachingParameters(bool visible)
{
	m_bleachedRegion_label.setVisible(visible);
	m_bleachedRegion_comboBx.setVisible(visible);
	m_startBleaching_pushButton.setVisible(visible);
}

void FluoSimView::showLivePhotoActivationParameters(bool visible)
{
	m_photoActivatedRegion_label.setVisible(visible);
	m_photoActivatedRegion_comboBx.setVisible(visible);
	m_startPhotoActivation_pushButton.setVisible(visible);
}

void FluoSimView::showTimeLapseParameters(bool visible)
{
	m_timelapse_period_label.setVisible(visible);
	m_timelapse_period_dSpinBox.setVisible(visible);


}

void FluoSimView::showFrapParameter(bool visible)
{
	m_frapPlane_label.setVisible(visible);
	m_frapPlane_spinBox.setVisible(visible);

	m_frapDPlane_label.setVisible(visible);
	m_frapDPlane_spinBox.setVisible(visible);

	m_frapKoff_label.setVisible(visible);
	m_frapKoff_dspinBox.setVisible(visible);
}

void FluoSimView::showPhotoActivationParameters(bool visible)
{
	m_photoActivationPlane_label.setVisible(visible);
	m_photoActivationPlane_spinBox.setVisible(visible);

	m_photoActivationDPlane_label.setVisible(visible);
	m_photoActivationDPlane_spinBox.setVisible(visible);

	m_photoActivationKon_label.setVisible(visible);
	m_photoActivationKon_dspinBox.setVisible(visible);
}

void FluoSimView::showExportStackParameters(bool visible)
{
	m_photonBackground_label.setVisible(visible);
	m_photonBackground_spinBx.setVisible(visible);

	m_stackPoissonN_checkBx.setVisible(visible);
	m_stackPoissonN_label.setVisible(visible);

    m_photonBackground_label.setVisible(visible);
    m_photonBackground_label.setVisible(visible);

    m_cameraOffset_label.setVisible(visible);
    m_cameraOffset_spinBx.setVisible(visible);

    m_cameraReadoutNoise_label.setVisible(visible);
    m_cameraReadoutNoise_dSpinBx.setVisible(visible);

	m_exportStack_checkBx.setVisible(visible);
	m_exportStack_label.setVisible(visible);

	updateParameterWidgetSize();
}

void FluoSimView::buildGraphicWidget()
{
	//create the graphic opengl object...
	m_graphic = new Graphic(m_main_app, 0, m_scrn.getRenderWindow());
		m_graphic->setXAxisText("Time[s]");
		m_graphic->setYAxisText("Intensity[]");

	m_graphic_layout.addWidget(m_graphic);
	m_graphic_layout.addLayout(&m_measuredRegion_layout);
	m_graphic_wdgt.setLayout(&m_graphic_layout);

	m_graphic_dockWdgt.setWidget(&m_graphic_wdgt);
	m_mainWindow.addDockWidget(Qt::BottomDockWidgetArea, &m_graphic_dockWdgt, Qt::Vertical);
	m_graphic_dockWdgt.setAllowedAreas(Qt::BottomDockWidgetArea);

	m_graphic_dockWdgt.setWindowTitle("Graphs");
	m_graphic_dockWdgt.setVisible(true);
	m_graphic_dockWdgt.setFloating(true);
}

void FluoSimView::enableExperimentParameters(bool enabled)
{
	m_timeStep_label.setEnabled(enabled);
	m_timeStep_dSpinBx.setEnabled(enabled);

	m_wProtocoleExperiment_dropMenu.setEnabled(enabled);
	m_FRAPExperimentParameters_dropMenu.setEnabled(enabled);
	m_PAFExperimentParameters_dropMenu.setEnabled(enabled);
	m_FCSExperimentParameters_dropMenu.setEnabled(enabled);
	m_drugExperimentParameters_dropMenu.setEnabled(enabled);
	m_fileDestination_dropMenu.setEnabled(enabled);
//	m_rendering_dropMenu.setEnabled(enabled);
	m_SRIParameters_dropMenu.setEnabled(enabled);
	m_wProtocoleExperimentMeasuredRegions_dropMenu.setEnabled(enabled);
	m_wProtocoleExperimentBleachedRegions_dropMenu.setEnabled(enabled);
}

void FluoSimView::enableSimulationStateButtons(bool enabled)
{
	m_play_pushBut.setEnabled(enabled);
	m_pause_pushBut.setEnabled(enabled);
	m_stop_pushBut.setEnabled(enabled);
}

void FluoSimView::enableDynamicParameters(bool enabled)
{
	for(QObject* obj : m_dynamicFrame.children())
	{
		QWidget* wdgt = qobject_cast<QWidget*>(obj);
		if(wdgt != NULL && wdgt != &m_fixation_checkBx && wdgt != &m_fixation_label)
		{
			wdgt->setEnabled(enabled);
		}
	}
}

void FluoSimView::enableExportStackParameters(bool enabled)
{
    for(uint wdgt_idx = 0; wdgt_idx<m_exportStackIlluminationProperty_Hlayout.count(); wdgt_idx++)
    {
        QWidget* wdgt = m_exportStackIlluminationProperty_Hlayout.itemAt(wdgt_idx)->widget();
        if(wdgt) wdgt->setEnabled(enabled && m_spotIntensity_spinBxedSlider.getComboBoxIndex() == SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC);
    }

    for(uint wdgt_idx = 0; wdgt_idx<m_exportStackCameraProperty_Hlayout.count(); wdgt_idx++)
    {
        QWidget* wdgt = m_exportStackCameraProperty_Hlayout.itemAt(wdgt_idx)->widget();
        if(wdgt) wdgt->setEnabled(enabled);
    }
}



void FluoSimView::enableExportStackCheckBox(bool enabled)
{
    m_exportStack_checkBx.setEnabled(enabled);
    m_exportStack_label.setEnabled(enabled);
}



void FluoSimView::updateParameterWidgetSize()
{
	m_wProtocoleExperimentMeasuredRegionsFrame.setMaximumSize(m_wProtocoleExperimentMeasuredRegions_Vlayout.maximumSize().width(),
															  m_wProtocoleExperimentMeasuredRegions_Vlayout.minimumSize().height());

	m_wProtocoleExperimentBleachedRegionsFrame.setMaximumSize(m_wProtocoleExperimentBleachedRegions_Vlayout.maximumSize().width(),
															  m_wProtocoleExperimentBleachedRegions_Vlayout.minimumSize().height());

	m_fileDestinationFrame.setMaximumSize(m_fileDestination_Vlayout.maximumSize().width(),
										  m_fileDestination_Vlayout.minimumSize().height());

	m_allRegionsFrame.setMaximumSize(m_allRegions_vlayout.maximumSize().width(),
									 m_allRegions_vlayout.minimumSize().height());

	m_rendering_dropMenu.unroll();
	m_fileDestination_dropMenu.unroll();
	m_wProtocoleExperiment_dropMenu.unroll();
	m_liveExperiment_dropMenu.unroll();

	m_ui_wdgt.adjustSize();
	m_ui_dockWdgt.adjustSize();
	int size1_x = m_ui_wdgt.width();
	int size2_x = m_ui_dockWdgt.width();
	m_ui_wdgt.setMinimumSize(size1_x, m_ui_VLayout.minimumSize().height());
	m_ui_dockWdgt.setMinimumSize(size2_x, m_ui_VLayout.minimumSize().height());

	QPoint pos;
	QWidget* central_wdgt = m_mainWindow.centralWidget();
	pos = m_mainWindow.mapToGlobal(central_wdgt->pos()
									+ QPoint(central_wdgt->geometry().width(),0));
	m_ui_dockWdgt.move(pos - QPoint(m_ui_dockWdgt.frameGeometry().width(), 0));

}


void FluoSimView::updateDinsideSpinBxedSlider()
{
    switch(m_insideD_dSpinBxedSlider.getComboBoxIndex())
    {
        case DIFFUSION_UNIT_COMBOBOX_PERCENTAGE :
        {
            m_insideD_dSpinBxedSlider.setMin(0);
            m_insideD_dSpinBxedSlider.setMax(100);
            m_insideD_dSpinBxedSlider.setValue(0);
            m_insideD_dSpinBxedSlider.setStep(0.1);
            m_insideD_dSpinBxedSlider.setDecimals(1);
        }
        break;

        case DIFFUSION_UNIT_COMBOBOX_UM2PERSEC :
        {
            m_insideD_dSpinBxedSlider.setMin(0);
            m_insideD_dSpinBxedSlider.setMax(m_outsideD_dSpinBxedSlider.value());
            m_insideD_dSpinBxedSlider.setValue(0);
            m_insideD_dSpinBxedSlider.setStep(m_outsideD_dSpinBxedSlider.step());
            m_insideD_dSpinBxedSlider.setDecimals(m_outsideD_dSpinBxedSlider.decimal());
        }
        break;
    }
}

void FluoSimView::setStyleSheets()
{
    m_main_app->setWindowIcon(QIcon("/Resources/Icons/logo.tif"));
	m_mainWindow.menuBar()->setStyleSheet("QMenuBar {background-color : rgb(72,72,72);}"
										  "QMenuBar::item {background-color: rgb(72,72,72); color:rgb(240,240,240)}"
										  "QMenuBar::item {background-color: rgb(72,72,72);}"
										  "QMenuBar::item:selected {background-color: rgb(150,150,150);}"
										  "QMenuBar::item:pressed {background-color: rgb(35,35,35); color : rgb(240,188,23);}");

	m_file_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
							  "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}"
							  "QMenu::item:disabled {background-color: rgb(72,72,72); color : rgb(139,139,139);}");
	m_example_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
								 "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}");
	m_window_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
								"QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}");
	m_help_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
							  "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}"
							  "QMenu::item:disabled {background-color: rgb(72,72,72); color : rgb(139,139,139);}");
	m_FRAPExample_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
							  "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}");
	m_FCSExample_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
							  "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}");
    m_PAFExample_menu.setStyleSheet("QMenu {background-color : rgb(72,72,72); color : rgb(240,240,240);}"
                              "QMenu::item:selected {background-color: rgb(35,35,35); color : rgb(240,188,23);}");

	m_mainWindow.centralWidget()->setStyleSheet("background : rgb(20,20,20);");

	m_graphic_dockWdgt.setObjectName("m_graphic_dockWdgt");
	m_graphic_dockWdgt.setStyleSheet("#m_graphic_dockWdgt {background : rgb(20,20,20);}"
									 "#m_graphic_dockWdgt {background : rgb(72,72,72);}"
									 "QLabel {color : rgb(230,230,230); height: 14px; font-size : 11px;}\n"
									 "QLabel:disabled {color : rgb(180,180,180); height: 14px; font-size : 11px;}\n"
									 "QComboBox QAbstractItemView {background-color : rgb(82,82,82); color : rgb(255,255,255);}\n"
									 "QComboBox QAbstractItemView {selection-background-color : rgb(100,100,100); selection-color : rgb(240,188,23);}\n"
									 "QComboBox {selection-background-color : transparent; selection-color : rgb(0,0,0);}\n"
									 "QComboBox::drop-down:disabled {border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n"
									 "QComboBox, QDoubleSpinBox, QLineEdit, QSpinBox {background-color : rgb(200,200,200); color : rgb(0,0,0); height: 14px; font-size : 11px;}\n"
									 "QComboBox:hover, QDoubleSpinBox:hover , QSpinBox:hover {background-color : rgb(230,230,230);}\n"
									 "QComboBox:focus, QDoubleSpinBox:focus , QSpinBox:focus {background-color : rgb(255,255,255);}\n"
									 "QComboBox:disabled, QDoubleSpinBox:disabled, QLineEdit:disabled, QSpinBox:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
																									  "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");

	m_drawing_toolBar.setStyleSheet("QToolBar {background: rgb(72,72,72);}");


    m_play_pushBut.setStyleSheet("QPushButton {image : url(Resources/Icons/playButton.png); border-style : none;}"
                                 "QPushButton:hover	{image : url(Resources/Icons/playButton_hover.png);}"
                                 "QPushButton:pressed {image : url(Resources/Icons/playButton_pressed.png);}");

    m_pause_pushBut.setStyleSheet("QPushButton {image : url(Resources/Icons/pauseButton.png); border-style : none;}"
                                 "QPushButton:hover	{image : url(Resources/Icons/pauseButton_hover.png);}"
                                 "QPushButton:pressed {image : url(Resources/Icons/pauseButton_pressed.png);}");

    m_stop_pushBut.setStyleSheet("QPushButton {image : url(Resources/Icons/stopButton.png); border-style : none;}"
                                 "QPushButton:hover	{image : url(Resources/Icons/stopButton_hover.png);}"
                                 "QPushButton:pressed {image : url(Resources/Icons/stopButton_pressed.png);}"
                                 "QPushButton:disabled	{image : url(Resources/Icons/stopButton_disabled.png);}");


	m_ui_dockWdgt.setObjectName("m_ui_wdgt");
	m_ui_dockWdgt.setStyleSheet("#m_ui_wdgt {background-color : rgb(72,72,72);}\n"
							"QLabel {color : rgb(230,230,230); height: 14px; font-size : 11px;}\n"
							"QLabel:disabled {color : rgb(180,180,180); height: 14px; font-size : 11px;}\n"
							"QComboBox QAbstractItemView {background-color : rgb(82,82,82); color : rgb(255,255,255);}\n"
							"QComboBox QAbstractItemView {selection-background-color : rgb(100,100,100); selection-color : rgb(240,188,23);}\n"
                            "QComboBox {selection-background-color : transparent; selection-color : rgb(0,0,0);}\n"
							"QComboBox::drop-down:disabled {border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n"
                            "QComboBox, QDoubleSpinBox, QLineEdit, QSpinBox {background-color : rgb(200,200,200); color : rgb(0,0,0); height: 14px; font-size : 11px;}\n"
                            "QComboBox:hover, QDoubleSpinBox:hover , QSpinBox:hover {background-color : rgb(230,230,230);}\n"
                            "QComboBox:focus, QDoubleSpinBox:focus , QSpinBox:focus {background-color : rgb(255,255,255);}\n"
							"QComboBox:disabled, QDoubleSpinBox:disabled, QLineEdit:disabled, QSpinBox:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
																							 "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n"
							"QSlider::handle:horizontal {width: 4px;\n height: 5px;\n background: rgb(240,188,23);margin-top: -4px; margin-bottom: -4px;}\n"
							"QSlider::handle:horizontal:disabled {width: 4px;\n height: 5px;\n background: rgb(180,180,180);margin-top: -4px; margin-bottom: -4px;}\n"
							"QSlider::handle:horizontal:hover {width: 4px;\n height: 5px;\n background: rgb(255,255,255);margin-top: -4px; margin-bottom: -4px;}\n"
							"QSlider::handle:horizontal:focus {width: 4px;\n height: 5px;\n background: rgb(255,255,255);margin-top: -4px; margin-bottom: -4px;}\n"
							"QSlider::groove:horizontal {height: 5px; background: rgb(180,180,180);}\n\n"
							"QSlider::groove:horizontal:disabled {height: 5px; background: rgb(49,49,49);}\n\n"
							"QSlider::groove:horizontal:hover {height: 5px; background: rgb(230,230,230);}\n\n"
							"QSlider::groove:horizontal:focus {height: 5px; background: rgb(240,188,23);}\n\n"
							"QCheckBox:checked {border-style : solid; border-width : 1px;  border-color : rgb(180,180,180);"
										"max-width:9px; min-width:9px; max-height:9px; min-height:9px}"
							"QCheckBox::indicator:checked {background-color: rgb(240,188,23); width: 7px; height: 7px;"
												  "border-style: solid; border-width: 1px; border-color: rgb(0,0,0);}"
							"QCheckBox:!checked {border-style : solid; border-width : 1px;  border-color : rgb(180,180,180);"
										"max-width:9px; min-width:9px; max-height:9px; min-height:9px}"
							"QCheckBox::indicator:!checked {background-color:transparent; width: 7px; height: 7px;"
												  "border-style: solid; border-width: 1px; border-color: rgb(0,0,0);}"
							"QCheckBox:hover {border-style : solid; border-width : 1px;  border-color : rgb(230,230,230);"
										"max-width:9px; min-width:9px; max-height:9px; min-height:9px}"
							"QCheckBox:disabled {border-style : solid; border-width : 1px;  border-color : rgb(49,49,49);"
										"max-width:9px; min-width:9px; max-height:9px; min-height:9px}"
							"QCheckBox::indicator:!checked:disabled {background-color:transparent; width: 7px; height: 7px;"
												  "border-style: solid; border-width: 1px; border-color: rgb(49,49,49);}"
							"QCheckBox::indicator:checked:disabled {background-color: rgb(100,100,100); width: 7px; height: 7px;"
												  "border-style: solid; border-width: 1px; border-color: rgb(49,49,49);}"
							);

	 m_backgroundImageFrame.setObjectName("frame1");
	 m_backgroundImageFrame.setStyleSheet("#frame1 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
										  "QPushButton {/*height : 14px; font-size : 11px; */background-color: rgb(200,200,200)}"
										  "QPushButton:hover {background-color: rgb(250,250,250)}"
										  "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
										  "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");

	 m_allRegionsFrame.setObjectName("frame2");
	 m_allRegionsFrame.setStyleSheet("#frame2 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
									 "QPushButton {/*height : 14px; font-size : 11px; */background-color: rgb(200,200,200)}"
									 "QPushButton:hover {background-color: rgb(250,250,250)}"
									 "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
									 "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");
	 m_simulationStatesFrame.setObjectName("frame3");
	 m_simulationStatesFrame.setStyleSheet("#frame3 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_particleSystemFrame.setObjectName("frame4");
	 m_particleSystemFrame.setStyleSheet("#frame4 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
                                         "QPushButton {background-color: rgb(200,200,200)}"
										 "QPushButton:hover {background-color: rgb(250,250,250)}"
										 "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
                                         "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n"
                                      "QSpinBox, QDoubleSpinBox {background-color: rgb(72,72,72); color: rgb(230,230,230);}"
                                      "QSpinBox:hover, QDoubleSpinBox:hover {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                      "QSpinBox:focus, QDoubleSpinBox:focus {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                      "QSpinBox:disabled, QDoubleSpinBox:disabled {border-style: solid; border-color: rgb(230,230,230); border-width:0px;"
                                         "background-color: rgb(72,72,72); color: rgb(139,139,139);}");

	 m_simulationFrame.setObjectName("frame5");
	 m_simulationFrame.setStyleSheet("#frame5 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_dynamicFrame.setObjectName("frame6");
     m_dynamicFrame.setStyleSheet("#frame6 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}\n"
                                  "QSpinBox, QDoubleSpinBox {background-color: rgb(72,72,72); color: rgb(230,230,230);}\n"
                                  "QSpinBox:hover, QDoubleSpinBox:hover {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}\n"
                                  "QSpinBox:focus, QDoubleSpinBox:focus {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}\n"
                                  "QSpinBox:disabled, QDoubleSpinBox:disabled {border-style: solid; border-color: rgb(230,230,230); border-width:0px;\n"
                                     "background-color: rgb(72,72,72); color: rgb(139,139,139);}\n"
                                "QComboBox QAbstractItemView {background-color : rgb(82,82,82); color : rgb(255,255,255);}\n"
                                "QComboBox QAbstractItemView {selection-background-color : rgb(100,100,100); selection-color : rgb(240,188,23);}\n"
                                "QComboBox::drop-down {width:15px;}\n"
                                "QComboBox {border-style:solid; border-width:0px; background-color : rgb(72,72,72); color:rgb(230,230,230); height: 20px; font-size : 11px;}\n");

	 m_PhotoPhysicsFrame.setObjectName("frame7");
     m_PhotoPhysicsFrame.setStyleSheet("#frame7 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
                                      "QSpinBox, QDoubleSpinBox {background-color: rgb(72,72,72); color: rgb(230,230,230);}"
                                      "QSpinBox:hover, QDoubleSpinBox:hover {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                      "QSpinBox:focus, QDoubleSpinBox:focus {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                      "QSpinBox:disabled, QDoubleSpinBox:disabled {border-style: solid; border-color: rgb(230,230,230); border-width:0px;"
                                         "background-color: rgb(72,72,72); color: rgb(139,139,139);}");

	 m_renderingFrame.setObjectName("frame8");
     m_renderingFrame.setStyleSheet("#frame8 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
                                  "QSpinBox, QDoubleSpinBox {background-color: rgb(72,72,72); color: rgb(230,230,230);}"
                                  "QSpinBox:hover, QDoubleSpinBox:hover {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                  "QSpinBox:focus, QDoubleSpinBox:focus {border-style: solid; border-color: rgb(230,230,230); border-width:1px;}"
                                  "QSpinBox:disabled, QDoubleSpinBox:disabled {border-style: solid; border-color: rgb(230,230,230); border-width:0px;"
                                     "background-color: rgb(72,72,72); color: rgb(139,139,139);}");
    m_spotIntensity_spinBxedSlider.setStyleSheet( \
                                "QComboBox QAbstractItemView {background-color : rgb(82,82,82); color : rgb(255,255,255);}\n"
                                "QComboBox QAbstractItemView {background-color : rgb(82,82,82); color : rgb(255,255,255);}\n"
                                "QComboBox QAbstractItemView {selection-background-color : rgb(100,100,100); selection-color : rgb(240,188,23);}\n"
                                "QComboBox::drop-down {/*background-color:transparent;image : url(Resources/Icons/playButton.png);border-style:none;*/ width:15px;}\n"
                                "QComboBox {border-style:solid; border-width:0px; background-color : rgb(72,72,72); color:rgb(230,230,230); height: 20px; font-size : 11px;}\n");

	 m_liveExperimentFrame.setObjectName("frame9");
	 m_liveExperimentFrame.setStyleSheet("#frame9 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
										 "QPushButton {height : 14px; font-size : 11px; background-color: rgb(200,200,200)}"
										 "QPushButton:hover {background-color: rgb(250,250,250)}"
										 "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
                                         "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");

	 m_wProtocoleExperimentFrame.setObjectName("frame10");;
	 m_wProtocoleExperimentFrame.setStyleSheet("#frame10 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_FRAPExperimentParameters_Frame.setObjectName("frame11");
	 m_FRAPExperimentParameters_Frame.setStyleSheet("#frame11 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_PAFExperimentParameters_Frame.setObjectName("frame12");
	 m_PAFExperimentParameters_Frame.setStyleSheet("#frame12 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_FCSExperimentParameters_Frame.setObjectName("frame13");
	 m_FCSExperimentParameters_Frame.setStyleSheet("#frame13 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_SRIParametersFrame.setObjectName("frame14");
	 m_SRIParametersFrame.setStyleSheet("#frame14 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_drugExperimentParameters_Frame.setObjectName("frame15");
	 m_drugExperimentParameters_Frame.setStyleSheet("#frame15 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	 m_wProtocoleExperimentMeasuredRegionsFrame.setObjectName("frame16");
	 m_wProtocoleExperimentMeasuredRegionsFrame.setStyleSheet("#frame16 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
															  "QPushButton {height : 14px; font-size : 11px; background-color: rgb(200,200,200)}"
															  "QPushButton:hover {background-color: rgb(250,250,250)}"
															  "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
															  "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");

	 m_wProtocoleExperimentBleachedRegionsFrame.setObjectName("frame17");
	 m_wProtocoleExperimentBleachedRegionsFrame.setStyleSheet("#frame17 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
															  "QPushButton {height : 14px; font-size : 11px; background-color: rgb(200,200,200)}"
															  "QPushButton:hover {background-color: rgb(250,250,250)}"
															  "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
															  "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");

	 m_fileDestinationFrame.setObjectName("frame18");
	 m_fileDestinationFrame.setStyleSheet("#frame18 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}"
										  "QPushButton {height : 14px; font-size : 11px; background-color: rgb(200,200,200)}"
										  "QPushButton:hover {background-color: rgb(250,250,250)}"
										  "QPushButton:disabled {background-color : rgb(49,49,49); color: rgb(139,139,139);"
										  "border-style: solid; border-color: rgb(49,49,49); border-width:0px;}\n");
     m_exportStack_noBackgroundImage_label.setStyleSheet("QLabel {color : rgb(230,0,0); height: 14px; font-size : 11px;}\n");


    m_measuredRegions_listWidget.setStyleSheet("QListWidget {font-size : 11px; color : rgb(240,240,240); background-color : rgb(72,72,72);}"
											   "QListWidget:disabled {color : rgb(139,139,139); background-color : rgb(49,49,49);}"
											   "QListWidget::item:hover {color : rgb(240,240,240); background-color : rgb(100,100,100);}"
											   "QListWidget::item:disabled {color : rgb(139,139,139); background-color : rgb(49,49,49);}"
											   "QListWidget::item:selected {color : rgb(240,188,23); background-color : rgb(100,100,100);}");
    m_bleachedRegions_listWidget.setStyleSheet("QListWidget {font-size : 11px; color : rgb(240,240,240); background-color : rgb(72,72,72);}"
											   "QListWidget:disabled {color : rgb(139,139,139); background-color : rgb(49,49,49);}"
											   "QListWidget::item:hover {color : rgb(240,240,240); background-color : rgb(100,100,100);}"
											   "QListWidget::item:disabled {color : rgb(139,139,139); background-color : rgb(49,49,49);}"
											   "QListWidget::item:selected {color : rgb(240,188,23); background-color : rgb(100,100,100);}");
}

void FluoSimView::displayAboutFluoSim()
{
    if(m_aboutFluoSim_dialog != 0) {
        m_aboutFluoSim_dialog->show();
        m_aboutFluoSim_dialog->raise();
        return;
    }

	//label
	m_aboutFluoSim_label = new QLabel;
	m_aboutFluoSim_label->setText("FluoSim v1.0\n"
                                  "\n"
                                  "Matthieu Lagardère\n"
								  "Olivier Thoumine\n"
                                  "Interdisciplinary Institute for Neuroscience (IINS)\n"
								  "\n"
                                  "Copyright (C) 2020 Matthieu Lagardère\n"
                                  "License: GPL v3 \nSee GPLv3.txt for more information.\n\n"
                                  "Latest version: https://github.com/mlagardere/FluoSim");

	m_aboutFluoSim_label->setStyleSheet("QLabel {font-size : 12px;}");

	//close button
	m_aboutFluoSim_dialog = new QDialog(&m_mainWindow);
	m_aboutFluoSim_dialog->setWindowTitle("About FluoSim");
	m_aboutFluoSim_dialog->setStyleSheet("QDialog {background-color : rgb(215,230,215);}");
	m_aboutFluoSim_dialog->show();
	m_aboutFluoSim_dialog->raise();

	//layout
	m_aboutFluoSim_layout = new QHBoxLayout;
	m_aboutFluoSim_layout->addWidget(m_aboutFluoSim_label);

	m_aboutFluoSim_dialog->setLayout(m_aboutFluoSim_layout);
}


void FluoSimView::mainWindowGetMoved(QMoveEvent* event)
{
	QPoint pos;
	QWidget* central_wdgt = m_mainWindow.centralWidget();
	pos = m_mainWindow.mapToGlobal(central_wdgt->pos()
									+ QPoint(central_wdgt->geometry().width(),0));
	m_ui_dockWdgt.move(pos - QPoint(m_ui_dockWdgt.frameGeometry().width(), 0));
}

void FluoSimView::mainWindowGetResized(QResizeEvent * event)
{
	QPoint pos;
	QWidget* central_wdgt = m_mainWindow.centralWidget();
	pos = m_mainWindow.mapToGlobal(central_wdgt->pos()
									+ QPoint(central_wdgt->geometry().width(),0));
	m_ui_dockWdgt.move(pos - QPoint(m_ui_dockWdgt.frameGeometry().width(), 0));
}


void FluoSimView::mainWindowGetLowered()
{
}

void FluoSimView::mainWindowGetRaised()
{
}

void FluoSimView::applicationStateGetChanged(Qt::ApplicationState state)
{
}

void FluoSimView::dropEvent(QDropEvent * event)
{
	treatEvent(event);
}

void FluoSimView::dragEnterEvent(QDragEnterEvent * event)
{
	treatEvent(event);
}


void FluoSimView::deleteRegionAllRegionsGetClicked(){}
void FluoSimView::regionCellGetDoubleClicked(int, int){}
void FluoSimView::regionCellSelectionGetChanged(){}
void FluoSimView::regionCellGetEntered(int row, int column){}
void FluoSimView::allRegionsTableGetLeft(){};
void FluoSimView::sliderValueGetChanged(SLIDER_ID slider_name, float value){}
void FluoSimView::actionGetTriggered(ACTION_ID id){}
void FluoSimView::onOngletGetChanged(int onglet_id){}
void FluoSimView::playButtonHasBeenClicked(bool){}
void FluoSimView::pauseButtonHasBeenClicked(bool){}
void FluoSimView::stopButtonHasBeenClicked(bool){}
void FluoSimView::NplanesGetChanged(double new_value){}
void FluoSimView::DinsideUnitGetChanged(int new_unit){}
void FluoSimView::fixationGetChanged(bool){}
void FluoSimView::immobileFractionGetChanged(double new_value){}
void FluoSimView::initialEnrichmentGetChanged(double){}
void FluoSimView::renderingModeGetChanged(int new_mode){}
void FluoSimView::particleColorGetChanged(int new_color){}
void FluoSimView::SRIParticleColorGetChanged(int new_color){}
void FluoSimView::pixelSizeGetChanged(double new_pixelSize){}
void FluoSimView::timeStepGetChanged(double new_timeStep){}
void FluoSimView::regionVisibilityGetChanged(bool){}
void FluoSimView::ADCountsPerPhotonGetChanged(double){}
void FluoSimView::autoscaleGetChanged(bool) {}
void FluoSimView::spotIntensityUnitGetChanged(int) {}
void FluoSimView::imageSmoothingGetChanged(bool value){}
void FluoSimView::backgroundVisiblityInGeomGetChanged(bool){}
void FluoSimView::backgroundVisiblityInLiveAndExpGetChanged(bool){}
void FluoSimView::measuredRegiongetChanged(QString){}
void FluoSimView::regionGetHighlighted(QString region_name){}
void FluoSimView::regionComboBoxFocusGetLost(){}
void FluoSimView::liveExperimentTypeGetChanged(int){}
void FluoSimView::bleachedRegionGetChanged(QString){}
void FluoSimView::photoActivatedRegionGetChanged(QString){}
void FluoSimView::regionGetBleached(){}
void FluoSimView::regionGetPhotoActivated(){}
void FluoSimView::experimentTypeGetChanged(int){}
void FluoSimView::acquisitionTypeGetChanged(int){}
void FluoSimView::acquisitionPeriodGetChanged(double){}
void FluoSimView::acquisitionPresequenceGetChanged(int){}
void FluoSimView::acquisitionNPlanesGetChanged(int){}
void FluoSimView::frapPlaneIdxGetChanged(int){}
void FluoSimView::frapDPlaneGetChanged(int){}
void FluoSimView::frapKoffGetChanged(double){}
void FluoSimView::photoActivationPlaneIdxGetChanged(int){}
void FluoSimView::photoActivationDPlaneGetChanged(int){}
void FluoSimView::photoActivationKonGetChanged(double){}
void FluoSimView::FCSBeamIsEnabledGetChanged(bool){}
void FluoSimView::FCSBeamSigmaGetChanged(double){}
void FluoSimView::FCSBeamMaxIntensityGetChanged(double){}
void FluoSimView::FCSBeamKoffGetChanged(double){}
void FluoSimView::SRIPointingAccuracyGetChanged(double){}
void FluoSimView::SRIIntensityGetChanged(int){}
void FluoSimView::SRIZoomLevelGetChanged(int){}
void FluoSimView::drugAdminPlaneChanged(int){}
void FluoSimView::drugAdminParamChanged(int){}
void FluoSimView::drugAdminNewValueChanged(double){}
void FluoSimView::NbRepetitionGetChanged(int){}
void FluoSimView::graphicDockWidgetClosed(){}
void FluoSimView::fileDestinationButtonHasBeenClicked(){}
void FluoSimView::isExportingStackGetChanged(bool){}
void FluoSimView::photonBackgroundHasBeenChanged(int){}
void FluoSimView::cameraOffsetHasBeenChanged(int){}
void FluoSimView::cameraReadoutNoiseHasBeenChanged(double){}
void FluoSimView::isStackUsingPoissonNoiseGetChanged(bool){}
void FluoSimView::treatEvent(QEvent* event){}

void FluoSimView::addRegionMeasuredRegionsGetClicked()
{
//	int rgn_idx = m_allMeasuredRegions_comboBx.currentIndex();
	string rgn_name = m_allMeasuredRegions_comboBx.currentText().toLocal8Bit().data();
	addRegionMeasuredRegions(rgn_name);
}
void FluoSimView::addRegionMeasuredRegions(string rgn_idx){}

void FluoSimView::removeRegionMeasuredRegionsGetClicked()
{
	int	item_idx = m_measuredRegions_listWidget.currentRow();
	if(item_idx == -1) return; //->

	QString rgn_name = m_measuredRegions_listWidget.item(item_idx)->text();
	auto item = m_measuredRegions_listWidget.item(item_idx);
	delete item;
	removeRegionMeasuredRegions(rgn_name.toLocal8Bit().data());
}

void FluoSimView::removeRegionMeasuredRegions(string rgn_name){}

void FluoSimView::addRegionBleachedRegionsGetClicked()
{
	string rgn_name = m_allBleachedRegions_comboBx.currentText().toLocal8Bit().data();
	addRegionBleachedRegions(rgn_name);
}
void FluoSimView::addRegionBleachedRegions(string rgn_idx){}

void FluoSimView::removeRegionBleachedRegionsGetClicked()
{
	int	item_idx = m_bleachedRegions_listWidget.currentRow();
	if(item_idx == -1) return; //->

	QString rgn_name = m_bleachedRegions_listWidget.item(item_idx)->text();
	auto item = m_bleachedRegions_listWidget.item(item_idx);
	delete item;
	removeRegionBleachedRegions(rgn_name.toLocal8Bit().data());
}

void FluoSimView::removeRegionBleachedRegions(string rgn_idx){}

