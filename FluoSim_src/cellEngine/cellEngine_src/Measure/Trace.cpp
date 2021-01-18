
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




#include "Trace.h"


using namespace std;
using namespace glm;


double linear(double t, const double* coef)
{
	return coef[0] + coef[1]*t;
}

/********************************
 *
 *			Class Trace
 *
 * ******************************/

Trace::Trace()
{
	m_isMSDCalculated = false;
	m_isDCalculated = false;
	m_is_DinstCalculated = false;

	m_MSD_maxDPlane = -1;
	m_D_firstMSDPoint_idx = -1;
	m_D_lastMSDPoint_idx = -1;
	m_pxlSize_in_um = -1;
	m_Dplane_in_seconds = -1;

	m_color_mode = UNIQUE_COLOR_MODE;
	float r,g,b;
	r = uniformDistributiion(0.0f,1.0f);
	g = uniformDistributiion(0.0f,1.0f);
	b = uniformDistributiion(0.0f,1.0f);
	m_unique_color = vec4(r,g,b,1.0);
	m_external_color = vec4(1.0,1.0,1.0,1.0);

	m_is_filtered = 0;
	m_trc_idx = -1;
	m_D = pow(10.0, -7.0);
	m_MSD0_fit = -1;
}

void Trace::addFluoEvent(FluoEvent event)
{
	m_fluo_events.push_back(event);

	m_isMSDCalculated = false;
	m_isDCalculated = false;
	m_is_DinstCalculated = false;
	m_MSD_maxDPlane = -1;
}

void Trace::setX(float x, int event_idx)
{
	if(event_idx < m_fluo_events.size()) m_fluo_events[event_idx].x = x;

	m_isMSDCalculated = false;
	m_isDCalculated = false;
	m_is_DinstCalculated = false;
	m_MSD_maxDPlane = -1;
}

void Trace::setY(float y, int event_idx)
{
	if(event_idx < m_fluo_events.size()) m_fluo_events[event_idx].y = y;

	m_isMSDCalculated = false;
	m_isDCalculated = false;
	m_is_DinstCalculated = false;
	m_MSD_maxDPlane = -1;
}

void Trace::setTraceIdx(int trc_idx)
{
	m_trc_idx = trc_idx;
}

void Trace::setD(float D)
{
	m_D = D;
}

void Trace::setColor(glm::vec4 color)
{
	m_external_color = color;
}

void Trace::setColorMode(COLOR_MODE color_mode)
{
	m_color_mode = color_mode;
}

void Trace::changeColor()
{
	float r,g,b;
	r = uniformDistributiion(0.0f,1.0);
	g = uniformDistributiion(0.0f,1.0);
	b = uniformDistributiion(0.0f,1.0);
	m_unique_color = vec4(r,g,b,0.0);
}

FluoEvent Trace::getFluoEvent(int i)
{
	if(i < m_fluo_events.size()) return m_fluo_events[i];
}

FluoEvent Trace::getLastEvent()
{
	return m_fluo_events.back();
}

FluoEvent Trace::getFirstEvent()
{
	return m_fluo_events.front();
}

FluoEvent& Trace::getFluoEventByRef(int i)
{
	if(i < m_fluo_events.size()) return m_fluo_events[i];
}

vector <FluoEvent> Trace::getFluoEventVector()
{
	return m_fluo_events;
}

void Trace::clearFluoEventVector()
{
	m_fluo_events.clear();
}

int Trace::getLength()
{
	return m_fluo_events.size();
}

vec2 Trace::getBarycenter()
{
	vec2 barycenter = vec2(0,0);

	for(int point_idx = 0; point_idx<= m_fluo_events.size()-1; point_idx++)
	{
		barycenter[0] += m_fluo_events[point_idx].x;
		barycenter[1] += m_fluo_events[point_idx].x;
	}

	barycenter[0] = barycenter[0]/m_fluo_events.size();
	barycenter[1] = barycenter[1]/m_fluo_events.size();

	return barycenter;

}

vec4 Trace::getColor()
{
	vec4 color;
	switch(m_color_mode)
	{
		case UNIQUE_COLOR_MODE :
		{
			color = m_unique_color;
		}
		break;

		case EXTERN_COLOR_MODE :
		{
			color = m_external_color;
		}
		break;
	}
	return color;
}

vector<vec4>& Trace::getColors()
{
	return m_external_colors_v;
}

Trace::COLOR_MODE Trace::getColorMode()
{
	return m_color_mode;
}

float Trace::getD()
{
	return m_D;
}

float Trace::getMSD0Fit()
{
	return m_MSD0_fit;
}

std::vector<double>& Trace::getMSDs()
{
	return m_MSD_v;
}

const std::vector<float>& Trace::getDInsts()
{
	return m_Dinst_v;
}

int Trace::getTraceIdx()
{
	return m_trc_idx;
}

bool Trace::isMSDCalculated()
{
	return m_isMSDCalculated;
}

bool Trace::isDCalculated()
{
	return m_isDCalculated;
}

bool Trace::isDInstCalculated()
{
	return m_is_DinstCalculated;
}

//    void getVecMSDFullTrace(vec2* *vecMSD_array, int* array_length, vec2 U, vec2 V)
void Trace::getVecMSDFullTrace(vector<vec2> *vecMSD_vect, vec2 U, vec2 V)
{
	vec2 u = U/length(U);
	vec2 v = V/length(V);

	if(u.x*v.y - v.x*u.y == 0)
	{
		return ;
	}
	mat2 M = 1/(u.x*v.y - v.x*u.y) * transpose(mat2(v.y, -v.x,
													-u.y, u.x));

	vec2 R_i, R_i_n, transformed_delta_R;

	vec2* summed_vecMSD_squared_array;
	int* number_associated_subTraces;



	int max_deltaT = -1;
	for(int event_idx1 = 0; event_idx1 <= getLength()-1; event_idx1++)
	{
		for(int event_idx2 = event_idx1; event_idx2 <= getLength()-1; event_idx2++)
		{
			if(max_deltaT <= m_fluo_events[event_idx2].plane-m_fluo_events[event_idx1].plane)
			{
				max_deltaT = m_fluo_events[event_idx2].plane-m_fluo_events[event_idx1].plane;
			}
		}
	}

    summed_vecMSD_squared_array = new vec2[max_deltaT+1]; //from 0 displacement to max_deltaT displacement
    number_associated_subTraces = new int[max_deltaT+1]; //from 0 displacement to max_deltaT displacement

	for(int summed_array_idx = 0; summed_array_idx <= max_deltaT+1-1; summed_array_idx++)
	{
		summed_vecMSD_squared_array[summed_array_idx] = vec2(0.0, 0.0);
		number_associated_subTraces[summed_array_idx]=0;
	}

	int array_index=-1;
	for(int event_idx1 = 0; event_idx1 <= getLength()-1; event_idx1++)
	{
		for(int event_idx2 = event_idx1 ; event_idx2 <= getLength()-1; event_idx2++)
		{
			array_index = m_fluo_events[event_idx2].plane - m_fluo_events[event_idx1].plane;

			R_i =   vec2(m_fluo_events[event_idx1].x, m_fluo_events[event_idx1].y);
			R_i_n = vec2(m_fluo_events[event_idx2].x, m_fluo_events[event_idx2].y);
			transformed_delta_R = M*(R_i_n - R_i);

			summed_vecMSD_squared_array[array_index][0] += pow(transformed_delta_R[0], 2);
			summed_vecMSD_squared_array[array_index][1] += pow(transformed_delta_R[1], 2);

			number_associated_subTraces[array_index] += 1;

		}
	}
	for(int summed_array_idx = 0; summed_array_idx <= max_deltaT+1-1; summed_array_idx++)
	{

		if(number_associated_subTraces[summed_array_idx]!=0)
		{
			summed_vecMSD_squared_array[summed_array_idx][0] = float(summed_vecMSD_squared_array[summed_array_idx][0]/number_associated_subTraces[summed_array_idx]);
			summed_vecMSD_squared_array[summed_array_idx][1] = float(summed_vecMSD_squared_array[summed_array_idx][1]/number_associated_subTraces[summed_array_idx]);
		}
		else
		{
			summed_vecMSD_squared_array[summed_array_idx][0] = 0.0;
			summed_vecMSD_squared_array[summed_array_idx][1] = 0.0;
		}

	}

	for(int summed_array_idx =0; summed_array_idx<= max_deltaT +1 - 1 ; summed_array_idx++)
	{
		vecMSD_vect->push_back(summed_vecMSD_squared_array[summed_array_idx]);
	}

	delete[] summed_vecMSD_squared_array;
	delete[] number_associated_subTraces;
}

void Trace::computeMSD(int max_DPlane)
{
	if(m_isMSDCalculated == true && m_MSD_maxDPlane == max_DPlane) return; //->

	int trc_length = getLength();
	if(trc_length <= max_DPlane)
	{
		m_isMSDCalculated = false;
		m_MSD_maxDPlane = -1;
		return; //->
	}

	m_MSD_v.clear();

	double MSD = 0;
	int DPlane_idx = 1;

	m_MSD_maxDPlane = max_DPlane;
	m_MSD_v.push_back(0);

	while(DPlane_idx != max_DPlane)
	{
		MSD = 0;
		for(int sum_plane_idx = 0; sum_plane_idx<= trc_length-1-DPlane_idx; sum_plane_idx++)
		{
			vec2 dr = vec2(getFluoEvent(sum_plane_idx + DPlane_idx).x,
						   getFluoEvent(sum_plane_idx + DPlane_idx).y) -
					  vec2(getFluoEvent(sum_plane_idx).x,
						   getFluoEvent(sum_plane_idx).y);
			MSD += length(dr)*length(dr);
		}
		MSD /= trc_length-1-DPlane_idx;
		m_MSD_v.push_back(MSD);

		DPlane_idx++;
	}

	m_isMSDCalculated = true;

	return; //->
}


void Trace::computeD(float pxlSize_in_um, float Dplane_in_seconds, int firstMSDPoint_idx, int lastMSDPoint_idx)
{
	if(m_isMSDCalculated == true && m_isDCalculated == true &&
	   m_D_firstMSDPoint_idx == firstMSDPoint_idx &&
	   m_D_lastMSDPoint_idx == lastMSDPoint_idx &&
	   m_pxlSize_in_um == pxlSize_in_um && m_Dplane_in_seconds == Dplane_in_seconds)
	{
		return; //->
	}


	if(m_isMSDCalculated == false || m_MSD_maxDPlane < lastMSDPoint_idx)
	{
		m_isDCalculated = false;
		m_D_firstMSDPoint_idx = -1;
		m_D_lastMSDPoint_idx = -1;
		m_D = -1;
		m_Dplane_in_seconds = -1;
		m_pxlSize_in_um = -1;

		return; //->
	}

	lm_control_struct control = lm_control_float;
	lm_status_struct status;
	control.verbosity = 0;

	double param[2] = {0,1};
	vector<double> t;
	vector<double> msd;
	for(int MSDPoint_idx = firstMSDPoint_idx; MSDPoint_idx <= lastMSDPoint_idx; MSDPoint_idx++)
	{
		t.push_back(MSDPoint_idx);
		msd.push_back(m_MSD_v[MSDPoint_idx]);
	}

	lmcurve(2, param, t.size(), t.data(), msd.data(),
	linear, &control, &status);

	m_D = 0.25*pxlSize_in_um*pxlSize_in_um*(1/Dplane_in_seconds)*param[1];
	if(m_D <= 0.00001f) m_D = 0.00001f;
	m_MSD0_fit = param[0];

	m_isDCalculated = true;
	m_D_firstMSDPoint_idx = firstMSDPoint_idx;
	m_D_lastMSDPoint_idx = lastMSDPoint_idx;
	m_pxlSize_in_um = pxlSize_in_um;
	m_Dplane_in_seconds = Dplane_in_seconds;

	return ;
}

void Trace::computeDInst(float pxlSize_in_um, float Dplane_in_seconds,
			  uint nbPoints_before, uint nbPoints_after)
{
	m_Dinst_v.clear();

	for(int fluoEvent_idx = 0; fluoEvent_idx < m_fluo_events.size(); fluoEvent_idx++)
	{
		double params[2] = {0,1};
		lm_control_struct control = lm_control_float;
		lm_status_struct status;
		control.verbosity = 0;
		float Dinst = -1;

		int fluoEventWindow_beg = fluoEvent_idx - nbPoints_before;
		int fluoEventWindow_end = fluoEvent_idx + nbPoints_after + 1; //we use the std convention

		if(fluoEventWindow_beg >= 0 && fluoEventWindow_end <= m_fluo_events.size())
		{
			vector<double> t_v;
			vector<double> msd_v;

			for(int dPlane = 0;
					dPlane < fluoEventWindow_end - fluoEventWindow_beg;
					dPlane++)
			{
				t_v.push_back(dPlane);
				float msd = 0;
				for(int MSDpoint_idx = fluoEventWindow_beg;
						MSDpoint_idx < fluoEventWindow_end - dPlane;
						MSDpoint_idx ++)
				{
					vec2 dr = {m_fluo_events[MSDpoint_idx+dPlane].x - m_fluo_events[MSDpoint_idx].x,
							   m_fluo_events[MSDpoint_idx+dPlane].y - m_fluo_events[MSDpoint_idx].y};
					msd += length(dr)*length(dr);


				}

				msd_v.push_back(msd/(fluoEventWindow_end-dPlane-fluoEventWindow_beg));
			}

			lmcurve(2, params, 4, t_v.data(), msd_v.data(),
			linear, &control, &status);

			Dinst = 0.25*pxlSize_in_um*pxlSize_in_um*(1/Dplane_in_seconds)*params[1];
			if(m_D <= 0.00001f) Dinst = 0.00001f;
		}
		m_Dinst_v.push_back(Dinst);
	}

	m_is_DinstCalculated = true;
}



void Trace::elongateTrc(Trace& toBe_added_trc)
{
	for(int fluoEvent_idx = 0; fluoEvent_idx <= toBe_added_trc.getLength()-1; fluoEvent_idx++)
	{
		m_fluo_events.push_back(toBe_added_trc.getFluoEvent(fluoEvent_idx));
	}
}

Trace Trace::getElongatedTrace(Trace& toBe_added_trc)
{
	Trace elongated_trc = *this;
	elongated_trc.elongateTrc(toBe_added_trc);

	return elongated_trc;
}

int Trace::findClosestTrace(vector<Trace> & traces, int starting_idx, int min_delay, int max_delay, int max_distance)
{
	int closest_trace_idx = -1;
	float distance;
	float closest_trace_dist = -1.0;

	for(int trc_idx = starting_idx; trc_idx <= traces.size()-1; trc_idx++)
	{
		if(isConnectable(traces[trc_idx], min_delay, max_delay, max_distance, &distance))
		{
			if(closest_trace_dist < 0)
			{
				closest_trace_idx = trc_idx;
				closest_trace_dist = distance;
			}

			else
			{
				if(distance <= closest_trace_dist)
				{
					closest_trace_idx = trc_idx;
					closest_trace_dist = distance;
				}
			}
		}
	}

	return closest_trace_idx;
}



bool Trace::isConnectable(Trace trace, int min_delay, int max_delay, float max_distance, float* distance_out)
{
	// *this is considered as a - trace and is checked if connected to a + trace.
	// the associated reaction is  - operator(+) + -> -+

	bool close_enough;
	bool not_too_delayed;
	bool delayed_enough;

	int delay = trace.getFirstEvent().plane - this->getLastEvent().plane;
	float distance = std::sqrt( std::pow(this->getLastEvent().x - trace.getFirstEvent().x,2.0)
								 + std::pow(this->getLastEvent().y - trace.getFirstEvent().y,2.0));
	close_enough = distance <= max_distance;
	delayed_enough = min_delay <= delay;
	not_too_delayed = delay <= max_delay;

	*distance_out = distance;

	if(close_enough && delayed_enough && not_too_delayed)
	{
		return 1;
	}

	else
	{
		return 0;
	}
}

bool Trace::isInsideRange(int plane_idx)
{
	if(getLength() == 0) return 0;

	return (plane_idx >= m_fluo_events.begin()->plane
			&& plane_idx <= (m_fluo_events.end()-1)->plane);

}

bool Trace::isInsideRegion(const Region& rgn, float& nbPoints_insideRgn)
{
	nbPoints_insideRgn = 0;
	for(const FluoEvent& event : m_fluo_events)
	{
		if(rgn.isInside(vec2(event.x, event.y))) nbPoints_insideRgn++;
	}

	return nbPoints_insideRgn;
}

void setTracesColorsUsingDs(std::vector<Trace> &traces, myLUT& lut, float min_logD, float max_logD)
{
	if(min_logD == -1 && max_logD == -1) min_logD = std::log10(getMinD(traces));
	if(min_logD == -1 && max_logD == -1) max_logD = std::log10(getMaxD(traces));
//	float min_logD = std::log10(getMinD(traces));
//	float max_logD = std::log10(getMaxD(traces));
	float alpha;

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->isDCalculated())
		{
			alpha =  (std::log10(trace->getD()) - min_logD)/(max_logD - min_logD);
			trace->setColor(lut.getColor(alpha));
		}
		else  trace->setColor(glm::vec4(1,1,1,1));
		trace->setColorMode(Trace::EXTERN_COLOR_MODE);
		trace++;
	}
}

void setTracesColorsUsingDInsts(std::vector<Trace> &traces, myLUT& lut, float min_logD, float max_logD)
{
	if(min_logD == -1 && max_logD == -1) min_logD = std::log10(getMinD(traces));
	if(min_logD == -1 && max_logD == -1) max_logD = std::log10(getMaxD(traces));

//	if(min_logD < 0) min_logD = std::log10(getMinD(traces));
//	if(max_logD < 0) max_logD = std::log10(getMaxD(traces));

	for(Trace& trc : traces)
	{
		trc.m_external_colors_v.clear();
		if(trc.isDInstCalculated() == false)
		{
			for(int fluoEvent_idx = 0; fluoEvent_idx <= trc.getLength()-1; fluoEvent_idx++)
			{
				trc.m_external_colors_v.push_back(vec4(1,1,1,1)); //bright white
			}
		}
		else
		{
			for(int fluoEvent_idx = 0; fluoEvent_idx <= trc.getLength()-1; fluoEvent_idx++)
			{
				if(trc.m_Dinst_v[fluoEvent_idx] < 0) trc.m_external_colors_v.push_back(vec4(0,0,0,1));
				else
				{
					float alpha;
					alpha =  (std::log10(trc.m_Dinst_v[fluoEvent_idx]) - min_logD)/(max_logD - min_logD);
					trc.m_external_colors_v.push_back(lut.getColor(alpha));
				}
			}
		}
		trc.setColorMode(Trace::PER_EVENT_EXTERN_COLOR_MODE);
	}
}

void setTracesColorsUsingLengths(std::vector<Trace> &traces, myLUT& lut)
{
	int min_length = getMinLength(traces);
	int max_length = getMaxLength(traces);
	float alpha;

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(float(max_length - min_length) == 0) trace->setColor({1,1,1,1});
		else
		{
			alpha = float(trace->getLength() - min_length)/float(max_length - min_length);
			trace->setColor(lut.getColor(alpha));
		}
		trace->setColorMode(Trace::EXTERN_COLOR_MODE);
		trace++;
	}
}

void setTracesColorsRandom(std::vector<Trace> &traces)
{
	auto trace = traces.begin();
	while(trace != traces.end())
	{
//		trace->changeColor();
		trace->setColorMode(Trace::UNIQUE_COLOR_MODE);
		trace++;
	}
}

vector<Trace> loadTracesAsString2(string file, QProgressBar* progressBar)
{
	vector<Trace> traces;
	Trace temp_trace;
	FluoEvent temp_fluo_event;

	ifstream myfile;
	myfile.open(file.data());
	if(myfile.is_open() == false) return traces; //->

	char line[256];
	myfile.getline(line, 256);
	string line1_str(line);

	FLUOEVENT_FILE_FORMAT format = PALMTRACER_FORMAT;
	if(line1_str.find("Width\tHeight\tnb_Planes\tnb_Tracks\tPixel_Size(um)\tFrame_Duration(s)\tGaussian_Fit\tSpectral") != line1_str.npos)
	{
		format = PALMTRACER2_FORMAT;
	}

	switch(format)
	{
		case PALMTRACER_FORMAT :
		{
			myfile.clear();
			myfile.seekg(0, ios_base::end);
			uint end_pos = myfile.tellg();

			myfile.seekg(0, ios_base::beg);
			float last_trace_idx;
				myfile>>last_trace_idx; //init
			float current_trace_idx = last_trace_idx;
			myfile.seekg(0, ios_base::beg);

			float plane;
			while(myfile.eof() == false)
			{
				if(progressBar != 0) progressBar->setValue(100*float(myfile.tellg())/float(end_pos));

				myfile>>current_trace_idx;
				if(current_trace_idx != last_trace_idx)
				{
					temp_trace.changeColor();
					temp_trace.setTraceIdx(last_trace_idx);
					traces.push_back(temp_trace);
					temp_trace.clearFluoEventVector();
					last_trace_idx = current_trace_idx;
				}

				temp_fluo_event.trace_number = current_trace_idx;
				myfile>>plane;
				temp_fluo_event.plane = plane;
				myfile>>temp_fluo_event.x;
				myfile>>temp_fluo_event.y;
				myfile>>temp_fluo_event.query;
				myfile>>temp_fluo_event.intensity;

				temp_trace.addFluoEvent(temp_fluo_event);
			}

			if(temp_trace.getLength() != 0)
			{
				temp_trace.setTraceIdx(last_trace_idx);
				traces.push_back(temp_trace);
			}
		}
		break;

		case PALMTRACER2_FORMAT :
		{
			myfile.getline(line, 256);
			myfile.getline(line, 256);
			string line3_str(line);

			map<int, string> columnTypes_map;
			map<string, float> data_map;
			int pos = 0;
			string word;
			int column_idx = 0;
			while(getWord(line3_str, word, pos, pos))
			{
				columnTypes_map[column_idx] = word;
				data_map[word] = -1;
				column_idx++;
			}

			if(data_map.find(string("Track")) == data_map.end() ||
			   data_map.find("Plane") == data_map.end() ||
			   data_map.find("CentroidX(px)") == data_map.end() ||
			   data_map.find("CentroidY(px)") == data_map.end() ||
			   data_map.find("Integrated_Intensity") == data_map.end())
			{
				return traces; //->
			}

			for(uint column_idx = 0; column_idx < columnTypes_map.size(); column_idx++)
			{
				myfile>>data_map[columnTypes_map[column_idx]];
			}
			int current_trace_idx = data_map["Track"];
			int last_trace_idx = data_map["Track"];

            //we restart from the beginning
			myfile.seekg(0, ios_base::end);
			uint end_pos = myfile.tellg();
			myfile.seekg(0, ios_base::beg);
			myfile.getline(line, 256);
			myfile.getline(line, 256);
			myfile.getline(line, 256);

			while(myfile.eof() == false)
			{
				if(progressBar != 0) progressBar->setValue(100*float(myfile.tellg())/float(end_pos));

				for(uint column_idx = 0; column_idx < columnTypes_map.size(); column_idx++)
				{
					myfile>>data_map[columnTypes_map[column_idx]];
				}

				current_trace_idx = data_map["Track"];
				if(current_trace_idx != last_trace_idx)
				{
					temp_trace.changeColor();
					temp_trace.setTraceIdx(last_trace_idx);
					traces.push_back(temp_trace);
					temp_trace.clearFluoEventVector();
					last_trace_idx = current_trace_idx;
				}

				temp_fluo_event.trace_number = current_trace_idx;
				temp_fluo_event.plane = data_map["Plane"];
				temp_fluo_event.x = data_map["CentroidX(px)"];
				temp_fluo_event.y = data_map["CentroidY(px)"];
				temp_fluo_event.intensity = data_map["Integrated_Intensity"];
				temp_fluo_event.query = -1;

				temp_trace.addFluoEvent(temp_fluo_event);
			}

			if(temp_trace.getLength() != 0)
			{
				temp_trace.setTraceIdx(last_trace_idx);
				traces.push_back(temp_trace);
			}
		}
		break;
	}

	myfile.close();

	return traces;
}



vector<Trace> loadTracesAsString(string file)
{
	vector<Trace> traces;

	string file_content;
	char* file_content_bis;
	bool file_opened = getFileContent(file, file_content);

	if(file_opened == false) return traces; //->

	long nb_words = getNumberWords(file_content);

	if(nb_words%6 != 0 && nb_words%7 !=0)
	{
		cout<<"error : file has not the right format!!!\n";
		return traces;
	}

	Trace temp_trace;
		FluoEvent temp_fluo_event;
	string line;
		string word;
	int file_pos = 0;
		int line_pos = 0;

	getLine(file_content, line, file_pos, file_pos);
			getWord(line, word, 0, line_pos); //first trace_idx
			int last_trace_idx = strtod(word.data(), 0);
	file_pos = 0;

	while(getLine(file_content, line, file_pos, file_pos))
	{
		getWord(line, word, 0, line_pos);
		int trace_idx = strtod(word.data(), 0);

		if(trace_idx != last_trace_idx)
		{
			temp_trace.changeColor();
			temp_trace.setTraceIdx(trace_idx);
			traces.push_back(temp_trace);
			temp_trace.clearFluoEventVector();
			last_trace_idx = trace_idx;
		}

		line_pos = 0;

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.trace_number = (strtod(word.data(), 0));

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.plane = (strtod(word.data(), 0));

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.x = (strtod(word.data(), 0));

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.y = (strtod(word.data(), 0));

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.query = (strtod(word.data(), 0));

		getWord(line, word, line_pos, line_pos);
		temp_fluo_event.intensity = (strtod(word.data(), 0));

		temp_trace.addFluoEvent(temp_fluo_event);
	}

	if(temp_trace.getLength() != 0)
	{
		temp_trace.setTraceIdx(last_trace_idx);
		traces.push_back(temp_trace);
	}

	return traces;
}


vector<Trace> loadTracesAsBinary(string file)
{
	vector<Trace> traces;
	Trace temp_trace;
		FluoEvent temp_fluo_event;

	int nb_bytes;
	char* file_content;
	bool file_opened = getFileContent((char*) file.data(), &file_content, &nb_bytes);

	if(file_opened == false) return traces; //->

	int nb_fluoEvent = nb_bytes/sizeof(FluoEvent);

	FluoEvent* fluo_v = (FluoEvent*) file_content;
	int last_trace_idx = (fluo_v+0)->trace_number;
	for(int fluo_idx = 0; fluo_idx <= nb_fluoEvent -1; fluo_idx++)
	{
		temp_fluo_event = *(fluo_v + fluo_idx);
		int trace_idx = temp_fluo_event.trace_number;

		if(trace_idx != last_trace_idx)
		{
			temp_trace.changeColor();
			traces.push_back(temp_trace);
			temp_trace.clearFluoEventVector();
			last_trace_idx = trace_idx;
		}
		temp_trace.addFluoEvent(temp_fluo_event);
	}

	if(temp_trace.getLength() != 0)
	{
		traces.push_back(temp_trace);
	}

	delete[] file_content;

	return traces;

}

void saveTracesAsString(vector<Trace> &traces, string file_directory, FLUOEVENT_FILE_FORMAT format, float dt,
						QProgressBar* progressBar, float px)
{
	switch(format)
	{

		case PALMTRACER_FORMAT :
		case TRXYT_FORMAT :
		case TRXYI_FORMAT :
		case TRXYumT_FORMAT :
		case TRXYumI_FORMAT :
		{
			ofstream myfile;
			myfile.open(file_directory);

			string a;
			int n_trc = traces.size();
			FluoEvent fluo_event;
			for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
			{
				int n_fluo_event = traces[trc_idx].getLength();

				for(int fluo_idx = 0; fluo_idx<=n_fluo_event-1; fluo_idx++)
				{
					fluo_event = traces[trc_idx].getFluoEvent(fluo_idx);
					getDataStr(fluo_event, a, trc_idx+1, false, format, dt, px);
					if(a.size() > 8*1024*1024)
					{
						myfile.write(a.data(), a.size());
						a.clear();
					}
				}

				if(progressBar !=0) progressBar->setValue(int(100.0f*float(trc_idx)/(n_trc-1)));
			}
			if(a.size() != 0)
			{
				myfile.write(a.data(), a.size());
			}

			myfile.close();
		}
		break;

		case SVG_FORMAT :
		{
			ofstream myfile;
			myfile.open(file_directory);

			int n_trc = traces.size();
			FluoEvent fluo_event;

			myfile<<"<svg width=\"400\" height=\"400\">\n";

			myfile<<" <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"0\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";

			for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
			{
				Trace& trc = traces[trc_idx];
				int n_fluo_event = traces[trc_idx].getLength();
				if(n_fluo_event >= 2)
				{

					myfile<<" <polyline points=\"";

					for(int fluo_idx = 0; fluo_idx<=n_fluo_event-1; fluo_idx++)
					{
						fluo_event = traces[trc_idx].getFluoEvent(fluo_idx);
						myfile<<fluo_event.x<<","<<fluo_event.y<<" ";
					}

					myfile<<"\" ";

					char digit[2] = "0";
					char trc_red[3] = "00", trc_green[3] = "00", trc_blue[3] = "00";
					itoa(int(trc.getColor()[0]*255.0f)/16, digit,16);
					trc_red[0] = digit[0];
					itoa(int(trc.getColor()[0]*255.0f)%16, digit,16);
					trc_red[1] = digit[0];

					itoa(int(trc.getColor()[1]*255.0f)/16, digit,16);
					trc_green[0] = digit[0];
					itoa(int(trc.getColor()[1]*255.0f)%16, digit,16);
					trc_green[1] = digit[0];

					itoa(int(trc.getColor()[2]*255.0f)/16, digit,16);
					trc_blue[0] = digit[0];
					itoa(int(trc.getColor()[2]*255.0f)%16, digit,16);
					trc_blue[1] = digit[0];

					myfile<<"stroke=\"#"<<trc_red<<trc_green<<trc_blue<<"\" ";
					myfile<<"/>\n";
				}

				if(progressBar !=0) progressBar->setValue(int(100.0f*float(trc_idx)/(n_trc-1)));

			}

			myfile<<"</svg>";
			myfile.close();
		}
		break;

		case SVG_MULTICOLOR_FORMAT :
		{
			ofstream myfile;
			myfile.open(file_directory);

			int n_trc = traces.size();
			FluoEvent fluo_event1, fluo_event2;

			myfile<<"<svg width=\"400\" height=\"400\">\n";

			myfile<<" <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"0\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";

			for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
			{
				Trace& trc = traces[trc_idx];
				int n_fluo_event = traces[trc_idx].getLength();
				if(n_fluo_event >= 2)
				{

					myfile<<" <svg>\n";
					for(int fluo_idx = 0; fluo_idx<=n_fluo_event-1-1; fluo_idx++)
					{
						fluo_event1 = traces[trc_idx].getFluoEvent(fluo_idx);
						fluo_event2 = traces[trc_idx].getFluoEvent(fluo_idx+1);

						char digit[2] = "0";
						char fluoEvent_red[3] = "00", fluoEvent_green[3] = "00", fluoEvent_blue[3] = "00";
						itoa(int(trc.getColors()[fluo_idx][0]*255.0f)/16, digit,16);
						fluoEvent_red[0] = digit[0];
						itoa(int(trc.getColors()[fluo_idx][0]*255.0f)%16, digit,16);
						fluoEvent_red[1] = digit[0];

						itoa(int(trc.getColors()[fluo_idx][1]*255.0f)/16, digit,16);
						fluoEvent_green[0] = digit[0];
						itoa(int(trc.getColors()[fluo_idx][1]*255.0f)%16, digit,16);
						fluoEvent_green[1] = digit[0];

						itoa(int(trc.getColors()[fluo_idx][2]*255.0f)/16, digit,16);
						fluoEvent_blue[0] = digit[0];
						itoa(int(trc.getColors()[fluo_idx][2]*255.0f)%16, digit,16);
						fluoEvent_blue[1] = digit[0];

//						myfile<<"stroke=\"#"<<fluoEvent_red<<fluoEvent_green<<fluoEvent_blue<<"\" ";
//						myfile<<"/>\n";

						myfile<<"  <line x1=\""<<fluo_event1.x<<"\" y1=\""<<fluo_event1.y<<"\""
								" x2=\""<<fluo_event2.x<<"\" y2=\""<<fluo_event2.y<<"\""
								" stroke=\"#"<<fluoEvent_red<<fluoEvent_green<<fluoEvent_blue<<"\" />\n";
					}
					myfile<<" </svg>\n";
				}

				if(progressBar !=0) progressBar->setValue(int(100.0f*float(trc_idx)/(n_trc-1)));

			}

			myfile<<"</svg>";
			myfile.close();
		}

	}
}


void saveTracesAsBinary(vector<Trace> &traces, string file_directory)
{
	ofstream myfile;
	myfile.open(file_directory, ios::out | ios::binary);

	vector<int8> a;
	int n_trc = traces.size();
	FluoEvent fluo_event;
	for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
	{
		int n_fluo_event = traces[trc_idx].getLength();

		for(int fluo_idx = 0; fluo_idx<=n_fluo_event-1; fluo_idx++)
		{
			fluo_event = traces[trc_idx].getFluoEvent(fluo_idx);
			getDataBinary(fluo_event, a, trc_idx+1, 0);
			if(a.size() > 8*1024*1024)
			{
				myfile.write((const char*) a.data(), a.size());
				a.clear();
			}
		}
	}
	if(a.size() != 0)
	{
		myfile.write((const char*) a.data(), a.size());
	}

	myfile.close();
}


void saveTracesDsAsString(std::vector<Trace> &traces, std::string file_dir, bool isVertical, bool isAppended)
{
	ofstream myfile;
	if(isAppended == true) myfile.open(file_dir, ios_base::app);
	else myfile.open(file_dir);
	if(myfile.is_open() == false) return; //->

	if(isVertical == true)
	{
		for(Trace& trace : traces)
		{
//			myfile<<trace.getTraceIdx()<<'\t';
//			myfile<<trace.getLength()<<'\t';
			myfile<<trace.getD()<<'\n';
		}
	}
	else
	{
//		for(Trace& trace : traces)
//		{
//			myfile<<trace.getTraceIdx()<<'\t';
//		}
//		myfile<<"\n";

//		for(Trace& trace : traces)
//		{
//			myfile<<trace.getLength()<<'\t';
//		}
//		myfile<<"\n";

		for(Trace& trace : traces)
		{
			myfile<<trace.getD()<<'\t';
		}
		myfile<<"\n";
	}
	myfile.close();
}

void saveTracesDInstsAsString(std::vector<Trace> &traces, std::string file_dir)
{
	ofstream myfile;
	myfile.open(file_dir);
	if(myfile.is_open() == false) return; //->

	int n_trc = traces.size();
	for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
	{
		if(traces[trc_idx].isDInstCalculated() == false) continue; //|_>
		const std::vector<float> DInsts_v = traces[trc_idx].getDInsts();

		myfile<<trc_idx+1<<'\t';
		int size = DInsts_v.size();
		myfile<<size<<"\t";
		for(int Dinst_idx = 0; Dinst_idx <= size-1; Dinst_idx++)
		{
			myfile<<DInsts_v[Dinst_idx]<<"\t";
		}
		myfile<<"\n";
	}
	myfile.close();
}

void saveTracesDInstsPulledAsString(std::vector<Trace> &traces, std::string file_dir)
{
	ofstream myfile;
	myfile.open(file_dir);
	if(myfile.is_open() == false) return; //->

	int n_trc = traces.size();
	for(int trc_idx = 0; trc_idx <= n_trc-1; trc_idx++)
	{
		if(traces[trc_idx].isDInstCalculated() == false) continue; //|_>
		const std::vector<float>& DInsts_v = traces[trc_idx].getDInsts();

		int size = DInsts_v.size();
		for(int Dinst_idx = 0; Dinst_idx <= size-1; Dinst_idx++)
		{
			if(DInsts_v[Dinst_idx] >= 0) myfile<<DInsts_v[Dinst_idx]<<"\n"; //to avoid first points which are set to -1...
		}
	}
	myfile.close();
}

void saveTracesMSDsAsString(std::vector<Trace> &traces, std::string file_dir, bool isAppended)
{

	ofstream myfile;
	if(isAppended == true) myfile.open(file_dir, ios_base::app);
	else myfile.open(file_dir);
	if(myfile.is_open() == false) return; //->

	for(Trace& trace : traces)
	{
		if(trace.isMSDCalculated() == false) continue; //|_>

		myfile<<trace.getTraceIdx()<<'\t';//trace_idx
		myfile<<trace.getLength()<<'\t';//trace_length
		myfile<<trace.getMSDs().size()<<"\t";//trace_nbDisplacements

		int nb_plane = trace.getMSDs().size();
		for(int Dplane_idx = 0; Dplane_idx <= nb_plane-1; Dplane_idx++)
		{
			myfile<<trace.getMSDs()[Dplane_idx]<<"\t";
		}
		myfile<<"\n";
	}

	myfile.close();
}

void saveTracesLogDsHistogramAsString(std::vector<Trace> &traces,
								   std::string file_dir, int nb_intervals,
								   float min_value, float max_value)
{
	vector<pair<float, int> > data_binned;
	dataBiner(getLogDs(traces), data_binned, nb_intervals, min_value, max_value,0);

	ofstream myfile;
	myfile.open(file_dir);
	if(myfile.is_open() == false) return; //->

	int n = data_binned.size();
	for(int data_idx = 0; data_idx <= n-1; data_idx++)
	{
		myfile<<data_binned[data_idx].first;
		myfile<<'\t';
		myfile<<data_binned[data_idx].second;
		myfile<<'\n';
	}
	myfile.close();
}

void saveTracesLogDInstsHistogramAsString(std::vector<Trace> &traces,
								   std::string file_dir, int nb_intervals,
								   float min_value, float max_value)
{
	vector<pair<float, int> > data_binned;
	dataBiner(getLogDInsts(traces), data_binned, nb_intervals, min_value, max_value,0);

	ofstream myfile;
	myfile.open(file_dir);

	int n = data_binned.size();
	for(int data_idx = 0; data_idx <= n-1; data_idx++)
	{
		myfile<<data_binned[data_idx].first;
		myfile<<'\t';
		myfile<<data_binned[data_idx].second;
		myfile<<'\n';
	}
	myfile.close();
}

//void saveTracesLogDsEventHistogramAsString(std::vector<Trace> &traces,
//										   std::string file_dir, int nb_intervals,
//										   float min_value, float max_value)
//{
//	vector<pair<float, int> > data_binned;
//	dataBiner(getLogDs(traces), data_binned, nb_intervals, min_value, max_value,0);

//	ofstream myfile;
//	myfile.open(file_dir);

//	int n = data_binned.size();
//	for(int data_idx = 0; data_idx <= n-1; data_idx++)
//	{
//		myfile<<data_binned[data_idx].first;
//		myfile<<'\t';
//		myfile<<data_binned[data_idx].second;
//		myfile<<'\n';
//	}
//	myfile.close();
//}



void saveTracesLengthsHistogramAsString(std::vector<Trace> &traces,
										std::string file_dir,  int nb_intervals,
										float min_value, float max_value)
{
	vector<pair<float, int> > data_binned;
	dataBiner(getLengths(traces), data_binned, nb_intervals, min_value, max_value,0);

	auto lengths = getLengths(traces);

	ofstream myfile;
	myfile.open(file_dir);

	int n = data_binned.size();
	for(int data_idx = 0; data_idx <= n-1; data_idx++)
	{
		myfile<<data_binned[data_idx].first;
		myfile<<'\t';
		myfile<<data_binned[data_idx].second;
		myfile<<'\n';
	}
	myfile.close();
}

vector<FluoEvent> getEventsInPlane(int plane_idx, vector<Trace> &traces) //traces is a copy we can operate on it!
{
	vector<FluoEvent> events;

	auto trace = traces.rbegin();
	while(trace != traces.rend())
	{
		if(trace->isInsideRange(plane_idx))
		{
		   events.push_back(trace->getFluoEvent(plane_idx - trace->getFirstEvent().plane));
		}
		trace++;
	}
	return events;
}

vector<glm::vec2> getEventCoordsInPlane(int plane_idx, vector<Trace> &traces)
{
	vector<vec2> eventCoords_v;

	auto trace = traces.rbegin();
	while(trace != traces.rend())
	{
		if(trace->isInsideRange(plane_idx))
		{
			FluoEvent temp_event =  trace->getFluoEvent(plane_idx - trace->getFirstEvent().plane);
			eventCoords_v.push_back(vec2(temp_event.x, temp_event.y));
		}
		trace++;
	}
	return eventCoords_v;
}

void getTracesCoordsInPlane(int plane_idx, std::vector<Trace> &traces, std::vector<glm::vec2> &r_v, std::vector<glm::vec4> &color_v)
{
	r_v.clear();
	color_v.clear();

	for(Trace& trc : traces)
	{
		if(trc.getColorMode() == Trace::UNIQUE_COLOR_MODE ||
		   trc.getColorMode() == Trace::EXTERN_COLOR_MODE)
		{
			int nb_event = trc.getLength();
			if(trc.isInsideRange(plane_idx) && nb_event >= 2)
			{
				for(int event_idx = 1; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc.getFluoEvent(event_idx).x, trc.getFluoEvent(event_idx).y));
					color_v.push_back(trc.getColor());

					r_v.push_back(vec2(trc.getFluoEvent(event_idx-1).x, trc.getFluoEvent(event_idx-1).y));
					color_v.push_back(trc.getColor());
				}
			}
		}
		else if(trc.getColorMode() == Trace::PER_EVENT_EXTERN_COLOR_MODE)
		{
			int nb_event = trc.getLength();
			if(trc.isInsideRange(plane_idx) && nb_event >= 2)
			{
				for(int event_idx = 1; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc.getFluoEvent(event_idx).x, trc.getFluoEvent(event_idx).y));
					r_v.push_back(vec2(trc.getFluoEvent(event_idx-1).x, trc.getFluoEvent(event_idx-1).y));
					color_v.push_back(trc.getColors()[event_idx]);
					color_v.push_back(trc.getColors()[event_idx]);
				}
			}
		}


	}
	return ;
}

void getEventCoordsInPlane(int plane_idx, vector<Trace> &traces, vector<glm::vec2> &r_v, vector<glm::vec4> &color_v)
{
	r_v.clear();
	color_v.clear();

	for(Trace& trc : traces)
	{
		if(trc.isInsideRange(plane_idx))
		{
			if(trc.getColorMode() == Trace::UNIQUE_COLOR_MODE ||
			   trc.getColorMode() == Trace::EXTERN_COLOR_MODE)
			{

				FluoEvent temp_event =  trc.getFluoEvent(plane_idx - trc.getFirstEvent().plane);
				r_v.push_back(vec2(temp_event.x, temp_event.y));
				color_v.push_back(trc.getColor());
			}

			else if(trc.getColorMode() == Trace::PER_EVENT_EXTERN_COLOR_MODE)
			{
				FluoEvent temp_event =  trc.getFluoEvent(plane_idx - trc.getFirstEvent().plane);
				r_v.push_back(vec2(temp_event.x, temp_event.y));
				color_v.push_back(trc.getColors()[plane_idx - trc.getFirstEvent().plane]);
			}
		}
	}
	return ;
}


int getMaxPlane(vector<Trace>& traces)
{
	if(traces.size() == 0) return 0;

	int max_plane = INT_MIN;

	auto trc = traces.begin();
	while(trc != traces.end())
	{
		if(trc->getLastEvent().plane > max_plane)
		{
			max_plane = trc->getLastEvent().plane;
		}
		trc++;
	}

	return max_plane;
}

int getMinPlane(vector<Trace>& traces)
{
	if(traces.size() == 0) return 0;

	int min_plane = INT_MAX;

	auto trc = traces.begin();
	while(trc != traces.end())
	{
		if(trc->getLastEvent().plane < min_plane)
		{
			min_plane = trc->getFirstEvent().plane;
		}
		trc++;
	}

	return min_plane;
}


int getMinLength(std::vector<Trace>& traces)
{
	int min_length = std::numeric_limits<int>::max();

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->getLength() <= min_length) min_length = trace->getLength();
		trace++;
	}

	return min_length;

}

int getMaxLength(std::vector<Trace>& traces)
{
	int max_length = std::numeric_limits<int>::min();

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->getLength() >= max_length) max_length = trace->getLength();
		trace++;
	}

	return max_length;
}


void getTracesCoords(vector<Trace> &traces, vector<glm::vec2> &r_v, vector<glm::vec4> &color_v)
{
	r_v.clear();
	color_v.clear();

	auto trc = traces.begin();
	while(trc != traces.end())
	{
		int nb_event = trc->getLength();
		if(nb_event >= 2)
		{
			if(trc->getColorMode() == Trace::UNIQUE_COLOR_MODE ||
			   trc->getColorMode() == Trace::EXTERN_COLOR_MODE)
			{
				for(int event_idx = 1; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc->getFluoEvent(event_idx).x, trc->getFluoEvent(event_idx).y));
					color_v.push_back(trc->getColor());

					r_v.push_back(vec2(trc->getFluoEvent(event_idx-1).x, trc->getFluoEvent(event_idx-1).y));
					color_v.push_back(trc->getColor());
				}
			}
			else if(trc->getColorMode() == Trace::PER_EVENT_EXTERN_COLOR_MODE)
			{
				for(int event_idx = 1; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc->getFluoEvent(event_idx).x, trc->getFluoEvent(event_idx).y));
					r_v.push_back(vec2(trc->getFluoEvent(event_idx-1).x, trc->getFluoEvent(event_idx-1).y));
					color_v.push_back(trc->getColors()[event_idx]);
					color_v.push_back(trc->getColors()[event_idx]);
				}


			}
		}
		trc++;
	}
}

void getEventsCoords(vector<Trace> &traces, vector<glm::vec2> &r_v, vector<glm::vec4> &color_v)
{
	r_v.clear();
	color_v.clear();

	auto trc = traces.begin();
	while(trc != traces.end())
	{
		int nb_event = trc->getLength();
		if(nb_event >= 2)
		{
			if(trc->getColorMode() == Trace::UNIQUE_COLOR_MODE ||
			   trc->getColorMode() == Trace::EXTERN_COLOR_MODE)
			{
				for(int event_idx = 0; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc->getFluoEvent(event_idx).x, trc->getFluoEvent(event_idx).y));
					color_v.push_back(trc->getColor());
				}
			}
			else if(trc->getColorMode() == Trace::PER_EVENT_EXTERN_COLOR_MODE)
			{
				for(int event_idx = 1; event_idx <= nb_event-1; event_idx++)
				{
					r_v.push_back(vec2(trc->getFluoEvent(event_idx).x, trc->getFluoEvent(event_idx).y));
					color_v.push_back(trc->getColors()[event_idx]);
				}
			}
		}
		trc++;
	}
}


int getNbEvents(vector<Trace> &traces)
{
	int nb_events = 0;

	auto trc = traces.begin();
	while(trc != traces.end())
	{
		nb_events += trc->getLength();
		trc++;
	}

	return nb_events;
}

std::vector<float> getLengths(std::vector<Trace> &traces)
{
	vector<float> length_v;

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		length_v.push_back(trace->getLength());
		trace++;
	}

	return length_v;
}


void loadDAsString(std::vector<Trace>& traces, string file)
{
	string file_content;
	bool file_opened = getFileContent(file, file_content);

	if(file_opened == false) return;/*->*/

	vector<pair<int, float> > traceIdx_D_pairs;
		pair<int, float> temp_pair(-1,-1);
	int nb_traces = getNumberLines(file_content) - 3;

	if(nb_traces <=0) return;/*->*/

	string line;
		string word;
	int file_pos = 0;
		int line_pos = 0;
	getLine(file_content, line, file_pos, file_pos);
	getLine(file_content, line, file_pos, file_pos);
	getLine(file_content, line, file_pos, file_pos);
	//we start at line 4

	while(getLine(file_content, line, file_pos, file_pos))
	{
//		cout<<line<<"\n";
		line_pos = 0;
		getWord(line, word, line_pos, line_pos); //ROI

		getWord(line, word, line_pos, line_pos); //TRACE
			temp_pair.first = strtod(word.data(), 0);
		getWord(line, word, line_pos, line_pos); //D
			temp_pair.second = strtod(word.data(), 0);

		traceIdx_D_pairs.push_back(temp_pair);
	}

	if(traceIdx_D_pairs.back().first > traces.size()) return;/*->*/

	auto traceIdx_D_pair = traceIdx_D_pairs.begin();
	while(traceIdx_D_pair != traceIdx_D_pairs.end())
	{
		traces[traceIdx_D_pair->first-1].setD(traceIdx_D_pair->second);
		traceIdx_D_pair++;
	}


	return;
}

vector<pair<int,float> > getIdxD_pairs(vector<Trace> &traces)
{
	vector<pair<int,float> > trcIdx_D_pairs;
		pair<int,float> temp_pair(-1,-1);

	int trc_idx = 1;
	auto trace = traces.begin();
	while(trace != traces.end())
	{
		temp_pair.first = trc_idx;
		temp_pair.second = trace->getD();

		trcIdx_D_pairs.push_back(temp_pair);

		trace++;
		trc_idx++;
	}
	return trcIdx_D_pairs;
}

vector<float>  getDs(vector<Trace> &traces)
{
	vector<float>  Ds;

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->isDCalculated())	Ds.push_back(trace->getD());
		trace++;
	}

	return Ds;
}

float getMinD(std::vector<Trace> &traces)
{
	float min_D = std::numeric_limits<float>::max();

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->getD() <= min_D) min_D = trace->getD();
		trace++;
	}

	return min_D;
}

float getMaxD(std::vector<Trace> &traces)
{
	float max_D = std::numeric_limits<float>::min();

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->getD() >= max_D) max_D = trace->getD();
		trace++;
	}

	return max_D;
}


vector<float>  getLogDs(vector<Trace> &traces)
{
	vector<float>  LogDs;

	auto trace = traces.begin();
	while(trace != traces.end())
	{
		if(trace->isDCalculated())   LogDs.push_back(log10(trace->getD()));
		trace++;
	}

	return LogDs;
}

vector<float>  getLogDInsts(vector<Trace> &traces)
{
	vector<float>  LogDInsts;

	auto trace = traces.begin();
	for(Trace& trace : traces)
	{
		if(trace.isDInstCalculated() == false) continue;//|_>

		const std::vector<float>& DInsts_v = trace.getDInsts();

		int size = DInsts_v.size();
		for(int Dinst_idx = 0; Dinst_idx <= size-1; Dinst_idx++)
		{
			if(DInsts_v[Dinst_idx] >= 0) LogDInsts.push_back(log10(DInsts_v[Dinst_idx]));
		}
	}

	return LogDInsts;
}


//vector<float> computeMSD(Trace& trace, int max_plane)
//{
//	std::vector<float> MSD_v;

//	float MSD = 0;
//	int trc_length = trace.getLength();
//	int Dplane_idx = 1;
//	MSD_v.push_back(0);

//	while(Dplane_idx <= trc_length-1 && Dplane_idx != max_plane)
//	{
//		MSD = 0;
//		for(int sum_plane_idx = 0; sum_plane_idx<= trc_length-1-Dplane_idx; sum_plane_idx++)
//		{
//			vec2 dr = vec2(trace.getFluoEvent(sum_plane_idx + Dplane_idx).x,
//						   trace.getFluoEvent(sum_plane_idx + Dplane_idx).y) -
//					  vec2(trace.getFluoEvent(sum_plane_idx).x,
//						   trace.getFluoEvent(sum_plane_idx).y);
//			MSD += length(dr);
//		}
//		MSD /= trc_length-1-Dplane_idx;
//		MSD_v.push_back(MSD);

//		Dplane_idx++;
//	}
//	return MSD_v;
//}

void computeMSDs(vector<Trace>& traces, int max_DPlane)
{
	int nb_trc = traces.size();
	int trc_idx = 0;
	while(trc_idx != nb_trc)
	{
		traces[trc_idx].computeMSD(max_DPlane);
		trc_idx++;
	}
}


void computeDs(std::vector<Trace>& traces, float pxlSize_in_um, float Dplane_in_seconds, int firstMSDPoint_idx, int lastMSDPoint_idx)
{
	int nb_trc = traces.size();
	int trc_idx = 0;
	while(trc_idx != nb_trc)
	{
		traces[trc_idx].computeD(pxlSize_in_um, Dplane_in_seconds, firstMSDPoint_idx, lastMSDPoint_idx);
		trc_idx++;
	}
}

void computeDInst(std::vector<Trace>& traces, float pxlSize_in_um, float Dplane_in_seconds, uint nbPoints_before, uint nbPoints_after)
{
	int nb_trc = traces.size();
	int trc_idx = 0;
	while(trc_idx != nb_trc)
	{
		traces[trc_idx].computeDInst(pxlSize_in_um, Dplane_in_seconds, nbPoints_before, nbPoints_after);
		trc_idx++;
	}
}


void saveTracesProperties(std::vector<Trace>& traces, std::string file_directory)
{
	ofstream myfile;
	myfile.open(file_directory);

	int trc_idx = 0;
	for(Trace& trace : traces)
	{
		if(trace.isDCalculated() == true && trace.getMSDs().size()>=2)
		{
			myfile<<trc_idx<<'\t'<<trace.getLength()<<'\t'<<trace.getD()<<'\t'<<trace.getMSD0Fit()<<"\n";
		}
		trc_idx++;
	}
		myfile.close();
}

void analyseTracesCrossings(std::vector<Trace>& traces, const Region& rgn,
							vector<float>& nbEventsBeforeLeaving, vector<float>& nbEventsBeforeEntering)
{
	for(Trace& trc : traces)
	{
		if(trc.getLength() <= 0) continue; //> //to avoid to consider empty traces...

		bool isInside = rgn.isInside(vec2(trc.m_fluo_events[0].x, trc.m_fluo_events[0].y));
		float begEvent_pos = 0;
		float currentEvent_pos = 0;
		for(const FluoEvent& fluo_ev : trc.m_fluo_events)
		{
			bool temp_isInside = rgn.isInside(vec2(fluo_ev.x, fluo_ev.y));
            if(temp_isInside != isInside) //we entered or we left...
			{
                if(begEvent_pos != 0) //we don't record the first crossing
				{
                    if(isInside == 0) //we have entered
					{
						nbEventsBeforeEntering.push_back(currentEvent_pos - begEvent_pos -1);
					}
                    else //we left
					{
						nbEventsBeforeLeaving.push_back(currentEvent_pos - begEvent_pos -1);
					}
				}

				isInside = temp_isInside;
				begEvent_pos = currentEvent_pos;
			}
			currentEvent_pos++;
		}
	}
}



