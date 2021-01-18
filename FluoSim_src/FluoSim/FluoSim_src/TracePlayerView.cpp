
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

#include "TracePlayer.h"


using namespace std;


TracePlayerView::TracePlayerView(QWidget* parent) :

	QMainWindow(parent),
	m_slider(this)

{
	m_slider.setGeometry(10,220,180,20);
	m_slider.setOrientation(Qt::Horizontal);

		m_slider.setStyleSheet("QSlider::handle:horizontal {width: 5px;\n height: 25px;\n background: rgb(240,188,23);\n"
							   "margin: 2 2px;}\n\n"
							   "QSlider::groove:horizontal {background: QColor(25,25,25);}\n\n");
	connectSignals();
	buildWidget();
	setStyleSheets();
}

QWidget* TracePlayerView::getParamtersWidget()
{
	return &m_parameters_wdgt;
}

QSlider* TracePlayerView::getPlaneSlider()
{
	return &m_slider;
}

void TracePlayerView::setProgressWdgt(myProgressWidget* progressWdgt)
{
	m_progressWdgt = progressWdgt;
}

void TracePlayerView::setMainWindow(const QMainWindow* mainWindow)
{
	m_mainWindow = mainWindow;
}

void TracePlayerView::connectSignals()
{
		m_action_signalMapper.setMapping(&m_fileLoader_pushButton, "openFile_action");
		m_action_signalMapper.setMapping(&m_fileLoader_clearFile_pushButton, "clearFile_action");
		m_action_signalMapper.setMapping(&m_movieLoader_pushButton, "openMovie_action");
		m_action_signalMapper.setMapping(&m_movieLoader_clearMovie_pushButton, "clearMovie_action");


	connect(&m_action_signalMapper, SIGNAL(mapped(QString)), this, SLOT(onFileAction(QString)));
		connect(&m_fileLoader_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_fileLoader_clearFile_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_movieLoader_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));
		connect(&m_movieLoader_clearMovie_pushButton, SIGNAL(clicked()), &m_action_signalMapper, SLOT(map()));

	connect(&m_timeStep_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(timestepGetChanged(double)));
	connect(&m_pixelSize_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(pixelSizeGetChanged(double)));
	connect(&m_slider, SIGNAL(valueChanged(int)), this, SLOT(planeSliderIdxChanged(int)));
	connect(&m_traceRenderingMode_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(onTrajectoriesRenderingModeGetChanged(int)));
	connect(&m_eventRenderingMode_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(onEventsRenderingModeGetChanged(int)));
	connect(&m_renderingColor_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(onColorModeGetChanged(int)));

	connect(&m_spotSize_slider, SIGNAL(valueChanged(int)), this, SLOT(spotSizeGetChanged(int)));
	connect(&m_spotIntensity_slider, SIGNAL(valueChanged(int)), this, SLOT(spotIntensityGetChanged(int)));
	connect(&m_lineWidth_slider, SIGNAL(valueChanged(int)), this, SLOT(lineWidthGetChanged(int)));
	connect(&m_lineIntensity_slider, SIGNAL(valueChanged(int)), this, SLOT(lineIntensityGetChanged(int)));

	connect(&m_lutMin_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(lutMinValueGetChanged(double)));
	connect(&m_lutMax_dSpinBx, SIGNAL(valueChanged(double)), this, SLOT(lutMaxValueGetChanged(double)));
	connect(&m_lut_comboBx, SIGNAL(currentIndexChanged(int)), this, SLOT(lutGetChanged(int)));

	connect(&m_minLength_slider, SIGNAL(valueChanged(int)), this, SLOT(filteringMinLengthValueGetChanged(int)));
	connect(&m_maxLength_slider, SIGNAL(valueChanged(int)), this, SLOT(filteringMaxLengthValueGetChanged(int)));
	connect(&m_minLogD_slider, SIGNAL(valueChanged(int)), this, SLOT(filteringMinLogDValueGetChanged(int)));
	connect(&m_maxLogD_slider, SIGNAL(valueChanged(int)), this, SLOT(filteringMaxLogDValueGetChanged(int)));

	connect(&m_D_nbPointsFit_spinBx, SIGNAL(valueChanged(int)), this, SLOT(DNbPointsMSDFitValueGetChanged(int)));
	connect(&m_DInst_nbPointsBefore_spinBx, SIGNAL(valueChanged(int)), this, SLOT(DInstNbPointsBeforeMSDFitValueGetChanged(int)));
	connect(&m_DInst_nbPointsAfter_spinBx, SIGNAL(valueChanged(int)), this, SLOT(DInstNbPointsAfterMSDFitValueGetChanged(int)));

	connect(&m_exportedType_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(exportTypeGetChanged(int)));
	connect(&m_MSDMaxDPlane_spinBox, SIGNAL(valueChanged(int)), this, SLOT(MSDMaxDPlaneGetChanged(int)));
	connect(&m_DsHistogram_min_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(DsHistogramMinValueGetChanged(double)));
	connect(&m_DsHistogram_max_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(DsHistogramMaxValueGetChanged(double)));
	connect(&m_DsHistogram_nInterval_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(DsHistogramNbIntervalsGetChanged(double)));
	connect(&m_lengthsHistogram_min_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(lengthsHistogramMinValueGetChanged(double)));
	connect(&m_lengthsHistogram_max_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(lengthsHistogramMaxValueGetChanged(double)));
	connect(&m_lengthsHistogram_nInterval_dSpinBox, SIGNAL(valueChanged(double)), this, SLOT(lengthsHistogramNbIntervalsGetChanged(double)));	
	connect(&m_trajectoryFormat_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(trajectoryExportedFormatGetChanged(int)));

	connect(&m_export_pushButton, SIGNAL(clicked()), this, SLOT(onExportButtonGetClicked()));
}


void TracePlayerView::buildWidget()
{
	string app_dir = m_main_app->applicationDirPath().toLocal8Bit().data();

	//***DROP MENU : FILE LOADER***

			//1ST LINE : FILE LOADER
				m_fileLoader_lineEdit.setReadOnly(true);
				m_fileLoader_lineEdit.setText("");
				m_fileLoader_pushButton.setText("Browse...");
				m_fileLoader_clearFile_pushButton.setFixedSize(17,17);
			m_fileLoader_Hlayout.addWidget(&m_fileLoader_lineEdit);
			m_fileLoader_Hlayout.addWidget(&m_fileLoader_pushButton);
			m_fileLoader_Hlayout.addWidget(&m_fileLoader_clearFile_pushButton);

			m_timeStep_label.setText("Time Step ");
			m_timeStep_dSpinBx.setPrefix("[s] ");
			m_timeStep_dSpinBx.setValue(0);
			m_timeStep_dSpinBx.setDecimals(3);
			m_timeStep_dSpinBx.setSingleStep(0.002);
			m_timeStep_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
			m_pixelSize_label.setText("Pixel Size ");
			m_pixelSize_dSpinBx.setPrefix("[µm] ");
			m_pixelSize_dSpinBx.setDecimals(3);
			m_pixelSize_dSpinBx.setSingleStep(0.005);
			m_pixelSize_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
			m_calibration_Hlayout.addStretch(10);
			m_calibration_Hlayout.addWidget(&m_timeStep_label);
			m_calibration_Hlayout.addWidget(&m_timeStep_dSpinBx);
			m_calibration_Hlayout.addStretch(5);
			m_calibration_Hlayout.addWidget(&m_pixelSize_label);
			m_calibration_Hlayout.addWidget(&m_pixelSize_dSpinBx);
			m_calibration_Hlayout.addStretch(10);

			m_fileLoader_Vlayout.addLayout(&m_fileLoader_Hlayout);
			m_fileLoader_Vlayout.addLayout(&m_calibration_Hlayout);

		m_fileLoader_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
		m_fileLoader_frame.setLineWidth(1);
		m_fileLoader_frame.setMidLineWidth(1);
		m_fileLoader_frame.setStyleSheet("QFrame {border-color : grey;}");
		m_fileLoader_frame.setLayout(&m_fileLoader_Vlayout);

	m_fileLoader_dropMenu.setText("Trajectories");
	m_fileLoader_dropMenu.setWidget(&m_fileLoader_frame);

	//***DROP MENU : MOVIE LOADER***

			//1ST LINE : FILE LOADER
				m_movieLoader_lineEdit.setReadOnly(true);
				m_movieLoader_lineEdit.setText("");
				m_movieLoader_pushButton.setText("Browse...");
			m_movieLoader_clearMovie_pushButton.setFixedSize(17,17);
			m_movieLoader_Hlayout.addWidget(&m_movieLoader_lineEdit);
			m_movieLoader_Hlayout.addWidget(&m_movieLoader_pushButton);
			m_movieLoader_Hlayout.addWidget(&m_movieLoader_clearMovie_pushButton);

			m_movieLoader_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
			m_movieLoader_frame.setLineWidth(1);
			m_movieLoader_frame.setMidLineWidth(1);
			m_movieLoader_frame.setStyleSheet("QFrame {border-color : grey;}");
			m_movieLoader_frame.setLayout(&m_movieLoader_Hlayout);

	m_movieLoader_dropMenu.setText("Movie");
	m_movieLoader_dropMenu.setWidget(&m_movieLoader_frame);

	//***DROP MENU : RENDERING***

			//1ST LINE : EVENT RENDERING MODE
				m_eventRenderingMode_label.setText("Event Rendering Mode ");
				m_eventRenderingMode_label.setStyleSheet("QLabel {font-weight : bold ;}");
				QStringList eventRenderingMode_items({"NONE", "CURRENT EVENTS", "ALL EVENTS"});
				m_eventRenderingMode_comboBx.addItems(eventRenderingMode_items);

			m_eventRenderingMode_HLayout.addWidget(&m_eventRenderingMode_label);
			m_eventRenderingMode_HLayout.addWidget(&m_eventRenderingMode_comboBx);

			//2ND LINE : TRACE RENDERING MODE
				m_traceRenderingMode_label.setText("Trace Rendering Mode ");
				m_traceRenderingMode_label.setStyleSheet("QLabel {font-weight : bold ;}");
				QStringList traceRenderingMode_items({"NONE", "CURRENT TRACES", "ALL TRACES"});
				m_traceRenderingMode_comboBx.addItems(traceRenderingMode_items);

			m_traceRenderingMode_HLayout.addWidget(&m_traceRenderingMode_label);
			m_traceRenderingMode_HLayout.addWidget(&m_traceRenderingMode_comboBx);

			//3RD LINE : RENDERING COLOR
				m_renderingColor_label.setText("Color Code ");
				m_renderingColor_label.setStyleSheet("QLabel {font-weight : bold ;}");
				QStringList renderingColor_items({"BLACK AND WHITE", "RANDOM", "DIFFUSION", "INSTANT DIFFUSION", "LENGTH"});
				m_renderingColor_comboBx.addItems(renderingColor_items);

			m_renderingColor_HLayout.addWidget(&m_renderingColor_label);
			m_renderingColor_HLayout.addWidget(&m_renderingColor_comboBx);

			//4th LINE : LUT
				m_lut_label.setText("Look Up Table ");
				m_lut_label.setStyleSheet("QLabel {font-weight : bold ;}");
				QStringList lut_items({"Jet", "Hsv", "Spring", "Winter"});
				m_lut_comboBx.addItems(lut_items);
                m_lut_comboBx.setItemIcon(0, QIcon("Resources/Luts/jet.png"));
                m_lut_comboBx.setItemIcon(1, QIcon("Resources/Luts/hsv.png"));
                m_lut_comboBx.setItemIcon(2, QIcon("Resources/Luts/spring.png"));
                m_lut_comboBx.setItemIcon(3, QIcon("Resources/Luts/winter.png"));
				m_lut_comboBx.setIconSize(QSize(100,15));
				m_lut_comboBx.setStyleSheet("QComboBox {background-color: transparent;}"
											"QComboBox {color: white;}"
											"QComboBox QAbstractItemView {selection-background-color : rgb(100,100,100); selection-color : white;}\n"
											"QComboBox {selection-background-color : transparent; selection-color : white;}\n");
			m_lut_HLayout.addWidget(&m_lut_label);
			m_lut_HLayout.addWidget(&m_lut_comboBx);

			//3RD LINE : SPOT SIZE
				m_spotSize_label.setText("Spot Size ");
				m_spotSize_slider.setOrientation(Qt::Horizontal);
				m_spotSize_slider.setMinimum(0.0);
				m_spotSize_slider.setMaximum(1000.0);
				m_spotSize_slider.setValue(0.0);
				m_spotSizeValue_label.setText(QString("%1").arg(m_spotSize_slider.value(),10));

			m_spotSize_HLayout.addSpacing(12);;
			m_spotSize_HLayout.addWidget(&m_spotSize_label);
			m_spotSize_HLayout.addWidget(&m_spotSize_slider);
			m_spotSize_HLayout.addWidget(&m_spotSizeValue_label);

			//3RD LINE : SPOT INTENSITY
				m_spotIntensity_label.setText("Spot Intensity ");
				m_spotIntensity_slider.setOrientation(Qt::Horizontal);
				m_spotIntensity_slider.setMinimum(0.0);
				m_spotIntensity_slider.setMaximum(1000.0);
				m_spotIntensity_slider.setValue(0.0);
				m_spotIntensityValue_label.setText(QString("%1").arg(m_spotIntensity_slider.value(),10));

			m_spotIntensity_HLayout.addSpacing(12);
			m_spotIntensity_HLayout.addWidget(&m_spotIntensity_label);
			m_spotIntensity_HLayout.addWidget(&m_spotIntensity_slider);
			m_spotIntensity_HLayout.addWidget(&m_spotIntensityValue_label);

			//4TH LINE : LINE SIZE
				m_lineWidth_label.setText("Line Size ");
				m_lineWidth_slider.setOrientation(Qt::Horizontal);
				m_lineWidth_slider.setMinimum(0.0);
				m_lineWidth_slider.setMaximum(1000.0);
				m_lineWidth_slider.setValue(0.0);
				m_lineWidthValue_label.setText(QString("%1").arg(m_lineWidth_slider.value(),10));
			m_lineWidth_HLayout.addSpacing(12);
			m_lineWidth_HLayout.addWidget(&m_lineWidth_label);
			m_lineWidth_HLayout.addWidget(&m_lineWidth_slider);
			m_lineWidth_HLayout.addWidget(&m_lineWidthValue_label);

			//5TH  LINE : LINE INTENSITY
				m_lineIntensity_label.setText("Line Intensity ");
				m_lineIntensity_slider.setOrientation(Qt::Horizontal);
				m_lineIntensity_slider.setMinimum(0.0);
				m_lineIntensity_slider.setMaximum(1000.0);
				m_lineIntensity_slider.setValue(0.0);
				m_lineIntensityValue_label.setText(QString("%1").arg(m_lineIntensity_slider.value(),10));
			m_lineIntensity_HLayout.addSpacing(12);
			m_lineIntensity_HLayout.addWidget(&m_lineIntensity_label);
			m_lineIntensity_HLayout.addWidget(&m_lineIntensity_slider);
			m_lineIntensity_HLayout.addWidget(&m_lineIntensityValue_label);

			//6TH  LINE : LUT Min/Max
				m_lutMin_label.setText("LUT Min Value ");
				m_lutMin_dSpinBx.setMinimum(-10);
				m_lutMin_dSpinBx.setMaximum(10);
				m_lutMin_dSpinBx.setSingleStep(0.1);
				m_lutMin_dSpinBx.setDecimals(1);
				m_lutMin_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
				m_lutMax_label.setText("LUT Max Value ");
				m_lutMax_dSpinBx.setMinimum(-10);
				m_lutMax_dSpinBx.setMaximum(10);
				m_lutMax_dSpinBx.setValue(10);
				m_lutMax_dSpinBx.setSingleStep(0.1);
				m_lutMax_dSpinBx.setDecimals(1);
				m_lutMax_dSpinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
			m_lutMinMax_HLayout.addSpacing(12);
			m_lutMinMax_HLayout.addWidget(&m_lutMin_label);
			m_lutMinMax_HLayout.addWidget(&m_lutMin_dSpinBx);
			m_lutMinMax_HLayout.addStretch(10);
			m_lutMinMax_HLayout.addWidget(&m_lutMax_label);
			m_lutMinMax_HLayout.addWidget(&m_lutMax_dSpinBx);

		//FRAME : RENDERING

		m_renderingProperties_Vlayout.setMargin(3);
		m_renderingProperties_Vlayout.setContentsMargins(7,5,7,6);
		m_renderingProperties_Vlayout.setSpacing(2);
		m_renderingProperties_Vlayout.addLayout(&m_eventRenderingMode_HLayout);
		m_renderingProperties_Vlayout.addLayout(&m_spotSize_HLayout);
		m_renderingProperties_Vlayout.addLayout(&m_spotIntensity_HLayout);
		m_renderingProperties_Vlayout.addSpacing(2);
		m_renderingProperties_Vlayout.addLayout(&m_traceRenderingMode_HLayout);
		m_renderingProperties_Vlayout.addLayout(&m_lineWidth_HLayout);
		m_renderingProperties_Vlayout.addLayout(&m_lineIntensity_HLayout);
		m_renderingProperties_Vlayout.addSpacing(2);
		m_renderingProperties_Vlayout.addLayout(&m_renderingColor_HLayout);
		m_renderingProperties_Vlayout.addSpacing(4);
		m_renderingProperties_Vlayout.addLayout(&m_lut_HLayout);
		m_renderingProperties_Vlayout.addLayout(&m_lutMinMax_HLayout);


		m_rendering_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
		m_rendering_frame.setLineWidth(1);
		m_rendering_frame.setMidLineWidth(1);
		m_rendering_frame.setStyleSheet("QFrame {border-color : grey;}");
		m_rendering_frame.setLayout(&m_renderingProperties_Vlayout);

	 m_rendering_dropMenu.setText("Rendering");
	 m_rendering_dropMenu.setWidget(&m_rendering_frame);

	//***DROP MENU : FILTERING***

		//1ST LINE : D Nb Points Fit
			m_D_nbPointsFit_label.setText("D: Nb Points MSD Fit ");
			m_D_nbPointsFit_spinBx.setMinimum(1);
			m_D_nbPointsFit_spinBx.setMaximum(1000);
			m_D_nbPointsFit_spinBx.setValue(0);
			m_D_nbPointsFit_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_D_nbPointsFit_HLayout.addWidget(&m_D_nbPointsFit_label);
		m_D_nbPointsFit_HLayout.addWidget(&m_D_nbPointsFit_spinBx);

		//2ND LINE : D Nb Points Before Fit
			m_DInst_nbPointsBefore_label.setText("DInst: Nb Points Before Fit");
			m_DInst_nbPointsBefore_spinBx.setMinimum(1);
			m_DInst_nbPointsBefore_spinBx.setMaximum(1000);
			m_DInst_nbPointsBefore_spinBx.setValue(0);
			m_DInst_nbPointsBefore_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_DInst_nbPointsBefore_HLayout.addWidget(&m_DInst_nbPointsBefore_label);
		m_DInst_nbPointsBefore_HLayout.addWidget(&m_DInst_nbPointsBefore_spinBx);

		//3RD LINE : D Nb Points After Fit
			m_DInst_nbPointsAfter_label.setText("DInst: Nb Points After Fit");
			m_DInst_nbPointsAfter_spinBx.setMinimum(1);
			m_DInst_nbPointsAfter_spinBx.setMaximum(1000);
			m_DInst_nbPointsAfter_spinBx.setValue(0);
			m_DInst_nbPointsAfter_spinBx.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_DInst_nbPointsAfter_HLayout.addWidget(&m_DInst_nbPointsAfter_label);
		m_DInst_nbPointsAfter_HLayout.addWidget(&m_DInst_nbPointsAfter_spinBx);

	//FRAME : ADVANCED PARAMETERS

	m_advancedParameters_Vlayout.setMargin(3);
	m_advancedParameters_Vlayout.setContentsMargins(7,5,7,6);
	m_advancedParameters_Vlayout.setSpacing(2);
	m_advancedParameters_Vlayout.addLayout(&m_D_nbPointsFit_HLayout);
	m_advancedParameters_Vlayout.addLayout(&m_DInst_nbPointsBefore_HLayout);
	m_advancedParameters_Vlayout.addLayout(&m_DInst_nbPointsAfter_HLayout);

	m_advancedParameters_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_advancedParameters_frame.setLineWidth(1);
	m_advancedParameters_frame.setMidLineWidth(1);
	m_advancedParameters_frame.setStyleSheet("QFrame {border-color : grey;}");
	m_advancedParameters_frame.setLayout(&m_advancedParameters_Vlayout);

	m_advancedParameters_dropMenu.setText("Advanced Parameters");
	m_advancedParameters_dropMenu.setWidget(&m_advancedParameters_frame);

	//***DROP MENU : FILTERING***

		//1ST LINE : MIN LENGTH
			m_minLength_label.setText("Min Length ");
			m_minLength_slider.setOrientation(Qt::Horizontal);
			m_minLength_slider.setMinimum(0.0);
			m_minLength_slider.setMaximum(1000.0);
			m_minLength_slider.setValue(0.0);
			m_minLengthValue_label.setText(QString("%1").arg(m_minLength_slider.value(),10));
		m_minLength_HLayout.addWidget(&m_minLength_label);
		m_minLength_HLayout.addWidget(&m_minLength_slider);
		m_minLength_HLayout.addWidget(&m_minLengthValue_label);

		//2ND LINE : MAX LENGTH
			m_maxLength_label.setText("Max Length ");
			m_maxLength_slider.setOrientation(Qt::Horizontal);
			m_maxLength_slider.setMinimum(0.0);
			m_maxLength_slider.setMaximum(1000.0);
			m_maxLength_slider.setValue(0.0);
			m_maxLengthValue_label.setText(QString("%1").arg(m_maxLength_slider.value(),10));
		m_maxLength_HLayout.addWidget(&m_maxLength_label);
		m_maxLength_HLayout.addWidget(&m_maxLength_slider);
		m_maxLength_HLayout.addWidget(&m_maxLengthValue_label);

		//3RD LINE : MIN LOG_D
			m_minLogD_label.setText("Min Log(D) ");
			m_minLogD_slider.setOrientation(Qt::Horizontal);
			m_minLogD_slider.setMinimum(0.0);
			m_minLogD_slider.setMaximum(1000.0);
			m_minLogD_slider.setValue(0.0);
			m_minLogDValue_label.setText(QString("%1").arg(m_minLogD_slider.value(),10));
		m_minLogD_HLayout.addWidget(&m_minLogD_label);
		m_minLogD_HLayout.addWidget(&m_minLogD_slider);
		m_minLogD_HLayout.addWidget(&m_minLogDValue_label);

		//4TH  LINE : MAX LOG_D
			m_maxLogD_label.setText("Max Log(D) ");
			m_maxLogD_slider.setOrientation(Qt::Horizontal);
			m_maxLogD_slider.setMinimum(0.0);
			m_maxLogD_slider.setMaximum(1000.0);
			m_maxLogD_slider.setValue(0.0);
			m_maxLogDValue_label.setText(QString("%1").arg(m_maxLogD_slider.value(),10));
		m_maxLogD_HLayout.addWidget(&m_maxLogD_label);
		m_maxLogD_HLayout.addWidget(&m_maxLogD_slider);
		m_maxLogD_HLayout.addWidget(&m_maxLogDValue_label);

	//FRAME : FILTERING

	m_filteringProperties_Vlayout.setMargin(3);
	m_filteringProperties_Vlayout.setContentsMargins(7,5,7,6);
	m_filteringProperties_Vlayout.setSpacing(2);
	m_filteringProperties_Vlayout.addLayout(&m_minLength_HLayout);
	m_filteringProperties_Vlayout.addLayout(&m_maxLength_HLayout);
	m_filteringProperties_Vlayout.addLayout(&m_minLogD_HLayout);
	m_filteringProperties_Vlayout.addLayout(&m_maxLogD_HLayout);

	m_filtering_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_filtering_frame.setLineWidth(1);
	m_filtering_frame.setMidLineWidth(1);
	m_filtering_frame.setStyleSheet("QFrame {border-color : grey;}");
	m_filtering_frame.setLayout(&m_filteringProperties_Vlayout);

 m_filtering_dropMenu.setText("Filtering");
 m_filtering_dropMenu.setWidget(&m_filtering_frame);

	//***DROP MENU : EXPORT***

		//1ST LINE : EXPORT TYPE
			m_exportedType_label.setText("Export Type:");
				QStringList  list = {"MSD Per Tracks",
									 "Diffusion Per Track",
									 "Instant Diffusion Per Track",
									 "Diffusion Histogram",
									 "Instant Diffusion Histogram",
									 "Length Histogram",
									 "Screenshot",
									 "Trajectories"};
			m_exportedType_comboBox.addItems(list);
		m_exportedType_HLayout.addWidget(&m_exportedType_label);
		m_exportedType_HLayout.addWidget(&m_exportedType_comboBox);

		//2ND LINE : MIN D
			m_MSDMaxDPlane_label.setText("Nb MSD Points ");
			m_MSDMaxDPlane_spinBox.setMinimum(0);
			m_MSDMaxDPlane_spinBox.setSingleStep(1);
			m_MSDMaxDPlane_spinBox.setMaximum(1000);
			m_MSDMaxDPlane_spinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_MSDMaxDPlane_HLayout.addWidget(&m_MSDMaxDPlane_label);
		m_MSDMaxDPlane_HLayout.addWidget(&m_MSDMaxDPlane_spinBox);

		//2ND LINE : MIN D
			m_DsHistogram_min_label.setText("Min Diffusion ");
			m_DsHistogram_min_dSpinBox.setMinimum(-1000.0);
			m_DsHistogram_min_dSpinBox.setMaximum(1000.0);
			m_DsHistogram_min_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_DsHistogram_min_HLayout.addWidget(&m_DsHistogram_min_label);
		m_DsHistogram_min_HLayout.addWidget(&m_DsHistogram_min_dSpinBox);

		//3rd LINE : MAX D
			m_DsHistogram_max_label.setText("Max Diffusion ");
			m_DsHistogram_max_dSpinBox.setMinimum(-1000.0);
			m_DsHistogram_max_dSpinBox.setMaximum(1000.0);
			m_DsHistogram_max_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_DsHistogram_max_HLayout.addWidget(&m_DsHistogram_max_label);
		m_DsHistogram_max_HLayout.addWidget(&m_DsHistogram_max_dSpinBox);


		//4th LINE : N INTERVAL
			m_DsHistogram_nInterval_label.setText("Nb Interval ");
			m_DsHistogram_nInterval_dSpinBox.setMinimum(0.0);
			m_DsHistogram_nInterval_dSpinBox.setSingleStep(1.0);
			m_DsHistogram_nInterval_dSpinBox.setDecimals(0);
			m_DsHistogram_nInterval_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_DsHistogram_nInterval_HLayout.addWidget(&m_DsHistogram_nInterval_label);
		m_DsHistogram_nInterval_HLayout.addWidget(&m_DsHistogram_nInterval_dSpinBox);

		//2ND LINE : MIN LENGTH
			m_lengthsHistogram_min_label.setText("Min Length ");
			m_lengthsHistogram_min_dSpinBox.setMinimum(0.0);
			m_lengthsHistogram_min_dSpinBox.setMaximum(1000.0);
			m_lengthsHistogram_min_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_lengthsHistogram_min_HLayout.addWidget(&m_lengthsHistogram_min_label);
		m_lengthsHistogram_min_HLayout.addWidget(&m_lengthsHistogram_min_dSpinBox);

		//3rd LINE : MAX LENGTH
			m_lengthsHistogram_max_label.setText("Max Length ");
			m_lengthsHistogram_max_dSpinBox.setMinimum(0.0);
			m_lengthsHistogram_max_dSpinBox.setMaximum(1000.0);
			m_lengthsHistogram_max_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_lengthsHistogram_max_HLayout.addWidget(&m_lengthsHistogram_max_label);
		m_lengthsHistogram_max_HLayout.addWidget(&m_lengthsHistogram_max_dSpinBox);


		//4th LINE : N INTERVAL
			m_lengthsHistogram_nInterval_label.setText("Nb Interval ");
			m_lengthsHistogram_nInterval_dSpinBox.setMinimum(0.0);
			m_lengthsHistogram_nInterval_dSpinBox.setMaximum(1000.0);
			m_lengthsHistogram_nInterval_dSpinBox.setDecimals(0);
			m_lengthsHistogram_nInterval_dSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
		m_lengthsHistogram_nInterval_HLayout.addWidget(&m_lengthsHistogram_nInterval_label);
		m_lengthsHistogram_nInterval_HLayout.addWidget(&m_lengthsHistogram_nInterval_dSpinBox);

		m_trajectoryFormat_label.setText("Trajectory Format");
		QStringList items({"PALMTRACER_FORMAT",
						   "TRXYT[px]_FORMAT",
						   "TRXYI[px]_FORMAT",
						   "SVG_FORMAT",
						   "TRXYT[um]_FORMAT",
						   "TRXYI[um]_FORMAT",});
		m_trajectoryFormat_comboBox.addItems(items);
		m_trajectoryFormat_HLayout.addWidget(&m_trajectoryFormat_label);
		m_trajectoryFormat_HLayout.addWidget(&m_trajectoryFormat_comboBox);

		m_export_pushButton.setText("Export...");

	//FRAME : EXPORT

	m_exportProperties_Vlayout.setMargin(3);
	m_exportProperties_Vlayout.setContentsMargins(7,5,7,6);
	m_exportProperties_Vlayout.setSpacing(2);
	m_exportProperties_Vlayout.addLayout(&m_exportedType_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_MSDMaxDPlane_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_DsHistogram_min_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_DsHistogram_max_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_DsHistogram_nInterval_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_lengthsHistogram_min_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_lengthsHistogram_max_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_lengthsHistogram_nInterval_HLayout);
	m_exportProperties_Vlayout.addLayout(&m_trajectoryFormat_HLayout);
	m_exportProperties_Vlayout.addWidget(&m_export_pushButton);

	m_exportProperties_frame.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_exportProperties_frame.setLineWidth(1);
	m_exportProperties_frame.setMidLineWidth(1);
	m_exportProperties_frame.setStyleSheet("QFrame {border-color : grey;}");
	m_exportProperties_frame.setLayout(&m_exportProperties_Vlayout);

	m_export_dropMenu.setText("Export ");
	m_export_dropMenu.setWidget(&m_exportProperties_frame);

//PARAMETER WIDGET ASSEMBLY

	m_parameters_vLayout.addWidget(&m_fileLoader_dropMenu);
	m_parameters_vLayout.addWidget(&m_movieLoader_dropMenu);
	m_parameters_vLayout.addWidget(&m_rendering_dropMenu);
	m_parameters_vLayout.addWidget(&m_filtering_dropMenu);
	m_parameters_vLayout.addWidget(&m_advancedParameters_dropMenu);
	m_parameters_vLayout.addWidget(&m_export_dropMenu);
	m_parameters_vLayout.setMargin(0);
	m_parameters_vLayout.setSpacing(0);

	m_parameters_wdgt.setLayout(&m_parameters_vLayout);

	this->setStyleSheet("QPushButton {height : 14px; font-size : 11px; background-color: rgb(200,200,200)}"
						"QPushButton:hover {background-color: rgb(250,250,250)}");
}

void TracePlayerView::setStyleSheets()
{
	//frames

	m_fileLoader_frame.setObjectName("frame1");
	m_fileLoader_frame.setStyleSheet("#frame1 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	m_movieLoader_frame.setObjectName("frame2");
	m_movieLoader_frame.setStyleSheet("#frame2 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	m_rendering_frame.setObjectName("frame3");
	m_rendering_frame.setStyleSheet("#frame3 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	m_filtering_frame.setObjectName("frame4");
	m_filtering_frame.setStyleSheet("#frame4 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	m_advancedParameters_frame.setObjectName("frame5");
	m_advancedParameters_frame.setStyleSheet("#frame5 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");

	m_exportProperties_frame.setObjectName("frame6");
	m_exportProperties_frame.setStyleSheet("#frame6 {border-style:solid; border-width:2px; border-color:rgb(120,120,120);}");


	//buttons

    m_fileLoader_clearFile_pushButton.setStyleSheet("QPushButton {image : url(Resources/Icons/deleteButton.png); border-style : none;}"
                                                    "QPushButton:hover {image : url(Resources/Icons/deleteButton_hover.png);}"
                                                    "QPushButton:pressed {image : url(Resources/Icons/deleteButton_pressed.png);}");

    m_movieLoader_clearMovie_pushButton.setStyleSheet("QPushButton {image : url(Resources/Icons/deleteButton.png); border-style : none;}"
                                                    "QPushButton:hover {image : url(Resources/Icons/deleteButton_hover.png);}"
                                                    "QPushButton:pressed {image : url(Resources/Icons/deleteButton_pressed.png);}");

}



void TracePlayerView::showMSDExportParameters(bool isShown)
{
	m_MSDMaxDPlane_label.setVisible(isShown);
	m_MSDMaxDPlane_spinBox.setVisible(isShown);
}

void TracePlayerView::showDHistoExportParameters(bool isShown)
{
	m_DsHistogram_min_label.setVisible(isShown);
	m_DsHistogram_min_dSpinBox.setVisible(isShown);

	m_DsHistogram_max_label.setVisible(isShown);
	m_DsHistogram_max_dSpinBox.setVisible(isShown);

	m_DsHistogram_nInterval_label.setVisible(isShown);
	m_DsHistogram_nInterval_dSpinBox.setVisible(isShown);
}

void TracePlayerView::showLengthHistoExportParameters(bool isShown)
{
	m_lengthsHistogram_min_label.setVisible(isShown);
	m_lengthsHistogram_min_dSpinBox.setVisible(isShown);

	m_lengthsHistogram_max_label.setVisible(isShown);
	m_lengthsHistogram_max_dSpinBox.setVisible(isShown);

	m_lengthsHistogram_nInterval_label.setVisible(isShown);
	m_lengthsHistogram_nInterval_dSpinBox.setVisible(isShown);
}

void TracePlayerView::showTrajectoryExportParameters(bool isShown)
{
	m_trajectoryFormat_comboBox.setVisible(isShown);
	m_trajectoryFormat_label.setVisible(isShown);
}


myGLWidget* TracePlayerView::getGLWidget()
{
//	return &m_glWidget;
	return 0;
}


void TracePlayerView::dropEvent(QDropEvent * event)
{
	treatEvent(event);
}

void TracePlayerView::dragEnterEvent(QDragEnterEvent * event)
{
	treatEvent(event);
}


void TracePlayerView::mouseMoveEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);

}

void TracePlayerView::mousePressEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);

}

void TracePlayerView::mouseReleaseEvent(QMouseEvent * event)
{
	treatEvent((QEvent*) event);

}


void TracePlayerView::wheelEvent(QWheelEvent * event)
{
	treatEvent((QEvent*) event);
}


void TracePlayerView::treatEvent(QEvent* event){}
void TracePlayerView::planeSliderIdxChanged(int new_planeIdx){}

void TracePlayerView::onFileAction(QString sender)
{
	string sender_str = sender.toLocal8Bit().data();

	if(sender_str == "openFile_action")
	{
		onOpenFileAction();
		return;
	}

	if(sender_str == "clearFile_action")
	{
		onClearFileAction();
		return;
	}

	if(sender_str == "openMovie_action")
	{
		onOpenMovieAction();
		return;
	}

	if(sender_str == "clearMovie_action")
	{
		onClearMovieAction();
		return;
	}


}


void TracePlayerView::timestepGetChanged(double){}
void TracePlayerView::pixelSizeGetChanged(double){}
void TracePlayerView::spotSizeGetChanged(double value){}
void TracePlayerView::lineWidthGetChanged(int value){}
void TracePlayerView::spotIntensityGetChanged(double value){}

void TracePlayerView::spotSizeGetChanged(int value){}
void TracePlayerView::spotIntensityGetChanged(int value){}

void TracePlayerView::lineIntensityGetChanged(int value){}
void TracePlayerView::lutMinValueGetChanged(double){}
void TracePlayerView::lutMaxValueGetChanged(double){}
void TracePlayerView::lutGetChanged(int){}

void TracePlayerView::DsHistogramMinValueGetChanged(double value){}
void TracePlayerView::DsHistogramMaxValueGetChanged(double value){}
void TracePlayerView::DsHistogramNbIntervalsGetChanged(double value){}
void TracePlayerView::lengthsHistogramMinValueGetChanged(double value){}
void TracePlayerView::lengthsHistogramMaxValueGetChanged(double value){}
void TracePlayerView::lengthsHistogramNbIntervalsGetChanged(double value){}
void TracePlayerView::filteringMinLengthValueGetChanged(int value){}
void TracePlayerView::filteringMaxLengthValueGetChanged(int value){}
void TracePlayerView::filteringMinLogDValueGetChanged(int value){}
void TracePlayerView::filteringMaxLogDValueGetChanged(int value){}
void TracePlayerView::MSDMaxDPlaneGetChanged(int value){}
void TracePlayerView::DFirstMSDPointIndexGetChanged(int value){}
void TracePlayerView::DlastMSDPointIndexGetChanged(int value){}
void TracePlayerView::exportTypeGetChanged(int value) {}
void TracePlayerView::onExportButtonGetClicked(){}
void TracePlayerView::DNbPointsMSDFitValueGetChanged(int value){}
void TracePlayerView::DInstNbPointsBeforeMSDFitValueGetChanged(int value){}
void TracePlayerView::DInstNbPointsAfterMSDFitValueGetChanged(int value){}
void TracePlayerView::trajectoryExportedFormatGetChanged(int value){}

void TracePlayerView::onTrajectoriesRenderingModeGetChanged(int idx){}
void TracePlayerView::onEventsRenderingModeGetChanged(int idx){}
void TracePlayerView::onColorModeGetChanged(int idx){}
void TracePlayerView::onOpenFileAction(){}
void TracePlayerView::onClearFileAction(){}
void TracePlayerView::onOpenMovieAction(){}
void TracePlayerView::onClearMovieAction(){}
void TracePlayerView::onSaveFileAction(){}
void TracePlayerView::onMSDExportAction(){}
void TracePlayerView::onDsHistogramExportAction(){}
void TracePlayerView::onLengthsHistogramExportAction(){}
void TracePlayerView::onMultipageTiffExportAction(){}
void TracePlayerView::onBatchModeSelectionAction(){}
void TracePlayerView::onAddPathsAction(){}
void TracePlayerView::onClearPathsAction(){}



