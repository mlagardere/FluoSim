
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

#ifndef MYCLOCK_H
#define MYCLOCK_H

#include "toolbox_library_global.h"
#include "windows.h"



class TOOLBOXSHARED_EXPORT myChrono
{	
public :

	myChrono();

	void startChrono();
		void startTour();
		float endTour();

	void setNbRecordedTours(int n);
	int getNbRecordedTours();


	float getCurrentTime() const;
	float getCurrentTimeInTour() const;

	float getTotalTimeInTours() const;
	float getAveragedTimeInTours() const;


private :

	void _computeTotalToursTime();

private :

	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_chronoStart_tick;
	LARGE_INTEGER m_tourStart_tick;
	LARGE_INTEGER m_tourEnd_tick;

	double m_lastTour_time;
	double m_totalTours_time;
		int m_tourIdx;
		int m_nbRecoredTours;
		std::vector<double> m_recoredTourTimes_v;



};




#endif // MYCLOCK_H
