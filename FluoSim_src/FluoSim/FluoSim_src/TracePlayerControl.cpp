
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

using namespace glm;
using namespace std;

TracePlayerControl::TracePlayerControl(myGLWidget* glWidget, QWidget* parent) : TracePlayerModel(glWidget, parent)
{
	TracePlayerModel::m_current_plane = 0;

	m_slider.setMinimum(0);
	m_slider.setMaximum(0);

    //model initialisation
    TracePlayerView::m_pixelSize_dSpinBx.setMinimum(0.001);
    TracePlayerView::m_pixelSize_dSpinBx.setMaximum(1.000f);
    TracePlayerView::m_pixelSize_dSpinBx.setValue(0.160);
	TracePlayerModel::m_pixel_size = 0.160;
    TracePlayerView::m_timeStep_dSpinBx.setMinimum(0);
    TracePlayerView::m_timeStep_dSpinBx.setMaximum(1);
    TracePlayerView::m_timeStep_dSpinBx.setValue(0.020f);
	TracePlayerModel::m_timestep = 0.020;

	TracePlayerModel::m_trajectories_renderingMode = ALL_TRAJECTORIES_RENDERING_MODE;
    TracePlayerView::m_traceRenderingMode_comboBx.setCurrentIndex((int) CURRENT_TRAJECTORIES_RENDERING_MODE);

	TracePlayerModel::m_events_renderingMode = CURRENT_EVENTS_RENDERING_MODE;
    TracePlayerView::m_eventRenderingMode_comboBx.setCurrentIndex((int) CURRENT_EVENTS_RENDERING_MODE);

	TracePlayerModel::m_colorMode = RANDOM_COLOR_MODE;
    TracePlayerView::m_renderingColor_comboBx.setCurrentIndex((int) RANDOM_COLOR_MODE);

    TracePlayerView::m_spotSize_slider.setMinimum(0);
    TracePlayerView::m_spotSize_slider.setMaximum(100);
    TracePlayerView::m_spotSizeSlider_minValue = 0;
    TracePlayerView::m_spotSizeSlider_maxValue = 10;
    TracePlayerView::m_spotSize_slider.setValue(10); //will automaically update the model...

    TracePlayerView::m_spotIntensity_slider.setMinimum(0);
    TracePlayerView::m_spotIntensity_slider.setMaximum(100);
    TracePlayerView::m_spotIntensitySlider_minValue = 0;
    TracePlayerView::m_spotIntensitySlider_maxValue = 4;
    TracePlayerView::m_spotIntensity_slider.setValue(50); //will automaically update the model...

    TracePlayerView::m_lineWidth_slider.setMinimum(0);
    TracePlayerView::m_lineWidth_slider.setMaximum(5);
    TracePlayerView::m_lineWidthSlider_minValue = 0;
    TracePlayerView::m_lineWidthSlider_maxValue = 5;
    TracePlayerView::m_lineWidth_slider.setValue(1); //will automaically update the model...

    TracePlayerView::m_lineIntensity_slider.setMinimum(0);
    TracePlayerView::m_lineIntensity_slider.setMaximum(100);
    TracePlayerView::m_lineIntensitySlider_minValue = 0;
    TracePlayerView::m_lineIntensitySlider_maxValue = 1;
    TracePlayerView::m_lineIntensity_slider.setValue(100); //will automaically update the model...

    TracePlayerView::m_lutMin_dSpinBx.setValue(-5);
	TracePlayerModel::m_lut_minValue = -5;
    TracePlayerView::m_lutMax_dSpinBx.setValue(1);
	TracePlayerModel::m_lut_maxValue = 1;
    TracePlayerModel::m_lut_paths.insert({	{HSV_LUT, "Resources/Luts/hsv.lut"},
                                            {JET_LUT, "Resources/Luts/jet.lut"},
                                            {SPRING_LUT, "Resources/Luts/spring.lut"},
                                            {WINTER_LUT, "Resources/Luts/winter.lut"}});
	m_lutName = JET_LUT;
    TracePlayerView::m_lut_comboBx.setCurrentIndex(JET_LUT);
	TracePlayerModel::setLUT(m_lut_paths[JET_LUT]);

	TracePlayerModel::m_min_length = 0;
    TracePlayerView::m_minLength_slider.setValue(0);
    TracePlayerView::m_minLength_slider.setMinimum(0);
    TracePlayerView::m_minLength_slider.setMaximum(1000);

	TracePlayerModel::m_max_length = 1000;
    TracePlayerView::m_maxLength_slider.setMinimum(0);
    TracePlayerView::m_maxLength_slider.setMaximum(1000);

    TracePlayerView::m_logDSlider_minValue = -10;
    TracePlayerView::m_logDSlider_maxValue = +10;
    TracePlayerView::m_minLogD_slider.setMinimum(0);
    TracePlayerView::m_minLogD_slider.setMaximum(50);
    TracePlayerView::m_minLogD_slider.setValue(0);
	TracePlayerModel::m_minLogD = -10;

    TracePlayerView::m_maxLogD_slider.setMinimum(0);
    TracePlayerView::m_maxLogD_slider.setMaximum(50);
    TracePlayerView::m_maxLogD_slider.setValue(50);
	TracePlayerModel::m_maxLogD = 10;

	TracePlayerModel::m_MSD_maxDPplane = 10;
    TracePlayerView::m_MSDMaxDPlane_spinBox.setValue(10);
    TracePlayerView::m_MSDMaxDPlane_spinBox.setMinimum(10);

	TracePlayerModel::m_D_firstMSDPoint_idx = 1;
	TracePlayerModel::m_D_nbPointsMSDFit = 4;
    TracePlayerView::m_D_nbPointsFit_spinBx.setValue(4);

	TracePlayerModel::m_D_lastMSDPoint_idx = 4;

	TracePlayerModel::m_DInst_nbPointsBeforeMSDFit = 10;
    TracePlayerView::m_DInst_nbPointsBefore_spinBx.setValue(10);
	TracePlayerModel::m_DInst_nbPointsAfterMSDFit = 10;
    TracePlayerView::m_DInst_nbPointsAfter_spinBx.setValue(10);

    TracePlayerView::m_exportedType_comboBox.setCurrentIndex(1);
    TracePlayerView::m_exportedType_comboBox.setCurrentIndex(0); //to send an event...

    TracePlayerView::m_trajectoryFormat_comboBox.setCurrentIndex(0);
	TracePlayerModel::m_exportedTrajectory_fileFormat = PALMTRACER_FORMAT;

	TracePlayerModel::m_DsHistogram_nbIntervals = 35;
    TracePlayerView::m_DsHistogram_nInterval_dSpinBox.setValue(35);
	TracePlayerModel::m_DsHistogram_minValue = -6;
    TracePlayerView::m_DsHistogram_min_dSpinBox.setValue(-6);
	TracePlayerModel::m_DsHistogram_maxValue = 1;
    TracePlayerView::m_DsHistogram_max_dSpinBox.setValue(1);

	TracePlayerModel::m_lengthsHistogram_nbIntervals = 35;
    TracePlayerView::m_lengthsHistogram_nInterval_dSpinBox.setValue(35);
	TracePlayerModel::m_lengthsHistogram_minValue = 0;
    TracePlayerView::m_lengthsHistogram_min_dSpinBox.setValue(0);
	TracePlayerModel::m_lengthsHistogram_maxValue = 200;
    TracePlayerView::m_lengthsHistogram_max_dSpinBox.setValue(200);

	TracePlayerModel::m_isBatchModeSelected = false;
}



void TracePlayerControl::treatEvent(QEvent * event)
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

		if(path_str.endsWith(".trc") || path_str.endsWith(".btrc") || path_str.endsWith(".lut"))
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


			myChrono clock2;

			if(path_str.endsWith(".trc"))
			{
				path_str.remove(0,1);

				float Dt_meas;
				clock2.startChrono();
				setTracesFromStringFile(path_str.toLocal8Bit().data());
				Dt_meas = clock2.endTour();
			}

		if(path_str.endsWith(".btrc"))
		{
			path_str.remove(0,1);
			float Dt_meas;
			clock2.startChrono();
			setTracesFromBinaryFile(path_str.toLocal8Bit().data());
			Dt_meas = clock2.endTour();
		}

		if(path_str.endsWith(".lut"))
		{
			path_str.remove(0,1);
			m_lut.loadLUT(path_str.toLocal8Bit().data());
		}

		return;
	}
}

void TracePlayerControl::render()
{
	TracePlayerModel::render(getGLWidget());
}

void TracePlayerControl::render(myGLWidget* glWdgt)
{
	TracePlayerModel::render(glWdgt);
}

void TracePlayerControl::planeSliderIdxChanged(int new_sliderIdx)
{
	//slider idx from 0 to maxplane -1
	//trace from 1 to max_plane

	TracePlayerModel::setCurrentPlane(new_sliderIdx+1);
	TracePlayerModel::updateRenderingPipeline();

	if(m_tiffMovie.getTiffParameters().numberDirectory == 1)
	{
		TracePlayerModel::m_tiffMovie.loadTiffPage(0);
	}
	else TracePlayerModel::m_tiffMovie.loadTiffPage(new_sliderIdx);

}

void TracePlayerControl::timestepGetChanged(double timestep)
{
	//model
	TracePlayerModel::setTimeStep(timestep);

	//view
}

void TracePlayerControl::pixelSizeGetChanged(double pixel_size)
{
	//model
	TracePlayerModel::setPixelSize(pixel_size);

	//view
}

void TracePlayerControl::nbTracesChanged(int new_nb_traces)
{

}

void TracePlayerControl::planeRangedChanged()
{
	int tiff_nbDir = m_tiffMovie.getTiffParameters().numberDirectory;
	int trc_minPlane = getMinPlane(m_traces);
	int trc_maxPlane = getMaxPlane(m_traces);

	int slider_minValue;
	int slider_maxValue;

	slider_minValue = 0;
	slider_maxValue = std::max(0, trc_maxPlane-1);
	slider_maxValue = std::max(0, std::max(tiff_nbDir-1, trc_maxPlane-1));


	m_min_plane = slider_minValue+1;
	m_max_plane = slider_maxValue+1;
    TracePlayerView::m_slider.setMinimum(slider_minValue);
    TracePlayerView::m_slider.setMaximum(slider_maxValue);

}

void TracePlayerControl::minPlaneChanged(int new_min_plane)
{
	TracePlayerModel::m_min_plane = getMinPlane(m_traces);
//	TracePlayerView::m_slider.setMinimum(new_min_plane);
//	TracePlayerModel::m_min_plane = new_min_plane;
}

void TracePlayerControl::maxPlaneChanged(int new_max_plane)
{
	TracePlayerModel::m_max_plane = new_max_plane;
}

void TracePlayerControl::maxLengthChanged(int new_maxLength)
{
	m_lengthSlider_maxValue = new_maxLength;
	m_maxLength_slider.setMaximum(new_maxLength);
	m_minLength_slider.setMaximum(new_maxLength);

}
void TracePlayerControl::minLengthChanged(int new_minLength)
{
	m_lengthSlider_minValue = new_minLength;
	m_minLength_slider.setMinimum(new_minLength);
	m_maxLength_slider.setMinimum(new_minLength);
}

void TracePlayerControl::spotSizeGetChanged(double value)
{
	TracePlayerModel::m_square_size = vec2(value, value);
}



void TracePlayerControl::spotIntensityGetChanged(double value)
{
	m_spotIntensity = value;
}

void TracePlayerControl::spotSizeGetChanged(int pos)
{
	float ratio = (m_spotSizeSlider_maxValue - m_spotSizeSlider_minValue)/(m_spotSize_slider.maximum() - m_spotSize_slider.minimum());
	float value = pos * ratio + m_spotSizeSlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_spotSizeValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_square_size = vec2(value, value);

	delete[] data_cstr;
}
void TracePlayerControl::spotIntensityGetChanged(int pos)
{
	float ratio = (m_spotIntensitySlider_maxValue - m_spotIntensitySlider_minValue)/(m_spotIntensity_slider.maximum() - m_spotIntensity_slider.minimum());
	float value = pos* ratio + m_spotIntensitySlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_spotIntensityValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_spotIntensity = value;

	delete[] data_cstr;
}

void TracePlayerControl::lineWidthGetChanged(int pos)
{
	float ratio = (m_lineWidthSlider_maxValue - m_lineWidthSlider_minValue)/(m_lineWidth_slider.maximum() - m_lineWidth_slider.minimum());
	float value = pos * ratio + m_lineWidthSlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_lineWidthValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_lineWidth = value;

	delete[] data_cstr;
}
void TracePlayerControl::lineIntensityGetChanged(int pos)
{
	float ratio = (m_lineIntensitySlider_maxValue - m_lineIntensitySlider_minValue)/(m_lineIntensity_slider.maximum() - m_lineIntensity_slider.minimum());
	float value = pos* ratio + m_lineIntensitySlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_lineIntensityValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_lineIntensity = value;

	delete[] data_cstr;
}

void TracePlayerControl::lutMinValueGetChanged(double new_value)
{
	//model
	TracePlayerModel::m_lut_minValue = new_value;
	updateRenderingPipeline();

}
void TracePlayerControl::lutMaxValueGetChanged(double new_value)
{
	//model
	TracePlayerModel::m_lut_maxValue = new_value;
	updateRenderingPipeline();
}

void TracePlayerControl::lutGetChanged(int new_lut)
{
	LUTName new_LUTName = (LUTName) new_lut;
	if(new_LUTName == m_lutName) return; //->

	TracePlayerModel::m_lutName = new_LUTName;
	TracePlayerModel::setLUT(m_lut_paths[new_LUTName]);
}


void TracePlayerControl::DsHistogramMinValueGetChanged(double value)
{
	m_DsHistogram_minValue = value;
}
void TracePlayerControl::DsHistogramMaxValueGetChanged(double value)
{
	m_DsHistogram_maxValue = value;
}
void TracePlayerControl::DsHistogramNbIntervalsGetChanged(double value)
{
	m_DsHistogram_nbIntervals = value;
}
void TracePlayerControl::lengthsHistogramMinValueGetChanged(double value)
{
	m_lengthsHistogram_minValue = value;
}
void TracePlayerControl::lengthsHistogramMaxValueGetChanged(double value)
{
	m_lengthsHistogram_maxValue = value;
}
void TracePlayerControl::lengthsHistogramNbIntervalsGetChanged(double value)
{
	m_lengthsHistogram_nbIntervals = value;
}

void TracePlayerControl::filteringMinLengthValueGetChanged(int value)
{
    TracePlayerView::m_minLengthValue_label.setText(QString("%1").arg(value));

	TracePlayerModel::m_min_length = value;
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::filteringMaxLengthValueGetChanged(int value)
{
    TracePlayerView::m_maxLengthValue_label.setText(QString("%1").arg(value));

	TracePlayerModel::m_max_length = value;
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();

}

void TracePlayerControl::filteringMinLogDValueGetChanged(int pos)
{	
	float ratio = (m_logDSlider_maxValue - m_logDSlider_minValue)/(m_minLogD_slider.maximum() - m_minLogD_slider.minimum());
	float value = pos* ratio + m_logDSlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_minLogDValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_minLogD = value;
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();

	delete[] data_cstr;

}

void TracePlayerControl::filteringMaxLogDValueGetChanged(int pos)
{
	float ratio = (m_logDSlider_maxValue - m_logDSlider_minValue)/(m_maxLogD_slider.maximum() - m_maxLogD_slider.minimum());
	float value = pos* ratio + m_logDSlider_minValue;
		char* data_cstr = new char[5+1+1];
		sprintf(data_cstr, "%5.3f\0", value);

	//view
    TracePlayerView::m_maxLogDValue_label.setText(QString(data_cstr));

	//model
	TracePlayerModel::m_maxLogD = value;
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();

	delete[] data_cstr;

}


void TracePlayerControl::MSDMaxDPlaneGetChanged(int value)
{
	TracePlayerModel::m_MSD_maxDPplane = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::DFirstMSDPointIndexGetChanged(int value)
{
	TracePlayerModel::m_D_firstMSDPoint_idx = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::DlastMSDPointIndexGetChanged(int value)
{

	TracePlayerModel::m_D_lastMSDPoint_idx = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::DNbPointsMSDFitValueGetChanged(int value)
{
	m_D_nbPointsMSDFit = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();

}

void TracePlayerControl::DInstNbPointsBeforeMSDFitValueGetChanged(int value)
{
	m_DInst_nbPointsBeforeMSDFit = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::DInstNbPointsAfterMSDFitValueGetChanged(int value)
{
	m_DInst_nbPointsAfterMSDFit = value;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerControl::trajectoryExportedFormatGetChanged(int value)
{
	m_exportedTrajectory_fileFormat = (FLUOEVENT_FILE_FORMAT) value;
}

void TracePlayerControl::exportTypeGetChanged(int value)
{
	EXPORT_TYPE export_type = (EXPORT_TYPE) value;

	switch(export_type)
	{
		case MSD_PER_TRACE_EXPORT :
		{
			//view part
			showMSDExportParameters(true);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(false);
		}
		break;

		case D_PER_TRACE_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(false);
		}
		break;

		case DINST_PER_TRACE_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(false);
		}
		break;

		case D_HISTOGRAM_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(true);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(false);
		}
		break;

		case DINST_HISTOGRAM_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(true);
			showLengthHistoExportParameters(false);
		}
		break;

		case LENGTH_HISTOGRAM_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(true);
			showTrajectoryExportParameters(false);
		}
		break;

		case SCREENSHOT_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(false);
		}
		break;

		case TRAJECTORIES_EXPORT :
		{
			//view part
			showMSDExportParameters(false);
			showDHistoExportParameters(false);
			showLengthHistoExportParameters(false);
			showTrajectoryExportParameters(true);
		}
		break;
	}
}

void TracePlayerControl::onExportButtonGetClicked()
{
	EXPORT_TYPE export_type = (EXPORT_TYPE) m_exportedType_comboBox.currentIndex();
	string file_path;
	bool isFile_selected;
	vector<pair<string, string>> typeGroupName_filteredTypes_v;

	switch(export_type)
	{
		case MSD_PER_TRACE_EXPORT :
		{
			typeGroupName_filteredTypes_v.push_back({"Mean Square Displacements", "txt"});
			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v,"MSDPerTrack.txt",
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				saveTracesMSDsAsString(m_filteredTraces, file_path);
			}

		}
		break;

		case D_PER_TRACE_EXPORT :
		{
			typeGroupName_filteredTypes_v.push_back({"Diffusion Coefficients", "txt"});
			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v,"DPerTrack.txt",
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				saveTracesDsAsString(m_filteredTraces, file_path);
			}
		}
		break;

		case DINST_PER_TRACE_EXPORT :
		{
			typeGroupName_filteredTypes_v.push_back({"Instant Diffusion Coefficients", "txt"});
			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v,"instantDPerTrack.txt",
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				saveTracesDInstsAsString(m_filteredTraces, file_path);
			}
		}
		break;

		case D_HISTOGRAM_EXPORT :
		{
			string D_path;
			switch(m_isBatchModeSelected)
			{
				case false :
				{
					D_path = "DHist_n"
						   + to_string(m_DsHistogram_nbIntervals)
						   + "_mn"
						   + to_string(int(m_DsHistogram_minValue))
						   + "_mx"
						   + to_string(int(m_DsHistogram_maxValue))
						   + ".txt";
					typeGroupName_filteredTypes_v.push_back({"Diffusion Histogram", "txt"});
					isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v, D_path,
															  "SimulationOutputs", this);
					if(isFile_selected == true)
					{
						saveTracesLogDsHistogramAsString(m_filteredTraces, file_path,
														 m_DsHistogram_nbIntervals,
														 m_DsHistogram_minValue,
														 m_DsHistogram_maxValue);
					}
				}
				break;


				case true :
				{
					int trace_path_idx = 0;
					QString path_qstr;

					auto trace_path = m_tracePaths_v.begin();
					while(trace_path != m_tracePaths_v.end())
					{

						path_qstr = QString(trace_path->data());
						if(path_qstr.endsWith(".trc"))
						{
							cout<<"****"<<path_qstr.toLocal8Bit().data()<<"\n";
							setTracesFromStringFile(path_qstr.toLocal8Bit().data());
						}

						if(path_qstr.endsWith(".btrc"))
						{
							setTracesFromBinaryFile(path_qstr.toLocal8Bit().data());
						}

						render();
						getGLWidget()->swapBuffers();
						D_path = *trace_path
							   + "_D.txt";

						trace_path_idx++;
						trace_path++;

						saveTracesLogDsHistogramAsString(m_filteredTraces, D_path, m_DsHistogram_nbIntervals,
																		 m_DsHistogram_minValue,
																		 m_DsHistogram_maxValue);
					}
				}
				break;
			}
		}
		break;

		case DINST_HISTOGRAM_EXPORT :
		{
			string D_path;
			switch(/*m_isBatchModeSelected*/false)
			{
				case false :
				{
					D_path = "DInstHist_n"
						   + to_string(m_DsHistogram_nbIntervals)
						   + "_mn"
						   + to_string(int(m_DsHistogram_minValue))
						   + "_mx"
						   + to_string(int(m_DsHistogram_maxValue))
						   + ".txt";

					typeGroupName_filteredTypes_v.push_back({"Instant Diffusion Histogram", "txt"});
					isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v, D_path,
															  "SimulationOutputs", this);
					if(isFile_selected == true)
					{
						saveTracesLogDInstsHistogramAsString(m_filteredTraces, file_path, m_DsHistogram_nbIntervals,
															 m_DsHistogram_minValue,
															 m_DsHistogram_maxValue);
					}
				}
				break;


//				case true :
//				{
//					int trace_path_idx = 0;
//					QString path_qstr;

//					auto trace_path = m_tracePaths_v.begin();
//					while(trace_path != m_tracePaths_v.end())
//					{

//						path_qstr = QString(trace_path->data());
//						if(path_qstr.endsWith(".trc"))
//						{
//							cout<<"****"<<path_qstr.toLocal8Bit().data()<<"\n";
//							setTracesFromStringFile(path_qstr.toLocal8Bit().data());
//						}

//						if(path_qstr.endsWith(".btrc"))
//						{
//							setTracesFromBinaryFile(path_qstr.toLocal8Bit().data());
//						}

//						render();
//						getGLWidget()->swapBuffers();
//						D_path = *trace_path
//							   + "_D.txt";

//						trace_path_idx++;
//						trace_path++;

//						saveTracesLogDsHistogramAsString(m_filteredTraces, D_path, m_DsHistogram_nbIntervals,
//																		 m_DsHistogram_minValue,
//																		 m_DsHistogram_maxValue);
//					}
//				}
//				break;
			}
		}
		break;

		case LENGTH_HISTOGRAM_EXPORT :
		{
			string file = "lgthHist_n"
				   + to_string(m_lengthsHistogram_nbIntervals)
				   + "_mn"
				   + to_string(int(m_lengthsHistogram_minValue))
				   + "_mx"
				   + to_string(int(m_lengthsHistogram_maxValue))
				   + ".txt";

			typeGroupName_filteredTypes_v.push_back({"Length Histogram", "txt"});
			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v, file,
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				saveTracesLengthsHistogramAsString(m_filteredTraces, file_path, m_lengthsHistogram_nbIntervals,
												   m_lengthsHistogram_minValue,
												   m_lengthsHistogram_maxValue);
			}


		}
		break;

		case SCREENSHOT_EXPORT :
		{
			typeGroupName_filteredTypes_v.push_back({"BMP Image", "bmp"});
			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v, "screenshot.bmp",
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				captureScreen(m_glWidget, file_path);
			}
		}
		break;

		case TRAJECTORIES_EXPORT :
		{
			std::map<string,FLUOEVENT_FILE_FORMAT> fileExtensionsFormats_mp;
			fileExtensionsFormats_mp.insert({".trc", PALMTRACER_FORMAT});
			fileExtensionsFormats_mp.insert({".trxyt_px", TRXYT_FORMAT});
			fileExtensionsFormats_mp.insert({".trXYi_px", TRXYI_FORMAT});
			fileExtensionsFormats_mp.insert({".svg", SVG_FORMAT});
			fileExtensionsFormats_mp.insert({".trxyt", TRXYumT_FORMAT});
			fileExtensionsFormats_mp.insert({".trxyi", TRXYumI_FORMAT});

			vector<pair<string, string>> typeGroupName_filteredTypes_v;
				typeGroupName_filteredTypes_v.push_back({"PALMTRACER_FORMAT", "trc"});
				typeGroupName_filteredTypes_v.push_back({"TRXYT_FORMAT-[frame][px][s]", "trxyt_px"});
				typeGroupName_filteredTypes_v.push_back({"TRXYI_FORMAT-[frame][px][frame]", "trXYi_px"});
				typeGroupName_filteredTypes_v.push_back({"SVG_FORMAT", "svg"});
				typeGroupName_filteredTypes_v.push_back({"TRXYumT_FORMAT-[frame][µm][s]", "trxyt"});
				typeGroupName_filteredTypes_v.push_back({"TRXYumI_FORMAT-[frame][µm][frame]", "trxyi"});

			isFile_selected = getSaveLocationFiltered(file_path,typeGroupName_filteredTypes_v, "test.trc",
													  "SimulationOutputs", this);
			if(isFile_selected == true)
			{
				uint pos;
				pos = file_path.find_last_of('.');
				if(pos != file_path.npos)
				{
					string file_extension = file_path.substr(pos);
					FLUOEVENT_FILE_FORMAT exported_format = fileExtensionsFormats_mp[file_extension];

					if(m_exportedTrajectory_fileFormat == SVG_FORMAT &&
					   m_colorMode == DINST_COLOR_MODE)
					{
						exported_format = SVG_MULTICOLOR_FORMAT;
					}

					saveTracesAsString(m_filteredTraces, file_path, exported_format, m_timestep, 0, m_pixel_size);
				}
			}
		}
		break;
	}
}


void TracePlayerControl::onTrajectoriesRenderingModeGetChanged(int idx)
{
	m_trajectories_renderingMode = (TRAJECTORIES_RENDERING_MODE) idx;
	TracePlayerModel::updateRenderingPipeline();
}
void TracePlayerControl::onEventsRenderingModeGetChanged(int idx)
{
	m_events_renderingMode = (EVENTS_RENDERING_MODE) idx;
	TracePlayerModel::updateRenderingPipeline();
}
void TracePlayerControl::onColorModeGetChanged(int idx)
{
	m_colorMode = (COLOR_MODE) idx;
	TracePlayerModel::updateRenderingPipeline();
}


void TracePlayerControl::onOpenFileAction()
{
	string path;

	vector<pair<string, string> > nameFilter_list;
		nameFilter_list.push_back(pair<string, string> ("trace files", "trc\tbtrc"));
		nameFilter_list.push_back(pair<string, string> ("diffusion files", "txt"));

	getFileLocationFiltered(path, nameFilter_list,"", &m_parameters_wdgt);

	if(QString(path.data()).endsWith(".trc"))
	{
	   TracePlayerModel::setTracesFromStringFile(path);
       TracePlayerView::m_fileLoader_lineEdit.setText(path.data());
	}

	if(QString(path.data()).endsWith(".btrc"))
	{
	   TracePlayerModel::setTracesFromBinaryFile(path);
       TracePlayerView::m_fileLoader_lineEdit.setText(path.data());
	}

	if(QString(path.data()).endsWith("D.txt"))
	{
	   TracePlayerModel::setTracesDsFromStringFile(path);
       TracePlayerView::m_fileLoader_lineEdit.setText(path.data());
	}
}

void TracePlayerControl::onClearFileAction()
{
	TracePlayerModel::clearTraces();
    TracePlayerView::m_fileLoader_lineEdit.clear();

}

void TracePlayerControl::onOpenMovieAction()
{
	string path;

	vector<pair<string, string> > nameFilter_list;
		nameFilter_list.push_back(pair<string, string> ("TIFF Files", "tiff\ tif"));

	getFileLocationFiltered(path, nameFilter_list,"", &m_parameters_wdgt);

	if(QString(path.data()).endsWith(".tiff") || QString(path.data()).endsWith(".tif"))
	{
		setMovieFromStringFile(path);
        TracePlayerView::m_movieLoader_lineEdit.setText(path.data());
	}
}

void TracePlayerControl::onClearMovieAction()
{
	TracePlayerModel::clearMovie();
    TracePlayerView::m_movieLoader_lineEdit.clear();
}


void TracePlayerControl::onSaveFileAction()
{
	string file_dir = "Ds.txt";
	saveTracesDsAsString(TracePlayerControl::m_filteredTraces, file_dir);
}

void TracePlayerControl::onMSDExportAction()
{
    saveTracesMSDsAsString(m_filteredTraces, "MSD.txt");
}

void TracePlayerControl::onMultipageTiffExportAction()
{
//	if(m_traces.size() == 0) return; //->

//	float dx, dy; //dx : shortest dimension
//		dx =  getGLWidget()->size().x;
//		dy = getGLWidget()->size().y;
//		if(dx <= dy)
//		{
//			dx = getGLWidget()->size().x;
//			dy = getGLWidget()->size().y;
//		}
//		else
//		{
//			dy = getGLWidget()->size().x;
//			dx = getGLWidget()->size().y;
//		}

//	vec2 scrn_size = vec2(dx,dy);

//	myTiff tiff;
//	tiff.open("tracesMovie.tiff", myTiff::WRITE_MODE);

//	uint8* data;
//	data = new uint8[int(scrn_size.x*scrn_size.y)*3];

//	long plane_idx = m_min_plane;
//	while(plane_idx != m_max_plane)
//	{
//		cout<<plane_idx<<"\n";
//		m_current_plane = plane_idx;
//		m_slider.setValue(plane_idx);

//		render();
////		getGLWidget()->updateGL();
//		glReadPixels(0,0,scrn_size.x,scrn_size.y,GL_RG, GL_UNSIGNED_BYTE, data);
//		tiff.addPage(scrn_size,1,16,data);
//		plane_idx++;
//		cout<<plane_idx<<"\n";
//	}
//	tiff.close();

//	cout<<"done!!";

//	delete[] data;
}


void TracePlayerControl::onDsHistogramExportAction()
{
	string D_path;
	switch(m_isBatchModeSelected)
	{
		case false :
		{

			D_path = "DHist_n"
				   + to_string(m_DsHistogram_nbIntervals)
				   + "_mn"
				   + to_string(int(m_DsHistogram_minValue))
				   + "_mx"
				   + to_string(int(m_DsHistogram_maxValue))
				   + ".txt";
			saveTracesLogDsHistogramAsString(m_filteredTraces, D_path, m_DsHistogram_nbIntervals,
															 m_DsHistogram_minValue,
															 m_DsHistogram_maxValue);
		}
		break;


		case true :
		{
			int trace_path_idx = 0;
			QString path_qstr;

			auto trace_path = m_tracePaths_v.begin();
			while(trace_path != m_tracePaths_v.end())
			{

				path_qstr = QString(trace_path->data());
				if(path_qstr.endsWith(".trc"))
				{
					setTracesFromStringFile(path_qstr.toLocal8Bit().data());
				}

				if(path_qstr.endsWith(".btrc"))
				{
					setTracesFromBinaryFile(path_qstr.toLocal8Bit().data());
				}

				render();
				getGLWidget()->swapBuffers();

				D_path = *trace_path
					   + "_D.txt";

				trace_path_idx++;
				trace_path++;

				saveTracesLogDsHistogramAsString(m_filteredTraces, D_path, m_DsHistogram_nbIntervals,
																 m_DsHistogram_minValue,
																 m_DsHistogram_maxValue);
			}
		}
		break;
	}


}

void TracePlayerControl::onLengthsHistogramExportAction()
{
	string file = "lgthHist_n"
		   + to_string(m_lengthsHistogram_nbIntervals)
		   + "_mn"
		   + to_string(int(m_lengthsHistogram_minValue))
		   + "_mx"
		   + to_string(int(m_lengthsHistogram_maxValue))
		   + ".txt";

	saveTracesLengthsHistogramAsString(m_filteredTraces, file, m_lengthsHistogram_nbIntervals,
													   m_lengthsHistogram_minValue,
													   m_lengthsHistogram_maxValue);
}

void TracePlayerControl::onBatchModeSelectionAction()
{
//	m_isBatchModeSelected = m_batchModeSelection_action.isChecked();
}


void TracePlayerControl::onAddPathsAction()
{
	vector<string> file_dirs_v;
	getFileLocations(file_dirs_v,m_last_dir);
	m_tracePaths_v.insert(m_tracePaths_v.begin(), file_dirs_v.begin(), file_dirs_v.end());
}

void TracePlayerControl::onClearPathsAction()
{
	m_tracePaths_v.clear();
}

/*virtual*/ void TracePlayerControl::setCurrentPlane(long plane)
{
	if(plane < 1 || plane > m_max_plane) return; //->

	//model
	TracePlayerModel::setCurrentPlane(plane);
	//view
	m_slider.setValue(plane-1);
	TracePlayerModel::updateRenderingPipeline();
	updateCurrentPlaneWord();
}


