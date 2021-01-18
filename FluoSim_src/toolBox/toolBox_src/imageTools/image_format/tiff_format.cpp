
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



#include "tiff_format.h"

using namespace std;
using namespace glm;

myTiff::myTiff()
{
	m_tiff_hdl = 0;
	m_hasBeenOpened = false;
	m_opening_mode = READ_MODE;
}

bool myTiff::open(string tiff_path, OPENING_MODE mode)
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

			uint16 bitsPerSample_v[4] = {0,0,0,0};
			TIFFGetField(m_tiff_hdl, TIFFTAG_BITSPERSAMPLE, bitsPerSample_v);
            TIFFGetField(m_tiff_hdl, TIFFTAG_SAMPLESPERPIXEL, &m_tiff_params.samplesPerPixel);
			TIFFGetField(m_tiff_hdl, TIFFTAG_ROWSPERSTRIP, &m_tiff_params.rowsPerStrip);
			TIFFGetField(m_tiff_hdl, TIFFTAG_PLANARCONFIG, &m_tiff_params.planarConfig);

			TIFFGetField(m_tiff_hdl, TIFFTAG_TILEWIDTH, &m_tiff_params.tileWidth);
			TIFFGetField(m_tiff_hdl, TIFFTAG_TILELENGTH, &m_tiff_params.tileHeight);
			TIFFGetField(m_tiff_hdl, TIFFTAG_ORIENTATION, &m_tiff_params.orientation);

//			for(uint i=0; i<3;i++)
//			{
//				if(m_tiff_params.bytesPerSample[i] != -1)
//				{
					m_tiff_params.bytesPerSample = m_tiff_params.bitsPerSample/8;
//				}
//			}

			m_hasBeenOpened = true;
			TIFFClose(m_tiff_hdl);
			m_tiff_hdl = 0;
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
			m_tiff_hdl = 0;
		}
		break;

		case APPEND_MODE :
		{
			m_tiff_hdl = TIFFOpen(tiff_path.data(), "a");
			if(m_tiff_hdl == 0)
			{
				m_hasBeenOpened = false;
				return 0; //->
			}

			m_opening_mode = APPEND_MODE;
			m_tiff_path = tiff_path;
			m_hasBeenOpened = true;
			TIFFClose(m_tiff_hdl);
			m_tiff_hdl = 0;
		}
		break;
	}


	return 1;
}

void myTiff::close()
{
	if(m_tiff_hdl != 0)
	{
		TIFFClose(m_tiff_hdl);
		m_tiff_hdl = 0;
	}

	myTIFFParameters initial_parameters;
	m_tiff_params = initial_parameters;
	m_tiff_path = "";
	m_hasBeenOpened = false;
}

void myTiff::setTiffSize(glm::vec2 size)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.width = size.x;
	m_tiff_params.height = size.y;
}

void myTiff::setBitsPerSample(uint16 bitsPerSample)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.bitsPerSample = bitsPerSample;
	m_tiff_params.bytesPerSample = bitsPerSample/8;
}

void myTiff::setBytesPerSample(uint16 bytesPerSample)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.bytesPerSample = bytesPerSample;
	m_tiff_params.bitsPerSample = bytesPerSample*8;
}

void myTiff::setSamplesPerPixel(uint16 samplesPerPixel)
{
	if(m_opening_mode == READ_MODE) return; //->

	m_tiff_params.samplesPerPixel = samplesPerPixel;
}

glm::vec2 myTiff::getTiffSize()
{
	return {m_tiff_params.width, m_tiff_params.height};

}

uint16 myTiff::getBitsPerSample()
{
	return m_tiff_params.bitsPerSample;
}

uint16 myTiff::getBytesPerSample()
{
	return m_tiff_params.bytesPerSample;
}

uint16 myTiff::getSamplesPerPixel()
{
	return m_tiff_params.samplesPerPixel;
}

myTIFFParameters myTiff::getTIFFParameters()
{
	return m_tiff_params;
}

void myTiff::addPage(uint8* data_inBytes)
{
	if((m_opening_mode != WRITE_MODE && m_opening_mode != APPEND_MODE) ||
	   m_hasBeenOpened != true) return; //->

	const char* opening_mode = m_opening_mode == WRITE_MODE ? "w" : "a";
	m_tiff_hdl = TIFFOpen(m_tiff_path.data(), opening_mode);

	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGEWIDTH, m_tiff_params.width);
	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGELENGTH, m_tiff_params.height);
	TIFFSetField(m_tiff_hdl, TIFFTAG_SAMPLESPERPIXEL, m_tiff_params.samplesPerPixel);
	TIFFSetField(m_tiff_hdl, TIFFTAG_BITSPERSAMPLE, m_tiff_params.bitsPerSample);
	TIFFSetField(m_tiff_hdl, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
	TIFFSetField(m_tiff_hdl, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(m_tiff_hdl, TIFFTAG_ROWSPERSTRIP, 1);
	TIFFSetField(m_tiff_hdl, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

	if(m_tiff_params.samplesPerPixel == 1)	TIFFSetField(m_tiff_hdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	else TIFFSetField(m_tiff_hdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	TIFFSetField(m_tiff_hdl, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
//	TIFFSetField(m_tiff_hdl, TIFFTAG_IMAGEDEPTH, m_tiff_params.bitsPerSample);

	for(int line_idx = 0; line_idx <= m_tiff_params.height-1; line_idx++)
	{
		TIFFWriteScanline(m_tiff_hdl, data_inBytes + (m_tiff_params.width*m_tiff_params.samplesPerPixel)*line_idx*m_tiff_params.bytesPerSample, line_idx);
	}

	TIFFWriteDirectory(m_tiff_hdl);
	TIFFClose(m_tiff_hdl);
	m_tiff_hdl = 0;
}

bool myTiff::getPage(uint8* &data, int page_idx)
{
	if(m_hasBeenOpened != true ||
//	   m_tiff_hdl == 0 ||
	   m_opening_mode != READ_MODE ||
	   page_idx < 0 ||
	   page_idx >= m_tiff_params.numberDirectory-1) return 0; //->

	m_tiff_hdl = TIFFOpen(m_tiff_path.c_str(), "r");

//	TIFFSetDirectory(m_tiff_hdl, page_idx);

//	for(int row = 0; row < m_tiff_params.height-1; row++)
//	{
//		TIFFReadScanline(m_tiff_hdl, data + row*m_tiff_params.width*m_tiff_params.bytesPerSample*m_tiff_params.samplesPerPixel, row);
//	}

	TIFFSetDirectory(m_tiff_hdl, page_idx);

	m_tiff_params.nbStrips = TIFFNumberOfStrips(m_tiff_hdl);
	m_tiff_params.stripSize = TIFFStripSize(m_tiff_hdl);

	data = new uint8[m_tiff_params.nbStrips*m_tiff_params.stripSize];
	for(uint strip_idx = 0; strip_idx < m_tiff_params.nbStrips; strip_idx++)
	{
		TIFFReadEncodedStrip(m_tiff_hdl, strip_idx, data + strip_idx*m_tiff_params.stripSize, (tsize_t) -1);
	}

	TIFFClose(m_tiff_hdl);
	m_tiff_hdl = 0;

	return 1;
}

bool myTiff::getPage(std::vector<uint8> &data, int page_idx)
{
	if(m_hasBeenOpened != true ||
//	   m_tiff_hdl == 0 ||
	   m_opening_mode != READ_MODE ||
	   page_idx < 0 ||
	   page_idx >= m_tiff_params.numberDirectory-1) return 0; //->

	m_tiff_hdl = TIFFOpen(m_tiff_path.c_str(), "r");
	TIFFSetDirectory(m_tiff_hdl, page_idx);

	m_tiff_params.nbStrips = TIFFNumberOfStrips(m_tiff_hdl);
	m_tiff_params.stripSize = TIFFStripSize(m_tiff_hdl);

	data.assign(m_tiff_params.nbStrips*m_tiff_params.stripSize, 0x00);
	for(uint strip_idx = 0; strip_idx < m_tiff_params.nbStrips; strip_idx++)
	{
		TIFFReadEncodedStrip(m_tiff_hdl, strip_idx, data.data() + strip_idx*m_tiff_params.stripSize, (tsize_t) -1);
	}

	TIFFClose(m_tiff_hdl);
	m_tiff_hdl = 0;


	return 1;
}


void myTiff::tellTiffParameters()
{
	cout<<"height = "<<m_tiff_params.height<<"\n";
	cout<<"width = "<<m_tiff_params.width<<"\n";
	cout<<"bitsPerSample = "<<m_tiff_params.bitsPerSample<<"\n";
	cout<<"bytesPerSample = "<<m_tiff_params.bytesPerSample<<"\n";
	cout<<"samplesPerPixel = "<<m_tiff_params.samplesPerPixel<<"\n";
	cout<<"numberDirectory = "<<m_tiff_params.numberDirectory<<"\n";
	cout<<"number RowsPerStrip = "<<m_tiff_params.rowsPerStrip<<"\n";
	cout<<"number strips = "<<m_tiff_params.nbStrips<<"\n";
	cout<<"strip size = "<<m_tiff_params.stripSize<<"\n";
	cout<<"planarConfig = "<<m_tiff_params.planarConfig<<"\n";
	cout<<"tileWidth = "<<m_tiff_params.tileWidth<<"\n";
	cout<<"tileHeight = "<<m_tiff_params.tileHeight<<"\n";
	cout<<"orientation = "<<m_tiff_params.orientation<<"\n";
}

bool myTiff::hasBeenOpened()
{
	return m_hasBeenOpened;
}






