
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

gShader::gShader()
{

    m_isSourced = 0;
    m_isCompiled = 0;
}

gShader::gShader(SHADER_TYPE type)
{
    switch(type)
    {
        case VERTEX_SHADER:
        {
            m_shader = glCreateShader(GL_VERTEX_SHADER);
        }
        break;

        case FRAGMENT_SHADER:
        {
            m_shader = glCreateShader(GL_FRAGMENT_SHADER);
        }
        break;

        case GEOMETRY_SHADER:
        {
            m_shader = glCreateShader(GL_GEOMETRY_SHADER);
        }
        break;
    }

    m_type = type;

    m_isSourced = 0;
    m_isCompiled = 0;
}

bool gShader::isSourced()
{
    return m_isSourced;
}

bool gShader::isCompiled()
{
    return m_isCompiled;
}

gShader::gShader(string shader_str, SHADER_TYPE type, bool binaryOrFile)
{
    switch(type)
    {
        case VERTEX_SHADER:
        {
            m_shader = glCreateShader(GL_VERTEX_SHADER);
        }
        break;

        case FRAGMENT_SHADER:
        {
            m_shader = glCreateShader(GL_FRAGMENT_SHADER);
        }
        break;

        case GEOMETRY_SHADER:
        {
            m_shader = glCreateShader(GL_GEOMETRY_SHADER);
        }
        break;
    }

    m_type = type;

    if(binaryOrFile == 0)
    {
        m_source = shader_str;
    }
    else
    {
        myFileHandler::getFileContent(shader_str, m_source);
    }

    int shader_str_size = shader_str.size();
    const char* source_cstr = m_source.data();
    glShaderSource(m_shader, 1, (const char**) &source_cstr, &shader_str_size);;

    findEntryPoints();

    m_isSourced = 1;
    m_isCompiled = 0;

    return;
}


gShader::~gShader()
{
   glDeleteShader(m_shader);
}

void gShader::setSource(string shader_str, bool binaryOrfile)
{
    bool weGet_content = 0;

    if(binaryOrfile == 0)
    {
        m_source = shader_str;
        if(m_source.size() != 0) weGet_content = 1;
    }
    else
    {
        weGet_content = myFileHandler::getFileContent(shader_str, m_source);
    }


    const char* source_cstr = m_source.data();
    if(weGet_content)
    {
        int source_size = m_source.size();
        glShaderSource(m_shader, 1, (const char**) &source_cstr, &source_size);
        findEntryPoints();
        m_isSourced = 1;
    }

    return;
}

string gShader::getSource()
{
    return m_source;
}


bool gShader::compile()
{
    if(isSourced())
    {
        glCompileShader(m_shader);
        GLint status;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);

        if(status != GL_FALSE)  m_isCompiled = true;
        else return false;
    }
    else
    {
        return false;
    }
}

bool gShader::compile(string& compilation_output)
{
    if(isSourced())
    {
        glCompileShader(m_shader);
        GLint status;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);

        if(status != GL_FALSE)
        {
            compilation_output = "***Compilation Succeed***\n\n";
            m_isCompiled = true;
        }
        else
        {
            int infoLog_length;

            glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLog_length);
            char buffer[infoLog_length+1];
            glGetShaderInfoLog(m_shader, infoLog_length, NULL, buffer);
            buffer[infoLog_length] = '\0';

            compilation_output.clear();
            compilation_output = "***Compilation Failed***\n\n";
            compilation_output += "Shader is sourced but contains the following errors :\n";
            compilation_output += string(buffer);
            compilation_output += "\n";

            return false;
        }
    }
    else
    {
        compilation_output = "***Compilation Failed***\n\n";
        compilation_output += "Shader is not sourced...\n\n";
        return false;
    }
}

entryPoint gShader::getEntryPoint(int EP_idx)
{
    if(EP_idx <= m_entryPoints_v.size()-1)
    {
        return m_entryPoints_v[EP_idx];
    }
    else
    {
        return NULL_ENTRYPOINT;
    }
}

vector<entryPoint> gShader::getEntryPoints_v()
{
    return m_entryPoints_v;
}

void gShader::findEntryPoints()
{
    string word_str;
    vector<string> words_v;

    int end_pos = 0;
    while(end_pos >= 0)
    {
        getWord(m_source, word_str, end_pos, end_pos, string(" \n\t;"), string(" \n\t"));
        if(word_str == "//")   getLine(m_source, word_str, end_pos, end_pos);
        else
        {
            words_v.push_back(word_str);
        }
    }

    vector<string>::iterator word = words_v.begin();

    vector<entryPoint> entryPoints_v;

    entryPoint temp_entryPoint;
    while(word != words_v.end())
    {
        temp_entryPoint.type = NONE_POINT;
        //TYPE

        if(*word == "in") temp_entryPoint.type = STREAM_IN_POINT;
        if(*word == "out") temp_entryPoint.type = STREAM_OUT_POINT;
        if(*word == "uniform") temp_entryPoint.type = UNIFORM_POINT;

        if(temp_entryPoint.type != NONE_POINT)
        {
			//SHADER

			temp_entryPoint.shader_type = m_type;

            //LOCATION

            temp_entryPoint.location = -1;

            //NAME

//            temp_entryPoint.name = *(word+2);

            string subWord;
            if(getSubWord(*(word+2), subWord,"[","]"))
            {
                temp_entryPoint.name = (*(word+2)).substr(0, (*(word+2)).find_first_of('['));

                temp_entryPoint.isArray = 1;
                if(subWord.size() == 0) temp_entryPoint.arraySize = -1; //infinite
                else temp_entryPoint.arraySize = strtod(subWord.data(), NULL);
            }
            else
            {
                temp_entryPoint.isArray = 0;
                temp_entryPoint.arraySize = 0;
                temp_entryPoint.name = *(word+2);
            }

            //DATA_TYPE & ARRAY

            temp_entryPoint.dataType = *(word+1);
            entryPoints_v.push_back(temp_entryPoint);
        }
        word++;
    }

    m_entryPoints_v = entryPoints_v;
}

void gShader::coutEntryPoints_v()
{
    int EP_idx = 0;
    while(EP_idx <= m_entryPoints_v.size()-1)
    {
        cout<<"******************\n";

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
        }

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
		}

        cout<<"entryPoint_idx : "<<EP_idx<<"\n";
        cout<<"entryPoint_name : "<<m_entryPoints_v[EP_idx].name<<"\n";
        cout<<"entryPoint_dataType : "<<m_entryPoints_v[EP_idx].dataType<<"\n";
        cout<<"entryPoint_isArray : "<<m_entryPoints_v[EP_idx].isArray<<"\n";
        cout<<"entryPoint_arraySize : "<<m_entryPoints_v[EP_idx].arraySize<<"\n";

        EP_idx++;
    }
}

GLuint gShader::getShader()
{
    return m_shader;
}

SHADER_TYPE gShader::getType()
{
    return m_type;
}

}
