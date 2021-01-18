
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

#include "myClock.h"


using namespace std;


myChrono::myChrono()
{
	QueryPerformanceFrequency(&m_frequency);
	setNbRecordedTours(10);

	startChrono(); //we start the chrono by default
}

void myChrono::startChrono()
{
	m_recoredTourTimes_v.clear();
	m_recoredTourTimes_v.assign(m_nbRecoredTours, 0.0);
	m_lastTour_time = 0;
	m_totalTours_time = 0;
	m_tourIdx = 0;

	QueryPerformanceCounter(&m_chronoStart_tick);
	startTour(); //we start a new tour by defaul...
}

void myChrono::startTour()
{
	QueryPerformanceCounter(&m_tourStart_tick);
}

float myChrono::endTour()
{
	QueryPerformanceCounter(&m_tourEnd_tick);
	m_lastTour_time = double((m_tourEnd_tick.QuadPart - m_tourStart_tick.QuadPart))/ m_frequency.QuadPart;

	m_totalTours_time -= m_recoredTourTimes_v[m_tourIdx];
	m_recoredTourTimes_v[m_tourIdx] = m_lastTour_time;
	m_totalTours_time += m_recoredTourTimes_v[m_tourIdx];

	m_tourIdx++;
	m_tourIdx %= m_nbRecoredTours; //to be in [0; nb_recordedTours-1]

	startTour(); //we start a new tour by defaul...

	return m_lastTour_time;
}


void myChrono::setNbRecordedTours(int n)
{
	m_tourIdx = 0;
	m_nbRecoredTours = n;
	m_recoredTourTimes_v.resize(n, 0.0);
	_computeTotalToursTime();
}

int myChrono::getNbRecordedTours()
{
	return m_nbRecoredTours;
}

float myChrono::getCurrentTime() const
{
	LARGE_INTEGER current_tick;
	QueryPerformanceCounter(&current_tick);

	return double((current_tick.QuadPart - m_chronoStart_tick.QuadPart))/ m_frequency.QuadPart;
}

float myChrono::getCurrentTimeInTour() const
{
	LARGE_INTEGER current_tick;
	QueryPerformanceCounter(&current_tick);

	return double((current_tick.QuadPart - m_tourStart_tick.QuadPart))/ m_frequency.QuadPart;
}

float myChrono::getTotalTimeInTours() const
{
	return m_totalTours_time;
}

float myChrono::getAveragedTimeInTours() const
{
	return m_totalTours_time/m_nbRecoredTours;
}

void myChrono::_computeTotalToursTime()
{
	m_totalTours_time = 0;
	for(const float& time : m_recoredTourTimes_v)
	{
		m_totalTours_time += time;
	}
}




