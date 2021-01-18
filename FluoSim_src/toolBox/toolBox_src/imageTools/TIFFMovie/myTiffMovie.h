
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

#ifndef MYTIFFMOVIE_H
#define MYTIFFMOVIE_H

#include "glGUI/glWidgets/myGLScreen/myGLScreen.h"
#include "imageTools/image_format/tiff_format.h"


//already defined in tiff_format...

//struct myTIFFParameters
//{
//	uint32 height = -1;
//	uint32 width = -1;
//	uint16 bitsPerSample = -1;
//	uint16 bytesPerSample = -1;
//	uint16 samplesPerPixel = -1;
//	uint32 numberDirectory = 0;
//};

class myTiffMovie
{
public :

	myTiffMovie();
	~myTiffMovie();

	void openTiff(string tiffPath);
	void loadTiffPlane();
	void renderTiffPlane(myGLObject* render_target);
	void setPlane(int current_plane);
	void setBottomLeft(glm::vec2 bottom_left);
	void setTopRight(glm::vec2 top_right);

	uint32 getNbDirectories();
	myTIFFParameters getTIFFParameters();

private :

	gstd::gMultiVector<glm::vec2> m_parent_gMV;
	myGLImage* m_glImage;
	TIFF* m_tiff;

	string m_tiffPath;
	myTIFFParameters m_tiff_params;
	long m_currentPlane;
	uint8* m_buffer;

};


#endif // MYTIFFMOVIE_H
