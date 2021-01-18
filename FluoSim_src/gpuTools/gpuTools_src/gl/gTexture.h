
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

#ifndef GTEXTURE_HH
#define GTEXTURE_HH

#include "GL_glew/glew.h"
#include "gl.h"

#include "glm.hpp"
#include "stdlib.h"
#include "iostream"
#include "string"

#include "other/gpu_tiff_manipulation.h"

#include "gpuTools_library_global.h"

#include "QImage"



namespace gstd
{

class GPUTOOLS_LIBRARYSHARED_EXPORT gTexture
{	
public:

	gTexture(GLenum texture_type); //GL_TEXTURE_2D

	void bindTexture(int textureUnit_idx = 0);
	bool loadTexture(int size_x,int size_y,
					 GLenum texture_format,GLenum image_format,GLenum data_type,void* data_prt, bool test = false);
	bool clearTexture();
//	bool loadTexture(std::string file_dir, GLenum texture_format = GL_RGBA);
	void getTextureData(std::vector<uint8> &samples);
	void getTextureData(void* ,
						GLenum pixel_format = GL_RGBA,
						GLenum pixel_dataType = GL_UNSIGNED_BYTE);

	template<typename T>
	void getTextureData(std::vector<T>& sample,
						GLenum pixel_format/* = GL_RGBA*/,
						GLenum pixel_dataType/* = GL_UNSIGNED_BYTE*/,
						int pixelsize);

	bool setMinFilter(GLint value);
	bool setMagFilter(GLint value);
	bool setWrapS(GLint value);
	bool setWrapT(GLint value);

	bool isReady();
	bool isLoaded();
	bool isMipMapped();
	bool isMipMapEnable();
	bool isBound();

	GLuint getTextureFormat();
	int getMinFilter();
	int getMaxFilter();
	int getWrapS();
	int getWrapT();
	int getTextureUnitIdx();
	glm::vec2 getSize();

	bool genMipMap();
	bool enableMipMap(bool mipmap);

	//to be deleted : used by myGLScreenProceduralObject
	GLuint getTextureHdl();


private :

	GLuint m_texture;
	GLint m_texture_type;
	GLint m_texture_format;

	glm::vec2 m_size;

	bool m_isLoaded;
	bool m_isMipMapped;
	bool m_mipMapEnable;
	GLint m_minFilter;
	GLint m_maxFilter;
	GLint m_wrapS;
	GLint m_wrapT;

	int m_textureUnit_idx;
};


template<typename T>
void gTexture::getTextureData(std::vector<T>& samples,
							  GLenum pixel_format/* = GL_RGBA*/,
							  GLenum pixel_dataType/* = GL_UNSIGNED_BYTE*/,
							  int pixelsize)
{
	if(m_isLoaded == 0) return;//->

	bindTexture(m_textureUnit_idx);

	samples.clear();
	samples.assign(m_size.x*m_size.y*pixelsize, 0);

	glGetTexImage(GL_TEXTURE_2D, 0, pixel_format, pixel_dataType, samples.data());
}


GPUTOOLS_LIBRARYSHARED_EXPORT gTexture genGaussianTexture(glm::vec2 size, float sigma);

}




#endif // GTEXTURE_HH
