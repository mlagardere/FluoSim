
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




#include "RandomNumberGenerator.h"

RandomNumberGenerator::RandomNumberGenerator() :

	m_poissonGenerator(1.0),
	m_gaussianGenerator(0.0,1.0),
	m_uniformGenerator(0.0,1.0)
{

}

void RandomNumberGenerator::setPoissonMean(float mean)
{
	if(m_poissonGenerator.mean() == mean) return; //->
	m_poissonGenerator = std::poisson_distribution<int>(mean);
}

float RandomNumberGenerator::getPoissonMean()
{
	return m_poissonGenerator.mean();
}

float RandomNumberGenerator::poissonRandomNumber()
{
	return m_poissonGenerator(m_defaultGenerator);
}


void RandomNumberGenerator::setGaussianMean(float mean)
{
	if(m_gaussianGenerator.mean() == mean) return; //->
	float sigma = m_gaussianGenerator.stddev();
	m_gaussianGenerator = std::normal_distribution<float>(mean, sigma);
}

void RandomNumberGenerator::setGaussianSigma(float sigma)
{
	if(m_gaussianGenerator.stddev() == sigma) return; //->
	float mean = m_gaussianGenerator.mean();
	m_gaussianGenerator = std::normal_distribution<float>(mean, sigma);
}

float RandomNumberGenerator::gaussianRandomNumber(bool pre_calc)
{
	if(pre_calc == false) return m_gaussianGenerator(m_defaultGenerator); //->

	int idx =  uniformRandomNumber()*m_gaussianRandomNumbers_v.size();
	return m_gaussianRandomNumbers_v[idx];
}

void RandomNumberGenerator::generatePreCalcRandomNumbers(uint size)
{
	m_next_idx = 0;
	m_gaussianRandomNumbers_v.clear();
	for(int rnb_idx = 0; rnb_idx <= size -1; rnb_idx++)
	{
		m_gaussianRandomNumbers_v.push_back(gaussianRandomNumber(false));
	}
}

void RandomNumberGenerator::setUniformBound(float lower_bound, float upper_bound)
{
	if(m_uniformGenerator.a() == lower_bound &&
	   m_uniformGenerator.b() == upper_bound) return; //->

	m_uniformGenerator = std::uniform_real_distribution<float>(lower_bound, upper_bound);
}

float RandomNumberGenerator::getUniformUpperBound()
{
	return m_uniformGenerator.b();

}

float RandomNumberGenerator::getUniformLowerBound()
{
	return m_uniformGenerator.a();

}

float RandomNumberGenerator::uniformRandomNumber()
{
	return m_uniformGenerator(m_defaultGenerator);
}
