
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

#ifndef FLUOSIM_H
#define FLUOSIM_H

#include "stdlib.h"
#include "iostream"
#include "vector"

#include "GL_glew/glew.h"
#include "GL/gl.h"

#include "cellEngine_library_global.h"
    #include "biologicalWorld/BiologicalWorld.h"
    #include "biologicalWorld/Probe.h"
    #include "biologicalWorld/FrapHead.h"
    #include "physicsEngine/DiffusionSubEngine.h"
    #include "displayAndcontrol/Graphics/Graphic.h"
    #include "Measure/Signal.h"
    #include "TracePlayer.h"

#include "toolBox_src/toolbox_library_global.h"
    #include "toolBox_src/glGUI/glObjects/myGLGeomtricObject.h"
    #include "toolBox_src/glGUI/glObjects/myGLImageObjects.h"
    #include "toolBox_src/myQtWidgets/myDropMenu.h"
    #include "toolBox_src/myQtWidgets/myTableWidget.h"
	#include "toolBox_src/myQtWidgets/myComboBox.h"
    #include "toolBox_src/myQtWidgets/myMainWindow.h"
    #include "toolBox_src/myQtWidgets/myProgressWidget.h"
    #include "toolBox_src/myQtWidgets/myDockwidget.h"
    #include "toolBox_src/myQtWidgets/mySpinBoxedSlider.h"
    #include "toolBox_src/myQtWidgets/myDoubleSpinBoxedSlider.h"

#include "tinytiffwriter.h"

#include "QDesktopServices"
#include "QPixmap"
#include "QSignalMapper"
#include "QMimeData"
#include "QApplication"
#include "QRgb"
#include "QtWidgets/QFrame"
#include "QtWidgets/QLineEdit"
#include "QtWidgets/QTextEdit"
#include "QtWidgets/QLabel"
#include "QtWidgets/QSlider"
#include "QtWidgets/QMainWindow"
#include "QtWidgets/QComboBox"
#include "QtWidgets/QMenu"
#include "QtWidgets/QHeaderView"
#include "QtWidgets/QMenuBar"
#include "QtWidgets/QCheckBox"
#include "QtWidgets/QListWidget"
#include "QtWidgets/QDockWidget"
#include "QtWidgets/QToolBar"
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QSplashScreen"
#include "QtWidgets/QWidgetAction"



int getRegionIdx(QString rgn_name);


enum RENDERING_MODE
{
	RENDERING_POINTS, RENDERING_GAUSSIANS, RENDERING_LINES, RENDERING_POINTS_AND_LINES,
	RENDERING_GAUSSIANS_AND_LINES, RENDERING_ACCUMULATION_GAUSSIANS, RENDERING_GAUSSIANS_LOOKUP_TABLE,RENDERING_CAMERA_TEXTURE
};

enum PARTICLE_COLOR
{
	PARTICLE_RED, PARTICLE_GREEN, PARTICLE_BLUE, PARTICLE_WHITE, PARTICLE_RANDOMCOLOR, PARTICLE_TRAPPING_STATE_COLOR, PARTICLE_LOOKUP_TABLE
};

enum SIMULATOR_MODE
{
	GEOMETRY_MODE, LIVE_MODE, EXPERIMENT_MODE, ANALYSIS_MODE
};

enum LIVE_EXPERIMENT_TYPE
{
	PHOTOBLEACHING_LIVE_EXPERIMENT, PHOTOACTIVATION_LIVE_EXPERIMENT, FCS_LIVE_EXPERIMENT
};

enum EXPERIMENT_TYPE
{
	SPT_EXPERIMENT, FRAP_EXPERIMENT, PAF_EXPERIMENT, FCS_EXPERIMENT, SRI_EXPERIMENT, DRUG_EXPERIMENT
};

enum DRUG_AFFECTED_PARAM
{
	DOUT_DRUGPARAM, DIN_DRUGPARAM, DTRAP_DRUGPARAM, KON_DRUGPARAM, KOFF_DRUGPARAM, POROS_DRUGPARAM
};

enum ACQUISITION_TYPE
{
	STREAM_ACQUISITION, TIMELAPSE_ACQUISITION
};

enum RECORDING_MODE
{
	SEQUENTIAL_RECORDING, ALL_IN_ONE_RECORDING
};

struct particleSystemParams
{
	int N_particles;
	glm::vec4 particle_color;
};

struct dynamicParems
{
	float D_outsideContact; //[px^2/s]
    float D_insideContact;
	float k_on;
	float k_off;
	float D_trapped; //[px^2/s]
	float crossingProbability_outIn;
	float immobileFraction;
	bool isFixed;

	bool isDrugAffected;
	DRUG_AFFECTED_PARAM drug_affectedParam;
	float drug_newValue;
};

struct photoPhysicsParams
{
	float k_on_fluo;
	float k_off_fluo;
};

struct renderingParams
{
	float image_intensity;
	list<string> highlightedRegion_names;
	bool areRegionsVisible;
	bool isBackgroundImageVisible_inGeom;
	bool isBackgroundImageVisible_inLiveAndExp;
	bool isBackgroundImageSmoothed;
	float spot_size; //[µm]
    float spot_intensity; //in AD counts and between 0 and 1.
    bool isSpotIntensity_inPhotonsPerSec;
    float ADCounts_perPhoton;
    bool isAutoscale;
    float autoscale_factor;
	uint point_size;
	float point_intensity;
	RENDERING_MODE rendering_mode;
	PARTICLE_COLOR particle_color;
	bool areWordsVisible;
	string backgroundImage_path;
	glm::vec4 clear_color;

    uint16 photonBackground; //[photon/s] -> the associated camera offset = ph.B.*dt*A/DCountsPerPhoton
    bool isUsingPoissonNoise;
    uint16 cameraOffset; //[A/D counts]
    float readoutNoise_sigma;

    Screen::RENDERING_TARGET rendering_target;
};

struct simulationParams
{
	float dt_sim; //[ms]
	int current_plane;
	float current_time;

	float pixel_size; //[µm]
};

struct liveExperimentParams
{
	LIVE_EXPERIMENT_TYPE experimentType;
	string measured_RgnName;
	string bleached_RgnName;
	string photoActivated_RgnName;
};

struct experimentParams
{
	EXPERIMENT_TYPE experimentType;
	ACQUISITION_TYPE acquisitionType;

	int N_planes;
	int N_frap;
	int dN_frap;
	float k_off_frap;
	float initial_enrichment;

	bool is_fcsUsingGaussianBeam;
	float fcs_beamSigma;
	float fcs_maxIntensity;
	float fcs_beamKoff;

	int N_photoActivation;
	int dN_photoActivation;
	float k_on_photoActivation;

	int N_drug;
	DRUG_AFFECTED_PARAM drug_affectedParam;
	float drug_newValue;

	float SRI_pointingAccuracy; //µm
	float SRI_zoom;

	int N_presequ;
	int N_repetition;
	int index_repetion;
	float acquisitionPeriod;

	vector<string> measuredRegionNames_v;
	vector<string> bleachedRegionNames_v;

	RECORDING_MODE recording_mode;
	string file_destination;
	bool isExportingStack;
};

struct simulationState
{
	SIMULATOR_MODE simulator_mode;

	bool simulationStarted;
	bool simulationEnded;
	bool simulationPaused;

	bool isSingleShot = false;
	bool isRenderingEnabled;

	bool isGraphicVisible;
	bool isParameterWidgetVisible;
};


class FluoSimView : public myMainWindow
{
Q_OBJECT

public :

	enum SLIDER_ID{imageIntensity_slider,
				   insideD_slider,
				   outsideD_slider,
				   trappedD_slider,
				   kon_slider,
				   koff_slider,
				   immobileFraction_slider,
				   crossingProbability_slider,
				   nbParticles_slider,
				   spotSize_slider,
				   spotIntensity_slider,
				   SRISpotSize_slider,
				   SRISpotIntensity_slider,
				   blinkingKon_slider,
				   blinkingKoff_slider};

    enum ACTION_ID{resetGeometry_action,
                   loadGeometry_action,
				   saveGeometry_action,
                   newProject_action,
				   loadProject_action,
				   saveProject_action,
				   loadBackgroundImage_action,
				   captureScreen_action,
				   evaluateSteadyState_action,
				   exit_action,
				   openUserManual_action,
				   openAbout_action,
				   loadSPT_action,
				   loadFRAPIn_action,
				   loadFRAPOut_action,
				   loadFCSIn_action,
				   loadFCSOut_action,
				   loadSRI_action,
                   loadPAFIn_action,
                   loadPAFOut_action,
                   showAndHideSimParametersWdgt_action,
				   showAndHideGraphicWdgt_action,
				   drawRect_action,
				   drawCircle_action,
				   drawPolygon_action};

	enum ONGLET_ID{GEOMETRY_ONGLET, LIVE_ONGLET, EXPERIMENT_ONGLET, ANALYSIS_ONGLET};

    enum DIFFUSION_UNIT_COMBOBOX_ITEMS{DIFFUSION_UNIT_COMBOBOX_PERCENTAGE, DIFFUSION_UNIT_COMBOBOX_UM2PERSEC};
    enum SPOTINTENSITY_UNIT_COMBOBOX_ITEMS{SPOTINTENSITY_UNIT_COMBOBOX_ADCOUNTS, SPOTINTENSITY_UNIT_COMBOBOX_PHOTONPERSEC};

	enum RENDERING_COMBOBOX_ITEMS
    {
        RENDERING_COMBOBOX_POINTS,
        RENDERING_COMBOBOX_GAUSSIANS,
    };

	enum PARTICLECOLOR_COMBOBOX_ITEMS
    {
        PARTICLECOLOR_COMBOBOX_RED,
        PARTICLECOLOR_COMBOBOX_GREEN,
        PARTICLECOLOR_COMBOBOX_BLUE,
        PARTICLECOLOR_COMBOBOX_WHITE,
        PARTICLECOLOR_COMBOBOX_RANDOMCOLOR,
        PARTICLECOLOR_COMBOBOX_TRAPPINGSTATECOLOR,
        PARTICLECOLOR_COMBOBOX_PSEUDOCOLOR
    };

	FluoSimView(QApplication* main_app);
	~FluoSimView();
	void connectSignals();

	bool pollEvents();
	void buildParameterWidget();
	void setStatusTip();

    void showGaussianParameters(bool visible);
    void showLivePhotoBleachingParameters(bool visible);
    void showLivePhotoActivationParameters(bool visible);
    void showTimeLapseParameters(bool visible);
    void showFrapParameter(bool visible);
    void showPhotoActivationParameters(bool visible);
    void showExportStackParameters(bool visible);
    void enableExperimentParameters(bool enabled);
    void enableSimulationStateButtons(bool enabled);
    void enableDynamicParameters(bool enabled);
    void enableExportStackParameters(bool enabled);
    void enableExportStackCheckBox(bool enabled);

	void buildGraphicWidget();
	void buildMenuBar();
    void buildToolBar();

	float updateSliderLabelValue(SLIDER_ID slider_id);
	void updateDrugNewValueSpinBox();
	void updateParameterWidgetSize();
    void updateDinsideSpinBxedSlider();

	void setStyleSheets();

	virtual void sliderValueGetChanged(SLIDER_ID slider_name, float value); //to convey information to the controler
	virtual void actionGetTriggered(ACTION_ID id);

	void displayAboutFluoSim();

public slots :

	void onSliderValueGetChanged(int slider_id); //use the enum defined above : SLIDER_NAMES
    void onSliderValueGetChanged(int slider_id, double value); //use the enum defined above : SLIDER_NAMES
    void onActionTriggered(int action_id);
	virtual void onOngletGetChanged(int onglet_id); //to gather changes...

	//*******************************
    //*	   1st Panel: GEOMETRY	*
	//*******************************

	virtual void imageSmoothingGetChanged(bool value);
	virtual void deleteRegionAllRegionsGetClicked();
	virtual void regionCellGetEntered(int row, int column);
	virtual void regionCellSelectionGetChanged();
    virtual void regionCellGetDoubleClicked(int row, int column);
    virtual void allRegionsTableGetLeft();

    //****************************
    //*	   2nd Panel: LIVE     	 *
    //****************************

	virtual void playButtonHasBeenClicked(bool);
	virtual void pauseButtonHasBeenClicked(bool);
	virtual void stopButtonHasBeenClicked(bool);

	virtual void NplanesGetChanged(double new_value);
    virtual void DinsideUnitGetChanged(int new_unit);
	virtual void fixationGetChanged(bool);
	virtual void immobileFractionGetChanged(double new_value);

	virtual void renderingModeGetChanged(int new_mode);
	virtual void particleColorGetChanged(int);
	virtual void SRIParticleColorGetChanged(int);
	virtual void pixelSizeGetChanged(double new_pixelSize);
	virtual void timeStepGetChanged(double new_timeStep);

	virtual void regionVisibilityGetChanged(bool);
	virtual void backgroundVisiblityInGeomGetChanged(bool);
	virtual void backgroundVisiblityInLiveAndExpGetChanged(bool);
    virtual void spotIntensityUnitGetChanged(int new_unit);
    virtual void ADCountsPerPhotonGetChanged(double value);
    virtual void autoscaleGetChanged(bool);

	virtual void regionGetHighlighted(QString region_name);
    virtual void regionComboBoxFocusGetLost();

	virtual void liveExperimentTypeGetChanged(int);
	virtual void measuredRegiongetChanged(QString);
	virtual void bleachedRegionGetChanged(QString);
	virtual void photoActivatedRegionGetChanged(QString);
//	virtual void bleachedRegiongetChanged(int);

	virtual void regionGetBleached();
	virtual void regionGetPhotoActivated();

	//**********************************
    //*	   3rd Panel: EXPERIMENT	   *
	//**********************************

	virtual void experimentTypeGetChanged(int);
	virtual void acquisitionTypeGetChanged(int);
	virtual void acquisitionPeriodGetChanged(double);
	virtual void acquisitionPresequenceGetChanged(int);
	virtual void acquisitionNPlanesGetChanged(int);
	virtual void frapPlaneIdxGetChanged(int);
	virtual void frapDPlaneGetChanged(int);
	virtual void frapKoffGetChanged(double);
	virtual void photoActivationPlaneIdxGetChanged(int);
	virtual void photoActivationDPlaneGetChanged(int);
	virtual void photoActivationKonGetChanged(double);
	virtual void FCSBeamIsEnabledGetChanged(bool);
	virtual void FCSBeamSigmaGetChanged(double);
	virtual void FCSBeamMaxIntensityGetChanged(double);
	virtual void FCSBeamKoffGetChanged(double);

	virtual void SRIPointingAccuracyGetChanged(double);
	virtual void SRIIntensityGetChanged(int);
	virtual void SRIZoomLevelGetChanged(int);

	virtual void drugAdminPlaneChanged(int);
	virtual void drugAdminParamChanged(int);
	virtual void drugAdminNewValueChanged(double);
	virtual void NbRepetitionGetChanged(int);
	virtual void initialEnrichmentGetChanged(double);

	virtual void fileDestinationButtonHasBeenClicked();
	virtual void isExportingStackGetChanged(bool);
    virtual void photonBackgroundHasBeenChanged(int);
	virtual void isStackUsingPoissonNoiseGetChanged(bool);
    virtual void cameraOffsetHasBeenChanged(int);
    virtual void cameraReadoutNoiseHasBeenChanged(double);

	void addRegionMeasuredRegionsGetClicked();
	virtual void addRegionMeasuredRegions(string rgn);
	void removeRegionMeasuredRegionsGetClicked();
	virtual void removeRegionMeasuredRegions(string rgn);

	void addRegionBleachedRegionsGetClicked();
	virtual void addRegionBleachedRegions(string rgn_name);
	void removeRegionBleachedRegionsGetClicked();
	virtual void removeRegionBleachedRegions(string rgn_name);

	virtual void graphicDockWidgetClosed();

private slots :

	void mainWindowGetMoved(QMoveEvent *event);
	void mainWindowGetResized(QResizeEvent *event);
	void mainWindowGetLowered();
	void mainWindowGetRaised();
	void applicationStateGetChanged(Qt::ApplicationState state);

protected :

	/**Event Handling***/
	void virtual treatEvent(QEvent* event);
		void dropEvent(QDropEvent* event);
		void dragEnterEvent(QDragEnterEvent* event);

protected :

	QSplashScreen m_splashScreen;

	myMainWindow& m_mainWindow;
	QWidget m_centralWidget;
	QVBoxLayout m_centralLayout;
    ScreenHandler m_scrn;

	myDockWidget m_graphic_dockWdgt;
	QWidget m_graphic_wdgt;
	QVBoxLayout m_graphic_layout;
	Graphic* m_graphic;

	QSignalMapper m_sliders_signalMapper;
	QSignalMapper m_action_signalMapper;

//*********Menu Bar***********
		QMenu m_file_menu;
			QAction m_reset_geometry_action;
			QAction m_load_geometry_action;
			QAction m_save_geometry_action;
            QAction m_new_project_action;
			QAction m_load_project_action;
			QAction m_save_project_action;
			QAction m_captureScreen_action;
			QAction m_exit_action;

		QMenu m_example_menu;
			QAction m_load_SPT_parameters_action;
			QMenu m_FRAPExample_menu;
				QAction m_load_FRAPIn_parameters_action;
				QAction m_load_FRAPOut_parameters_action;
			QMenu m_FCSExample_menu;
				QAction m_load_FCSIn_parameters_action;
				QAction m_load_FCSOut_parameters_action;
			QAction m_load_SRI_parameters_action;
            QMenu m_PAFExample_menu;
                QAction m_load_PAFIn_parameters_action;
                QAction m_load_PAFOut_parameters_action;

		QMenu m_window_menu;
			QAction m_showAndHideSimParametersWdgt_action;
			QAction m_showAndHideGraphicWdgt_action;

		QMenu m_help_menu;
			QAction m_openUserManual_action;
			QAction m_openAbout_action;

//*********Dialog Widgets***********
	QDialog* m_aboutFluoSim_dialog;
	QHBoxLayout* m_aboutFluoSim_layout;

		QLabel* m_aboutFluoSim_label;
		QPushButton* m_aboutFluoSim_pushButton;

//*********Main Window***********
	myProgressWidget m_progressWdgt;

	QDockWidget m_ui_dockWdgt;
	QWidget m_ui_wdgt;
	QVBoxLayout m_ui_VLayout;

		QHBoxLayout m_onglets_layout;
		QSignalMapper m_ongletsPushButton_signalMapper;
		QPushButton m_geometryOnglet_pushButton;  QPushButton m_liveOnglet_pushButton;
		QPushButton m_experimentOnglet_pushButton;	QPushButton m_analysisOnglet_pushButton;

		QWidget m_parameters_wdgt;
		QVBoxLayout m_parameters_VLayout;

		//*******************************
        //*	   1st Panel: GEOMETRY	*
		//*******************************

		myDropMenu m_backgroundImage_dropMenu;
		QFrame m_backgroundImageFrame;
		QVBoxLayout m_backgroundImage_vlayout;
			QHBoxLayout m_loadBackgroundImage_Hlayout;
					QLineEdit m_loadBackgroundImage_lineEdit;   QPushButton m_loadBackgroundImage_pushButton;
			QHBoxLayout m_imageIntensity_Hlayout;
					float m_imageIntensity_slider_minValue; float m_imageIntensity_slider_maxValue;
					QLabel m_imageIntensity_label;	QSlider m_imageIntensity_slider; QLabel m_imageIntensityValue_label;
			QHBoxLayout m_imageVisibilityAndSmoothing_Hlayout;	QLabel m_imageVisibility_label;                     QLabel m_imageSmoothing_label;
																	QCheckBox m_backgroundVisibleInGeom_checkBox;			    QCheckBox m_imageSmoothing_checkBox;

		myDropMenu m_allRegions_dropMenu;
		QFrame m_allRegionsFrame;
		QVBoxLayout m_allRegions_vlayout;
				QHBoxLayout m_importExportGeometry_layout; QPushButton m_importGeometry_pushButton; QPushButton m_exportGeometry_pushButton;
                myTableWidget m_allRegions_tableWidget;
				QHBoxLayout m_deleteRegions_hLayout; QPushButton m_deleteRegionAllRegion_pushButton; QPushButton m_deleteAllRegionAllRegion_pushButton;

        //***************************
        //*	   2nd Panel: LIVE      *
        //***************************

		myDropMenu m_simulationStates_dropMenu;
		QFrame m_simulationStatesFrame;
		QVBoxLayout m_simulationStates_vlayout;

			QHBoxLayout m_simulationStatesButtons_layout;
			QPushButton m_play_pushBut; QPushButton m_pause_pushBut; QPushButton m_stop_pushBut;
			QProgressBar m_simulation_progressBar; QDoubleSpinBox m_simulationMaxPlane_dSpinBx;
			QHBoxLayout m_timeStep_layout; QLabel m_timeStep_label; QDoubleSpinBox m_timeStep_dSpinBx;

		QGridLayout m_particleSystem_Glayout;

		myDropMenu m_particleSystem_dropMenu;
		QFrame m_particleSystemFrame;
		QVBoxLayout m_particleSystemProperties_Vlayout;
            mySpinBoxedSlider m_nbParticles_spinBxedSlider;
//			QHBoxLayout m_nbParticles_layout;	QLabel m_nbParticles_label; QSlider m_nbParticles_slider; QLabel m_nbParticlesValue_label;
			QHBoxLayout m_evaluateSteadyState_layout;	QPushButton m_evaluateSteadyState_pushBut;

		myDropMenu m_simulation_dropMenu;
		QFrame m_simulationFrame;
			QHBoxLayout m_simulation_layout;

				QHBoxLayout m_pixelSize_layout; QLabel m_pixelSize_label; QDoubleSpinBox m_pixelSize_dSpinBx;

		myDropMenu m_dynamic_dropMenu;
		QFrame m_dynamicFrame;
		QVBoxLayout m_rows_layout;
			float m_DSlider_minValue; float m_DSlider_maxValue;
			float m_KSlider_minValue; float m_KSlider_maxValue;

            myDoubleSpinBoxedSlider m_outsideD_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_insideD_dSpinBxedSlider; QMetaObject::Connection m_insideD_dSpinBxedSlider_connection;
            myDoubleSpinBoxedSlider m_trappedD_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_crossingProbability_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_kon_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_koff_dSpinBxedSlider;

			QHBoxLayout m_immobileFraction_layout;	QLabel m_immobileFraction_label; QDoubleSpinBox m_immobileFraction_dSpinBx;
					QLabel m_fixation_label; QCheckBox m_fixation_checkBx;

		myDropMenu m_PhotoPhysics_dropMenu;
		QFrame m_PhotoPhysicsFrame;
		QVBoxLayout m_PhotoPhysics_Vlayout;
			float m_blinkingKSlider_minValue; float m_blinkingKSlider_maxValue;

            myDoubleSpinBoxedSlider m_blinkingKon_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_blinkingKoff_dSpinBxedSlider;

		myDropMenu m_rendering_dropMenu;
		QFrame m_renderingFrame;
		QVBoxLayout m_renderingProperties_Vlayout;
			float m_spotSizeSlider_minValue; float m_spotSizeSlider_maxValue;
			float m_spotIntensitySlider_minValue; float m_spotIntensitySlider_maxValue;
            float m_spotIntensitySlider_photonPerSec_minValue; float m_spotIntensitySlider_photonPerSec_maxValue;
            float m_photonConversion_minValue; float m_photonConversion_maxValue;

			QHBoxLayout m_propertiesVisibility_layout;
			QHBoxLayout m_regionsVisible_layout;	QLabel m_regionsVisible_label; QCheckBox m_regionsVisible_checkBx;
			QHBoxLayout m_backgroundVisible_layout;	QLabel m_backgroundVisible_label; QCheckBox m_backgroundVisibleInLiveAndExp_checkBx;
			QHBoxLayout m_renderingParticle_layout;	QLabel m_renderingMode_label; QComboBox m_renderingMode_comboBx;
																QLabel m_particleColor_label; QComboBox m_particleColor_comboBx;
			QVBoxLayout m_renderingPropertiesSliders_Vlayout;
            myDoubleSpinBoxedSlider m_spotSize_dSpinBxedSlider;
            myDoubleSpinBoxedSlider m_spotIntensity_spinBxedSlider;
            QWidget m_autoscale_wLayout; 		QHBoxLayout m_autoscale_hLayout; 	QLabel m_photonConversion_label; QDoubleSpinBox m_photonConversion_dSpinBx;
                                                QLabel m_autoscale_label; QCheckBox m_autoscale_checkBx;

		myDropMenu m_liveExperiment_dropMenu;
		QFrame m_liveExperimentFrame;
		QVBoxLayout m_liveExperimentProperties_Vlayout;

			QHBoxLayout m_measuredRegion_layout;	QLabel m_measuredRegion_label; myComboBox m_measuredRegion_comboBx;
			QHBoxLayout m_liveExperimentType_layout;	QLabel m_liveExperimentType_label; QComboBox m_liveExperimentType_comboBx;

                QHBoxLayout m_bleachedRegion_layout;	QLabel m_bleachedRegion_label; myComboBox m_bleachedRegion_comboBx;
				QHBoxLayout m_startBleaching_layout; QPushButton m_startBleaching_pushButton;

                QHBoxLayout m_photoActivatedRegion_layout;	QLabel m_photoActivatedRegion_label; myComboBox m_photoActivatedRegion_comboBx;
				QHBoxLayout m_startPhotoActivation_layout; QPushButton m_startPhotoActivation_pushButton;

		//**********************************
        //*	   3rd Panel: EXPERIMENT	   *
		//**********************************

		myDropMenu m_wProtocoleExperiment_dropMenu;
		QFrame m_wProtocoleExperimentFrame;
		QVBoxLayout m_wProtocoleExperimentProperties_Vlayout;

			QHBoxLayout m_experimentType_layout;	QLabel m_experimentType_label; QComboBox m_experimentType_comboBx;
			QHBoxLayout m_acquisitionMode_layout;	QLabel m_acquisitionMode_label; QComboBox m_acquisitionMode_comboBox;
			QHBoxLayout m_timelapse_period_layout;	QLabel m_timelapse_period_label; QDoubleSpinBox m_timelapse_period_dSpinBox;
			QHBoxLayout m_presequ_layout;	QLabel m_presequ_label; QSpinBox m_presequ_spinBox;
			QHBoxLayout m_duration_layout;	QLabel m_duration_label; QSpinBox m_duration_spinBox;
			QHBoxLayout m_repetition_layout;	QLabel m_repetition_label; QSpinBox m_repetitioon_spinBox;
			QHBoxLayout m_initialEnrichment_layout;	QLabel m_initialEnrichment_label; QDoubleSpinBox m_initialEnrichment_spinBox;

		myDropMenu m_FRAPExperimentParameters_dropMenu;
		QFrame m_FRAPExperimentParameters_Frame;
		QVBoxLayout m_FRAPExperimentParameters_Vlayout;

			QHBoxLayout m_frapPlane_layout;	QLabel m_frapPlane_label; QSpinBox m_frapPlane_spinBox;
			QHBoxLayout m_frapDPlane_layout;	QLabel m_frapDPlane_label; QSpinBox m_frapDPlane_spinBox;
			QHBoxLayout m_frapKoff_layout;	QLabel m_frapKoff_label; QDoubleSpinBox m_frapKoff_dspinBox;

		myDropMenu m_PAFExperimentParameters_dropMenu;
		QFrame m_PAFExperimentParameters_Frame;
		QVBoxLayout m_PAFExperimentParameters_Vlayout;

			QHBoxLayout m_photoActivationPlane_layout;	QLabel m_photoActivationPlane_label; QSpinBox m_photoActivationPlane_spinBox;
			QHBoxLayout m_photoActivationDPlane_layout;	QLabel m_photoActivationDPlane_label; QSpinBox m_photoActivationDPlane_spinBox;
			QHBoxLayout m_photoActivationKon_layout;	QLabel m_photoActivationKon_label; QDoubleSpinBox m_photoActivationKon_dspinBox;

		myDropMenu m_FCSExperimentParameters_dropMenu;
		QFrame m_FCSExperimentParameters_Frame;
		QVBoxLayout m_FCSExperimentParameters_Vlayout;

			QHBoxLayout m_FCSBeamIsEnabled_layout;	QLabel m_FCSBeamIsEnabled_label; QCheckBox m_FCSBeamIsEnabled_checkBox;
			QHBoxLayout m_FCSBeamSigma_layout;	QLabel m_FCSBeamSigma_label; QDoubleSpinBox m_FCSBeamSigma_dSpinBox;
			QHBoxLayout m_FCSBeamMaxIntensity_layout;	QLabel m_FCSBeamMaxIntensity_label; QDoubleSpinBox m_FCSBeamMaxIntensity_dSpinBox;
			QHBoxLayout m_FCSBeamKoff_layout;	QLabel m_FCSBeamKoff_label; QDoubleSpinBox m_FCSBeamKoff_dSpinBox;

		myDropMenu m_SRIParameters_dropMenu;
		QFrame m_SRIParametersFrame;
		QVBoxLayout m_SRIParameters_Vlayout;

			float m_SRISpotSizeSlider_minValue; float m_SRISpotSizeSlider_maxValue;
			float m_SRISpotIntensitySlider_minValue; float m_SRISpotIntensitySlider_maxValue;

			QHBoxLayout m_SRIPointingAccuracy_layout;	QLabel m_SRIPointingAccuracy_label;  QDoubleSpinBox m_SRIPointingAccuracy_dSpinBx;
			QHBoxLayout m_SRIIntensity_layout;	QLabel m_SRIIntensity_label; QSpinBox m_SRIIntensity_spinBx;
			QHBoxLayout m_SRIZoomLevel_layout;	QLabel m_SRIZoomLevel_label; QSpinBox m_SRIZoomLevel_spinBx;

		myDropMenu m_drugExperimentParameters_dropMenu;
		QFrame m_drugExperimentParameters_Frame;
		QVBoxLayout m_drugExperimentParameters_Vlayout;

			QHBoxLayout m_drugAdminPlane_layout;	QLabel m_drugAdminPlane_label; QSpinBox m_drugAdminPlane_spinBox;
			QHBoxLayout m_drugAdminParamChanged_layout;	QLabel m_drugAdminParamChanged_label; QComboBox m_drugAdminParamChanged_comboBx;
			QHBoxLayout m_drugAdminNewValue_layout;	QLabel m_drugAdminNewValue_label; QDoubleSpinBox m_drugAdminNewValue_dspinBox;

		myDropMenu m_wProtocoleExperimentMeasuredRegions_dropMenu;
		QFrame m_wProtocoleExperimentMeasuredRegionsFrame;
		QVBoxLayout m_wProtocoleExperimentMeasuredRegions_Vlayout;

		QHBoxLayout m_addMeasuredRegion_Hlayout;
            QPushButton m_addMeasuredRegion_pushButton; myComboBox m_allMeasuredRegions_comboBx;
			QListWidget m_measuredRegions_listWidget;
			QPushButton m_removeMeasuredRegion_pushButton;

		myDropMenu m_wProtocoleExperimentBleachedRegions_dropMenu;
		QFrame m_wProtocoleExperimentBleachedRegionsFrame;
		QVBoxLayout m_wProtocoleExperimentBleachedRegions_Vlayout;

		QHBoxLayout m_addBleachedRegion_Hlayout;
            QPushButton m_addBleachedRegion_pushButton; myComboBox m_allBleachedRegions_comboBx;
			QListWidget m_bleachedRegions_listWidget;
			QPushButton m_removeBleachedRegion_pushButton;

		myDropMenu m_fileDestination_dropMenu;
		QFrame m_fileDestinationFrame;

		QVBoxLayout m_fileDestination_Vlayout;
			QHBoxLayout m_fileDestination_Hlayout; QLineEdit m_fileDestination_lineEdit;
													QPushButton m_fileDestination_pushButton;
			QHBoxLayout m_exportStack_Hlayout; QLabel m_exportStack_label;
                                                QCheckBox m_exportStack_checkBx; QLabel m_exportStack_noBackgroundImage_label;
            QHBoxLayout m_exportStackIlluminationProperty_Hlayout; QLabel m_photonBackground_label; QSpinBox m_photonBackground_spinBx;
												QLabel m_stackPoissonN_label; QCheckBox m_stackPoissonN_checkBx;
            QHBoxLayout m_exportStackCameraProperty_Hlayout; QLabel m_cameraOffset_label; QSpinBox m_cameraOffset_spinBx;
                                                QLabel m_cameraReadoutNoise_label; QDoubleSpinBox m_cameraReadoutNoise_dSpinBx;


	QToolBar m_drawing_toolBar;
		QActionGroup m_drawing_actionGroup;
			QAction m_drawRect_action;
			QAction m_drawPolygon_action;
			QAction m_drawCircle_action;

	QApplication* m_main_app;

};



class FluoSimModel : public FluoSimView
{
	friend class myProjectGenerator;

public :

	FluoSimModel(QApplication* main_app);
	~FluoSimModel();

	void initializeBioWorld();
    void initializeModelParameters();
	virtual void resetGeometry();//used
	void deleteGeometry(string rgn_name);
	void deleteGeometries(vector<string>& rgn_names_v);
	void addGeometry(std::vector<glm::vec2>& r_v);
	void evaluateSteadyState();
    void evaluateAutoscaleFactor();
	void imposeSteadyState();

    void resetProject();
    virtual void loadProject(string project_path);//used
	virtual void saveProject(string project_path);//used
	virtual void loadGeometry(string path = "");//used
	void saveGeometry();

	bool loadBackgroundImage(string& imageFile_path);

	virtual void renderBioWorld();//used
	void renderCamera(); //used
	virtual void renderBioWorldCamera(bool wDrawing = true);//used
	virtual void updateBioWorld();//used
	virtual void measureBioWorld();//used
	void computeAutoCorrelogram();
	void clearProbeSignals();
	void clearRecordedProbeSignals();
	void captureScreen(string& file_path);
	void captureCamera(string tiff_path = string(""), myTiff::OPENING_MODE mode = myTiff::WRITE_MODE);
	void captureCamera(TinyTIFFFile* tiff_hdl, myTiff::OPENING_MODE mode = myTiff::WRITE_MODE);

	virtual void renderGraphicCurves();

	void runSimulator();

	virtual void startSimulation();//used
	virtual void pauseSimulation();//used
	virtual void stopSimulation();//used

	void setDestinationPath(string path);
	void saveSimulationProducts();

	virtual void setSimulatorMode(SIMULATOR_MODE simulator_mode);//used
	SIMULATOR_MODE getSimulatorMode();
	virtual void setExperimentType(EXPERIMENT_TYPE experiment_type);//used
	virtual void setRenderingParams(renderingParams* rendering_params);
//	virtual void setAcquisitionType(ACQUISITION_TYPE acquisition_type);//used

    //views change params in the data structures, then we need to tell the bioWorld the new values
    //i.e. : views -> control -> model (structures fist, then the bioWorld with the methods below)
	void updateParticleSystemParams();
	void updateDynamicParams();
	void updatePhotoPhysicsParams();
	void updateSimulationParams();
	void updateExperimentParams();
	void updateLiveExperimentParams();
	void updateRenderingParams();

	void updateGLWords();

protected :

	myFont m_font1;
	myWord m_simulationFPS_glWord;
	myWord m_singleThreadedTime_glWord;
	myWord m_multiThreadedTime_glWord;
	myWord m_nbThreads_glWord;
	myWord m_diffTime_glWord;
	myWord m_trappTime_glWord;
	myWord m_photoTime_glWord;

	myChrono m_fullLoop_clock;
	myChrono m_fullLoop_clock2;
	myChrono m_rendering_clock;
	myChrono m_updatingBioWorld_clock;
	myChrono m_measuringBioWorld_clock;

	QApplication* m_main_app;
	bool m_app_running;
	float m_frame_rate;

	myGLImage* m_backgroundImage;
	myGLImage* m_cameraImage;
	BiologicalWorld* m_bioWorld;
	FrapHead* m_frapHead;

	DiffusionSubEngine* m_engine;
	TracePlayerControl m_tracePlayer;
	Probe m_probe;
    Signal m_signal;
	TinyTIFFFile* m_stack_tiffHdl;

	vector<Probe*> m_experimental_probes_v;
	vector<vector<vector<glm::vec2> > >m_recordedSignalValues_perRep_perProbe;
	vector<FrapHead> m_experimental_frapHead_v;

	particleSystemParams m_particleSystem_params;
	dynamicParems m_dynamic_params;
    photoPhysicsParams m_photoPhysics_params;
	simulationParams m_simulation_params;
	liveExperimentParams m_liveExperiment_params;
	experimentParams m_experiment_params;
	renderingParams* m_current_rendering_params;
		renderingParams m_rendering_params;
		renderingParams m_stormRendering_params;
		renderingParams m_cameraRendering_params;
	simulationState m_simulation_states;

	gstd::gMultiVector<glm::vec2> m_textureRect_coords_gMV;
};





class FluoSim : public FluoSimModel
{
Q_OBJECT

public :

	FluoSim(QApplication* main_app);

	//***CORE FUNCTIONS***
	virtual void loadGeometry(string path = "");
	virtual void loadProject(string project_path);
	virtual void saveProject(string project_path);

	virtual void renderBioWorld();
	virtual void renderBioWorldCamera();
	virtual void updateBioWorld();
	virtual void measureBioWorld();
	virtual void renderGraphicCurves();

	virtual void startSimulation();
	virtual void pauseSimulation();
	virtual void stopSimulation();

	virtual void setSimulatorMode(SIMULATOR_MODE simulator_mode);

	//***COMMUNICATION FUNCTIONS***
	//view -> model
	virtual void sliderValueGetChanged(SLIDER_ID slider_id, float value);
	virtual void actionGetTriggered(ACTION_ID id);
	virtual void onOngletGetChanged(int onglet_id);

	//*******************************
    //*	   1st Panel: GEOMETRY   	*
	//*******************************

	virtual void imageSmoothingGetChanged(bool value);
	virtual void deleteRegionAllRegionsGetClicked();
	virtual void regionCellGetEntered(int row, int column);
	virtual void regionCellSelectionGetChanged();
	virtual void regionCellGetDoubleClicked(int row, int column);
    virtual void allRegionsTableGetLeft();

    //*****************************
    //*	   2nd Panel: LIVE    	  *
    //*****************************

	virtual void playButtonHasBeenClicked(bool);
	virtual void pauseButtonHasBeenClicked(bool);
	virtual void stopButtonHasBeenClicked(bool);

	virtual void NplanesGetChanged(double new_value);
    virtual void DinsideUnitGetChanged(int new_unit);
    virtual void fixationGetChanged(bool isFixated);
	virtual void immobileFractionGetChanged(double new_value);

	virtual void renderingModeGetChanged(int new_mode);
	virtual void particleColorGetChanged(int);
	virtual void SRIParticleColorGetChanged(int);
	virtual void pixelSizeGetChanged(double new_pixelSize);
	virtual void timeStepGetChanged(double new_timeStep);

	virtual void regionVisibilityGetChanged(bool);
	virtual void backgroundVisiblityInGeomGetChanged(bool);
    virtual void backgroundVisiblityInLiveAndExpGetChanged(bool);
    virtual void spotIntensityUnitGetChanged(int new_unit);
    virtual void ADCountsPerPhotonGetChanged(double value);
    virtual void autoscaleGetChanged(bool);

	virtual void measuredRegiongetChanged(QString rgn_name);
    virtual void regionComboBoxFocusGetLost();
    virtual void regionGetHighlighted(QString region_name);
	virtual void liveExperimentTypeGetChanged(int);
	virtual void bleachedRegionGetChanged(QString rgn_name);
	virtual void photoActivatedRegionGetChanged(QString rgn_name);

	virtual void regionGetBleached();
	virtual void regionGetPhotoActivated();

	//**********************************
    //*	   3rd Panel: EXPERIMENT	   *
	//**********************************

	virtual void experimentTypeGetChanged(int);
	virtual void acquisitionTypeGetChanged(int);
	virtual void acquisitionPeriodGetChanged(double);
	virtual void acquisitionPresequenceGetChanged(int);
	virtual void acquisitionNPlanesGetChanged(int);
	virtual void frapPlaneIdxGetChanged(int);
	virtual void frapDPlaneGetChanged(int);
	virtual void frapKoffGetChanged(double);
	virtual void photoActivationPlaneIdxGetChanged(int);
	virtual void photoActivationDPlaneGetChanged(int);
	virtual void photoActivationKonGetChanged(double);
	virtual void FCSBeamIsEnabledGetChanged(bool);
	virtual void FCSBeamSigmaGetChanged(double);
	virtual void FCSBeamMaxIntensityGetChanged(double);
	virtual void FCSBeamKoffGetChanged(double);
	virtual void SRIPointingAccuracyGetChanged(double);
	virtual void SRIIntensityGetChanged(int);
	virtual void SRIZoomLevelGetChanged(int);
	virtual void drugAdminPlaneChanged(int);
	virtual void drugAdminParamChanged(int);
	virtual void drugAdminNewValueChanged(double);
	virtual void NbRepetitionGetChanged(int);
	virtual void initialEnrichmentGetChanged(double);
	virtual void fileDestinationButtonHasBeenClicked();
	virtual void isExportingStackGetChanged(bool);
    virtual void photonBackgroundHasBeenChanged(int);
	virtual void isStackUsingPoissonNoiseGetChanged(bool);
    virtual void cameraOffsetHasBeenChanged(int);
    virtual void cameraReadoutNoiseHasBeenChanged(double);
    virtual void addRegionMeasuredRegions(string rgn_name);
	virtual void removeRegionMeasuredRegions(string rgn_name);
	virtual void addRegionBleachedRegions(string rgn_name);
	virtual void removeRegionBleachedRegions(string rgn_name);

	virtual void graphicDockWidgetClosed();

	//model -> view
	void updateSimulationProgressViews();
	void setViewsFromModel();

protected :

	void virtual treatEvent(QEvent* event);

protected slots :

	void rgnHasBeenDrawn_slot(std::vector<glm::vec2> r_v);

private :


};






#endif // FLUOSIM_H
