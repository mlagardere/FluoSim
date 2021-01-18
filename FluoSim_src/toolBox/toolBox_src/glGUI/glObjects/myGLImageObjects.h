
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

#ifndef MYGLIMAGEOBJECTS_H
#define MYGLIMAGEOBJECTS_H

#include "toolbox_library_global.h"

#include "gpuTools_library_global.h"
	#include "gl/gTexture.h"
	#include "gl/gMultiVector.h"
    #include "imageTools/image_format/tiff_format.h"
#include "glGUI/glObjects/myGLGeomtricObject.h"
#include "otherFunctions/otherFunctions.h"

#include "glm.hpp"

class TOOLBOXSHARED_EXPORT myGLImage : public myGLObject //for rendering
{
public :

	myGLImage(std::string file_dir, glm::vec2 rect_bottomLeft, glm::vec2 rect_size, gstd::gMultiVector<glm::vec2>& parent_gMV);
	myGLImage(glm::vec2 rect_bottomLeft, glm::vec2 rect_size, gstd::gMultiVector<glm::vec2>& parent_gMV);

	virtual void render(myGLObject* renderingTarget);
	void setIntensity(float intensity);
	void setIsXInverted(bool isInverted);
	void setIsYInverted(bool isInverted);

	void loadImage(std::string file_dir);
	void loadImage(glm::vec2 image_size, GLenum texture_format,
				   GLenum image_format,GLenum data_type,void* data_prt);
	void loadImage(gstd::gTexture& text);

	void setTexCoordinates();
	void setTexSize(glm::vec2 size);

	myGLRectangle& getRect();
	gstd::gTexture& getTexture();

	void scaleR16Image();

//	void setBottomLeft(glm::vec2 rect_bottomLeft);
//	void setSize(glm::vec2 rect_size);

private :

	float m_intensity = 20.0f;
	float m_GS_minimum = 0;
	float m_GS_maximum = 256*256-1;

	gstd::gTexture m_texture;
	gstd::gVector<glm::vec2> m_texCoords;
	myGLRectangle m_imageRect; //we use its vertices to render the image

	bool m_isXInverted;
	bool m_isYInverted;

};


bool getPixelHistogramR16Image(vector<uint> &data_in, glm::vec2 size,
							   vector<glm::vec2>& histogram);


void getPixelMinMaxR16Image(vector<uint16> &data_in, glm::vec2 size,
                            uint16& pixel_minimum, uint16& pixel_maximum);



#endif // MYGLIMAGEOBJECTS_H
