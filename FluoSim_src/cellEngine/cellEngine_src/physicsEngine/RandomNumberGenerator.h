
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

#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H
#include "random"

#include "cellEngine_library_global.h"

class CELLENGINE_LIBRARYSHARED_EXPORT RandomNumberGenerator
{
public :

    RandomNumberGenerator();

	//poisson
	void setPoissonMean(float mean);
	float getPoissonMean();
	float poissonRandomNumber();

	//gaussian
	void setGaussianMean(float mean);
	void setGaussianSigma(float sigma);
	float gaussianRandomNumber(bool pre_calc);
	void generatePreCalcRandomNumbers(uint size);

	//uniform
	void setUniformBound(float lower_bound, float upper_bound);
	float getUniformUpperBound();
	float getUniformLowerBound();
	float uniformRandomNumber();



private :

	std::default_random_engine m_defaultGenerator;
	std::poisson_distribution<int> m_poissonGenerator;
	std::normal_distribution<float> m_gaussianGenerator;
	std::uniform_real_distribution<float> m_uniformGenerator;

	int m_next_idx;
	std::vector<float> m_gaussianRandomNumbers_v;


};




#endif // RANDOMNUMBERGENERATOR_H
