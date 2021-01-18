
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

#ifndef GENUM_H
#define GENUM_H

#include "gpuTools_library_global.h"

#include <string>
#include "vector"

using namespace std;

namespace gstd {


enum SHADER_TYPE {NONE_SHADER, VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER};
enum ENTRYPOINT_TYPE {NONE_POINT, STREAM_IN_POINT, STREAM_OUT_POINT, UNIFORM_POINT};
enum G_TYPE{VOIDD, BOOL, INT, UINT, FLOAT, VEC2, VEC3, VEC4, IVEC2, IVEC3, IVEC4,UVEC2, UVEC3, UVEC4, BVEC2, BVEC3, BVEC4,MAT2, MAT3, MAT4,SAMPLER2D, SAMPLER1D};

struct gTypeDescriptor
{
    string name;
	G_TYPE type;
    int nb_components;
    int component_atomicType;
    int size_inBytes;
    bool is_atomic;
};

struct entryPoint
{
    string name;
	SHADER_TYPE shader_type;
    ENTRYPOINT_TYPE type;
    string dataType;
    long location;
    bool isArray;
    long arraySize;
};

extern entryPoint NULL_ENTRYPOINT;

extern gTypeDescriptor g_void;
extern gTypeDescriptor g_bool;
extern gTypeDescriptor g_int;
extern gTypeDescriptor g_uint;
extern gTypeDescriptor g_float;

extern gTypeDescriptor g_bvec2;
extern gTypeDescriptor g_bvec3;
extern gTypeDescriptor g_bvec4;

extern gTypeDescriptor g_ivec2;
extern gTypeDescriptor g_ivec3;
extern gTypeDescriptor g_ivec4;

extern gTypeDescriptor g_uvec2;
extern gTypeDescriptor g_uvec3;
extern gTypeDescriptor g_uvec4;

extern gTypeDescriptor g_vec2;
extern gTypeDescriptor g_vec3;
extern gTypeDescriptor g_vec4;

extern gTypeDescriptor g_mat2;
extern gTypeDescriptor g_mat3;
extern gTypeDescriptor g_mat4;

extern gTypeDescriptor g_sampler2D;


extern vector<gTypeDescriptor> g_mainTypes;

gTypeDescriptor GPUTOOLS_LIBRARYSHARED_EXPORT getgTypeDescription(string gType_name);
gTypeDescriptor GPUTOOLS_LIBRARYSHARED_EXPORT getgTypeDescription(G_TYPE gType);

}


#endif // GENUM_H
