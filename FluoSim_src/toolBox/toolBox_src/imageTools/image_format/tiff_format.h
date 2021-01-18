
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

#ifndef TIFF_FORMAT_H
#define TIFF_FORMAT_H

#include "glm.hpp"
#include "tiffio.h"
#include "tiff.h"
#include "iostream"

#include "toolbox_library_global.h"

struct myTIFFParameters
{
	uint32 height = -1;
	uint32 width = -1;
	uint16 bitsPerSample = -1;
	uint16 bytesPerSample = -1;
	uint16 samplesPerPixel = -1;
	uint32 rowsPerStrip = -1;
	uint32 nbStrips = -1;
	uint32 stripSize = -1;
	uint32 numberDirectory = 0;
	uint16 planarConfig = 1;
	uint16 tileWidth = 0;
	uint16 tileHeight = 0;
	uint16 orientation = -1;

};



class TOOLBOXSHARED_EXPORT myTiff
{
public :

	enum OPENING_MODE {READ_MODE, WRITE_MODE, APPEND_MODE};

	myTiff();

	bool open(std::string tiff_path, OPENING_MODE mode);
	void close();

    //only for writing
	void setTiffSize(glm::vec2 size);
	void setBitsPerSample(uint16 bitsPerSample);
	void setBytesPerSample(uint16 bytesPerSample);
	void setSamplesPerPixel(uint16 samplesPerPixel);

	glm::vec2 getTiffSize();
	uint16 getBitsPerSample();
	uint16 getBytesPerSample();
	uint16 getSamplesPerPixel();
	myTIFFParameters getTIFFParameters();

	void addPage(uint8* data);
	bool getPage(uint8* &data, int page_idx);
	bool getPage(std::vector<uint8> &data, int page_idx);

	void tellTiffParameters();

	bool hasBeenOpened();

private :

	std::string m_tiff_path;
	TIFF* m_tiff_hdl;

	bool m_hasBeenOpened;
	myTiff::OPENING_MODE m_opening_mode = READ_MODE;

	myTIFFParameters m_tiff_params;

};


#endif // TIFF_FORMAT_H
