
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



#include "myTiffMovie.h"


using namespace glm;


myTiffMovie::myTiffMovie() :

	m_parent_gMV()

{
    m_glImage = new myGLImage("", vec2(1,0), vec2(100,100), m_parent_gMV);

	m_tiff = 0;
	m_tiffPath = "";
	m_tiff_params.numberDirectory = 4000;
	m_currentPlane = -1;
	m_buffer = 0;
	m_glImage->setIsYInverted(false);
	m_glImage->setRenderingMode(myGLImage::IMAGE_GS_8_16_24_RENDERING);
}

myTiffMovie::~myTiffMovie()
{
	delete m_glImage;
	if(m_tiff != 0) TIFFClose(m_tiff);
}

void myTiffMovie::openTiff(string tiffPath)
{
	m_tiffPath = tiffPath;

	m_tiff = TIFFOpen(tiffPath.c_str(), "r");

	TIFFSetErrorHandler(0);
	TIFFSetWarningHandler(0);

	if(m_tiff != 0)
	{
		m_tiff_params.numberDirectory = 0;
		while(TIFFReadDirectory(m_tiff)) m_tiff_params.numberDirectory++;

		TIFFSetDirectory(m_tiff, 0);
		TIFFGetField(m_tiff, TIFFTAG_IMAGELENGTH, &m_tiff_params.height);
		TIFFGetField(m_tiff, TIFFTAG_IMAGEWIDTH, &m_tiff_params.width);
		TIFFGetField(m_tiff, TIFFTAG_BITSPERSAMPLE, &m_tiff_params.bitsPerSample);
		TIFFGetField(m_tiff, TIFFTAG_SAMPLESPERPIXEL, &m_tiff_params.samplesPerPixel);
		m_tiff_params.bytesPerSample = m_tiff_params.bitsPerSample/8;

		m_buffer = 0;

		if(m_buffer != 0) delete[] m_buffer;
		m_buffer = new uint8[m_tiff_params.height*m_tiff_params.width*m_tiff_params.bytesPerSample*1];

		TIFFClose(m_tiff);
	}
	else
	{
		m_tiff_params.numberDirectory = -1;
	}
}

void myTiffMovie::loadTiffPlane()
{
	if(m_tiff == 0 || m_buffer == 0 || m_currentPlane < 0 || m_currentPlane >= m_tiff_params.numberDirectory-1)  return; //->

	m_tiff = TIFFOpen(m_tiffPath.c_str(), "r");

	TIFFSetDirectory(m_tiff, m_currentPlane);
	for(int row = 0; row < m_tiff_params.height; row++)
	{
		TIFFReadScanline(m_tiff, m_buffer + row*m_tiff_params.width*m_tiff_params.bytesPerSample*1, row);
	}

	TIFFClose(m_tiff);

	//image loading

	m_glImage->loadImage({m_tiff_params.width,m_tiff_params.height}, GL_RG, GL_RG, GL_UNSIGNED_BYTE, m_buffer);

}

void myTiffMovie::renderTiffPlane(myGLObject* render_target)
{
	if(m_tiff == 0) return; //->
	m_glImage->render(render_target);

}

void myTiffMovie::setPlane(int current_plane)
{
	if(m_tiff == 0 || current_plane < 0 || current_plane >= m_tiff_params.numberDirectory -1)  return; //->

	m_currentPlane = current_plane;

}

uint32 myTiffMovie::getNbDirectories()
{
	return m_tiff_params.numberDirectory;
}

myTIFFParameters myTiffMovie::getTIFFParameters()
{
	return m_tiff_params;
}

void myTiffMovie::setBottomLeft(vec2 bottom_left)
{
	m_glImage->getRect().setBottomLeft(bottom_left);

}

void myTiffMovie::setTopRight(vec2 top_right)
{
	m_glImage->getRect().setTopRight(top_right);
}
