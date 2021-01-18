
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

#include "myGLImageObjects.h"

using namespace glm;
using namespace std;

myGLImage::myGLImage(string file_dir, vec2 rect_bottomLeft, vec2 rect_size, gstd::gMultiVector<vec2>& parent_gMV) :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT, GL_IMAGE_TYPE),
	m_texture(GL_TEXTURE_2D),
	m_imageRect(parent_gMV)
{
	m_imageRect.setBottomLeft(rect_bottomLeft);
	m_imageRect.setTopRight(rect_bottomLeft + rect_size);
	this->loadImage(file_dir);

	myRenderableObject::m_rendering_mode = IMAGE_GS_32_RENDERING;

	//shaders
	string vs_src;
	vs_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec2 TexCoords_vsIn;\n"
				"out vec2 TexCoords_vsOut;\n"
				"uniform mat4 zoom_matrix;\n"
				"\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = zoom_matrix*temp;\n"
				"	TexCoords_vsOut = TexCoords_vsIn;\n"
				"\n"
				"}\n";

	string fs_RGBA_src;
	fs_RGBA_src +=	"#version 150 core\n\n"
					""
					"in vec2 TexCoords_vsOut;\n"
					"uniform sampler2D texture_id;\n"
					"uniform float intensity;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"//	gl_FragColor=intensity*texture(texture_id, TexCoords_vsOut);\n"
					"	gl_FragColor=vec4(intensity*texture(texture_id, TexCoords_vsOut).xyz, 1.0);\n"
					"}\n";

	string fs_GS_R16_src;
	fs_GS_R16_src +=	"#version 150 core\n\n"
						""
						"in vec2 TexCoords_vsOut;\n"
						"uniform sampler2D texture_id;\n"
						"uniform float intensity;\n"
						"uniform float minimum;\n"
						"uniform float maximum;\n"
						"out vec4 gl_FragColor;\n"
						"\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
						"	float i = (r*(256*256-1)- minimum)/(maximum-minimum);\n"
						"	gl_FragColor = vec4(intensity*i*vec3(1.0f,1.0f,1.0f),1.0f);\n"
						"}\n";

	string fs_GS_8_16_24_src;
	fs_GS_8_16_24_src +=	"#version 150 core\n\n"
					""
					"in vec2 TexCoords_vsOut;\n"
					"uniform sampler2D texture_id;\n"
					"uniform float intensity;\n"
					"uniform float minimum;\n"
					"uniform float maximum;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"//	gl_FragColor=intensity*texture(texture_id, TexCoords_vsOut);\n"
					"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
					"	float g = texture(texture_id, TexCoords_vsOut).y;\n"
					"	float b = texture(texture_id, TexCoords_vsOut).z;\n"
					"	float i = (r*255 + g*255*256 + b*255*256*256 - minimum)/(maximum-minimum);\n"
					"	gl_FragColor = vec4(intensity*i*vec3(1.0f,1.0f,1.0f),1.0f);\n"
					"}\n";

	string fs_GS_32_src;
	fs_GS_32_src +=	"#version 150 core\n\n"
					""
					"in vec2 TexCoords_vsOut;\n"
					"uniform sampler2D texture_id;\n"
					"uniform float intensity;\n"
					"uniform float minimum;\n"
					"uniform float maximum;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"//	gl_FragColor=intensity*texture(texture_id, TexCoords_vsOut);\n"
					"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
					"	float g = texture(texture_id, TexCoords_vsOut).y;\n"
					"	float b = texture(texture_id, TexCoords_vsOut).z;\n"
					"	float a = texture(texture_id, TexCoords_vsOut).w;\n"
					"	float i = (r*255 + g*255*256 + b*255*256*256 + a*255*256*256*256 - minimum)/(maximum-minimum);\n"
//					"	float i = (r*255 + g*255*256)/(256.0f*256.0f);\n"
					"	gl_FragColor=    vec4(intensity*i*vec3(1,1,1),1.0f);\n"
					"//	gl_FragColor=vec4(intensity*texture(texture_id, TexCoords_vsOut).xyz, 1.0);\n"
					"}\n";

	gstd::gProgram pgmGS_8_16_24("image_GS_8_16_24_rendering");
	gstd::gProgram pgmGS_R16("image_GS_R16_rendering");
	gstd::gProgram pgmGS_32("image_GS_32_rendering");
	gstd::gProgram pgmRGBA("image_RGBA_rendering2");

	gstd::gShader vs(gstd::VERTEX_SHADER);
		vs.setSource(vs_src, 0);
		vs.compile();

	gstd::gShader fs_GS_8_16_24(gstd::FRAGMENT_SHADER);
		fs_GS_8_16_24.setSource(fs_GS_8_16_24_src, 0);
		fs_GS_8_16_24.compile();

	gstd::gShader fs_GS_32(gstd::FRAGMENT_SHADER);
		fs_GS_32.setSource(fs_GS_32_src, 0);
		fs_GS_32.compile();

	gstd::gShader fs_GS_R16(gstd::FRAGMENT_SHADER);
		fs_GS_R16.setSource(fs_GS_R16_src, 0);
		fs_GS_R16.compile();

	gstd::gShader fs_RGBA(gstd::FRAGMENT_SHADER);
		fs_RGBA.setSource(fs_RGBA_src, 0);
		fs_RGBA.compile();

	pgmGS_8_16_24.addShader(vs);
	pgmGS_8_16_24.addShader(fs_GS_8_16_24);
	pgmGS_8_16_24.linkShaders();

	pgmGS_32.addShader(vs);
	pgmGS_32.addShader(fs_GS_32);
	pgmGS_32.linkShaders();

	pgmGS_R16.addShader(vs);
	pgmGS_R16.addShader(fs_GS_R16);
	pgmGS_R16.linkShaders();

	pgmRGBA.addShader(vs);
	pgmRGBA.addShader(fs_RGBA);
	pgmRGBA.linkShaders();

	myGLObject::addProgram(pgmGS_8_16_24);
	myGLObject::addProgram(pgmGS_32);
	myGLObject::addProgram(pgmRGBA);
	myGLObject::addProgram(pgmGS_R16);

	m_intensity = 1.0f;

	m_isXInverted = 0;
	m_isYInverted = 1;//from myGLRectangle : 0 is bottomLeft, 1 is topLeft, ...

	setTexCoordinates();

}

myGLImage::myGLImage(glm::vec2 rect_bottomLeft, glm::vec2 rect_size, gstd::gMultiVector<vec2>& parent_gMV) :

	myGLObject(myGLObject::IS_RENDERABLE_OBJECT, GL_IMAGE_TYPE),
	m_texture(GL_TEXTURE_2D),
	m_imageRect(parent_gMV)

{
	m_imageRect.setBottomLeft(rect_bottomLeft);
	m_imageRect.setTopRight(rect_bottomLeft + rect_size);

	myRenderableObject::m_rendering_mode = IMAGE_GS_32_RENDERING;

	string vs_src;
	vs_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec2 TexCoords_vsIn;\n"
				"out vec2 TexCoords_vsOut;\n"
				"uniform mat4 zoom_matrix;\n"
				"\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = zoom_matrix*temp;\n"
				"	TexCoords_vsOut = TexCoords_vsIn;\n"
				"\n"
				"}\n";

	string fs_RGBA_src;
	fs_RGBA_src +=	"#version 150 core\n\n"
				""
				"in vec2 TexCoords_vsOut;\n"
				"uniform sampler2D texture_id;\n"
				"uniform float intensity;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=vec4(intensity*texture(texture_id, TexCoords_vsOut).xyz, 1.0);\n"
				"}\n";

	string fs_GS_R16_src;
	fs_GS_R16_src +=	"#version 150 core\n\n"
						""
						"in vec2 TexCoords_vsOut;\n"
						"uniform sampler2D texture_id;\n"
						"uniform float intensity;\n"
						"uniform float minimum;\n"
						"uniform float maximum;\n"
						"out vec4 gl_FragColor;\n"
						"\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
						"	float i = (r*(256*256-1)- minimum)/(maximum-minimum);\n"
						"	gl_FragColor = vec4(intensity*i*vec3(1.0f,1.0f,1.0f),1.0f);\n"
						"}\n";

	string fs_GS_8_16_24_src;
	fs_GS_8_16_24_src +=	"#version 150 core\n\n"
					""
					"in vec2 TexCoords_vsOut;\n"
					"uniform sampler2D texture_id;\n"
					"uniform float intensity;\n"
					"uniform float minimum;\n"
					"uniform float maximum;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"//	gl_FragColor=intensity*texture(texture_id, TexCoords_vsOut);\n"
					"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
					"	float g = texture(texture_id, TexCoords_vsOut).y;\n"
					"	float b = texture(texture_id, TexCoords_vsOut).z;\n"
					"	float i = (r*255 + g*(256*256-1) + b*(256*256*256-1) - minimum)/(maximum-minimum);\n"
					"	gl_FragColor=    vec4(intensity*i*vec3(1.0f,1.0f,1.0f),1.0f);\n"
					"}\n";

	string fs_GS_32_src;
	fs_GS_32_src +=	"#version 150 core\n\n"
					""
					"in vec2 TexCoords_vsOut;\n"
					"uniform sampler2D texture_id;\n"
					"uniform float intensity;\n"
					"uniform float minimum;\n"
					"uniform float maximum;\n"
					"out vec4 gl_FragColor;\n"
					"\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"//	gl_FragColor=intensity*texture(texture_id, TexCoords_vsOut);\n"
					"	float r = texture(texture_id, TexCoords_vsOut).x;\n"
					"	float g = texture(texture_id, TexCoords_vsOut).y;\n"
					"	float b = texture(texture_id, TexCoords_vsOut).z;\n"
					"	float a = texture(texture_id, TexCoords_vsOut).w;\n"
					"	float i = (r*255 + g*255*256 + b*255*256*256 + a*255*256*256*256 - minimum)/(maximum-minimum);\n"
//					"	float i = (r*255 + g*255*256)/(256.0f*256.0f);\n"
					"	gl_FragColor=    vec4(1.0f*intensity*i*vec3(1,1,1),1.0f);\n"
					"//	gl_FragColor=vec4(intensity*texture(texture_id, TexCoords_vsOut).xyz, 1.0);\n"
					"}\n";



	gstd::gProgram pgmGS_8_16_24("image_GS_8_16_24_rendering");
	gstd::gProgram pgmGS_R16("image_GS_R16_rendering");
	gstd::gProgram pgmGS_32("image_GS_32_rendering");
	gstd::gProgram pgmRGBA("image_RGBA_rendering2");

	gstd::gShader vs(gstd::VERTEX_SHADER);
		vs.setSource(vs_src, 0);
		vs.compile();

	gstd::gShader fs_GS_8_16_24(gstd::FRAGMENT_SHADER);
		fs_GS_8_16_24.setSource(fs_GS_8_16_24_src, 0);
		fs_GS_8_16_24.compile();

	gstd::gShader fs_GS_32(gstd::FRAGMENT_SHADER);
		fs_GS_32.setSource(fs_GS_32_src, 0);
		fs_GS_32.compile();

	gstd::gShader fs_GS_R16(gstd::FRAGMENT_SHADER);
		fs_GS_R16.setSource(fs_GS_R16_src, 0);
		fs_GS_R16.compile();

	gstd::gShader fs_RGBA(gstd::FRAGMENT_SHADER);
		fs_RGBA.setSource(fs_RGBA_src, 0);
		fs_RGBA.compile();

	pgmGS_8_16_24.addShader(vs);
	pgmGS_8_16_24.addShader(fs_GS_8_16_24);
	pgmGS_8_16_24.linkShaders();

	pgmGS_32.addShader(vs);
	pgmGS_32.addShader(fs_GS_32);
	pgmGS_32.linkShaders();

	pgmGS_R16.addShader(vs);
	pgmGS_R16.addShader(fs_GS_R16);
	pgmGS_R16.linkShaders();

	pgmRGBA.addShader(vs);
	pgmRGBA.addShader(fs_RGBA);
	pgmRGBA.linkShaders();

	myGLObject::addProgram(pgmGS_8_16_24);
	myGLObject::addProgram(pgmGS_32);
	myGLObject::addProgram(pgmRGBA);
	myGLObject::addProgram(pgmGS_R16);

	m_intensity = 1.0f;

	m_isXInverted = 0;
	m_isYInverted = 1;//from myGLRectangle : 0 is bottomLeft, 1 is topLeft, ...

	setTexCoordinates();
}

void myGLImage::loadImage(string file_dir)
{
//	m_image.loadTexture(file_dir);

	if(QString(file_dir.c_str()).endsWith(".tiff") ||
	   QString(file_dir.c_str()).endsWith(".tif"))
	{
		myTiff tiff;
		if(tiff.open(file_dir, myTiff::READ_MODE) == false) return; //->

		vector<uint8> pixels;
		myTIFFParameters tiff_params = tiff.getTIFFParameters();
		tiff.getPage(pixels, 0);
		tiff.tellTiffParameters();

		enum SAMPLES_PER_PIXEL {ZERO_SAMPLE_PER_PIXEL, ONE_SAMPLE_PER_PIXEL, TWO_SAMPLES_PER_PIXEL,
								THREE_SAMPLES_PER_PIXEL, FOUR_SAMPLES_PER_PIXEL};
		enum BYTES_PER_SAMPLE  {ZERO_BYTE_PER_SAMPLE, ONE_BYTE_PER_SAMPLE, TWO_BYTES_PER_SAMPLE,
								THREE_BYTES_PER_SAMPLE, FOUR_BYTES_PER_SAMPLE};
		switch(tiff_params.bytesPerSample)
		{
			case ONE_BYTE_PER_SAMPLE:
			{
				switch(tiff_params.samplesPerPixel)
				{
					case ONE_SAMPLE_PER_PIXEL:
					{
						m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
											GL_RED, GL_UNSIGNED_BYTE, pixels.data());

						uint32 pixel_minimum = std::numeric_limits<uint8>::max();
						uint32 pixel_maximum = std::numeric_limits<uint8>::min();
						for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
						{
							uint8* pixel = pixels.data()+pixel_idx;
							uint8 pixel_value = *pixel;

							if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
							if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
						}

						m_GS_minimum = pixel_minimum;
						m_GS_maximum = pixel_maximum;

						myRenderableObject::m_rendering_mode = IMAGE_GS_8_16_24_RENDERING;
					}
					break;

					case TWO_SAMPLES_PER_PIXEL:
					{
						m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
											GL_RG, GL_UNSIGNED_BYTE, pixels.data());

						uint32 pixel_minimum = std::numeric_limits<uint16>::max();
						uint32 pixel_maximum = std::numeric_limits<uint16>::min();
						for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
						{
							uint16* pixel = (uint16*)(pixels.data()) + pixel_idx;
							uint16 pixel_value = *pixel;

							if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
							if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
						}

						m_GS_minimum = pixel_minimum;
						m_GS_maximum = pixel_maximum;

						myRenderableObject::m_rendering_mode = IMAGE_GS_8_16_24_RENDERING;

					}
					break;

					case THREE_SAMPLES_PER_PIXEL:
					{
						m_texture.bindTexture();
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
											GL_RGB, GL_UNSIGNED_BYTE, pixels.data(), true);
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

						uint32 pixel_minimum = std::numeric_limits<uint32>::max();
						uint32 pixel_maximum = std::numeric_limits<uint32>::min();
						for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
						{
							uint32* pixel = (uint32*) (pixels.data() + 3*pixel_idx);
							uint32 pixel_value = (*pixel & 0xFFFFFF00)>>8;
							if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
							if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
						}

						m_GS_minimum = pixel_minimum;
						m_GS_maximum = pixel_maximum;

//						myRenderableObject::m_rendering_mode = IMAGE_GS_8_16_24_RENDERING;
						myRenderableObject::m_rendering_mode = IMAGE_RGBA_RENDERING;
					}
					break;

					case FOUR_SAMPLES_PER_PIXEL:
					{
						m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
											GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true);

						uint32 pixel_minimum = std::numeric_limits<uint32>::max();
						uint32 pixel_maximum = std::numeric_limits<uint32>::min();
						for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
						{
							uint32* pixel = (uint32*)(pixels.data()) + pixel_idx;
							uint32 pixel_value = *pixel;

							if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
							if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
						}

						m_GS_minimum = pixel_minimum;
						m_GS_maximum = pixel_maximum;

						myRenderableObject::m_rendering_mode = IMAGE_GS_32_RENDERING;
					}
					break;
				}
			}
			break;

			case TWO_BYTES_PER_SAMPLE:
			{
				m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
									GL_RG, GL_UNSIGNED_BYTE, pixels.data());
				vector<uint8> samples;
				m_texture.getTextureData(samples);

				uint32 pixel_minimum = std::numeric_limits<uint16>::max();
				uint32 pixel_maximum = std::numeric_limits<uint16>::min();
				for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
				{
					uint16* pixel = (uint16*)(pixels.data()) + pixel_idx;
					uint16 pixel_value = *pixel;

					if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
					if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
				}

				m_GS_minimum = pixel_minimum;
				m_GS_maximum = pixel_maximum;

				myRenderableObject::m_rendering_mode = IMAGE_GS_8_16_24_RENDERING;
			}
			break;


			case THREE_BYTES_PER_SAMPLE:
			{
				m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
									GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

				uint32 pixel_minimum = std::numeric_limits<uint32>::max();
				uint32 pixel_maximum = std::numeric_limits<uint32>::min();
				for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
				{
					uint32* pixel = (uint32*) (pixels.data() + 3*pixel_idx);
					uint32 pixel_value = (*pixel & 0xFFFFFF00)>>8;

					if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
					if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
				}

				m_GS_minimum = pixel_minimum;
				m_GS_maximum = pixel_maximum;

				myRenderableObject::m_rendering_mode = IMAGE_GS_8_16_24_RENDERING;

			}
			break;

			case FOUR_BYTES_PER_SAMPLE:
			{
				m_texture.loadTexture(tiff_params.width, tiff_params.height, GL_RGBA,
									GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

				uint32 pixel_minimum = std::numeric_limits<uint32>::max();
				uint32 pixel_maximum = std::numeric_limits<uint32>::min();
				for(int pixel_idx = 0; pixel_idx < tiff_params.width*tiff_params.height; pixel_idx++)
				{
					uint32* pixel = (uint32*)(pixels.data()) + pixel_idx;
					uint32 pixel_value = *pixel;

					if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
					if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
				}

				m_GS_minimum = pixel_minimum;
				m_GS_maximum = pixel_maximum;

				myRenderableObject::m_rendering_mode = IMAGE_GS_32_RENDERING;
			}
			break;
		}
	}

	else
	{
		myGLObject::setRenderingMode(IMAGE_RGBA_RENDERING);

		myRenderableObject::m_rendering_mode = IMAGE_RGBA_RENDERING;

		QImage image(QString(file_dir.data()));

		int size_x = image.size().width();
		int size_y = image.size().height();

		if(image.isNull()) return;

		QImage image_argb32 = image.convertToFormat(QImage::Format_ARGB32);

		uint8* data_8;
		data_8 = new uint8[4*size_x*size_y];
		data_8 = (uint8*) convertBGRA32toRGBA32(image_argb32.bits(), 4*size_x*size_y);

		m_texture.loadTexture(size_x, size_y, GL_RGBA,
							  GL_RGBA, GL_UNSIGNED_BYTE, data_8);

		delete[] data_8;
	}
	return;
}

void myGLImage::loadImage(vec2 image_size, GLenum texture_format,
						  GLenum image_format,GLenum data_type,void* data_prt)
{
	m_texture.loadTexture(image_size.x, image_size.y,
						texture_format, image_format, data_type,
						data_prt);
}

void myGLImage::loadImage(gstd::gTexture& text)
{
	m_texture = text;
}


void myGLImage::render(myGLObject* renderingTarget)
{
	if(renderingTarget->isRenderingTargetObject() == false) return; //->

	vec2 S = renderingTarget->size();
	mat4 zoom_matrix;
	switch(m_space)
	{
		case WORLD_SPACE : zoom_matrix = renderingTarget->getWorldToExtendedHomMatrix();
		break;

		case HOMOGENEOUS_SCREEN_SPACE : zoom_matrix = mat4(1.0);
		break;

		case GL_SCREEN_SPACE : zoom_matrix = mat4(2/S.x, 0, 0, -1,
												  0, 2/S.y, 0, -1,
												  0, 0, 0, 0,
												  0, 0, 0, 1);
		break;

		case SCREEN_SPACE : zoom_matrix = mat4(+2/S.x, 0, 0, -1,
											   0, -2/S.y, 0, +1,
											   0, 0, 0, 0,
											   0, 0, 0, 1);
		break;
	}


	switch(m_rendering_mode)
	{
		case IMAGE_GS_8_16_24_RENDERING :
		{
			gstd::gProgram& pgm1 = getProgram(0);
			pgm1.useProgram(true);

			gstd::myConnector<vec2>::connect(pgm1, "R", m_imageRect.getgVector());
			gstd::myConnector<vec2>::connect(pgm1, "TexCoords_vsIn", m_texCoords);

			gstd::connectUniform(pgm1, "zoom_matrix", &zoom_matrix, 1);
			gstd::connectUniform(pgm1, "intensity", &m_intensity, 1);
			gstd::connectUniform(pgm1, "minimum", &m_GS_minimum, 1);
			gstd::connectUniform(pgm1, "maximum", &m_GS_maximum, 1);
			gstd::connectTexture(pgm1, "texture_id", m_texture);

			glDrawArrays(GL_QUADS, 0, 4);

			pgm1.useProgram(0);
		}
		break;

		case IMAGE_GS_32_RENDERING :
		{
			gstd::gProgram& pgm1 = getProgram(1);
			pgm1.useProgram(true);

			gstd::myConnector<vec2>::connect(pgm1, "R", m_imageRect.getgVector());
			gstd::myConnector<vec2>::connect(pgm1, "TexCoords_vsIn", m_texCoords);

			gstd::connectUniform(pgm1, "zoom_matrix", &zoom_matrix, 1);
			gstd::connectUniform(pgm1, "intensity", &m_intensity, 1);
			gstd::connectUniform(pgm1, "minimum", &m_GS_minimum, 1);
			gstd::connectUniform(pgm1, "maximum", &m_GS_maximum, 1);
			gstd::connectTexture(pgm1, "texture_id", m_texture);

			glDrawArrays(GL_QUADS, 0, 4);

			pgm1.useProgram(0);
		}
		break;

		case IMAGE_RGBA_RENDERING :
		{
			gstd::gProgram& pgm2 = getProgram(2);
			pgm2.useProgram(1);

			gstd::myConnector<vec2>::connect(pgm2, "R", m_imageRect.getgVector());
			gstd::myConnector<vec2>::connect(pgm2, "TexCoords_vsIn", m_texCoords);

			gstd::connectUniform(pgm2, "zoom_matrix", &zoom_matrix, 1);
			gstd::connectUniform(pgm2, "intensity", &m_intensity, 1);
			gstd::connectTexture(pgm2, "texture_id", m_texture);

			glDrawArrays(GL_QUADS, 0, 4);

			pgm2.useProgram(0);
		}
		break;

		case IMAGE_GS_R16_RENDERING :
		{
			gstd::gProgram& pgm3 = getProgram(3);
			pgm3.useProgram(true);

			gstd::myConnector<vec2>::connect(pgm3, "R", m_imageRect.getgVector());
			gstd::myConnector<vec2>::connect(pgm3, "TexCoords_vsIn", m_texCoords);

			gstd::connectUniform(pgm3, "zoom_matrix", &zoom_matrix, 1);
			gstd::connectUniform(pgm3, "intensity", &m_intensity, 1);
			gstd::connectUniform(pgm3, "minimum", &m_GS_minimum, 1);
			gstd::connectUniform(pgm3, "maximum", &m_GS_maximum, 1);
			gstd::connectTexture(pgm3, "texture_id", m_texture);

			glDrawArrays(GL_QUADS, 0, 4);

			pgm3.useProgram(0);
		}
		break;
	}
}

void myGLImage::setIntensity(float intensity)
{
	m_intensity = intensity;
}

void myGLImage::setIsXInverted(bool isInverted)
{
	m_isXInverted = isInverted;
	setTexCoordinates();
}

void myGLImage::setIsYInverted(bool isInverted)
{
	m_isYInverted = isInverted;
	setTexCoordinates();
}

void myGLImage::setTexCoordinates()
{
	vector<vec2> texCoords; //from myGLRectangle : 0 is bottomLeft, 1 is topLeft, ...
		texCoords.push_back(vec2(m_isXInverted,m_isYInverted));
		texCoords.push_back(vec2(m_isXInverted,!m_isYInverted));
		texCoords.push_back(vec2(!m_isXInverted,!m_isYInverted));
		texCoords.push_back(vec2(!m_isXInverted,m_isYInverted));
	m_texCoords.clear();
	m_texCoords.insert(0, texCoords);
}

void myGLImage::setTexSize(glm::vec2 size)
{
	m_texture.loadTexture(size.x, size.y, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void myGLImage::scaleR16Image()
{
	if(m_texture.isLoaded() == false) return; //->

//	vector<uint> data_text;
//	data_text.assign(m_texture.getSize().x*m_texture.getSize().y, 0);

    vector<uint16> data_text;
    m_texture.getTextureData<uint16>(data_text, GL_RED, GL_UNSIGNED_SHORT,1);

    uint16 min,max;
	getPixelMinMaxR16Image(data_text, m_texture.getSize(), min, max);

	m_GS_minimum = min;
	m_GS_maximum = max;

//	bool isUniform = getPixelHistogramR16Image(data_text, m_texture.getSize(), histo);

//	if(isUniform)
//	{


//	}
//	else
//	{
//		float totalPixels = 0;
//		for(glm::vec2& bin : histo)
//		{
//			totalPixels += bin.y;
//		}

//		int histoMin_idx = 0;
//		float percentPixels = 0;
//		while(1)
//		{
//			percentPixels += 100.0f*histo[histoMin_idx].y/(totalPixels);
//			if(percentPixels > 1) break;
//			else histoMin_idx++;
//			cout<<histoMin_idx<<"\n";
//		}

//		int histoMax_idx = histo.size()-1;
//		percentPixels = 0;
//		while(1)
//		{
//			percentPixels += 100.0*histo[histoMax_idx].y/(totalPixels);
//			if(percentPixels > 1) break;
//			else histoMax_idx--;
//		}

//		m_GS_minimum =  histo[histoMin_idx].x;
//		m_GS_maximum =  histo[histoMax_idx].x;
//	}


}


myGLRectangle& myGLImage::getRect()
{
	return m_imageRect;
}


gstd::gTexture& myGLImage::getTexture()
{
	return m_texture;
}


bool getPixelHistogramR16Image(vector<uint16> &data_in, glm::vec2 size,
							   vector<glm::vec2>& histogram)
{
	vector<float> pixel_values_v;

	uint32 pixel_minimum;
	uint32 pixel_maximum;

	pixel_minimum = std::numeric_limits<uint16>::max();
	pixel_maximum = std::numeric_limits<uint16>::min();
	for(int pixel_idx = 0; pixel_idx < size.x*size.y; pixel_idx++)
	{
		uint16 pixel_value = data_in[pixel_idx] & 0x0000FFFF;

		if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
		if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;

		pixel_values_v.push_back(pixel_value);
	}

	if(pixel_minimum == pixel_maximum) return true; //->

	dataBiner(pixel_values_v, histogram, 200, pixel_minimum, pixel_maximum);

	return false;
}


void getPixelMinMaxR16Image(vector<uint16> &data_in, glm::vec2 size,
                            uint16& pixel_minimum, uint16& pixel_maximum)
{
	pixel_minimum = std::numeric_limits<uint16>::max();
	pixel_maximum = std::numeric_limits<uint16>::min();
    for(uint16& pixel : data_in)
	{
		uint16 pixel_value = pixel & 0x0000FFFF;

		if(pixel_value >= pixel_maximum) pixel_maximum = pixel_value;
		if(pixel_value <= pixel_minimum) pixel_minimum = pixel_value;
	}
}


