
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

#ifndef GVECTOR_H
#define GVECTOR_H

#include "gpuTools_library_global.h"

#include "GL_glew/glew.h"
#include "gl.h"

#include "stdlib.h"
#include "vector"
#include "iostream"
#include "cmath"

#include "gEnum.h"


using namespace std;

namespace gstd {

void* GPUTOOLS_LIBRARYSHARED_EXPORT align(size_t alignment, size_t size, void*& ptr,	std::size_t& space);

template<typename T>
std::vector<int> getComponentOffsets(std::vector<gstd::G_TYPE> component_types_v)
{
	std::vector<int> offsets_v;

	size_t alignment = alignof(T);
	size_t space = sizeof(T);
    char* buff = new char[sizeof(T)]; //buffer to create the object
	void* start_adr(buff);
	void* current_adr = start_adr;

	auto component_type = component_types_v.begin();
	while(component_type != component_types_v.end())
	{
		current_adr = align(alignment, gstd::getgTypeDescription(*component_type).size_inBytes, current_adr, space);
		offsets_v.push_back(int(current_adr) - int(start_adr));
		current_adr += gstd::getgTypeDescription(*component_type).size_inBytes;
		component_type++;
	}

    delete[] buff; //new
	return offsets_v;
}




template<typename type> class gVector;

template<typename type>
class GPUTOOLS_LIBRARYSHARED_EXPORT gVectorElement
{
public :

	gVectorElement(long element_idx, gVector<type>* associated_gVector);
	void operator=(type value); //needed to do (handler) b = (type) a

	operator type(); //type conversion from handler to type...needed to do (type) a = (handler) b;

private :

	long m_element_idx;
	gVector<type>* m_associated_gVector;
};

template<typename type>
gVectorElement<type>::gVectorElement(long element_idx, gVector<type>* associated_gVector)
{
	m_element_idx = element_idx;
	m_associated_gVector = associated_gVector;
}

template<typename type>
void gVectorElement<type>::operator=(type value)
{
	m_associated_gVector->setValue(m_element_idx, value);
}

template<typename type>
gVectorElement<type>:: operator type()
{
	return m_associated_gVector->getValue(m_element_idx);
}


template<typename type> class gVector;


template<typename type>
class GPUTOOLS_LIBRARYSHARED_EXPORT gVector
{

public :

	gVector(bool isInterleaved = false, vector<G_TYPE> components_types_v = {});
	~gVector();

    gVector(vector<type> data);
	void deleteBuffer();
    void clear();
    void reserve(long new_capacity);
    void reserveInBytes(long new_capacity_bytes);

    long size();
    long capacity();
    long capacityInBytes();

	void push_back(type element);
	void pop_back();
	void insert(long offset, vector<type> elements_v);
	void insert(long offset, type element);

	void erase(long offset);
	void erase(long offset, long nb);

	void setValue(long idx, type new_value);
	type getValue(long idx);

    void bindVector(bool binding);

    bool isCreated();
	bool isInterleaved();

	int getComponentOffset(int component_idx);
	G_TYPE getComponentType(int component_idx);


	vector<type> data_v();

	gVectorElement<type> operator[](long idx); //used as an interface to access gpuArray elements

private:

    GLuint m_data_buff;
    long m_buff_capacity;
    long m_buff_capacity_bytes;
    long m_buff_size;
    bool m_isCreated;

	bool m_isInterleaved;
	vector<int> m_component_offsets_v;
	vector<G_TYPE> m_components_types_v;
};


template<typename type>
gVector<type>::gVector(bool isInterleaved, vector<G_TYPE> components_types_v)
{
    glGenBuffers(1, &m_data_buff);
    m_isCreated = 1;

    m_buff_size = 0;
    m_buff_capacity = 0;
    m_buff_capacity_bytes = 0;

	m_isInterleaved = isInterleaved;

	if(isInterleaved == true)
	{
		m_component_offsets_v = getComponentOffsets<type>(components_types_v);
		m_components_types_v = components_types_v;
	}
}

template<typename type>
gVector<type>::~gVector()
{
	if(0)	glDeleteBuffers(1, &m_data_buff);

}


template<typename type>
gVector<type>::gVector(vector<type> data)
{
    glGenBuffers(1, &m_data_buff);
    m_isCreated = 1;
    glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

    //we allocate a power of 2 bytes for this buffer
        double data_storage_size = data.size()*sizeof(type);
        double pow;

        pow = std::log(data_storage_size)/std::log(2.0);
        if(pow - int(pow) > 0) pow ++;
        pow = int(pow);

        //allocation
        glBufferData(GL_ARRAY_BUFFER, std::pow(2, pow), NULL, GL_DYNAMIC_DRAW);
        m_buff_size = data.size();
        m_buff_capacity = std::pow(2, pow)/sizeof(type);
        m_buff_capacity_bytes = std::pow(2, pow);
        //loading...
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_storage_size, data.data());

        glFinish();

     glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename type>
void gVector<type>::deleteBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
		glBufferData(GL_ARRAY_BUFFER, capacityInBytes(), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_buff_capacity = 0;
    m_buff_capacity_bytes = 0;
    m_buff_size = 0;

    glDeleteBuffers(1, &m_data_buff);
    m_isCreated = 0;
}

template<typename type>
void gVector<type>::clear()
{
	if(m_buff_capacity == 0) return;

	glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
		glBufferData(GL_ARRAY_BUFFER, capacityInBytes(), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_buff_capacity = 0;
	m_buff_capacity_bytes = 0;
	m_buff_size = 0;

//    glDeleteBuffers(1, &m_data_buff);
//    m_isCreated = 0;
}


template<typename type>
void gVector<type>::reserve(long new_capacity)
{
    if(isCreated() == 0)
    {
        glGenBuffers(1, &m_data_buff);
        m_isCreated = 1;

        m_buff_size = 0;
        m_buff_capacity = 0;
        m_buff_capacity_bytes = 0;
    }

    if(new_capacity <= m_buff_capacity) return;

    double pow;
    pow = std::log(new_capacity*sizeof(type))/std::log(2.0);
    if(pow - int(pow) > 0) pow ++;
    pow = int(pow);

    //copy
	if(m_buff_capacity_bytes == 0)
	//no need to copy, just allocate
	{

		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_ARRAY_BUFFER, std::pow(2, pow), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glFinish();

		m_buff_capacity_bytes = std::pow(2, pow);
		m_buff_capacity = new_capacity;
	}
	else
	//we need to copy our data
	// from GL_ARRAY_BUFFER to GL_COPY_WRITE_BUFFER
	{
		GLuint temp_buff;
		glGenBuffers(1, &temp_buff);

		glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_COPY_WRITE_BUFFER, m_buff_capacity_bytes, NULL, GL_DYNAMIC_COPY);
			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0,0, m_buff_capacity_bytes);
			glBufferData(GL_ARRAY_BUFFER, std::pow(2, pow), NULL, GL_DYNAMIC_DRAW);
			glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER, 0, 0, m_buff_capacity_bytes);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glDeleteBuffers(1, &temp_buff);

		glFinish();

		m_buff_capacity_bytes = std::pow(2, pow);
		m_buff_capacity = new_capacity;
	}
}

template<typename type>
void gVector<type>::reserveInBytes(long new_capacity_bytes)
{
    if(isCreated() == 0)
    {
        glGenBuffers(1, &m_data_buff);
        m_isCreated = 1;
        m_buff_size = 0;
        m_buff_capacity = 0;
        m_buff_capacity_bytes = 0;
    }

    if(new_capacity_bytes <= m_buff_capacity_bytes) return;

    double pow;
    pow = std::log(new_capacity_bytes)/std::log(2.0);
    if(pow - int(pow) > 0) pow ++;
    pow = int(pow);


	if(m_buff_capacity_bytes == 0)
	//no need to copy, just allocate
	{

		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_ARRAY_BUFFER, std::pow(2, pow), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glFinish();

		m_buff_capacity_bytes = new_capacity_bytes;
		m_buff_capacity = new_capacity_bytes/sizeof(type); //there might be unused bytes!
	}
	else
	//we need to copy our data
	// from GL_ARRAY_BUFFER to GL_COPY_WRITE_BUFFER
	{
		GLuint temp_buff;
		glGenBuffers(1, &temp_buff);

		glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_COPY_WRITE_BUFFER, m_buff_capacity_bytes, NULL, GL_DYNAMIC_COPY);
			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0,0, m_buff_capacity_bytes);
			glBufferData(GL_ARRAY_BUFFER, std::pow(2, pow), NULL, GL_DYNAMIC_DRAW);
			glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER, 0, 0, m_buff_capacity_bytes);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glDeleteBuffers(1, &temp_buff);

		glFinish();

		m_buff_capacity_bytes = new_capacity_bytes;
		m_buff_capacity = new_capacity_bytes/sizeof(type); //there might be unused bytes!
	}
}


template<typename type>
void gVector<type>::push_back(type element)
{
	if(isCreated() == 0)
	{
		glGenBuffers(1, &m_data_buff);
		m_isCreated = 1;
		m_buff_size = 0;
		m_buff_capacity = 0;
		m_buff_capacity_bytes = 0;
	}

	if(m_buff_size >= m_buff_capacity)
	{
		if(m_buff_size == 0)
		{
            //we determine the number of bytes to store the data as a power of 2
			double pow;
			pow = std::log(sizeof(type))/std::log(2.0);
			if(pow - int(pow) > 0) pow ++;
			pow = int(pow);

			reserveInBytes(std::pow(2, pow));
		}
		else
		{
			reserveInBytes(2* m_buff_capacity_bytes);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
	glBufferSubData(GL_ARRAY_BUFFER, m_buff_size*sizeof(type), sizeof(type), &element);
	m_buff_size++;
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glFinish();
}


template<typename type>
void gVector<type>::pop_back()
{
	if(isCreated() == 0 || m_buff_size == 0) return;
	else
	{
		m_buff_size = m_buff_size-1;
	}
}


template<typename type>
void gVector<type>::insert(long offset, vector<type> elements_v)
{
	if(offset > m_buff_size || elements_v.size() == 0) return; //->

	if(isCreated() == 0)
	{
		glGenBuffers(1, &m_data_buff);
		m_isCreated = 1;
		m_buff_size = 0;
		m_buff_capacity = 0;
		m_buff_capacity_bytes = 0;
	}
	long nb_element = elements_v.size();

	if(m_buff_size + nb_element > m_buff_capacity)
	{
		reserveInBytes((m_buff_size+nb_element)*sizeof(type));
	}

	if(offset == m_buff_size)
	//no need to copy data, else 0bytesCopy_error...
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(type), nb_element*sizeof(type), elements_v.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glFinish();
	}
	else
	{
		GLuint temp_buff;
		glGenBuffers(1, &temp_buff);

		glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_COPY_WRITE_BUFFER, (m_buff_size - offset)*sizeof(type), NULL, GL_DYNAMIC_COPY);
			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, offset*sizeof(type),0,(m_buff_size - offset)*sizeof(type));
			glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(type), nb_element*sizeof(type), elements_v.data());
			glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER,
							0, (offset + nb_element)*sizeof(type), (m_buff_size - offset)*sizeof(type));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glDeleteBuffers(1, &temp_buff);

		glFinish();
	}

	m_buff_size =  m_buff_size + nb_element;
}

template<typename type>
void gVector<type>::insert(long offset, type element)
{
	if(offset > m_buff_size) return;

	if(isCreated() == 0)
	{
		glGenBuffers(1, &m_data_buff);
		m_isCreated = 1;
		m_buff_size = 0;
		m_buff_capacity = 0;
		m_buff_capacity_bytes = 0;
	}
	long nb_element = 1;

	if(m_buff_size + nb_element > m_buff_capacity)
	{
		reserveInBytes((m_buff_size+nb_element)*sizeof(type));
	}

	if(offset == m_buff_size)
	//no need to copy data, else 0bytesCopy_error...
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(type), nb_element*sizeof(type), &element);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glFinish();
	}
	else
	{
		GLuint temp_buff;
		glGenBuffers(1, &temp_buff);

		glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);

			glBufferData(GL_COPY_WRITE_BUFFER, (m_buff_size - offset)*sizeof(type), NULL, GL_DYNAMIC_COPY);
			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, offset*sizeof(type),0,(m_buff_size - offset)*sizeof(type));

			glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(type), nb_element*sizeof(type), &element);
			glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER,
								0, (offset + nb_element)*sizeof(type), (m_buff_size - offset)*sizeof(type));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glDeleteBuffers(1, &temp_buff);

		glFinish();
	}
	m_buff_size += nb_element;
}



template<typename type>
void gVector<type>::erase(long offset)
{
	if(isCreated() == 0 || (offset > m_buff_size-1)) return;

	GLuint temp_buff;
	glGenBuffers(1, &temp_buff);
	glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBufferData(GL_COPY_WRITE_BUFFER, (m_buff_size - offset)*sizeof(type), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
	glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, offset*sizeof(type),0,(m_buff_size - offset)*sizeof(type));

	glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER,
						sizeof(type), (offset)*sizeof(type), (m_buff_size - offset - 1)*sizeof(type));


	glDeleteBuffers(1, &temp_buff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	glFinish();

	m_buff_size = m_buff_size-1;
}

template<typename type>
void gVector<type>::erase(long offset, long nb)
{
	long nb_toDelete = nb;
	if(m_buff_size - offset <= nb) nb_toDelete = m_buff_size - offset;

	if(isCreated() == 0 || nb_toDelete <= 0) return;

	GLuint temp_buff;
	glGenBuffers(1, &temp_buff);
	glBindBuffer(GL_COPY_WRITE_BUFFER, temp_buff);
		glBufferData(GL_COPY_WRITE_BUFFER, (m_buff_size - offset)*sizeof(type), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
	glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, offset*sizeof(type),0,(m_buff_size - offset)*sizeof(type));

	glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER,
						nb_toDelete*sizeof(type), (offset)*sizeof(type), (m_buff_size - offset - nb_toDelete)*sizeof(type));


	glDeleteBuffers(1, &temp_buff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	glFinish();

	m_buff_size = m_buff_size-nb_toDelete;
}


template<typename type>
void gVector<type>::setValue(long idx, type new_value)
{
	if(isCreated() == 0 || (idx > m_buff_size-1)) return;

	glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
	glBufferSubData(GL_ARRAY_BUFFER, idx*sizeof(type), sizeof(type), &new_value);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glFinish();
}

template<typename type>
type gVector<type>::getValue(long idx)
{
	vector<type> v = data_v();
	return v[idx];
}



template<typename type>
long gVector<type>::capacity()
{
    return m_buff_capacity;
}

template<typename type>
long gVector<type>::capacityInBytes()
{
    return m_buff_capacity_bytes;
}

template<typename type>
long gVector<type>::size()
{
    return m_buff_size;
}

template<typename type>
void gVector<type>::bindVector(bool binding)
{
    if(binding) glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
    else glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename type>
vector<type> gVector<type>::data_v()
{
    vector<type> data_v;

    if(isCreated())
    {
        type* data_ptr;
        data_ptr = new type[m_buff_size];

        glBindBuffer(GL_ARRAY_BUFFER, m_data_buff);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_buff_size*sizeof(type), data_ptr);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        data_v.insert(data_v.begin(), data_ptr, data_ptr + m_buff_size);
		delete[] data_ptr;
    }

    return data_v;
}

template<typename type>
bool gVector<type>::isCreated()
{
    return m_isCreated;
}

template<typename type>
int gVector<type>::getComponentOffset(int component_idx)
{
	return m_component_offsets_v[component_idx];
}


template<typename type>
gVectorElement<type> gVector<type>::operator[](long idx)
{
	//error handling will be done at the next stage ie at the gvectorElement lvl!
	gVectorElement<type> l = gVectorElement<type>(idx, this);
	return l;
}

template<typename type>
G_TYPE gVector<type>::getComponentType(int component_idx)
{
	return m_components_types_v[component_idx];
}

}

#endif // GVECTOR_H
