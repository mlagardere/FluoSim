
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

#include "gEnum.h"

#include "GL_glew/glew.h"
#include "gl.h"

using namespace  std;

namespace gstd {



entryPoint NULL_ENTRYPOINT = entryPoint{"-1", NONE_SHADER, NONE_POINT, "-1", -1, 0, -1};

//Atomic types

gTypeDescriptor g_void = gTypeDescriptor{"void", VOIDD, 0, VOIDD, 0, 1};
gTypeDescriptor g_bool = gTypeDescriptor{"bool", BOOL, 1, GL_BOOL, sizeof(bool), 1};
gTypeDescriptor g_int = gTypeDescriptor{"int", INT, 1, GL_INT, sizeof(int), 1};
gTypeDescriptor g_uint = gTypeDescriptor{"uint", UINT, 1, GL_UNSIGNED_INT, sizeof(unsigned int), 1};
gTypeDescriptor g_float = gTypeDescriptor{"float", FLOAT, 1, GL_FLOAT, sizeof(float), 1};

//bool vector types

gTypeDescriptor g_bvec2 = gTypeDescriptor{"bvec2", BVEC2, 2, GL_BOOL, 2*sizeof(bool), 0};
gTypeDescriptor g_bvec3 = gTypeDescriptor{"bvec3", BVEC3, 3, GL_BOOL, 2*sizeof(bool), 0};
gTypeDescriptor g_bvec4 = gTypeDescriptor{"bvec4", BVEC4, 4, GL_BOOL, 2*sizeof(bool), 0};

//int vector types

gTypeDescriptor g_ivec2 = gTypeDescriptor{"ivec2", IVEC2, 2, GL_INT, 2*sizeof(int), 0};
gTypeDescriptor g_ivec3 = gTypeDescriptor{"ivec3", IVEC3, 3, GL_INT, 2*sizeof(int), 0};
gTypeDescriptor g_ivec4 = gTypeDescriptor{"ivec4", IVEC4, 4, GL_INT, 2*sizeof(int), 0};

//uint vector types

gTypeDescriptor g_uvec2 = gTypeDescriptor{"uvec2", UVEC2, 2, GL_UNSIGNED_INT, 2*sizeof(unsigned int), 0};
gTypeDescriptor g_uvec3 = gTypeDescriptor{"uvec3", UVEC3, 3, GL_UNSIGNED_INT, 2*sizeof(unsigned int), 0};
gTypeDescriptor g_uvec4 = gTypeDescriptor{"uvec4", UVEC4, 4, GL_UNSIGNED_INT, 2*sizeof(unsigned int), 0};

//float vector types

gTypeDescriptor g_vec2 = gTypeDescriptor{"vec2", VEC2, 2, GL_FLOAT, 2*sizeof(float), 0};
gTypeDescriptor g_vec3 = gTypeDescriptor{"vec3", VEC3, 3, GL_FLOAT, 3*sizeof(float), 0};
gTypeDescriptor g_vec4 = gTypeDescriptor{"vec4", VEC4, 4, GL_FLOAT, 4*sizeof(float), 0};

//float squared matrices types

gTypeDescriptor g_mat2 = gTypeDescriptor{"mat2", MAT2, 4, GL_FLOAT, 4*sizeof(float), 0};
gTypeDescriptor g_mat3 = gTypeDescriptor{"mat3", MAT3, 9, GL_FLOAT, 9*sizeof(float), 0};
gTypeDescriptor g_mat4 = gTypeDescriptor{"mat4", MAT4, 16, GL_FLOAT, 16*sizeof(float), 0};

gTypeDescriptor g_sampler2D = gTypeDescriptor{"sampler2D", SAMPLER2D, 1, GL_INT, 1*sizeof(int), 0};
gTypeDescriptor g_sampler1D = gTypeDescriptor{"sampler1D", SAMPLER1D, 1, GL_INT, 1*sizeof(int), 0};


vector<gTypeDescriptor> g_mainTypes{g_void, g_bool, g_int, g_uint, g_float,
									g_bvec2, g_bvec3,g_bvec4,
									g_ivec2, g_ivec3,g_ivec4,
									g_uvec2, g_uvec3,g_uvec4,
									g_vec2, g_vec3,g_vec4,
									g_mat2, g_mat3, g_mat4,
									g_sampler2D, g_sampler1D};

gTypeDescriptor getgTypeDescription(string gType_name)
{
	auto gType_descriptor = g_mainTypes.begin();
	while(gType_name != gType_descriptor->name && gType_descriptor != g_mainTypes.end())
	{
		gType_descriptor++;
	}
	if(gType_descriptor == g_mainTypes.end()) return g_void;

	else return *gType_descriptor;
}


gTypeDescriptor getgTypeDescription(G_TYPE gType)
{
	auto gType_descriptor = g_mainTypes.begin();
	while(gType != gType_descriptor->type && gType_descriptor != g_mainTypes.end())
	{
		gType_descriptor++;
	}
	if(gType_descriptor == g_mainTypes.end()) return g_void;

	else return *gType_descriptor;
}


}





