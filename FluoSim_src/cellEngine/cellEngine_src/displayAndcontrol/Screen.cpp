
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



#include "Screen.h"

using namespace std;
using namespace glm;

Screen::Screen() :

	m_screen(200,200, 0),
	m_gaussianTexture(GL_TEXTURE_2D),
	m_colorBuffer_2DTexture(GL_TEXTURE_2D),
	m_cameraColorBuffer_2DTexture(GL_TEXTURE_2D),
	m_1DText(GL_TEXTURE_1D)

{
    m_screen.setWindowTitle("Screen");
	m_screen.QWidget::hide();

	m_renderingMode = POINTS;

	m_point_intensity = 1.0f;
	m_point_size = 3;

	m_spot_size = vec2(10,10);
	m_spot_intensity = 0.05;

    m_camera_prePoissonOffset = 100;

    m_unif_rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    m_unif_rng_v.resize(std::thread::hardware_concurrency());
    for(std::default_random_engine& rng : m_unif_rng_v) \
        rng.seed(m_unif_rng());
	m_poisson_rng_v.resize(std::thread::hardware_concurrency());
    m_normal_rng_v.resize(std::thread::hardware_concurrency());
}

Screen::~Screen()
{
}


bool Screen::intializeScreen(uint* size, QWidget* parent)
{
//	m_screen = new myGLScreen(200,200, 0);
	m_screen.QGLWidget::show();

	m_rawPrimitiveRendering_pgm = new gstd::gProgram("raw_pgm");
	m_texturedRendering_pgm = new gstd::gProgram("texture_pgm");
	m_fullScrnTexture_pgm = new gstd::gProgram("fullScrnTexture");
	m_simpleTexture_pgm = new gstd::gProgram("simpleTexture");
	m_gaussianTexture = gstd::genGaussianTexture(vec2(256,256), 0.2);
		m_gaussianTexture.setMagFilter(GL_LINEAR);
		m_gaussianTexture.setMinFilter(GL_LINEAR);


	myLUT lut;
	lut.loadLUT("ncl_default.rgb");
	vector<vec4> color_v = lut.getColors();

	m_1DText.bindTexture(0);
	m_1DText.loadTexture(color_v.size(), -1, GL_RGBA, GL_RGBA, GL_FLOAT, color_v.data());
	m_1DText.genMipMap();
	m_1DText.enableMipMap(true);

	m_colorBuffer_2DTexture.bindTexture(0);
	m_colorBuffer_2DTexture.loadTexture(1024,1024, GL_RGBA, GL_RGBA, GL_FLOAT, NULL);
	m_colorBuffer_2DTexture.genMipMap();
	m_colorBuffer_2DTexture.enableMipMap(true);

	m_cameraColorBuffer_2DTexture.bindTexture(0);
    m_cameraColorBuffer_2DTexture.loadTexture(512,512, GL_R32F, GL_RGBA, GL_FLOAT, NULL);
	m_cameraColorBuffer_2DTexture.genMipMap();
	m_cameraColorBuffer_2DTexture.enableMipMap(true);
	m_cameraColorBuffer_2DTexture.setMinFilter(GL_NEAREST);
	m_cameraColorBuffer_2DTexture.setMagFilter(GL_NEAREST);


	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	m_colorBuffer_2DTexture.bindTexture(0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   m_colorBuffer_2DTexture.getTextureHdl(), 0);

	glGenFramebuffers(1, &m_camera_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_camera_frameBuffer);
	m_cameraColorBuffer_2DTexture.bindTexture(0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   m_cameraColorBuffer_2DTexture.getTextureHdl(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	string vs_fullScrnTexture_src;
	vs_fullScrnTexture_src +="#version 150 core\n"
				"\n"
				"in vec2 Rw_vsIn;\n"
				"in vec2 rText_vsIn;\n"
				"in vec4 color_vsIn;\n"
				"out vec4 color_fsIn;\n"
				"out vec2 rText_fsIn;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(Rw_vsIn,0.0, 1.0);\n"
				"	gl_Position = temp;\n"
				"	color_fsIn = color_vsIn;\n"
				"	rText_fsIn = rText_vsIn;\n"
				"}\n";

	string fs_fullScrnTexture_src;
	fs_fullScrnTexture_src +="#version 150 core\n\n"
				"\n"
				"in vec2 rText_fsIn;\n"
				"in vec4 color_fsIn;\n"
				"uniform sampler2D texture_unit0;\n"
				"uniform sampler2D texture_unit1;\n"
				"uniform sampler1D texture_unit2;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	if(length(texture(texture_unit0, rText_fsIn).xyz) <=  0.01) discard;\n"
				"	vec4 color2 = texture(texture_unit2, clamp(length(texture(texture_unit0, rText_fsIn).xyz), 0.05,0.95)); \n"
				"	gl_FragColor = color2;\n"
				"}\n";


	//________SHADER SOURCES________//

	string vs_raw_src;
	vs_raw_src +="#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec4 color_in;\n"
				"uniform mat4 scaling_matrix;\n"
//				"uniform mat4 screen_matrix;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = scaling_matrix*temp;\n"
				"	color = color_in;\n"
				"}\n";

	string fs_raw_src;
	fs_raw_src +="#version 150 core\n\n"
				"\n"
				"in vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"uniform float intensity;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=vec4(intensity*color.xyz,1.0f);\n"
				"}\n";

	string vs_textured_src;
	vs_textured_src +="#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec4 color_in;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = temp;\n"
				"	color = color_in;\n"
				"}\n";

	string gs_textured_src;
	gs_textured_src +="#version 150 core"
				"\n\n"
				"layout(points) in;\n"
				"layout (triangle_strip, max_vertices = 4) out ;\n"
				"uniform vec2 square_size;\n"
				"uniform mat4 scaling_matrix;\n"
				"in vec4 color[];\n"
				"out vec2 TexCoords_GS;\n"
				"out vec4 colorGS;\n"
				"\n"
				"\n\n"
				"void main(void)\n"
				"{\n"
				"		float size_text = 10;\n"
				"		colorGS = color[0];\n"
				"		gl_Position = scaling_matrix*(gl_in[0].gl_Position + 0.5*vec4(square_size.x,square_size.y,0,0));\n"
				"		TexCoords_GS = vec2(1,1);\n"
				"		EmitVertex();\n"
				"		colorGS = color[0];\n"
				"		gl_Position = scaling_matrix*(gl_in[0].gl_Position + 0.5*vec4(square_size.x,-square_size.y,0,0));\n"
				"		TexCoords_GS = vec2(1,0);\n"
				"		EmitVertex();\n"
				"		colorGS = color[0];\n"
				"		gl_Position = scaling_matrix*(gl_in[0].gl_Position + 0.5*vec4(-square_size.x,square_size.y,0,0));\n"
				"		TexCoords_GS = vec2(0,1);\n"
				"		EmitVertex();\n"
				"		colorGS = color[0];\n"
				"		gl_Position = scaling_matrix*(gl_in[0].gl_Position + 0.5*vec4(-square_size.x,-square_size.y,0,0));\n"
				"		TexCoords_GS = vec2(0,0);\n"
				"		EmitVertex();\n"
				"}";

	string fs_textured_src;
	fs_textured_src+="#version 150 core\n\n"
				"\n"
				"in vec4 colorGS;\n"
				"in vec2 TexCoords_GS;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"uniform sampler2D texture_id;\n"
				"uniform float spot_intensity;\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor = vec4(spot_intensity*texture(texture_id, TexCoords_GS).x*colorGS.xyz ,1);\n"
//			    "	gl_FragColor = vec4(spot_intensity*length(texture(texture_id, TexCoords_GS).xyz)*colorGS.xyz ,1);\n"
				"}\n";


	string vs_simpleTexture_src;
	vs_simpleTexture_src +="#version 150 core\n"
						   "\n"
						   "in vec2 Rw_vsIn;\n"
						   "in vec2 rText_vsIn;\n"
						   "out vec2 rText_vsOut;\n"
						   "uniform mat4 scaling_matrix;\n"
						   "\n"
						   "void main(void)\n"
						   "{\n"
						   "	vec4 temp = vec4(Rw_vsIn,0.0, 1.0);\n"
						   "	gl_Position = scaling_matrix*temp;\n"
						   "	rText_vsOut = rText_vsIn;\n"
						   "}\n";

	string fs_simpleTexture_src;
	fs_simpleTexture_src+="#version 150 core\n\n"
				"\n"
				"in vec2 rText_vsOut;\n"
				"out vec4 gl_FragColor;\n"
				"uniform sampler2D texture_id;\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor = vec4(texture(texture_id, rText_vsOut).xyz ,1);\n"
				"}\n";


	gstd::gShader vs_fullScrnTexture(gstd::VERTEX_SHADER);
		vs_fullScrnTexture.setSource(vs_fullScrnTexture_src, 0);
		vs_fullScrnTexture.compile();
	gstd::gShader fs_fullScrnTexture(gstd::FRAGMENT_SHADER);
		fs_fullScrnTexture.setSource(fs_fullScrnTexture_src, 0);
		fs_fullScrnTexture.compile();
	m_fullScrnTexture_pgm->addShader(vs_fullScrnTexture);
	m_fullScrnTexture_pgm->addShader(fs_fullScrnTexture);

	string linking;
	m_fullScrnTexture_pgm->linkShaders(linking);

	m_r_texture_gv.insert(0, {{-1,-1}, {-1,+1}, {+1,-1}, {+1,+1}});
	m_r_cameraTexture_gv.insert(0, {{0,0}, {0,1024}, {1024,0}, {1024,1024}});
	m_color_texture_gv.insert(0, {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,1,1,1}}); //blue
	m_rText_texture_gv.insert(0, {{0,0}, {0,+1}, {1,0}, {1,1}});

	string log;
	gstd::gShader vs_raw(gstd::VERTEX_SHADER);
		vs_raw.setSource(vs_raw_src, 0);
		vs_raw.compile(log);
	gstd::gShader fs_raw(gstd::FRAGMENT_SHADER);
		fs_raw.setSource(fs_raw_src, 0);
		fs_raw.compile(log);
	m_rawPrimitiveRendering_pgm->addShader(vs_raw);
	m_rawPrimitiveRendering_pgm->addShader(fs_raw);
	m_rawPrimitiveRendering_pgm->linkShaders(linking);



	gstd::gShader vs_textured(vs_textured_src, gstd::VERTEX_SHADER, 0);
		vs_textured.compile();
	gstd::gShader gs_textured(gs_textured_src, gstd::GEOMETRY_SHADER, 0);
		gs_textured.compile();
	gstd::gShader fs_textured(fs_textured_src, gstd::FRAGMENT_SHADER, 0);
		fs_textured.compile();
	m_texturedRendering_pgm->addShader(vs_textured);
	m_texturedRendering_pgm->addShader(gs_textured);
	m_texturedRendering_pgm->addShader(fs_textured);
	m_texturedRendering_pgm->linkShaders(linking);

	gstd::gShader vs_simpleTexture(gstd::VERTEX_SHADER);
		vs_simpleTexture.setSource(vs_simpleTexture_src, 0);
		vs_simpleTexture.compile();
	gstd::gShader fs_simpleTexture(gstd::FRAGMENT_SHADER);
		fs_simpleTexture.setSource(fs_simpleTexture_src, 0);
		fs_simpleTexture.compile();
	m_simpleTexture_pgm->addShader(vs_simpleTexture);
	m_simpleTexture_pgm->addShader(fs_simpleTexture);
	m_simpleTexture_pgm->linkShaders(linking);


	m_simpleTexture_pgm->coutEntryPoints();

}



void Screen::bufferData( vec2* r, vec4* color, int n, int type) // 0 : point, 1 line
{
	vector<vec2> r_v;
	vector<vec4> color_v;

	if(type == 0 )
	{
		int offset1 = m_r_point_gv.size();
		int offset2 = m_color_point_gv.size();

			r_v.insert(r_v.end(), r, r+n);
			m_r_point_gv.insert(offset1, r_v);
			color_v.insert(color_v.end(), color, color+n);
			m_color_point_gv.insert(offset2, color_v);
	}

	if(type == 1)
	{
		int offset1 = m_r_segment_gv.size();
		int offset2 = m_color_segment_gv.size();

			r_v.insert(r_v.end(), r, r+n);
			m_r_segment_gv.insert(offset1, r_v);
			color_v.insert(color_v.end(), color, color+n);
			m_color_segment_gv.insert(offset2, color_v);
	}
}

void Screen::bufferData( vector<vec2> r_v, vector<vec4> color_v, int type) // 0 : point, 1 line
{
	if(type == 0 )
	{
		int offset1 = m_r_point_gv.size();
		int offset2 = m_color_point_gv.size();

		m_r_point_gv.insert(offset1, r_v);
		m_color_point_gv.insert(offset2, color_v);
	}

	if(type == 1)
	{
		int offset1 = m_r_segment_gv.size();
		int offset2 = m_color_segment_gv.size();

		m_r_segment_gv.insert(offset1, r_v);
		m_color_segment_gv.insert(offset2, color_v);
	}
}


void Screen::clearBuffer(int type)
{
	if(type == 0)
	{
		m_r_point_gv.clear();
		m_color_point_gv.clear();
	}

	if(type == 1)
	{
		m_r_segment_gv.clear();
		m_color_segment_gv.clear();
	}
}

void Screen::drawBuffer(RENDERING_TARGET target)
{

	mat4 m;
    float spot_intensity;
	vec2 target_definition;
	GLuint target_frameBuffer;

	if(target == CAMERA_FRAMEBUFFER)
	{
		target_definition = m_camera_definition;
		target_frameBuffer = m_camera_frameBuffer;

		mat4 lastZoomMatrix = m_screen.getZoomMatrix();
		m_screen.fitQuadToScreen(m_cameraField_bottomLeft, m_cameraField_topRight);
		m = m_screen.getZoomMatrix();
		m_screen.setZoomMatrix(lastZoomMatrix);
        spot_intensity = m_spot_intensity;
	}
	else
	{
		target_definition = m_screen.size();
		target_frameBuffer = 0;
		m = m_screen.getWorldToExtendedHomMatrix();
        if(m_isAutoscale == true) \
            spot_intensity = m_spot_intensity*m_autoscale_factor;
        else spot_intensity = m_spot_intensity;

	}

	switch(m_renderingMode)
	{
		case POINTS :
		{
			glBindFramebuffer(GL_FRAMEBUFFER, target_frameBuffer);
			glViewport(0,0, target_definition.x, target_definition.y);

//			glDisable(GL_BLEND);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			glPointSize(m_point_size);

			m_rawPrimitiveRendering_pgm->useProgram(true);

			gstd::myConnector<vec2>::connect(*m_rawPrimitiveRendering_pgm, "R", m_r_point_gv);
			gstd::myConnector<vec4>::connect(*m_rawPrimitiveRendering_pgm, "color_in", m_color_point_gv);
			gstd::connectUniform(*m_rawPrimitiveRendering_pgm, "scaling_matrix", (float*) &m);
			gstd::connectUniform(*m_rawPrimitiveRendering_pgm, "intensity", (float*) &m_point_intensity);

			int n_point = m_r_point_gv.size();
			glDrawArrays(GL_POINTS, 0, n_point);

			m_rawPrimitiveRendering_pgm->useProgram(false);
		}
		break;

		case LINES :
		{
			glBindFramebuffer(GL_FRAMEBUFFER, target_frameBuffer);
			glViewport(0,0,
					   target_definition.x, target_definition.y);

			glDisable(GL_BLEND);
			glLineWidth(2.0);

			m_rawPrimitiveRendering_pgm->useProgram(true);

			float line_intensity = 0.0f;

			gstd::myConnector<vec2>::connect(*m_rawPrimitiveRendering_pgm, "R", m_r_segment_gv);
			gstd::myConnector<vec4>::connect(*m_rawPrimitiveRendering_pgm, "color_in", m_color_segment_gv);
			gstd::connectUniform(*m_rawPrimitiveRendering_pgm, "scaling_matrix", (float*) &m);
			gstd::connectUniform(*m_rawPrimitiveRendering_pgm, "intensity", (float*) &line_intensity);

			int n_vertices_segment = m_r_segment_gv.size();
			glDrawArrays(GL_LINES, 0, n_vertices_segment);

			m_rawPrimitiveRendering_pgm->useProgram(false);
		}
		break;

		case GAUSSIANS :
		{
			glBindFramebuffer(GL_FRAMEBUFFER, target_frameBuffer);
			glViewport(0,0,
					   target_definition.x, target_definition.y);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			m_texturedRendering_pgm->useProgram(true);
			int n_point = m_r_point_gv.size();

			gstd::myConnector<vec2>::connect(*m_texturedRendering_pgm, "R", m_r_point_gv);
			gstd::myConnector<vec4>::connect(*m_texturedRendering_pgm, "color_in", m_color_point_gv);
			gstd::connectUniform(*m_texturedRendering_pgm, "scaling_matrix", (float*) &m);
			gstd::connectUniform(*m_texturedRendering_pgm, "square_size", &m_spot_size);
            gstd::connectUniform(*m_texturedRendering_pgm, "spot_intensity", &spot_intensity);
			gstd::connectTexture(*m_texturedRendering_pgm, "texture_id", m_gaussianTexture, 0);

			glDrawArrays(GL_POINTS, 0, n_point);
		}
		break;


		case GAUSSIANS_LOOKUP_TABLE :
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glViewport(0,0,
					   m_colorBuffer_2DTexture.getSize().x, m_colorBuffer_2DTexture.getSize().y);

			glClearColor(0.0,0.0,0.0,0.0);
			glClear(GL_COLOR_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			m_texturedRendering_pgm->useProgram(true);
			int n_point = m_r_point_gv.size();

			gstd::myConnector<vec2>::connect(*m_texturedRendering_pgm, "R", m_r_point_gv);
			gstd::myConnector<vec4>::connect(*m_texturedRendering_pgm, "color_in", m_color_point_gv);
			gstd::connectUniform(*m_texturedRendering_pgm, "scaling_matrix", (float*) &m);
			gstd::connectUniform(*m_texturedRendering_pgm, "square_size", &m_spot_size);
            gstd::connectUniform(*m_texturedRendering_pgm, "spot_intensity", &spot_intensity);
			gstd::connectTexture(*m_texturedRendering_pgm, "texture_id", m_gaussianTexture, 0);

			glDrawArrays(GL_POINTS, 0, n_point);


			glBindFramebuffer(GL_FRAMEBUFFER, target_frameBuffer);
			glViewport(0,0,
					   target_definition.x, target_definition.y);

			glDisable(GL_BLEND);
			glPointSize(3);

			m_fullScrnTexture_pgm->useProgram(true);

			gstd::myConnector<vec2>::connect(*m_fullScrnTexture_pgm, "Rw_vsIn", m_r_texture_gv);
			gstd::myConnector<vec4>::connect(*m_fullScrnTexture_pgm, "color_vsIn", m_color_texture_gv);
			gstd::myConnector<vec2>::connect(*m_fullScrnTexture_pgm, "rText_vsIn", m_rText_texture_gv);
            gstd::connectUniform(*m_fullScrnTexture_pgm, "spot_intensity", &spot_intensity);
			gstd::connectTexture(*m_fullScrnTexture_pgm, "texture_unit0", m_colorBuffer_2DTexture, 0);
			gstd::connectTexture(*m_fullScrnTexture_pgm, "texture_unit1", m_gaussianTexture, 1);
			gstd::connectTexture(*m_fullScrnTexture_pgm, "texture_unit2", m_1DText, 2);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			m_rawPrimitiveRendering_pgm->useProgram(false);

		}
		break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,
			   m_screen.size().x, m_screen.size().y);
}


void Screen::applyOffsetsAndNoises()
{
    //retrieve camera data
    gstd::gTexture camera_texture = getCameraTexture();
    vector<float> pixels16_v;
    camera_texture.getTextureData(pixels16_v, GL_RED, GL_FLOAT, 1);
//    camera_texture.getTextureData(pixels16_v, GL_RED, GL_UNSIGNED_SHORT, 1);
    uint nb_pixels	 = pixels16_v.size();

    uint nb_thread = std::min(nb_pixels, std::thread::hardware_concurrency());

    //format data for parallel calculus
    vector<uint> startIdx_v, endIdx_v;
    for(uint th_idx = 0; th_idx < nb_thread; th_idx++)
    {
        startIdx_v.push_back(th_idx*nb_pixels/nb_thread);
        endIdx_v.push_back(th_idx*nb_pixels/nb_thread + nb_pixels/nb_thread);
            if(th_idx == nb_thread)	endIdx_v.back() += nb_pixels%nb_thread;
    }

    typedef std::poisson_distribution<uint16>::param_type poisson_param_t;
    typedef std::normal_distribution<float>::param_type normal_param_t;

    vector<std::thread> threads_v;
    for(uint th_idx = 0; th_idx < nb_thread; th_idx++)
    {
        threads_v.push_back(thread([th_idx](vector<float>& px_in,
                                      uint start_idx, uint end_idx,
                                      float prePoisson_offset, uint16 postGain_offset,
                                      bool isUsingPoissonNoise, float gain, bool isBypassingPoissonAndGain, float readoutNoise_sigma,
                                      vector<poisson_distribution<uint16> >& poisson_rng_v,
                                      vector<normal_distribution<float> >& normal_rng_v,
                                      vector<default_random_engine>& unif_rng_v)
                                    {
                                        for(uint px_idx = start_idx; px_idx < end_idx; px_idx++)
                                        {
                                            float px = px_in[px_idx]*65535.0f;
                                            if(isBypassingPoissonAndGain == false)
                                            {
                                                //offset prePoisson
                                                px += prePoisson_offset;
                                                //poisson noise
                                                if(isUsingPoissonNoise == true)
                                                {
                                                    poisson_rng_v[th_idx].param(poisson_param_t{px});//poisson_param can be a floating point number
                                                    px = poisson_rng_v[th_idx](unif_rng_v[th_idx]);
                                                }
                                                px *= gain;
                                            }
                                            //offset postPoisson
                                            px += postGain_offset;
                                            //Readout Noise (gaussian noise)
                                            if(readoutNoise_sigma != 0.0f)
                                            {
                                                normal_rng_v[th_idx].param(normal_param_t{0.0f, readoutNoise_sigma});
                                                px += normal_rng_v[th_idx](unif_rng_v[th_idx]);
                                            }
                                            px_in[px_idx] = std::max(0.0f, std::min(px/65535.0f, 1.0f)); //we need to clamp the pixel value to stay in the 16bits range
                                        }
                                    },
                            std::ref(pixels16_v), startIdx_v[th_idx], endIdx_v[th_idx],
                            m_camera_prePoissonOffset, m_camera_postGainOffset,
                            m_camera_isUsingPoissonNoise, m_camera_gain, m_camera_isBypassingPoissonAndGain, m_camera_readoutNoiseSigma,
                            std::ref(m_poisson_rng_v), std::ref(m_normal_rng_v), std::ref(m_unif_rng_v)));

    }

    for(std::thread& thread : threads_v)
    {
        thread.join();
    }

    //write camera buffer (texture)
    glm::vec2 texture_size = camera_texture.getSize();
    camera_texture.loadTexture(texture_size.x, texture_size.y,
                               GL_R32F, GL_RED, GL_FLOAT, pixels16_v.data());


}



myGLScreen* Screen::getRenderWindow()
{
	return &m_screen;
}


void Screen::showScreen()
{
}

void Screen::setLUT(string lut_str)
{
	myLUT lut;
	lut.loadLUT(lut_str);
	vector<vec4> colors_v = lut.getColors();

	m_1DText.bindTexture(0);
	m_1DText.loadTexture(colors_v.size(), -1, GL_RGBA, GL_RGBA, GL_FLOAT, colors_v.data());
	m_1DText.genMipMap();
	m_1DText.enableMipMap(true);
}

void readPixelsIn(vec2 r, vec2 dr)
{

}

void Screen::setPointSize(int pointSize)
{
	m_point_size = pointSize;
}

void Screen::setPointIntensity(float intensity)
{
	m_point_intensity = intensity;
}

void Screen::setSpotSize(glm::vec2 spotSize)
{
	m_spot_size = spotSize;
}

void Screen::setSpotIntensity(float intensity)
{
	m_spot_intensity = intensity;
}

void Screen::setIsAutoscale(bool isAutoscale)
{
    m_isAutoscale = isAutoscale;
}

void Screen::setAutoscaleFactor(float autoscale_factor)
{
    m_autoscale_factor = autoscale_factor;
}

void Screen::setRenderingMode(Screen::RENDERING_MODE mode)
{
	m_renderingMode = mode;
}

void Screen::clearCamera()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_camera_frameBuffer);
	//camera depth = 16bits and is stored in the red channel

    float red = m_camera_prePoissonOffset*1.0f/65535;
    red = 0.0f; //the offset is now applied by the cpu, using the applyOffsetsAndNoises method!
	glClearColor(red,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Screen::setCameraField(glm::vec2& bottomLeft,glm::vec2& topRight)
{
	m_cameraField_bottomLeft = bottomLeft;
	m_cameraField_topRight = topRight;
}


void Screen::setCameraDefinition(vec2& definition)
{
	m_camera_definition = definition;

	m_cameraColorBuffer_2DTexture.bindTexture(0);
    m_cameraColorBuffer_2DTexture.loadTexture(definition.x,definition.y, GL_R32F, GL_RGBA, GL_FLOAT, NULL);
	m_cameraColorBuffer_2DTexture.bindTexture(0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_camera_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   m_cameraColorBuffer_2DTexture.getTextureHdl(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Screen::setCameraPrePoissonOffset(float bckg)
{
    m_camera_prePoissonOffset = bckg;
}

void Screen::setCameraIsUsingPoissonNoise(bool noise)
{
    m_camera_isUsingPoissonNoise = noise;
}

void Screen::setCameraGain(float gain)
{
    m_camera_gain = gain;
}

void Screen::setCameraIsBypassingPoissonAndGain(bool isBypassing)
{
    m_camera_isBypassingPoissonAndGain = isBypassing;
}

void Screen::setCameraPostGainOffset(float offset)
{
    m_camera_postGainOffset = offset;
}

void Screen::setCameraReadoutNoiseSigma(float sigma)
{
    m_camera_readoutNoiseSigma = sigma;
}

gstd::gTexture& Screen::getCameraTexture()
{
	return m_cameraColorBuffer_2DTexture;
}
