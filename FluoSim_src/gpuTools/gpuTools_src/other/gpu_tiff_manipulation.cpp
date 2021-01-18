
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




#include "gpu_tiff_manipulation.h"

using namespace std;
using namespace glm;

_myTiff::_myTiff()
{
	m_tiff_hdl = 0;
	m_hasBeenOpened = false;
	m_opening_mode = READ_MODE;
}

bool _myTiff::open(string tiff_path, OPENING_MODE mode)
{
	switch (mode)
	{
		case READ_MODE :
		{
			m_tiff_hdl = TIFFOpen(tiff_path.data(), "r");
			if(m_tiff_hdl == 0)
			{
				m_hasBeenOpened = false;
				return 0; //->
			}

			m_opening_mode = READ_MODE;
			m_tiff_path = tiff_path;
			m_tiff_params.numberDirectory = 0;
			while(TIFFReadDirectory(m_tiff_hdl)) m_tiff_params.numberDirectory++;

			TIFFSetDirectory(m_tiff_hdl, 0);
			TIFFGetField(m_tiff_hdl, TIFFTAG_IMAGELENGTH, &m_tiff_params.height);
			TIFFGetField(m_tiff_hdl, TIFFTAG_IMAGEWIDTH, &m_tiff_params.width);
			TIFFGetField(m_tiff_hdl, TIFFTAG_BITSPERSAMPLE, &m_tiff_params.bitsPerSample);
			TIFFGetField(m_tiff_hdl, TIFFTAG_SAMPLESPERPIXEL, &m_tiff_params.samplesPerPixel);
			m_tiff_params.bytesPerSample = m_tiff_params.bitsPerSample/8;
			cout<<"bytePerSamples : "<<m_tiff_params.bytesPerSample<<"\n";

			m_hasBeenOpened = true;

			TIFFClose(m_tiff_hdl);
		}
		break;


		case WRITE_MODE :
		{
			m_tiff_hdl = TIFFOpen(tiff_path.data(), "w");
			if(m_tiff_hdl == 0)
			{
				m_hasBeenOpened = false;
				return 0; //->
			}


			m_opening_mode = WRITE_MODE;
			m_tiff_path = tiff_path;
			m_hasBeenOpened = true;
			TIFFClose(m_tiff_hdl);
		}
		break;
	}


	return 1;
}

void _myTiff::close()
{
	if(m_tiff_hdl) TIFFClose(m_tiff_hdl);
}

void _myTiff::setTiffSize(glm::vec2 size)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.width = size.x;
	m_tiff_params.height = size.y;
}

void _myTiff::setBitsPerSample(uint16 bitsPerSample)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.bitsPerSample = bitsPerSample;
	m_tiff_params.bytesPerSample = bitsPerSample/8;
}

void _myTiff::setBytesPerSample(uint16 bytesPerSample)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.bytesPerSample = bytesPerSample;
	m_tiff_params.bitsPerSample = bytesPerSample*8;
}

void _myTiff::setSamplesPerPixel(uint16 samplesPerPixel)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.samplesPerPixel = samplesPerPixel;
}

glm::vec2 _myTiff::getTiffSize()
{
	return {m_tiff_params.width, m_tiff_params.height};

}

uint16 _myTiff::getBitsPerSample()
{
	return m_tiff_params.bitsPerSample;
}

uint16 _myTiff::getBytesPerSample()
{
	return m_tiff_params.bytesPerSample;
}

uint16 _myTiff::getSamplesPerPixel()
{
	return m_tiff_params.samplesPerPixel;
}

void _myTiff::addPage(uint8* data_inBytes)
{
	if(m_opening_mode != WRITE_MODE ||
	   m_hasBeenOpened != true) return; //->

	m_tiff_hdl = TIFFOpen(m_tiff_path.data(), "w");


	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGEWIDTH, m_tiff_params.width);
	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGELENGTH, m_tiff_params.height);
	TIFFSetField(m_tiff_hdl, TIFFTAG_SAMPLESPERPIXEL, m_tiff_params.samplesPerPixel);
	TIFFSetField(m_tiff_hdl, TIFFTAG_BITSPERSAMPLE, m_tiff_params.bitsPerSample);
	TIFFSetField(m_tiff_hdl, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
	TIFFSetField(m_tiff_hdl, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	if(m_tiff_params.samplesPerPixel == 1)	TIFFSetField(m_tiff_hdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	else TIFFSetField(m_tiff_hdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	TIFFSetField(m_tiff_hdl, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGEDEPTH, m_tiff_params.bitsPerSample);

	for(int line_idx = 0; line_idx <= m_tiff_params.height-1; line_idx++)
	{
		TIFFWriteScanline(m_tiff_hdl, data_inBytes + (m_tiff_params.width*m_tiff_params.samplesPerPixel)*line_idx*m_tiff_params.bytesPerSample, line_idx);
	}

	TIFFWriteDirectory(m_tiff_hdl);

	TIFFClose(m_tiff_hdl);
}

bool _myTiff::getPage(uint8* &data, int page_idx)
{
	if(m_hasBeenOpened != true ||
//	   m_tiff_hdl == 0 ||
	   m_opening_mode != READ_MODE ||
	   page_idx < 0 ||
	   page_idx >= m_tiff_params.numberDirectory-1) return 0; //->


	m_tiff_hdl = TIFFOpen(m_tiff_path.c_str(), "r");

	cout<<"size : "<<m_tiff_params.height*m_tiff_params.width*m_tiff_params.bytesPerSample*1<<"\n";
	data = new uint8[m_tiff_params.height*m_tiff_params.width*m_tiff_params.bytesPerSample*1];

	TIFFSetDirectory(m_tiff_hdl, page_idx);
	for(int row = 0; row < m_tiff_params.height; row++)
	{
		TIFFReadScanline(m_tiff_hdl, data + row*m_tiff_params.width*m_tiff_params.bytesPerSample*1, row);
	}

	TIFFClose(m_tiff_hdl);

	return 1;
}


void _myTiff::tellTiffParameters()
{
	cout<<"height = "<<m_tiff_params.height<<"\n";
	cout<<"width = "<<m_tiff_params.width<<"\n";
	cout<<"bitsPerSample = "<<m_tiff_params.bitsPerSample<<"\n";
	cout<<"bytesPerSample = "<<m_tiff_params.bytesPerSample<<"\n";
	cout<<"samplesPerPixel = "<<m_tiff_params.samplesPerPixel<<"\n";
	cout<<"numberDirectory = "<<m_tiff_params.numberDirectory<<"\n";
}
