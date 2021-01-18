
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

#ifndef TRACE_H
#define TRACE_H

#include <stdlib.h>
#include "vector"
#include <math.h>
#include <limits.h>
#include "iostream"
#include "fstream"

#include "QProgressBar"

#include <glm.hpp>
    #define GLM_SWIZZLE
    #include <gtc/matrix_transform.hpp>
    #include <gtc/matrix_access.hpp>

#include "cellEngine_library_global.h"
    #include "FluoEvent.h"
    #include "biologicalWorld/Region_gpu.h"

#include "toolBox_src/toolbox_library_global.h"
    #include "toolBox_src/fileAndstring_manipulation/file_manipulation.h"
    #include "toolBox_src/fileAndstring_manipulation/string_manipulation.h"
    #include "toolBox_src/otherFunctions/otherFunctions.h"
    #include "toolBox_src/developmentTools/myClock.h"
    #include "toolBox_src/imageTools/image_colorisation/myLUT.h"

#include "lmstruct.h"
#include "lmcurve.h"

double linear(double t, const double* coef);


class CELLENGINE_LIBRARYSHARED_EXPORT Trace
{	
    friend void analyseTracesCrossings(std::vector<Trace>&,
									   const Region&,
									   vector<float>&,
									   vector<float>&);
    friend void setTracesColorsUsingDInsts(std::vector<Trace>&,myLUT&, float, float);



public :

	enum COLOR_MODE {UNIQUE_COLOR_MODE, EXTERN_COLOR_MODE, PER_EVENT_EXTERN_COLOR_MODE};

    Trace();

	void addFluoEvent(FluoEvent event);
	void setX(float x, int event_idx);
	void setY(float y, int event_idx);
	void setTraceIdx(int trc_idx);
	void setD(float D);

	void setColor(glm::vec4 color);
	void setColorMode(COLOR_MODE mode);
	void changeColor();

	bool isInsideRange(int plane_idx);
	bool isInsideRegion(const Region& rgn, float& nbPoints_insideRgn);

	FluoEvent getFluoEvent(int i);
	FluoEvent getLastEvent();
	FluoEvent getFirstEvent();
	FluoEvent& getFluoEventByRef(int i);
	std::vector <FluoEvent> getFluoEventVector();

	bool isMSDCalculated();
	bool isDCalculated();
	bool isDInstCalculated();


	void getVecMSDFullTrace(std::vector<glm::vec2> *vecMSD_vect, glm::vec2 U, glm::vec2 V);
	void computeMSD(int max_DPlane);
	void computeD(float pxlSize_in_um, float Dplane_in_seconds,
				  int firstMSDPoint_idx, int lastMSDPoint_idx);
	void computeDInst(float pxlSize_in_um, float Dplane_in_seconds,
				  uint nbPoints_before, uint nbPoints_after);


	int getLength();
	glm::vec2 getBarycenter();
	glm::vec4 getColor();
	vector<glm::vec4>& getColors();
	COLOR_MODE getColorMode();
	float getD();
	float getMSD0Fit();
	std::vector<double>& getMSDs();
	const std::vector<float>& getDInsts();
	int getTraceIdx();

	void clearFluoEventVector();

    void elongateTrc(Trace& toBe_added_trc);
    Trace getElongatedTrace(Trace& toBe_added_trc);
    int findClosestTrace(std::vector<Trace> & traces, int starting_idx, int min_delay, int max_delay, int max_distance);
    bool isConnectable(Trace trace, int min_delay, int max_delay, float max_distance, float* distance_out);

private :

	std::vector <FluoEvent> m_fluo_events;
	std::vector<float> m_Dinst_v;

	COLOR_MODE m_color_mode;
	glm::vec4 m_unique_color;

	glm::vec4 m_external_color;
	std::vector<glm::vec4> m_external_colors_v;
    bool m_is_filtered;
	bool m_is_DinstCalculated;
    int m_trc_idx;


	bool m_isDCalculated;
	int m_D_firstMSDPoint_idx;
	int m_D_lastMSDPoint_idx;
	float m_D;
	float m_MSD0_fit;
	float m_pxlSize_in_um; //µm
	float m_Dplane_in_seconds; //s

	int m_MSD_maxDPlane;
	bool m_isMSDCalculated;
	std::vector<double> m_MSD_v;

};

//class Trace;

//set
CELLENGINE_LIBRARYSHARED_EXPORT void setTracesColorsUsingDs(std::vector<Trace> &traces, myLUT& lut, float min_logD = -1, float max_logD = -1);
CELLENGINE_LIBRARYSHARED_EXPORT void setTracesColorsUsingDInsts(std::vector<Trace> &traces, myLUT& lut, float min_logD = -1, float max_logD = -1);
CELLENGINE_LIBRARYSHARED_EXPORT void setTracesColorsUsingLengths(std::vector<Trace> &traces, myLUT& lut);
CELLENGINE_LIBRARYSHARED_EXPORT void setTracesColorsRandom(std::vector<Trace> &traces);

//load
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<Trace> loadTracesAsString(std::string file_dir);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<Trace> loadTracesAsString2(std::string file_dir, QProgressBar* progressBar = 0);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<Trace> loadTracesAsBinary(std::string file);
CELLENGINE_LIBRARYSHARED_EXPORT void loadDAsString(std::vector<Trace>& traces_ref, std::string file);

//save
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesAsString(std::vector<Trace> &traces, std::string file_dir,
														  FLUOEVENT_FILE_FORMAT format = PALMTRACER_FORMAT, float dt=-1,
														  QProgressBar* progressBar = 0, float px = -1);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesAsBinary(std::vector<Trace> &traces, std::string file_dir);

CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesDsAsString(std::vector<Trace> &traces, std::string file_dir, bool isVertical = true, bool isAppended = false);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesDInstsAsString(std::vector<Trace> &traces, std::string file_dir);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesDInstsPulledAsString(std::vector<Trace> &traces, std::string file_dir);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesMSDsAsString(std::vector<Trace> &traces, std::string file_dir, bool isAppended = false);

CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesLogDsHistogramAsString(std::vector<Trace> &traces,
																	 std::string file_dir, int nb_intervals,
																	 float min_value=-1, float max_value=-1);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesLogDInstsHistogramAsString(std::vector<Trace> &traces,
																	 std::string file_dir, int nb_intervals,
																	 float min_value=-1, float max_value=-1);
CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesLengthsHistogramAsString(std::vector<Trace> &traces,
																	 std::string file_dir,  int nb_intervals,
																	 float min_value=-1, float max_value=-1);

//get
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<FluoEvent> getEventsInPlane(int plane_idx, std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<glm::vec2> getEventCoordsInPlane(int plane_idx, std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT void getEventCoordsInPlane(int plane_idx, std::vector<Trace> &traces, std::vector<glm::vec2> &r_v, std::vector<glm::vec4> &color_v);
CELLENGINE_LIBRARYSHARED_EXPORT void getTracesCoordsInPlane(int plane_idx, std::vector<Trace> &traces, std::vector<glm::vec2> &r_v, std::vector<glm::vec4> &color_v);
CELLENGINE_LIBRARYSHARED_EXPORT int getMaxPlane(std::vector<Trace>& traces);
CELLENGINE_LIBRARYSHARED_EXPORT int getMinPlane(std::vector<Trace>& traces);

CELLENGINE_LIBRARYSHARED_EXPORT int getMaxLength(std::vector<Trace>& traces);
CELLENGINE_LIBRARYSHARED_EXPORT int getMinLength(std::vector<Trace>& traces);

CELLENGINE_LIBRARYSHARED_EXPORT void getTracesCoords(std::vector<Trace> &traces, std::vector<glm::vec2> &r_v, std::vector<glm::vec4> &color_v);
CELLENGINE_LIBRARYSHARED_EXPORT void getEventsCoords(std::vector<Trace> &traces, std::vector<glm::vec2> &r_v, std::vector<glm::vec4> &color_v);
CELLENGINE_LIBRARYSHARED_EXPORT int getNbEvents(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<float> getLengths(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<float> getDs(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<float> getLogDs(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<float>  getLogDInsts(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT float getMinD(std::vector<Trace> &traces);
CELLENGINE_LIBRARYSHARED_EXPORT float getMaxD(std::vector<Trace> &traces);


CELLENGINE_LIBRARYSHARED_EXPORT void computeMSDs(std::vector<Trace>& traces, int max_DPlane);
CELLENGINE_LIBRARYSHARED_EXPORT void computeDs(std::vector<Trace>& traces, float pxlSize_in_um, float Dplane_in_seconds,  int firstMSDPoint_idx, int lastMSDPoint_idx);
CELLENGINE_LIBRARYSHARED_EXPORT void computeDInst(std::vector<Trace>& traces, float pxlSize_in_um, float Dplane_in_seconds,  uint nbPoints_before, uint nbPoints_after);


CELLENGINE_LIBRARYSHARED_EXPORT void saveTracesProperties(std::vector<Trace>& traces, std::string file_directory);

//trace analysis
CELLENGINE_LIBRARYSHARED_EXPORT void analyseTracesCrossings(std::vector<Trace>& traces, const Region& rgn,
															  vector<float>& nbEventsBeforeLeaving, vector<float>& nbEventsBeforeEntering);





#endif // MYTRACE_H
