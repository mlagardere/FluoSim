
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

#ifndef MYMultiARRAY_H
#define MYMultiARRAY_H

#include "stdlib.h"
#include "vector"
#include "list"
#include "iostream"
#include "memory"

#include "glm.hpp"




/*******************************
 *
 *        class : mySubVector
 *
 * *****************************/


template<typename T> class myMultiVector;

struct mySubVectorParam
{
	unsigned int m_id;
	unsigned int m_begin; //[m_begin; m_end[
	unsigned int m_end; //[m_begin; m_end[
	unsigned int m_size; // m_size = m_end - m_begin
};



template<typename T>
class mySubVector	
{
	friend class myMultiVector<T>; //so that myMultivector can create the handler ie : the vector

private :

	mySubVector();

public :

	mySubVector(mySubVector& subvector);
	~mySubVector();

	void insert(unsigned int pos, T value);
	void insert(unsigned int pos, std::vector<T> values_v);

	void push_back(T value);
	void pop_back();

	void erase(unsigned int pos);
	void erase(unsigned int pos, unsigned int nb);

	void clear();

	unsigned int size() const;

	typename std::vector<T>::iterator begin() const;
	typename std::vector<T>::iterator end() const;

	T& operator[](unsigned int idx);  //called in non-const context : for modification
	T operator[](unsigned int idx) const; //called in const context

	void param();

	mySubVectorParam* getParam();


public :

	myMultiVector<T>* m_multiVector;
	mySubVectorParam* m_param;
	bool m_isAttributed;
	bool m_isAnInternalCopy;

};

template<typename T>
mySubVector<T>::mySubVector()
{
	m_isAttributed = false;
	m_isAnInternalCopy = true;
}

template<typename T>
mySubVector<T>::mySubVector(mySubVector& subvector)
{
	m_param = subvector.m_param;
	m_multiVector = subvector.m_multiVector;
	m_isAnInternalCopy = false;

	subvector.m_isAttributed = true; //if object has been copied,

//	isAttributed = 1;
}


template<typename T>
mySubVector<T>::~mySubVector()
{
	if(m_isAnInternalCopy == false)
	{
		m_multiVector->deleteVector(m_param->m_id);
	}
}

template<typename T>
void mySubVector<T>::insert(unsigned int pos, T value)
{
	m_multiVector->insert(m_param->m_id, pos, value);
}

template<typename T>
void mySubVector<T>::insert(unsigned int pos, std::vector<T> values_v)
{
	m_multiVector->insert(m_param->m_id, pos, values_v);

}

template<typename T>
void mySubVector<T>::push_back(T value)
{
	m_multiVector->push_back(m_param->m_id, value);
}

template<typename T>
void mySubVector<T>::pop_back()
{
	m_multiVector->pop_back(m_param->m_id);
}

template<typename T>
void mySubVector<T>::erase(unsigned int pos)
{
	m_multiVector->erase(m_param->m_id, pos);
}

template<typename T>
void mySubVector<T>::erase(unsigned int pos, unsigned int nb)
{
	m_multiVector->erase(m_param->m_id, pos, nb);
}

template<typename T>
void mySubVector<T>::clear()
{
	m_multiVector->clear(m_param->m_id);
}

template<typename T>
unsigned int mySubVector<T>::size() const
{
	return m_param->m_size;
}

template<typename T>
T& mySubVector<T>::operator[](unsigned int idx)
{
	T* ptr = m_multiVector->data() + m_param->m_begin + idx;
	return * ptr;
}

template<typename T>
T mySubVector<T>::operator[](unsigned int idx) const
{
	T* ptr = m_multiVector->data() + m_param->m_begin + idx;
	return * ptr;
}

template<typename T>
typename std::vector<T>::iterator mySubVector<T>::begin() const
{
	return m_multiVector->begin()+m_param->m_begin;
}

template<typename T>
typename std::vector<T>::iterator mySubVector<T>::end() const
{
	return m_multiVector->begin()+m_param->m_begin+m_param->m_size;
}

template<typename T>
void mySubVector<T>::param()
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
mySubVectorParam* mySubVector<T>::getParam()
{
	return m_param;
}



/*******************************
 *
 *     class : myMultiVector
 *
 * *****************************/



template<typename T>
class myMultiVector
{
public :

	myMultiVector();

	mySubVector<T>& addSubVector();
	void deleteVector(unsigned int vector_id);

	void insert(unsigned int vector_id, unsigned int pos, T value);
	void insert(unsigned int vector_id, unsigned int pos, std::vector<T> values_v);

	void push_back(unsigned int vector_id, T value);
	void pop_back(unsigned int vector_id);

	void erase(unsigned int vector_id, unsigned int pos);
	void erase(unsigned int vector_id, unsigned int pos, unsigned int nb);

	void clear(unsigned int vector_id);
	void clear();

//	T* operator[] (unsigned int vector_hld);
	mySubVector<T>& operator[] (unsigned int vector_hld);

	typename std::vector<T>::iterator begin();
	typename std::vector<T>::iterator end();
	std::vector<T> getData();
	void coutData();

	T* data();

public :

	std::vector<mySubVectorParam *> m_vector_v;
	std::list<mySubVector<T>*> m_vector_copy_v;

	std::vector<T> m_data_v;
	unsigned int next_id;
};


template<typename T>
myMultiVector<T>::myMultiVector()
{
	next_id = 0;
}


template<typename T>
mySubVector<T>& myMultiVector<T>::addSubVector()
{
	if(m_vector_copy_v.size() != 0 && m_vector_copy_v.back()->m_isAttributed == false)
	{
		std::cout<<m_vector_copy_v.back()->m_param->m_id<<"\n";
		deleteVector(m_vector_copy_v.back()->m_param->m_id);

	}
		mySubVectorParam* param = new mySubVectorParam{next_id,
													   m_data_v.size(),
													   m_data_v.size(),
													   0};
		mySubVector<T>* vector;
			vector = new mySubVector<T>;
			vector->m_param = param;
			vector->m_multiVector = this;
		m_vector_copy_v.push_back(vector);


	m_vector_v.push_back(param);
	next_id++;
	return *vector;
}


template<typename T>
void myMultiVector<T>::deleteVector(unsigned int vector_id)
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
void myMultiVector<T>::insert(unsigned int vector_id, unsigned int offset, T value)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_begin + offset > (*vector)->m_end) return;
	else
	{
        //we add the element
		auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + offset);

		m_data_v.insert(data, 1, value);
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
void myMultiVector<T>::insert(unsigned int vector_id, unsigned int offset, std::vector<T> values_v)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}

	if(vector == m_vector_v.end() || (*vector)->m_begin + offset > (*vector)->m_end) return;
	else
	{
        //we add the element
		auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + offset);

		m_data_v.insert(data, values_v.begin(), values_v.end());
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
void myMultiVector<T>::push_back(unsigned int vector_id, T value)
{
//	if(m_vector_copy_v.size() !=0 && (m_vector_copy_v.back()->isAttributed) == false)
//	{
//		std::cout<<m_vector_copy_v.back()->m_param->m_id<<"\n";
//		deleteVector(m_vector_copy_v.back()->m_param->m_id); //we can enter m_param directly because of the friendship
//	}


	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end()) return;
	else
    {
        //we add the element at the end (ie. push_back)
		auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + (*vector)->m_size);

		m_data_v.insert(data, 1, value);
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
void myMultiVector<T>::pop_back(unsigned int vector_id)
{
	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_size == 0) return;
	else
	{
        //we add the element at the end (ie. push_back)
        auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + (*vector)->m_size -1);

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
void myMultiVector<T>::erase(unsigned int vector_id, unsigned int pos)
{

	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
	{
		vector++;
	}
	if(vector == m_vector_v.end() || (*vector)->m_begin + pos >= (*vector)->m_end) return;
	else
	{
        //we add the element at the end (ie. push_back)
        auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + pos);

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
void myMultiVector<T>::erase(unsigned int vector_id, unsigned int pos, unsigned int nb)
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
        //we add the element at the end (ie. push_back)
        auto data = m_data_v.begin();
		std::advance(data, (*vector)->m_begin + pos);

		m_data_v.erase(data, data + nb_toDelete);
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
void myMultiVector<T>::clear(unsigned int vector_id)
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
void myMultiVector<T>::clear()
{

	auto vector = m_vector_v.begin();
	while(vector != m_vector_v.end())
	{
		erase((*vector)->m_id, 0, (*vector)->m_size);
		vector++;
	}
}

template<typename T>
std::vector<T> myMultiVector<T>::getData()
{
	return m_data_v;

}

template<typename T>
mySubVector<T>&  myMultiVector<T>::operator[] (unsigned int vector_id)
{
	auto subVector = m_vector_copy_v.begin();
	while(subVector != m_vector_copy_v.end() && (*subVector)->m_param->m_id != vector_id)
	{
		subVector++;
	}
	return **subVector;
}


//template<typename T>
//T* myMultiVector<T>::operator[] (unsigned int vector_id)
//{
//	auto vector = m_vector_v.begin();
//	while(vector != m_vector_v.end() && (*vector)->m_id != vector_id)
//	{
//		vector++;
//	}
//	if(vector == m_vector_v.end()) return nullptr;
//	else
//	{
//		T* ptr;
//		ptr = m_data_v.data() + (*vector)->m_begin;
//		return  ptr;
//	}
//}

template<typename T>
typename std::vector<T>::iterator myMultiVector<T>::begin()
{
	return m_data_v.begin();
}

template<typename T>
typename std::vector<T>::iterator myMultiVector<T>::end()
{
	return m_data_v.end();
}

template<typename T>
void myMultiVector<T>::coutData()
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
T* myMultiVector<T>::data()
{
	return m_data_v.data();
}




/* by making the normal constructor private and using friendship
 * we only let myMultiVector the ability to create subVector
 * avoiding the risk to have multiple handlers for the same part of
 * the data in the multiVector.
 * This is really the subVector which owns the data. It is the unique
 * owner of the data and is responsible of its destruction
 *
 * Indeed subVector can only be created through the function addSubVector
 * then using the copy constructor (which is public) we can get the object
 *
 * */



#endif // MYMultiARRAY_H
