
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

#ifndef SCREEN_H
#define SCREEN_H

#include "cellEngine_library_global.h"

#include <glm.hpp>
    #define GLM_SWIZZLE
    #include <gtc/type_ptr.hpp>
    #include <gtc/matrix_transform.hpp>
    #include <gtc/matrix_access.hpp>

#include <GL_glew/glew.h>
#include <gl.h>

#include "stdlib.h"
#include <vector>
#include "iostream"
#include <random>
#include <thread>
#include <functional>

//#include "toolBox_src/toolBox_library_global.h"
#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/glGUI/glScreenObjects/myGLScreenGeometricObjects.h"
    #include "toolBox_src/glGUI/glWidgets/myGLScreen/myGLScreen.h"
	#include "toolBox_src/imageTools/image_colorisation/myLUT.h"
	#include "toolBox_src/imageTools/image_format/tiff_format.h"


enum BUFFER_TYPE {VERTEX_BUFFER, COLOR_BUFFER};
enum PRIMITIVE_TYPE {s_POINT, s_LINE};


class CELLENGINE_LIBRARYSHARED_EXPORT Screen
{


public : 	enum RENDERING_MODE {POINTS, LINES, GAUSSIANS,
								 GAUSSIANS_LOOKUP_TABLE, GAUSSIANS_CAMERA};
			enum RENDERING_TARGET {SCREEN_FRAMEBUFFER, CAMERA_FRAMEBUFFER};
;
public :

    Screen();
    ~Screen();
	bool intializeScreen(uint* size, QWidget* parent =0);

	void bufferData(glm::vec2* r, glm::vec4* color, int n, int type); // 0 : point, 1 line
	void bufferData(std::vector<glm::vec2> r, std::vector<glm::vec4> color, int type);
	void clearBuffer(int type);
	void drawBuffer(RENDERING_TARGET target = SCREEN_FRAMEBUFFER);
//	void applyPoissonNoise();
    void applyOffsetsAndNoises();

	myGLScreen* getRenderWindow();
    void showScreen();

	void setLUT(string lut);
    void setRenderingMode(Screen::RENDERING_MODE mode);
	void setPointSize(int pointSize);
	void setPointIntensity(float pointIntensity);
	void setSpotSize(glm::vec2 spotSize);
	void setSpotIntensity(float intensity);
    void setIsAutoscale(bool isAutoscale);
    void setAutoscaleFactor(float autoscale_factor);

//spots -> prePoissonOffset -> poisson -> gain -> postPoissonOffset -> ReadoutNoise / without bypass
//spots -> postPoissonOffset -> ReadoutNoise / with bypass
    void clearCamera();
	void setCameraField(glm::vec2& bottomLeft,glm::vec2& topRight);
	void setCameraDefinition(glm::vec2& definition);
    void setCameraPrePoissonOffset(float offset);
    void setCameraIsUsingPoissonNoise(bool);
    void setCameraGain(float gain);
    void setCameraIsBypassingPoissonAndGain(bool isBypassing);
    void setCameraPostGainOffset(float offset);
    void setCameraReadoutNoiseSigma(float sigma);

    gstd::gTexture& getCameraTexture();

private:

//screen
	myGLScreen m_screen;

	default_random_engine m_unif_rng;
	poisson_distribution<uint16> m_poisson_rng;

//    vector<std::mt19937> m_unif_rng_v;
    vector<default_random_engine> m_unif_rng_v;
    vector<poisson_distribution<uint16>> m_poisson_rng_v;
    vector<normal_distribution<float>> m_normal_rng_v;

//camera

	glm::vec2 m_camera_definition;
	glm::vec2 m_cameraField_bottomLeft;
	glm::vec2 m_cameraField_topRight;
    float m_camera_prePoissonOffset;
    bool m_camera_isUsingPoissonNoise;
    float m_camera_gain;
    bool m_camera_isBypassingPoissonAndGain;
    float m_camera_postGainOffset;
    float m_camera_readoutNoiseSigma;

	uint8* m_cameraBuffer;
	GLuint m_camera_frameBuffer;
	gstd::gTexture m_cameraColorBuffer_2DTexture;

	gstd::gVector<glm::vec2> m_r_texture_gv;
	gstd::gVector<glm::vec2> m_r_cameraTexture_gv;
	gstd::gVector<glm::vec4> m_color_texture_gv;
	gstd::gVector<glm::vec2> m_rText_texture_gv;

//primitives
	gstd::gVector<glm::vec2> m_r_point_gv;
	gstd::gVector<glm::vec4> m_color_point_gv;
	gstd::gVector<glm::vec2> m_r_segment_gv;
	gstd::gVector<glm::vec4> m_color_segment_gv;

//rendering programs & tools
	int m_point_size;
	float m_point_intensity;
	glm::vec2 m_spot_size;
    float m_spot_intensity;
    bool m_isAutoscale;
    float m_autoscale_factor;
	RENDERING_MODE m_renderingMode;

	gstd::gTexture m_gaussianTexture;

	GLuint m_frameBuffer;
	gstd::gTexture m_colorBuffer_2DTexture;
	gstd::gTexture m_1DText;

	gstd::gProgram* m_fullScrnTexture_pgm;
	gstd::gProgram* m_simpleTexture_pgm;
	gstd::gProgram* m_rawPrimitiveRendering_pgm;
	gstd::gProgram* m_texturedRendering_pgm;


};



#endif // SCREEN_H
