
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


#include "gProgram.h"


namespace gstd {

/********************
*
*  Classe definition
*
*********************/


gProgram::gProgram(string name)
{
    m_name = name;
    m_program = glCreateProgram();

    m_isShaderLoaded.insert(std::pair<SHADER_TYPE, bool>(VERTEX_SHADER,0));
    m_isShaderLoaded.insert(std::pair<SHADER_TYPE, bool>(GEOMETRY_SHADER,0));
    m_isShaderLoaded.insert(std::pair<SHADER_TYPE, bool>(FRAGMENT_SHADER,0));

    m_isProgramLink = 0;
}

gProgram::~gProgram()
{
//	glDeleteProgram(m_program);
}

//gProgram::gProgram(string name, vector<gShader> shaders_v)
//{
//    m_name = name;
//    m_shaders_v = shaders_v;
//    m_program = glCreateProgram();

//    for(int shader_idx = 0; shader_idx <= shaders_v.size()-1; shader_idx++)
//    {
//        glAttachShader(m_program, m_shaders_v[shader_idx].getShader());
//    }
//}

void gProgram::addShader(gShader shader)
{
    switch (shader.getType())
    {
        case VERTEX_SHADER:
        {
            if(isShaderLoaded(VERTEX_SHADER)) glDetachShader(m_program, m_vx_shader.getShader());
            m_vx_shader = shader;
            glAttachShader(m_program, m_vx_shader.getShader());
            m_isShaderLoaded[VERTEX_SHADER] = 1;
        }
        break;

        case GEOMETRY_SHADER:
        {
            if(isShaderLoaded(GEOMETRY_SHADER)) glDetachShader(m_program, m_geom_shader.getShader());
            m_geom_shader = shader;;
            glAttachShader(m_program, m_geom_shader.getShader());
            m_isShaderLoaded[GEOMETRY_SHADER] = 1;
        }
        break;

        case FRAGMENT_SHADER:
        {
            if(isShaderLoaded(FRAGMENT_SHADER)) glDetachShader(m_program, m_fgt_shader.getShader());
            m_fgt_shader = shader;
            glAttachShader(m_program, m_fgt_shader.getShader());
            m_isShaderLoaded[FRAGMENT_SHADER] = 1;
        }
        break;
        default : break;
    }
}

gShader& gProgram::getShaderRef(SHADER_TYPE shader_type)
{
	switch (shader_type)
	{
        case VERTEX_SHADER:
        {
            return m_vx_shader;
        }
        break;

        case GEOMETRY_SHADER:
        {
            return m_geom_shader;
        }
        break;

        case FRAGMENT_SHADER:
        {
            return m_fgt_shader;
        }
        break;
        default : break;
    }
}

gShader gProgram::getShader(SHADER_TYPE shader_type)
{
	switch (shader_type)
	{
        case VERTEX_SHADER:
        {
            return m_vx_shader;
        }
        break;

        case GEOMETRY_SHADER:
        {
            return m_geom_shader;
        }
        break;

        case FRAGMENT_SHADER:
        {
            return m_fgt_shader;
        }
        break;
        default : break;
    }
}

bool gProgram::linkShaders()
{
	if(m_isProgramLink) return 1;

    glLinkProgram(m_program);
    int status;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    analyseEntryPoints();

	if(status != GL_FALSE)
	{
		m_isProgramLink = 1;
		analyseEntryPoints();
	}
    else m_isProgramLink = 0;
}

bool gProgram::linkShaders(string& infoLog_str)
{
	if(m_isProgramLink) return 1;

    glLinkProgram(m_program);
    int status;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);


    if(status != GL_FALSE)
    {
        infoLog_str.clear();
        infoLog_str = "***Program Linking Succeed***\n\n";
        m_isProgramLink = true;
        analyseEntryPoints();
    }
    else
    {
        int infoLog_length;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLog_length);
        char infoLog_cstr[infoLog_length+1];
        glGetProgramInfoLog(m_program, infoLog_length, NULL, infoLog_cstr);
        infoLog_cstr[infoLog_length] = '\0';

        infoLog_str.clear();
        infoLog_str = "***Program Linking Failed***\n\n";
        infoLog_str += "GL finds the following errors :\n";
        infoLog_str += string(infoLog_cstr);
		infoLog_str += "\n";


        m_isProgramLink = false;
    }
}

bool gProgram::isProgramLink()
{
    return m_isProgramLink;
}

bool gProgram::useProgram(bool toBe_used)
{
    if(toBe_used) glUseProgram(m_program);
    else glUseProgram(0);

	return false;
}

bool gProgram::isShaderLoaded(SHADER_TYPE shader_type)
{
    return m_isShaderLoaded[shader_type];
}

void gProgram::analyseEntryPoints()
{
    m_entryPoints_v.clear();
	int sampler_idx = 0;

    if(isShaderLoaded(VERTEX_SHADER))
    {
        vector<entryPoint> vs_EP_v = m_vx_shader.getEntryPoints_v();

		int vs_EP_v_size = vs_EP_v.size();
		for(int vs_EP_idx = 0; vs_EP_idx <= vs_EP_v_size-1; vs_EP_idx++)
        {
			if(vs_EP_v[vs_EP_idx].type == STREAM_IN_POINT ||
			   vs_EP_v[vs_EP_idx].type == UNIFORM_POINT)
            {
                m_entryPoints_v.push_back(vs_EP_v[vs_EP_idx]);
            }
        }
    }

    if(isShaderLoaded(FRAGMENT_SHADER))
    {
        vector<entryPoint> fs_EP_v = m_fgt_shader.getEntryPoints_v();

		int fs_EP_v_size = fs_EP_v.size();
		for(int fs_EP_idx = 0; fs_EP_idx <= fs_EP_v_size-1; fs_EP_idx++)
        {
			if(fs_EP_v[fs_EP_idx].type == UNIFORM_POINT)
            {
                m_entryPoints_v.push_back(fs_EP_v[fs_EP_idx]);
            }
        }
    }

    if(isShaderLoaded(GEOMETRY_SHADER))
    {
        vector<entryPoint> gs_EP_v = m_geom_shader.getEntryPoints_v();

		int gs_EP_v_size = gs_EP_v.size();
		for(int gs_EP_idx = 0; gs_EP_idx <= gs_EP_v_size-1; gs_EP_idx++)
        {
			if(gs_EP_v[gs_EP_idx].type == UNIFORM_POINT)
            {
                m_entryPoints_v.push_back(gs_EP_v[gs_EP_idx]);
            }
        }
    }

//    useProgram();
    auto entryPoint = m_entryPoints_v.begin();
    while(entryPoint != m_entryPoints_v.end())
    {
        if(entryPoint->type == STREAM_IN_POINT)
        {
            entryPoint->location = glGetAttribLocation(m_program, entryPoint->name.data());
        }
        if(entryPoint->type == UNIFORM_POINT)
		{
			if(entryPoint->dataType == "sampler2D")
			{
				entryPoint->location = sampler_idx;
				sampler_idx++;
			}
			else
			{
				entryPoint->location = glGetUniformLocation(m_program, entryPoint->name.data());
			}
        }
        entryPoint++;
    }
//    glUseProgram(0);
}

void gProgram::coutEntryPoints()
{
	uint EP_idx = 0;
    while(EP_idx <= m_entryPoints_v.size()-1)
    {
        cout<<"******************\n";
		switch(m_entryPoints_v[EP_idx].shader_type)
		{
			case VERTEX_SHADER:
			{
				cout<<"entryPoint_type : "<<"VERTEX_SHADER"<<"\n";
			}
			break;

			case FRAGMENT_SHADER:
			{
				cout<<"entryPoint_type : "<<"FRAGMENT_SHADER"<<"\n";
			}
			break;

			case GEOMETRY_SHADER:
			{
				cout<<"entryPoint_type : "<<"GEOMETRY_SHADER"<<"\n";
			}
			break;
			default : break;
		}


        switch(m_entryPoints_v[EP_idx].type)
        {
			case STREAM_IN_POINT:
			{
				cout<<"entryPoint_type : "<<"STREAM_IN_POINT"<<"\n";
			}
			break;

			case STREAM_OUT_POINT:
			{
				cout<<"entryPoint_type : "<<"STREAM_OUT_POINT"<<"\n";
			}
			break;

			case UNIFORM_POINT:
			{
				cout<<"entryPoint_type : "<<"UNIFORM_POINT"<<"\n";
			}
			break;
			default : break;
        }

        cout<<"entryPoint_idx : "<<EP_idx<<"\n";
        cout<<"entryPoint_location : "<<m_entryPoints_v[EP_idx].location<<"\n";
        cout<<"entryPoint_name : "<<m_entryPoints_v[EP_idx].name<<"\n";
        cout<<"entryPoint_dataType : "<<m_entryPoints_v[EP_idx].dataType<<"\n";
        cout<<"entryPoint_isArray : "<<m_entryPoints_v[EP_idx].isArray<<"\n";
        cout<<"entryPoint_arraySize : "<<m_entryPoints_v[EP_idx].arraySize<<"\n";

        EP_idx++;
    }
}

entryPoint gProgram::getEntryPoint(string EP_name)
{
    analyseEntryPoints();
    auto entryPoint = m_entryPoints_v.begin();

    while(entryPoint != m_entryPoints_v.end() && entryPoint->name!= EP_name)
    {
        entryPoint++;
    }

    if(entryPoint == m_entryPoints_v.end()) return NULL_ENTRYPOINT;
    else return *entryPoint;
}


}
