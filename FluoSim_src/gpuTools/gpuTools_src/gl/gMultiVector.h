
/*
 *  FluoSim (simulation program of live cell fluorescence microscopy experiments)
 *  Copyright (C) 2020 Matthieu Lagardère
 *
 *	FluoSim is free software: you can redistribute it and/or modify
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

#ifndef GMULTIVECTOR_H
#define GMULTIVECTOR_H

#include "gpuTools_library_global.h"

#include "gVector.h"

#include "stdlib.h"
#include "vector"
#include "list"
#include "iostream"
#include "memory"

#include "glm.hpp"


/*******************************
 *                             *
 *        class : gSubVector
 *                             *
 * *****************************/

namespace gstd
{


template<typename T> class gMultiVector;

struct GPUTOOLS_LIBRARYSHARED_EXPORT gSubVectorParam
{
	unsigned int m_id;
	unsigned int m_begin; //[m_begin; m_end[
	unsigned int m_end; //[m_begin; m_end[
	unsigned int m_size; // m_size = m_end - m_begin
};


template<typename T>
class GPUTOOLS_LIBRARYSHARED_EXPORT gSubVector
{
	friend class gMultiVector<T>; //so that gMultivector can create the handler ie : the vector

private :

	gSubVector();

public :

	gSubVector(gSubVector& subvector);
	~gSubVector();

	void insert(unsigned int pos, T value);
	void insert(unsigned int pos, std::vector<T>& values_v);

	void push_back(T value);
	void pop_back();

	void erase(unsigned int pos);
	void erase(unsigned int pos, unsigned int nb);

	void clear();

	unsigned int size();

	gVectorElement<T> operator[](unsigned int idx);

	void param();
	gSubVectorParam* getParam();

	unsigned int getOffsetInBytes();

	void bindSubVector(bool binding);
	int getComponentOffset(int component_idx);
	G_TYPE getComponentType(int component_idx);

private :

	gMultiVector<T>* m_multiVector;
	gSubVectorParam* m_param;
	bool m_isAttributed;
	bool m_isAnInternalCopy;

};

template<typename T>
gSubVector<T>::gSubVector()
{
	m_isAttributed = false;
	m_isAnInternalCopy = true;
}

template<typename T>
gSubVector<T>::gSubVector(gSubVector& subvector)
{
	//on object
	m_param = subvector.m_param;
	m_multiVector = subvector.m_multiVector;
	m_isAnInternalCopy = false;

	//on internal copy
	subvector.m_isAttributed = true; //if object has been copied,
//	isAttributed = 1;
}


template<typename T>
gSubVector<T>::~gSubVector()
{
	if(m_isAnInternalCopy == false)
	{
		m_multiVector->deleteVector(m_param->m_id);
	}
}

template<typename T>
void gSubVector<T>::insert(unsigned int pos, T value)
{
	m_multiVector->insert(m_param->m_id, pos, value);
}

template<typename T>
void gSubVector<T>::insert(unsigned int pos, std::vector<T>& values_v)
{
	m_multiVector->insert(m_param->m_id, pos, values_v);

}

template<typename T>
void gSubVector<T>::push_back(T value)
{
	m_multiVector->push_back(m_param->m_id, value);
}

template<typename T>
void gSubVector<T>::pop_back()
{
	m_multiVector->pop_back(m_param->m_id);
}

template<typename T>
void gSubVector<T>::erase(unsigned int pos)
{
	m_multiVector->erase(m_param->m_id, pos);
}

template<typename T>
void gSubVector<T>::erase(unsigned int pos, unsigned int nb)
{
	m_multiVector->erase(m_param->m_id, pos, nb);
}

template<typename T>
void gSubVector<T>::clear()
{
	m_multiVector->clear(m_param->m_id);
}

template<typename T>
unsigned int gSubVector<T>::size()
{
	return m_param->m_size;
}

template<typename T>
gVectorElement<T> gSubVector<T>::operator[](unsigned int idx)
{
	return m_multiVector->m_data_v[m_param->m_begin +idx];
}


template<typename T>
void gSubVector<T>::param()
{
	std::cout<<"****************\n";
	std::cout<<"*****Param******\n";
	std::cout<<"****************\n";
	std::cout<<"m_id = "<<m_param->m_id<<"\n";
	std::cout<<"m_begin = "<<m_param->m_begin<<"\n";
	std::cout<<"m_end = "<<m_param->m_end<<"\n";
	std::cout<<"m_size = "<<m_param->m_size<<"\n";
	std::cout<<"isAttributed = "<<m_isAttributed<<"\n";
	std::cout<<"****************\n";

}

template<typename T>
gSubVectorParam* gSubVector<T>::getParam()
{
	return m_param;
}


template<typename T>
unsigned int gSubVector<T>::getOffsetInBytes()
{
	return m_param->m_begin*sizeof(T);
}


template<typename T>
void gSubVector<T>::bindSubVector(bool binding)
{
	m_multiVector->bindMultiVector(binding);
}


template<typename T>
int gSubVector<T>::getComponentOffset(int component_idx)
{
	return m_multiVector->getComponentOffset(component_idx);
}


template<typename T>
G_TYPE gSubVector<T>::getComponentType(int component_idx)
{
	return m_multiVector->getComponentType(component_idx);
}



/*******************************
 *
 *     class : gMultiVector
 *
 * *****************************/



template<typename T>
class GPUTOOLS_LIBRARYSHARED_EXPORT gMultiVector
{
	friend class gSubVector<T>; //so that gSubVector can access its data in multiVector

public :

//	struct vectorHandle
//	{
//		unsigned int m_id;
//		unsigned int m_begin; //[m_begin; m_end[
//		unsigned int m_end; //[m_begin; m_end[
//		unsigned int m_size; // m_size = m_end - m_begin
//	};

	gMultiVector(bool isInterleaved = false, vector<G_TYPE> components_types_v = {});

	gSubVector<T>& addSubVector();
	void deleteVector(unsigned int vector_id);

	void insert(unsigned int vector_id, unsigned int pos, T value);
	void insert(unsigned int vector_id, unsigned int pos, std::vector<T>& values_v);

	void push_back(unsigned int vector_id, T value);
	void pop_back(unsigned int vector_id);

	void erase(unsigned int vector_id, unsigned int pos);
	void erase(unsigned int vector_id, unsigned int pos, unsigned int nb);

	void clear(unsigned int vector_id);
	void clear();

//	T& operator[] (unsigned int vector_hld);

	typename std::vector<T>::iterator begin();
	typename std::vector<T>::iterator end();
	gVector<T> getData();
	void coutData();

	void bindMultiVector(bool binding);

	int getComponentOffset(int component_idx);
	G_TYPE getComponentType(int component_idx);


private :

	std::vector<gSubVectorParam *> m_vector_v;
	std::list<gSubVector<T>*> m_vector_copy_v;

	gVector<T> m_data_v;
//	gVector<gSubVectorParam> m_vector_v;
	unsigned int next_id;
};


template<typename T>
gMultiVector<T>::gMultiVector(bool isInterleaved, vector<G_TYPE> components_types_v) : m_data_v(isInterleaved,
																							components_types_v)
{
	next_id = 0;
}


template<typename T>
gSubVector<T>& gMultiVector<T>::addSubVector()
{
	if(m_vector_copy_v.size() != 0 && m_vector_copy_v.back()->m_isAttributed == false)
	{
		std::cout<<m_vector_copy_v.back()->m_param->m_id<<"\n";
		deleteVector(m_vector_copy_v.back()->m_param->m_id);

	}


		gSubVectorParam* param = new gSubVectorParam{next_id,
													   m_data_v.size(),
													   m_data_v.size(),
													   0};

		gSubVector<T>* vector;
			vector = new gSubVector<T>;
			vector->m_param = param;
			vector->m_multiVector = this;
		m_vector_copy_v.push_back(vector);


	m_vector_v.push_back(param);
	next_id++;
	return *vector;
}


template<typename T>
void gMultiVector<T>::deleteVector(unsigned int vector_id)
{
	auto param = m_vector_v.begin();
	while(param != m_vector_v.end() && (*param)->m_id != vector_id)
	{
		param++;
	}

	auto vector_cpy = m_vector_copy_v.begin();
	while(vector_cpy != m_vector_copy_v.end() && (**vector_cpy).m_param->m_id != vector_id)
	{
		vector_cpy++;
	}

	if(param == m_vector_v.end()) return;
	else
	{
		clear((*param)->m_id);
		delete *param;
		m_vector_v.erase(param);
		delete *vector_cpy;
		m_vector_copy_v.erase(vector_cpy);

//		std::cout<<m_vector_v.size();
	}
}


template<typename T>
void gMultiVector<T>::insert(unsigned int vector_id, unsigned int offset, T value)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_begin + offset > (*vector)->m_end) return;
	else
	{
		m_data_v.insert((*vector)->m_begin + offset, value);

		(*vector)->m_end++;
		(*vector)->m_size++;

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin++;
				(*vector2)->m_end++;
			}
			vector2++;
		}
	}
}


template<typename T>
void gMultiVector<T>::insert(unsigned int vector_id, unsigned int offset, std::vector<T>& values_v)
{


	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}

	if(vector == m_vector_v.end() || (*vector)->m_begin + offset > (*vector)->m_end) return;
	else
	{
		m_data_v.insert((*vector)->m_begin + offset, values_v);
		(*vector)->m_end += values_v.size();
		(*vector)->m_size += values_v.size();

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin += values_v.size();
				(*vector2)->m_end += values_v.size();
			}
			vector2++;
		}
	}
}


template<typename T>
void gMultiVector<T>::push_back(unsigned int vector_id, T value)
{
//	if(m_vector_copy_v.size() !=0 && (m_vector_copy_v.back()->isAttributed) == false)
//	{
//		std::cout<<m_vector_copy_v.back()->m_param->m_id<<"\n";
//		deleteVector(m_vector_copy_v.back()->m_param->m_id); //we can enter m_param directly because of friendship
//	}


	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end()) return;
	else
	{
		//on ajoute l'élément à la fin (ie push_back)
		m_data_v.insert((*vector)->m_begin + (*vector)->m_size, value);
		(*vector)->m_end++;
		(*vector)->m_size++;

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin++;
				(*vector2)->m_end++;
			}
			vector2++;
		}
	}
}

template<typename T>
void gMultiVector<T>::pop_back(unsigned int vector_id)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_size == 0) return;
	else
	{
		//on ajoute l'élément à la fin (ie push_back)
		auto data = (*vector)->m_begin + (*vector)->m_size -1;
		m_data_v.erase(data);

		(*vector)->m_end--;
		(*vector)->m_size--;

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin--;
				(*vector2)->m_end--;
			}
			vector2++;
		}
	}
}


template<typename T>
void gMultiVector<T>::erase(unsigned int vector_id, unsigned int pos)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_begin + pos >= (*vector)->m_end) return;
	else
	{
		//on ajoute l'élément à la fin (ie push_back)
		auto data = (*vector)->m_begin + pos;

		m_data_v.erase(data);
		(*vector)->m_end--;
		(*vector)->m_size--;

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin--;
				(*vector2)->m_end--;
			}
			vector2++;
		}
	}
}

template<typename T>
void gMultiVector<T>::erase(unsigned int vector_id, unsigned int pos, unsigned int nb)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	int nb_toDelete = (*vector)->m_size - pos;
	if(nb_toDelete >= nb) nb_toDelete = nb;

	if(vector == m_vector_v.end() || nb_toDelete <= 0) return;
	else
	{
		//on ajoute l'élément à la fin (ie push_back)
		auto data = (*vector)->m_begin + pos;

		m_data_v.erase(data, nb_toDelete);
		(*vector)->m_end -= nb_toDelete;
		(*vector)->m_size -= nb_toDelete;

		auto vector2 = m_vector_v.begin();
		while(vector2!= m_vector_v.end())
		{
			if((*vector2)->m_id > (*vector)->m_id)
			{
				(*vector2)->m_begin -= nb_toDelete;
				(*vector2)->m_end -= nb_toDelete;
			}
			vector2++;
		}
	}
}

template<typename T>
void gMultiVector<T>::clear(unsigned int vector_id)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end()) return;
	else
	{
		erase((*vector)->m_id, 0, (*vector)->m_size);
	}
}

template<typename T>
void gMultiVector<T>::clear()
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end())
	{
		erase((*vector)->m_id, 0, (*vector)->m_size);
		vector++;
	}
}

template<typename T>
gVector<T> gMultiVector<T>::getData()
{
	return m_data_v;
}



template<typename T>
void gMultiVector<T>::coutData()
{
	std::cout<<"****************\n";
	std::cout<<"*****Data******\n";
	std::cout<<"****************\n";

	auto data = m_data_v.begin();
	while(data != m_data_v.end())
	{
		std::cout<<*data<<"_";
		data++;
	}
}

template<typename T>
void gMultiVector<T>::bindMultiVector(bool binding)
{
	m_data_v.bindVector(binding);
}




template<typename T>
int gMultiVector<T>::getComponentOffset(int component_idx)
{
	return m_data_v.getComponentOffset(component_idx);
}


template<typename T>
G_TYPE gMultiVector<T>::getComponentType(int component_idx)
{
	return m_data_v.getComponentType(component_idx);
}


}

/* by making the normal constructor private and using friendship
 * we only let gMultiVector the ability to create subVector
 * avoiding the risk to have multiple handlers for the same part of
 * the data in the multiVector.
 * This is really the subVector which owns the data. It is the unique
 * owner of the data and is responsible of its destruction
 *
 * Indeed subVector can only be created through the function addSubVector
 * then using the copy constructor (which is public) we can get the object
 */

#endif // GMULTIVECTOR_H
