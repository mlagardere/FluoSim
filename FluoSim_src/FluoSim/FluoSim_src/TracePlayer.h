
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

#ifndef TRACEPLAYER_H
#define TRACEPLAYER_H

#include "algorithm"
#include "stdlib.h"

#include "glm.hpp"

#include "QtWidgets/QFrame"
#include "QtWidgets/QLineEdit"
#include "QtWidgets/QLabel"
#include "QtWidgets/QSlider"
#include "QtWidgets/QMainWindow"
#include "QtWidgets/QComboBox"
#include "QSignalMapper"
#include "QMimeData"
#include "QDoubleSpinBox"

#include "gpuTools_library_global.h"
    #include "gpuTools_src/gl/gProgram.h"
    #include "gpuTools_src/gl/gTexture.h"
    #include "gpuTools_src/gl/gToolBox.h"

#include "cellEngine_library_global.h"
    #include "Measure/Trace.h"
    #include "Measure/Signal.h"

#include "toolBox_src/toolbox_library_global.h"
    #include "toolBox_src/developmentTools/myClock.h"
    #include "toolBox_src/fileAndstring_manipulation/file_manipulation.h"
    #include "toolBox_src/imageTools/image_format/tiff_format.h"
    #include "toolBox_src/glGUI/glScreenObjects/myGLScreenProceduralImage.h"
    #include "toolBox_src/glGUI/glScreenObjects/myGLScreenGeometricObjects.h"
	#include "toolBox_src/glGUI/glObjects/textRendering/myFont.h"
	#include "toolBox_src/glGUI/glObjects/textRendering/myWord.h"
	#include "toolBox_src/glGUI/glObjects/myGLTiffMovieObject.h"
    #include "toolBox_src/imageTools/image_colorisation/myLUT.h"
    #include "toolBox_src/myQtWidgets/myDropMenu.h"
    #include "toolBox_src/imageTools/TIFFMovie/myTiffMovie.h"
    #include "toolBox_src/myQtWidgets/myProgressWidget.h"

class TracePlayerView : public QMainWindow
{
Q_OBJECT

friend class FluoSim;

public :

	enum EXPORT_TYPE {MSD_PER_TRACE_EXPORT,
					  D_PER_TRACE_EXPORT,
					  DINST_PER_TRACE_EXPORT,
					  D_HISTOGRAM_EXPORT,
					  DINST_HISTOGRAM_EXPORT,
					  LENGTH_HISTOGRAM_EXPORT,
					  SCREENSHOT_EXPORT,
					  TRAJECTORIES_EXPORT};

    TracePlayerView(QWidget* parent = 0);

	void connectSignals();
	void buildWidget();
	void setStyleSheets();
	void showMSDExportParameters(bool isShown);
	void showDHistoExportParameters(bool isShown);
	void showLengthHistoExportParameters(bool isShown);
	void showTrajectoryExportParameters(bool isShown);

	void virtual treatEvent(QEvent * event);
		virtual void dropEvent(QDropEvent * event);
		virtual void dragEnterEvent(QDragEnterEvent * event);
		virtual void mouseMoveEvent(QMouseEvent * event);
		virtual void mousePressEvent(QMouseEvent * event);
		virtual void mouseReleaseEvent(QMouseEvent * event);
		virtual void wheelEvent(QWheelEvent * event);
	myGLWidget* getGLWidget();
	QWidget*  getParamtersWidget();
	QSlider* getPlaneSlider();
	void setProgressWdgt(myProgressWidget* progressWdgt);
	void setMainWindow(const QMainWindow* mainWindow);

private slots :

	virtual void onFileAction(QString sender);
	virtual void timestepGetChanged(double);
	virtual void pixelSizeGetChanged(double);

	virtual void onTrajectoriesRenderingModeGetChanged(int idx);
	virtual void onEventsRenderingModeGetChanged(int idx);
	virtual void onColorModeGetChanged(int idx);

	virtual void planeSliderIdxChanged(int new_planeIdx);

	virtual void spotSizeGetChanged(double value);
	virtual void spotIntensityGetChanged(double value);

	virtual void spotSizeGetChanged(int value);
	virtual void spotIntensityGetChanged(int value);

	virtual void lineWidthGetChanged(int value);
	virtual void lineIntensityGetChanged(int value);

	virtual void lutMinValueGetChanged(double);
	virtual void lutMaxValueGetChanged(double);

	virtual void lutGetChanged(int);

//	virtual void lineWidthGetChanged(int value);
//	virtual void lineIntensityGetChanged(double value);

	virtual void filteringMinLengthValueGetChanged(int value);
	virtual void filteringMaxLengthValueGetChanged(int value);
	virtual void filteringMinLogDValueGetChanged(int value);
	virtual void filteringMaxLogDValueGetChanged(int value);

	virtual void DNbPointsMSDFitValueGetChanged(int value);
	virtual void DInstNbPointsBeforeMSDFitValueGetChanged(int value);
	virtual void DInstNbPointsAfterMSDFitValueGetChanged(int value);

	virtual void DsHistogramMinValueGetChanged(double value);
	virtual void DsHistogramMaxValueGetChanged(double value);
	virtual void DsHistogramNbIntervalsGetChanged(double value);
	virtual void lengthsHistogramMinValueGetChanged(double value);
	virtual void lengthsHistogramMaxValueGetChanged(double value);
	virtual void lengthsHistogramNbIntervalsGetChanged(double value);

	virtual void MSDMaxDPlaneGetChanged(int value);
	virtual void DFirstMSDPointIndexGetChanged(int value);
	virtual void DlastMSDPointIndexGetChanged(int value);

	virtual void trajectoryExportedFormatGetChanged(int value);

	virtual void exportTypeGetChanged(int);
	virtual void onExportButtonGetClicked();

	virtual void onOpenFileAction();
	virtual void onClearFileAction();
	virtual void onOpenMovieAction();
	virtual void onClearMovieAction();
	virtual void onSaveFileAction();
	virtual void onDsHistogramExportAction();
	virtual void onLengthsHistogramExportAction();
	virtual void onMSDExportAction();
	virtual void onMultipageTiffExportAction();
	virtual void onBatchModeSelectionAction();
	virtual void onAddPathsAction();
	virtual void onClearPathsAction();

protected :

	QApplication* m_main_app;

	QSignalMapper m_action_signalMapper;


	const QMainWindow* m_mainWindow = 0;
	myProgressWidget* m_progressWdgt = 0;

	QWidget m_parameters_wdgt;
	QVBoxLayout m_parameters_vLayout;

	//FILE LOADER
		myDropMenu m_fileLoader_dropMenu;
		QFrame m_fileLoader_frame;
		QVBoxLayout m_fileLoader_Vlayout;

		QHBoxLayout m_fileLoader_Hlayout;
			QLineEdit m_fileLoader_lineEdit;   QPushButton m_fileLoader_pushButton; QPushButton m_fileLoader_clearFile_pushButton;
		QHBoxLayout m_calibration_Hlayout;
			QLabel m_timeStep_label;   QDoubleSpinBox m_timeStep_dSpinBx; QLabel m_pixelSize_label; QDoubleSpinBox m_pixelSize_dSpinBx;

	//MOVIE LOADER
		myDropMenu m_movieLoader_dropMenu;
		QFrame m_movieLoader_frame;

		QHBoxLayout m_movieLoader_Hlayout;
			QLineEdit m_movieLoader_lineEdit;   QPushButton m_movieLoader_pushButton; QPushButton m_movieLoader_clearMovie_pushButton;

	//RENDERING
		myDropMenu m_rendering_dropMenu;
		QFrame m_rendering_frame;
		QVBoxLayout m_renderingProperties_Vlayout;
			float m_spotSizeSlider_minValue; float m_spotSizeSlider_maxValue;
			float m_spotIntensitySlider_minValue; float m_spotIntensitySlider_maxValue;

			float m_lineWidthSlider_minValue; float m_lineWidthSlider_maxValue;
			float m_lineIntensitySlider_minValue; float m_lineIntensitySlider_maxValue;

		QHBoxLayout m_eventRenderingMode_HLayout;	QLabel m_eventRenderingMode_label; QComboBox m_eventRenderingMode_comboBx;
		QHBoxLayout m_traceRenderingMode_HLayout;	QLabel m_traceRenderingMode_label; QComboBox m_traceRenderingMode_comboBx;
		QHBoxLayout m_renderingColor_HLayout;	QLabel m_renderingColor_label; QComboBox m_renderingColor_comboBx;
		QHBoxLayout m_lut_HLayout;	QLabel m_lut_label; QComboBox m_lut_comboBx;

		QHBoxLayout m_spotSize_HLayout;	QLabel m_spotSize_label; QSlider m_spotSize_slider; QLabel m_spotSizeValue_label;
		QHBoxLayout m_spotIntensity_HLayout;	QLabel m_spotIntensity_label; QSlider m_spotIntensity_slider; QLabel m_spotIntensityValue_label;
		QHBoxLayout m_lineWidth_HLayout;	QLabel m_lineWidth_label; QSlider m_lineWidth_slider; QLabel m_lineWidthValue_label;
		QHBoxLayout m_lineIntensity_HLayout;	QLabel m_lineIntensity_label; QSlider m_lineIntensity_slider; QLabel m_lineIntensityValue_label;
		QHBoxLayout m_lutMinMax_HLayout;	QLabel m_lutMin_label; QDoubleSpinBox m_lutMin_dSpinBx;
												QLabel m_lutMax_label; QDoubleSpinBox m_lutMax_dSpinBx;

	//FILTERING
		myDropMenu m_filtering_dropMenu;
		QFrame m_filtering_frame;
		QVBoxLayout m_filteringProperties_Vlayout;
			float m_lengthSlider_minValue; float m_lengthSlider_maxValue;
			float m_logDSlider_minValue; float m_logDSlider_maxValue;

		QHBoxLayout m_minLength_HLayout;	QLabel m_minLength_label; QSlider m_minLength_slider; QLabel m_minLengthValue_label;
		QHBoxLayout m_maxLength_HLayout;	QLabel m_maxLength_label; QSlider m_maxLength_slider; QLabel m_maxLengthValue_label;
		QHBoxLayout m_minLogD_HLayout;	QLabel m_minLogD_label; QSlider m_minLogD_slider; QLabel m_minLogDValue_label;
		QHBoxLayout m_maxLogD_HLayout;	QLabel m_maxLogD_label; QSlider m_maxLogD_slider; QLabel m_maxLogDValue_label;


	//ADVANCED PARAMETERS
		myDropMenu m_advancedParameters_dropMenu;
		QFrame m_advancedParameters_frame;
		QVBoxLayout m_advancedParameters_Vlayout;

		QHBoxLayout m_D_nbPointsFit_HLayout;	QLabel m_D_nbPointsFit_label; QSpinBox m_D_nbPointsFit_spinBx;
		QHBoxLayout m_DInst_nbPointsBefore_HLayout;	QLabel m_DInst_nbPointsBefore_label; QSpinBox m_DInst_nbPointsBefore_spinBx;
		QHBoxLayout m_DInst_nbPointsAfter_HLayout;	QLabel m_DInst_nbPointsAfter_label; QSpinBox m_DInst_nbPointsAfter_spinBx;

	//EXPORT
		myDropMenu m_export_dropMenu;
		QFrame m_exportProperties_frame;
		QVBoxLayout m_exportProperties_Vlayout;
//			float m_lengthSlider_minValue; float m_lengthSlider_maxValue;
//			float m_logDSlider_minValue; float m_logDSlider_maxValue;

		QHBoxLayout m_exportedType_HLayout;	QLabel m_exportedType_label; QComboBox m_exportedType_comboBox;

		QHBoxLayout m_MSDMaxDPlane_HLayout;	QLabel m_MSDMaxDPlane_label; QSpinBox m_MSDMaxDPlane_spinBox;

		QHBoxLayout m_DsHistogram_min_HLayout;	QLabel m_DsHistogram_min_label; QDoubleSpinBox m_DsHistogram_min_dSpinBox;
		QHBoxLayout m_DsHistogram_max_HLayout;	QLabel m_DsHistogram_max_label; QDoubleSpinBox m_DsHistogram_max_dSpinBox;
		QHBoxLayout m_DsHistogram_nInterval_HLayout;	QLabel m_DsHistogram_nInterval_label; QDoubleSpinBox m_DsHistogram_nInterval_dSpinBox;

		QHBoxLayout m_lengthsHistogram_min_HLayout;	QLabel m_lengthsHistogram_min_label; QDoubleSpinBox m_lengthsHistogram_min_dSpinBox;
		QHBoxLayout m_lengthsHistogram_max_HLayout;	QLabel m_lengthsHistogram_max_label; QDoubleSpinBox m_lengthsHistogram_max_dSpinBox;
		QHBoxLayout m_lengthsHistogram_nInterval_HLayout;	QLabel m_lengthsHistogram_nInterval_label; QDoubleSpinBox m_lengthsHistogram_nInterval_dSpinBox;

		QHBoxLayout m_trajectoryFormat_HLayout;	QLabel m_trajectoryFormat_label; QComboBox m_trajectoryFormat_comboBox;

		QPushButton m_export_pushButton;


	QWidget m_central_widget;
	QVBoxLayout m_layout;
//		myGLWidget m_glWidget;
		QSlider m_slider;
};



class TracePlayerModel : public TracePlayerView
{
public :

	enum RENDERING_MODE {GAUSSIAN_BANDW, GAUSSIAN_COLOR, LINE_COLOR, LINE_COLOR_AND_GAUSSIAN_COLOR};
	enum LUTName {JET_LUT, HSV_LUT, SPRING_LUT, WINTER_LUT};
	enum COLOR_MODE{BANDW_COLOR_MODE, RANDOM_COLOR_MODE, D_COLOR_MODE, DINST_COLOR_MODE, LENGTH_COLOR_MODE};
	enum TRAJECTORIES_RENDERING_MODE {NONE_TRAJECTORIES_RENDERING_MODE, CURRENT_TRAJECTORIES_RENDERING_MODE, ALL_TRAJECTORIES_RENDERING_MODE};
	enum EVENTS_RENDERING_MODE {NONE_EVENTS_RENDERING_MODE, CURRENT_EVENTS_RENDERING_MODE, ALL_EVENTS_RENDERING_MODE};

    TracePlayerModel(myGLWidget* glWidget, QWidget* parent = 0);

private : void setTraces(std::vector<Trace> &traces);

public  :

	void setTimeStep(float timestep);
	void setPixelSize(float pixel_size);

	void setLUT(const string& lut_path);

	void setTracesFromBinaryFile(string path);
	void setTracesFromStringFile(string path);
	void clearTraces();
	void setTracesDsFromStringFile(string path);
	void setMovieFromStringFile(string path);
	void clearMovie();

		virtual void nbTracesChanged(int new_nb_traces);
		virtual void maxPlaneChanged(int new_max_plane);
		virtual void minPlaneChanged(int new_min_plane);
		virtual void planeRangedChanged();

		virtual void maxLengthChanged(int new_maxLength);
		virtual void minLengthChanged(int new_maxLength);

	virtual void setCurrentPlane(long plane);
	long getCurrentPlane();

	void updateRenderingPipeline(); //update pipeline...needed after filtering, changes in representation mode
	void render(myGLWidget* glWidget);

	void captureScreen(myGLWidget* window, string& file_path);
	void captureScreen2(myGLWidget* window);

	void filter();
	void computeMSDsAndDs(float pixel_size, float dt);

	//rendering text

	void updateCurrentPlaneWord();
	void updateNbTracesWord();
	void updateNbEventsWord();

protected :

	//file
	string m_last_dir;
	float m_pixel_size;
	float m_timestep;

	//filtering
	int m_min_length;
	int m_max_length;
	float m_minLogD;
	float m_maxLogD;

	//rendering
	float m_lut_minValue;
	float m_lut_maxValue;
	LUTName m_lutName;
	std::map<LUTName, string> m_lut_paths;

	//MSD
	int m_MSD_maxDPplane;
	int m_D_firstMSDPoint_idx;
	int m_D_lastMSDPoint_idx;

	int m_D_nbPointsMSDFit;
	int m_DInst_nbPointsBeforeMSDFit;
	int m_DInst_nbPointsAfterMSDFit;

	//histograms_prop
	int m_DsHistogram_nbIntervals;
	float m_DsHistogram_minValue;
	float m_DsHistogram_maxValue;

	int m_lengthsHistogram_nbIntervals;
	float m_lengthsHistogram_minValue;
	float m_lengthsHistogram_maxValue;

	//exported_trajectory
	FLUOEVENT_FILE_FORMAT m_exportedTrajectory_fileFormat;

	//data
	std::vector<Trace> m_traces;
	std::vector<Trace> m_filteredTraces;

	long m_current_plane;
	long m_min_plane;
	long m_max_plane;
	std::string m_trace_path;

	//batch
	bool m_isBatchModeSelected;
	std::vector<string> m_tracePaths_v;

	//gl
	myGLWidget* m_glWidget;
	myLUT m_lut;
	glm::vec2 m_square_size;
	float m_spotIntensity;
	float m_lineWidth;
	float m_lineIntensity;
	glm::mat4 m_scaling_matrix;

	gstd::gProgram m_pgm_gaussianBandW;
	gstd::gProgram m_pgm_gaussianColor;
	gstd::gProgram m_pgm_lineColor;
	gstd::gProgram m_pgm_lineBandW;

	gstd::gProgram m_pgm_events;
	gstd::gProgram m_pgm_trajectories;

	gstd::gVector<glm::vec4> m_gColor;
	gstd::gVector<glm::vec2> m_gR;
	gstd::gVector<glm::vec2> m_gR_traces;
	gstd::gVector<glm::vec4> m_gColor_traces;
	gstd::gTexture m_gaussian_texture;

	Signal m_D_distribution_sig;
	Signal m_lengths_distribution_sig;

	gstd::gMultiVector<glm::vec2> r_geom_gMV;
	gstd::gMultiVector<glm::vec2> r_contour_gMV;
	myMultiVector<glm::vec2> r_geom_MV;
	myMultiVector<glm::vec2> r_contour_MV;
//		myGLScreenProceduralImage m_Length_graphic;
//		myGLScreenProceduralImage m_D_graphic;


	TRAJECTORIES_RENDERING_MODE m_trajectories_renderingMode;
	EVENTS_RENDERING_MODE m_events_renderingMode;
	COLOR_MODE m_colorMode;

	RENDERING_MODE m_rendering_mode;

    //textures
	myFont m_font;
	myWord m_currentPlane_word;
	myWord m_numberTraces_word;
	myWord m_numberEvents_word;

    //TIFF MOVIE
	myGLTiffMovie m_tiffMovie;
};



class TracePlayerControl : public TracePlayerModel
{
public :

    TracePlayerControl(myGLWidget* glWidget, QWidget* parent = 0);
	virtual void treatEvent(QEvent * event);
	void render();
	void render(myGLWidget* glWdgt);

	//from view
		//sliders :
		virtual void planeSliderIdxChanged(int new_sliderIdx);

		virtual void timestepGetChanged(double);
		virtual void pixelSizeGetChanged(double);

		virtual void spotSizeGetChanged(double value);
		virtual void spotIntensityGetChanged(double value);

		virtual void spotSizeGetChanged(int value);
		virtual void spotIntensityGetChanged(int value);

		virtual void lineWidthGetChanged(int value);
		virtual void lineIntensityGetChanged(int value);

		void virtual lutMinValueGetChanged(double);
		void virtual lutMaxValueGetChanged(double);

		virtual void lutGetChanged(int);

		virtual void filteringMinLengthValueGetChanged(int value);
		virtual void filteringMaxLengthValueGetChanged(int value);
		virtual void filteringMinLogDValueGetChanged(int value);
		virtual void filteringMaxLogDValueGetChanged(int value);

		virtual void DNbPointsMSDFitValueGetChanged(int value);
		virtual void DInstNbPointsBeforeMSDFitValueGetChanged(int value);
		virtual void DInstNbPointsAfterMSDFitValueGetChanged(int value);

		virtual void DsHistogramMinValueGetChanged(double value);
		virtual void DsHistogramMaxValueGetChanged(double value);
		virtual void DsHistogramNbIntervalsGetChanged(double value);
		virtual void lengthsHistogramMinValueGetChanged(double value);
		virtual void lengthsHistogramMaxValueGetChanged(double value);
		virtual void lengthsHistogramNbIntervalsGetChanged(double value);

		virtual void MSDMaxDPlaneGetChanged(int value);
		virtual void DFirstMSDPointIndexGetChanged(int value);
		virtual void DlastMSDPointIndexGetChanged(int value);

		virtual void trajectoryExportedFormatGetChanged(int value);

		virtual void exportTypeGetChanged(int);
		virtual void onExportButtonGetClicked();


        //rendering actions
		virtual void onTrajectoriesRenderingModeGetChanged(int idx);
		virtual void onEventsRenderingModeGetChanged(int idx);
		virtual void onColorModeGetChanged(int idx);

        //file actions
		virtual void onOpenFileAction();
		virtual void onClearFileAction();
		virtual void onOpenMovieAction();
		virtual void onClearMovieAction();
		virtual void onSaveFileAction();
		virtual void onDsHistogramExportAction();
		virtual void onLengthsHistogramExportAction();
		virtual void onMSDExportAction();
		virtual void onMultipageTiffExportAction();
		virtual void onBatchModeSelectionAction();
		virtual void onAddPathsAction();
		virtual void onClearPathsAction();

	//from model
	virtual void nbTracesChanged(int new_nb_traces);
	virtual void minPlaneChanged(int new_min_plane);
	virtual void maxPlaneChanged(int new_max_plane);
	virtual void planeRangedChanged();

	virtual void maxLengthChanged(int new_maxLength);
	virtual void minLengthChanged(int new_maxLength);

	virtual void setCurrentPlane(long plane);

private :
};



#endif // TRACEPLAYER_H
