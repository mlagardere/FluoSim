
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

#ifndef MYGLTIFFMOVIE_H
#define  MYGLTIFFMOVIE_H

#include "toolbox_library_global.h"

#include "gpuTools_library_global.h"
	#include "gl/gTexture.h"
	#include "gl/gMultiVector.h"
	#include "imageTools/image_format/tiff_format.h"
#include "glGUI/glObjects/myGLGeomtricObject.h"
#include "glGUI/glObjects/myGLImageObjects.h" //getPixelHistogramR16Image & getPixelMinMaxR16Image
#include "otherFunctions/otherFunctions.h"
#include "tinytiffreader.h"

#include "glm.hpp"

class TOOLBOXSHARED_EXPORT myGLTiffMovie : public myGLObject //for rendering
{
public :

//	myGLTiffMovie(std::string file_dir, glm::vec2 rect_bottomLeft, glm::vec2 rect_size, gstd::gMultiVector<glm::vec2>& parent_gMV);
	myGLTiffMovie(glm::vec2 rect_bottomLeft, glm::vec2 rect_size, gstd::gMultiVector<glm::vec2>& parent_gMV);

	virtual void render(myGLObject* renderingTarget);
	void setIntensity(float intensity);
	void setIsXInverted(bool isInverted);
	void setIsYInverted(bool isInverted);

	void openTiffMovie(string tiff_path);
	void loadTiffPage(uint directory);
	myTIFFParameters getTiffParameters();
	void closeTiffMovie();

	void setTexCoordinates();
	void setTexSize(glm::vec2 size);

	myGLRectangle& getRect();
	gstd::gTexture& getTexture();

	void scaleR16Image();

private :

	float m_intensity = 20.0f;
	float m_GS_minimum = 0;
	float m_GS_maximum = 256*256-1;

	bool m_isStk;
	myTiff m_tiff; //use if file is a tiff
	TinyTIFFReaderFile* m_stk; //used if file is a stack

	gstd::gTexture m_texture;
	gstd::gVector<glm::vec2> m_texCoords;
	myGLRectangle m_imageRect; //we use its vertices to render the image

	bool m_isXInverted;
	bool m_isYInverted;

};


#endif //  MYGLTIFFMOVIE_H
