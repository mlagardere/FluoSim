
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


#include "otherFunctions.h"

using namespace std;
using namespace glm;

void dataBiner(std::vector<float> data_in, vector<vec2>& data_out,
			   int n_subdivision, double x_min_imposed, double x_max_imposed, int type)
{

	int n_data  = data_in.size();

	vec2 temp_data;

	double min_x= +100000000.0;
	double max_x= -100000000.0;

	if(x_min_imposed==-1 && x_max_imposed==-1)
	{

		for(int i=0; i<= n_data-1; i++)
		{
			if(data_in[i]<min_x)
			{
				min_x = data_in[i];
			}
			if(data_in[i]>max_x)
			{
				max_x = data_in[i];
			}
		}
	}

	else
	{
		max_x = x_max_imposed;
		min_x = x_min_imposed;
	}

	switch(type)
	{
	case 0:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/n_subdivision;
		int count;

		for(int i=0; i<=n_subdivision-1; i++)
		{
			temp_data.x = min_x + i* delta_x;
			count=0;

			for(int j=0; j<=n_data-1; j++)
			{
				if(data_in[j] >= temp_data.x && data_in[j] < temp_data.x + delta_x) count++;
			}
			temp_data.y = count;
			data_out.push_back(temp_data);
		}
	}
	break;

	case 1:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/n_subdivision;

		int count;

		for(int i=1; i<=n_subdivision; i++)
		{
			temp_data.x = min_x + i* delta_x;
			count=0;

			for(int j=0; j<=n_data-1; j++)
			{
				if(data_in[j] < temp_data.x && data_in[j] >= temp_data.x - delta_x) count++;
			}
			temp_data.y = count;
			data_out.push_back(temp_data);

		}
	break;
	}

	case 2:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/(2*n_subdivision);

		int count;

		for(int i=0; i<=n_subdivision-1; i++)
		{
			temp_data.x = min_x + 2*i* delta_x;
			count=0;

			if(i==0)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] < temp_data.x +delta_x && data_in[j] >= temp_data.x) count++;
				}
				temp_data.y = count;
				data_out.push_back(temp_data);
			}

			if(i==n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] <= temp_data.x && data_in[j] >= temp_data.x-delta_x) count++;
				}
				temp_data.y = count;
				data_out.push_back(temp_data);
			}

			if(i!=0 && i!= n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] >= temp_data.x-delta_x && data_in[j] < temp_data.x+delta_x) count++;
				}
				temp_data.y = count;
				data_out.push_back(temp_data);
			}
		}
	break;
	}

	}
}


void dataBiner(double *data_in, int n_data, double* *data_out_x, double* *data_out_y,
			   int n_subdivision, double x_min_imposed, double x_max_imposed, int type)
{


	double min_x= +100000000.0;
	double max_x= -100000000.0;

	if(x_min_imposed==-1 && x_max_imposed==-1)
	{

		for(int i=0; i<= n_data-1; i++)
		{
			if(data_in[i]<min_x)
			{
				min_x = data_in[i];
			}
			if(data_in[i]>max_x)
			{
				max_x = data_in[i];
			}
		}
	}

	else
	{
		max_x = x_max_imposed;
		min_x = x_min_imposed;
	}

	switch(type)
	{
	case 0:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/n_subdivision;


		*data_out_x = new double[n_subdivision];
		*data_out_y = new double[n_subdivision];
		int count;

		for(int i=0; i<=n_subdivision-1; i++)
		{
			(*data_out_x)[i] = min_x + i* delta_x;
			count=0;

			for(int j=0; j<=n_data-1; j++)
			{
				if(data_in[j] >= (*data_out_x)[i] && data_in[j] < (*data_out_x)[i]+ delta_x) count++;
			}
			(*data_out_y)[i] = count;

		}
	}
	break;

	case 1:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/n_subdivision;

		*data_out_x = new double[n_subdivision];
		*data_out_y = new double[n_subdivision];
		int count;

		for(int i=1; i<=n_subdivision; i++)
		{
			(*data_out_x)[i] = min_x + i* delta_x;
			count=0;

			for(int j=0; j<=n_data-1; j++)
			{
				if(data_in[j] < (*data_out_x)[i] && data_in[j] >= (*data_out_x)[i]-delta_x) count++;
			}
			(*data_out_y)[i] = count;

		}
	break;
	}

	case 2:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/(2*n_subdivision);

		*data_out_x = new double[n_subdivision];
		*data_out_y = new double[n_subdivision];
		int count;

		for(int i=0; i<=n_subdivision-1; i++)
		{
			(*data_out_x)[i] = min_x + 2*i* delta_x;
			count=0;

			if(i==0)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] < (*data_out_x)[i]+delta_x && data_in[j] >= (*data_out_x)[i]) count++;
				}
				(*data_out_y)[i] = count;
			}

			if(i==n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] <= (*data_out_x)[i] && data_in[j] >= (*data_out_x)[i]-delta_x) count++;
				}
				(*data_out_y)[i] = count;
			}

			if(i!=0 && i!= n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] >= (*data_out_x)[i]-delta_x && data_in[j] < (*data_out_x)[i]+delta_x) count++;
				}
				(*data_out_y)[i] = count;
			}
		}
	break;
	}

	}
}

void dataBiner(vector<float> data_in, vector<pair<float,int> >& data_out,
			   int n_subdivision, float x_min_imposed, float x_max_imposed, int type)
{


	float min_x = std::numeric_limits<float>::lowest();
	float max_x = std::numeric_limits<float>::max();
	int n_data = data_in.size();

	if(x_min_imposed==-1 && x_max_imposed==-1)
	{

		for(int i=0; i<= n_data-1; i++)
		{
			if(data_in[i]<min_x)
			{
				min_x = data_in[i];
			}
			if(data_in[i]>max_x)
			{
				max_x = data_in[i];
			}
		}
	}

	else
	{
		max_x = x_max_imposed;
		min_x = x_min_imposed;
	}

	switch(type)
	{

		case 0:
		{
			int count;
			for(int i=0; i<=n_subdivision-1; i++)
			{
				data_out.push_back(pair<float,int>(0.0,0));
				data_out[i].first = min_x + i*(max_x - min_x)/n_subdivision;

				count=0;
				for(unsigned int j=0; j < n_data; j++)
				{
					if(data_in[j] >= data_out[i].first &&
					   data_in[j] < data_out[i].first + (max_x - min_x)/n_subdivision)
					{
						count++;
					}
				}
				data_out[i].second = count;
			}
		}
		break;

		case 1:
		{
			int count;
			for(unsigned int i=1; i <= n_subdivision; i++)
			{
				data_out.push_back(pair<float,int>(0.0,0));
				data_out.back().first = min_x + i*(max_x - min_x)/n_subdivision;

				count=0;
				for(unsigned int j=0; j < n_data; j++)
				{
					if(data_in[j] < data_out.back().first &&
					   data_in[j] >= data_out.back().first - (max_x - min_x)/n_subdivision)
					{
						count++;
					}
				}
				data_out.back().second = count;
			}
		break;
		}

	case 2:
	{

		double Delta_x = max_x - min_x;
		double delta_x = Delta_x/(2*n_subdivision);

		int count;

		for(int i=0; i<=n_subdivision-1; i++)
		{
			data_out.push_back(pair<float,int>(0.0,0));
				data_out[i].first = min_x + i* delta_x;
				count=0;

				if(i==0)
				{
					for(int j=0; j<=n_data-1; j++)
					{
						if(data_in[j] < data_out[i].first + delta_x && data_in[j] >= data_out[i].first) count++;
					}
					data_out[i].second = count;
			}

			if(i==n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] <= data_out[i].first && data_in[j] >= data_out[i].first-delta_x) count++;
				}
				data_out[i].second = count;
			}

			if(i!=0 && i!= n_subdivision-1)
			{
				for(int j=0; j<=n_data-1; j++)
				{
					if(data_in[j] >= data_out[i].first - delta_x && data_in[j] < data_out[i].first + delta_x) count++;
				}
				data_out[i].second = count;
			}
		}
	break;
	}

	}
}

int convertCharToInt(vector <char> numberChar)
{
	int number_int=0;
	int str_lgth = numberChar.size();
	int asciiValue, numericalValue;

	for(int str_index=0; str_index<=str_lgth-1; str_index++)
	{
		asciiValue = numberChar[str_lgth-1 - str_index];
		numericalValue = asciiValue - 48;
		number_int = number_int + numericalValue * std::pow(10.0, float(str_index));

	}
	return number_int;
}

int readNumber(char* file_str, int readIndex, int* endReadIndex, char end_char)
{
	int number;
//    int readIndex=m_current_index;
	vector <char> number_str;
	while(file_str[readIndex] != end_char
		  && file_str[readIndex] != ','
		  && file_str[readIndex] != '\0'
		  && file_str[readIndex] != '\n')
	{
		number_str.push_back(file_str[readIndex]);
		readIndex++;
	}
	number = convertCharToInt(number_str);

	if(file_str[readIndex] == ',')
	{
		*endReadIndex = *endReadIndex + number_str.size() + 2;
	}
	else
	{
		*endReadIndex = *endReadIndex + number_str.size() + 1;
	}

	return number;
}

float uniformDistributiion(float min_value, float max_value)
{
	float rand_0_1 = float(rand()%RAND_MAX)/(RAND_MAX-1); //r -> [0,1]
	return (max_value - min_value) * rand_0_1 + min_value;
}

int uniformDistributiion(int min_value, int max_value)
{
	float rand_0_1 = float(rand()%RAND_MAX)/(RAND_MAX-1); //r -> [0,1]
	return int((max_value - min_value) * rand_0_1) + min_value;
}


bool binomial_1(float p)
{
	if(p == 0) return 0; //->

	float threshold_p = 1000.0/RAND_MAX;
	float rand_0_MAXRAND;

	// will be used for very small probabilty
	if(p <= threshold_p)
	{
		bool output = 1;
		// p = t^(n-1+a) = t^(n) * t^(a)
		int n = log(p)/log(threshold_p);
		float a = log(p)/log(threshold_p) - n;

		int i = 0;
		while(output && i <= n-1)
		{
			rand_0_MAXRAND = float(rand());
			if(rand_0_MAXRAND > threshold_p*RAND_MAX)
			{
				output = 0;
			}
			i++;
		}

		if(output)
		{
			rand_0_MAXRAND = float(rand());
			if(rand_0_MAXRAND > pow(threshold_p,a)*RAND_MAX)
			{
				output = 0;
			}
		}
		return output;

	}
	else
	{
		rand_0_MAXRAND = float(rand());
		if(rand_0_MAXRAND <= p*RAND_MAX)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}



vec2 random_gaussian (float sigma)
{
	float random_x;
	float random_y;

	bool sup_to_one=1;
	float S_square;

	while(sup_to_one)
	{
		random_x= 2*float(rand())/RAND_MAX-1.0;
		random_y= 2*float(rand())/RAND_MAX-1.0;

		S_square =   random_x * random_x +
					 random_y * random_y;
		if(S_square<1) sup_to_one=0;
	}

	return vec2(sigma* random_x * std::sqrt(-2*log(S_square)/S_square),
				sigma* random_y * std::sqrt(-2*log(S_square)/S_square));

}


myGaussainRNG::myGaussainRNG()
{

}

void myGaussainRNG::generateRandomVectors(int n)
{
	int i=0;
	while(i!=n-1)
	{
		m_random_vector.push_back(random_gaussian(1));
		i++;
	}
}

vec2 myGaussainRNG::getRandomVector()
{
	return m_random_vector[uniformDistributiion(0, m_random_vector.size()-1)];
}


string floatToString(float my_float)
{
	ostringstream ss;
	ss << my_float;
	std::string s(ss.str());

	return s;
}

void* convertBGRA32toRGBA32(void* data_in_void, int size_inBytes)
{
	uint8* data_in_8 = (uint8*) data_in_void;
	uint8* data_out_8;
	data_out_8 = new uint8[size_inBytes];

	for(int byte_idx = 0; byte_idx <= size_inBytes-1; byte_idx++)
	{
		if(byte_idx % 4 == 0)
		{
			data_out_8[byte_idx] = data_in_8[byte_idx+2];
		}

		if(byte_idx % 4 == 1)
		{
			data_out_8[byte_idx] = data_in_8[byte_idx];
		}

		if(byte_idx % 4 == 2)
		{
			data_out_8[byte_idx] = data_in_8[byte_idx-2];
		}

		if(byte_idx % 4 == 3)
		{
			data_out_8[byte_idx] = data_in_8[byte_idx];
		}
	}
	return (void*) data_out_8;
}





