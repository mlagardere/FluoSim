
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

#ifndef GPROGRAM_H
#define GPROGRAM_H

#include "gpuTools_library_global.h"

#include "GL_glew/glew.h"
#include "gl.h"

#include "stdlib.h"
#include "string"
#include "vector"

#include "other/gpu_myFileHandler.h"
#include "other/gpu_string_manipulation.h"
#include "iostream"
#include "iomanip"

#include "gEnum.h"


namespace gstd
{


class GPUTOOLS_LIBRARYSHARED_EXPORT gShader
{
public :

	gShader();
    gShader(SHADER_TYPE type);
	gShader(string shader_str, SHADER_TYPE type, bool binaryOrFile);
    ~gShader();

    bool isSourced();
    bool isCompiled();

    void setSource(string shader_str, bool binaryOrFile); // in both memory
    string getSource();

    bool compile();
    bool compile(string& compilation_output);

    void findEntryPoints();
    entryPoint getEntryPoint(int ep_idx);
    vector<entryPoint> getEntryPoints_v();
    void coutEntryPoints_v();

    GLuint getShader();
    SHADER_TYPE getType();

private :

    string m_source;
    bool m_isSourced;
    bool m_isCompiled;

    SHADER_TYPE m_type;
    GLuint m_shader;

    vector<entryPoint> m_entryPoints_v;

};


class GPUTOOLS_LIBRARYSHARED_EXPORT gProgram
{
public :

    gProgram(string name);
	~gProgram();

    void addShader(gstd::gShader shader);
	gShader& getShaderRef(SHADER_TYPE shader_type);
	gShader getShader(SHADER_TYPE shader_type);

    bool linkShaders();
    bool linkShaders(string& infoLog_str);

    bool isProgramLink();
    bool isShaderLoaded(SHADER_TYPE shader_type);

    bool useProgram(bool toBe_used);

    void analyseEntryPoints();
    entryPoint getEntryPoint(string EP_name);
    void coutEntryPoints();

private :

    string m_name;
    GLuint m_program;

    vector<entryPoint> m_entryPoints_v;

    gShader m_vx_shader;
    gShader m_geom_shader;
    gShader m_fgt_shader;
    map<SHADER_TYPE, bool> m_isShaderLoaded;

    bool m_isProgramLink;
};


}


#endif // GPROGRAM_H
