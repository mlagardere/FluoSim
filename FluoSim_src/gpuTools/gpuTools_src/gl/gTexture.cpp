
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



#include "gTexture.h"

using namespace glm;
using namespace std;


namespace gstd
{

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




gTexture::gTexture(GLenum texture_type)
{
	m_wrapS = -1;
	m_wrapT = -1;

	glGenTextures(1, &m_texture);

	glActiveTexture(0);
	glBindTexture(texture_type, m_texture);
	m_texture_type = texture_type;
	m_texture_format = 0;
	m_isLoaded = false;
	m_textureUnit_idx = 0;
	m_isMipMapped = false;
	m_mipMapEnable = false;
	m_size = vec2(-1, -1);

	setMinFilter(GL_NEAREST);
	setMagFilter(GL_NEAREST);

	if(m_texture_type == GL_TEXTURE_1D)
	{
		setWrapS(GL_CLAMP_TO_EDGE);
		m_wrapT = -1;
	}
	if(m_texture_type == GL_TEXTURE_2D)
	{
		setWrapS(GL_CLAMP_TO_EDGE);
		setWrapT(GL_CLAMP_TO_EDGE);
	}

//	glexture(texture_type, 0);
}

void gTexture::bindTexture(int textureUnit_idx)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit_idx);
	glBindTexture(m_texture_type, m_texture);
	m_textureUnit_idx = textureUnit_idx;

}

bool gTexture::loadTexture(int size_x,
						   int size_y,
						   GLenum texture_format,
						   GLenum image_format,
						   GLenum data_type,
						   void* data_prt,
						   bool test)
{
	bindTexture(m_textureUnit_idx);
	m_texture_format = texture_format;

	switch(m_texture_type)
	{
		case GL_TEXTURE_1D :
		{
			glTexImage1D(m_texture_type, 0, texture_format, size_x, 0, image_format, data_type, data_prt);
			m_size = vec2(size_x, -1);
		}
		break;

		case GL_TEXTURE_2D :
		{
			glTexImage2D(m_texture_type, 0, texture_format, size_x, size_y, 0, image_format, data_type, data_prt);
			m_size = vec2(size_x, size_y);
		}
		break;
	}

	m_isLoaded = true;
	return true;
}

bool gTexture::clearTexture()
{
	if(m_isLoaded == false) return false;//->

	bindTexture(m_textureUnit_idx);
	switch(m_texture_type)
	{
		case GL_TEXTURE_1D :
		{
			glTexImage1D(m_texture_type, 0, m_texture_format, m_size.x, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
		break;

		case GL_TEXTURE_2D :
		{
			vector<uint8> data;
			data.assign(m_size.x*m_size.y*4, 0x00);
			glTexImage2D(m_texture_type, 0, m_texture_format, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		}
		break;
	}

	return true;
}


void gTexture::getTextureData(std::vector<uint8> &samples)
{
	if(m_isLoaded == 0) return;//->

	bindTexture(m_textureUnit_idx);

	samples.clear();
	samples.assign(m_size.x*m_size.y*4, 0);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, samples.data());
}

void gTexture::getTextureData(void* data, GLenum pixel_format/* = GL_RGBA*/,
							  GLenum pixel_dataType/* = GL_UNSIGNED_BYTE*/)
{

}

bool gTexture::setMinFilter( GLint value)
{
	bindTexture(m_textureUnit_idx);

	glTexParameteri(m_texture_type, GL_TEXTURE_MIN_FILTER, value);
	m_minFilter = value;

	return 1;
}

bool gTexture::setMagFilter(GLint value)
{
	bindTexture(m_textureUnit_idx);

	glTexParameteri(m_texture_type, GL_TEXTURE_MAG_FILTER, value);
	m_maxFilter = value;

	return 1;
}

bool gTexture::setWrapS(GLint value)
{
	bindTexture(m_textureUnit_idx);

	glTexParameteri(m_texture_type, GL_TEXTURE_WRAP_S, value);
	m_wrapS = value;

	return 1;
}

bool gTexture::setWrapT(GLint value)
{
	bindTexture(m_textureUnit_idx);

	glTexParameteri(m_texture_type, GL_TEXTURE_WRAP_T, value);
	m_wrapT = value;

	return 1;
}

bool gTexture::isReady()
{
	return (isLoaded() && isBound());
}

bool gTexture::isLoaded()
{
	return m_isLoaded;
}

bool gTexture::isMipMapped()
{
	return m_isMipMapped;
}

bool gTexture::isBound()
{
	return isBound();
}

GLuint gTexture::getTextureFormat()
{
	return m_texture_format;
}

int gTexture::getMinFilter()
{
	return m_minFilter;
}

int gTexture::getMaxFilter()
{
	return m_maxFilter;
}

int gTexture::getWrapS()
{
	return m_wrapS;
}

int gTexture::getWrapT()
{
	return m_wrapT;
}

int gTexture::getTextureUnitIdx()
{
	return m_textureUnit_idx;
}

bool gTexture::genMipMap()
{
//	if(isMipMapped()) return 0;
//	else
//	{
		if(isLoaded())
		{
			glGenerateMipmap(m_texture_type);
			m_isMipMapped = 1;
		}
		return 1;
//	}
}

vec2 gTexture::getSize()
{
	return m_size;
}


bool gTexture::enableMipMap(bool mipmap)
{

	if(isMipMapped())
	{
		if(mipmap == true)
		{

			setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
			setMagFilter(GL_NEAREST_MIPMAP_NEAREST);

			m_mipMapEnable = mipmap;
		}
		else
		{
			setMinFilter(GL_LINEAR);
			setMagFilter(GL_LINEAR);
			m_mipMapEnable = mipmap;
		}
		return 1;
	}
	else return false;
}

bool gTexture::isMipMapEnable()
{
	return m_mipMapEnable;
}


GLuint gTexture::getTextureHdl()
{
	return m_texture;
}


gTexture genGaussianTexture(vec2 size, float sigma)
{
	vector<float> texture_gaussian;
	int Dx_tex = size.x;
	int Dy_tex = size.y;

		vec3 white(1,1,1);
		vec3 color;
		float s = 0;

	for(int x_i = 0; x_i <= Dx_tex-1; x_i++)
	{
		float x = -1 + 2*x_i * 1.0/(Dx_tex-1);
		for(int y_i = 0; y_i <= Dy_tex-1; y_i++)
		{
			float y = -1 + 2*y_i * 1.0/(Dy_tex-1);

			s = exp((-1.0/(2*pow(sigma,2.0)))*(pow(x,2.0) + pow(y,2.0)));
			color = s*white;
			texture_gaussian.push_back(color.x);
			texture_gaussian.push_back(color.y);
			texture_gaussian.push_back(color.z);
		}
	}

	gTexture gaussianTexture(GL_TEXTURE_2D);
	gaussianTexture.loadTexture(size.x, size.y, GL_RGB, GL_RGB, GL_FLOAT, texture_gaussian.data());

	return gaussianTexture;
}




}





