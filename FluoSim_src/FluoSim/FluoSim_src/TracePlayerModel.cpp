
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



#include "TracePlayer.h"

using namespace glm;



bool comp(vec2 v1, vec2 v2)
{
	if(v1.y <= v2.y) return true;
	else return false;
}


TracePlayerModel::TracePlayerModel(myGLWidget* glWidget, QWidget* parent) : TracePlayerView(parent),

	m_pgm_gaussianBandW("gaussianBandW"),
	m_pgm_gaussianColor("gaussianColor"),
	m_pgm_lineColor("lineColor"),
	m_pgm_lineBandW("lineBandW"),
	m_pgm_events("events"),
	m_pgm_trajectories("trajectories"),
	m_gaussian_texture(GL_TEXTURE_2D),
    m_font("Resources/Fonts/font_glWord.png", test_desc),
	m_currentPlane_word(&m_font),
	m_numberTraces_word(&m_font),
	m_numberEvents_word(&m_font),
	m_tiffMovie(vec2(0,0), vec2(10,10), r_geom_gMV)
{

	m_lut.loadLUT("ncl_default.rgb");

	string vsBandW_src;
	vsBandW_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = temp;\n"
				"	color = vec4(1,1,1,1);\n"
				"}\n";

	string vsColor_src;
	vsColor_src +=	"#version 150 core\n"
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

	string vsLineColor_src;
	vsLineColor_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec4 color_in;\n"
				"uniform mat4 scaling_matrix;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = scaling_matrix*temp;\n"
				"	color = color_in;\n"
				"}\n";

	string vsLineBandW_src;
	vsLineBandW_src +=	"#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"in vec4 color_in;\n"
				"uniform mat4 scaling_matrix;\n"
				"out vec4 color;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = scaling_matrix*temp;\n"
				"	color =  vec4(1,1,1,1);\n"
				"}\n";

	string gs_src;
	gs_src+= "#version 150 core";
	gs_src+= "\n\n";
	gs_src+= "layout(points) in;\n";
	gs_src+= "layout (triangle_strip, max_vertices = 4) out ;\n";
	gs_src+= "uniform vec2 square_size;\n";
	gs_src+= "uniform mat4 scaling_matrix;\n";
//		gs_src+= "in int isVisible[];\n";
	gs_src+= "in vec4 color[];\n";
	gs_src+= "out vec2 TexCoords_GS;\n";
	gs_src+= "out vec4 colorGS;\n";
	gs_src+= "\n";
	gs_src+= "\n\n";
	gs_src+= "void main(void)\n";
	gs_src+= "{\n";
//		gs_src+= "	if(isVisible[0] == 1)\n";
//		gs_src+= "	{\n";
	gs_src+= "		float size_text = 10;\n";
	gs_src+= "		colorGS = color[0];\n";
	gs_src+= "		gl_Position = scaling_matrix*(gl_in[0].gl_Position + vec4(square_size.x,square_size.y,0,0));\n";
	gs_src+= "		TexCoords_GS = vec2(1,1);\n";
	gs_src+= "		EmitVertex();\n";
	gs_src+= "		colorGS = color[0];\n";
	gs_src+= "		gl_Position = scaling_matrix*(gl_in[0].gl_Position + vec4(square_size.x,-square_size.y,0,0));\n";
	gs_src+= "		TexCoords_GS = vec2(1,0);\n";
	gs_src+= "		EmitVertex();\n";
	gs_src+= "		colorGS = color[0];\n";
	gs_src+= "		gl_Position = scaling_matrix*(gl_in[0].gl_Position + vec4(-square_size.x,square_size.y,0,0));\n";
	gs_src+= "		TexCoords_GS = vec2(0,1);\n";
	gs_src+= "		EmitVertex();\n";
	gs_src+= "		colorGS = color[0];\n";
	gs_src+= "		gl_Position = scaling_matrix*(gl_in[0].gl_Position + vec4(-square_size.x,-square_size.y,0,0));\n";
	gs_src+= "		TexCoords_GS = vec2(0,0);\n";
	gs_src+= "		EmitVertex();\n";
//		gs_src+= "	}\n";
	gs_src+= "}";

	string fs_src;
	fs_src +=	"#version 150 core\n\n"
				"\n"
				"in vec4 colorGS;\n"
				"in vec2 TexCoords_GS;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"uniform sampler2D texture_id;\n"
				"uniform float spot_intensity;\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor = spot_intensity*length(texture(texture_id, TexCoords_GS).xyz)*colorGS;\n"
				"}\n";

	string fsLineColor_src;
	fsLineColor_src +=	"#version 150 core\n\n"
				"\n"
				"in vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"uniform float line_intensity;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor = line_intensity*color;\n"
				"}\n";

//vs_shaders
	gstd::gShader vs_BandW(gstd::VERTEX_SHADER);
		vs_BandW.setSource(vsBandW_src, 0);
		vs_BandW.compile();

	gstd::gShader vs_Color(gstd::VERTEX_SHADER);
		vs_Color.setSource(vsColor_src, 0);
		vs_Color.compile();

	gstd::gShader vs_lineColor(gstd::VERTEX_SHADER);
		vs_lineColor.setSource(vsLineColor_src, 0);
		vs_lineColor.compile();

	gstd::gShader vs_lineBandW(gstd::VERTEX_SHADER);
		vs_lineBandW.setSource(vsLineBandW_src, 0);
		vs_lineBandW.compile();

//gs_shaders
	gstd::gShader gs(gstd::GEOMETRY_SHADER);
		gs.setSource(gs_src, 0);
		string output;
		gs.compile(output);


//fs_shaders


	gstd::gShader fs(gstd::FRAGMENT_SHADER);
		fs.setSource(fs_src, 0);
		fs.compile();

	gstd::gShader fs_lineColor(gstd::FRAGMENT_SHADER);
		fs_lineColor.setSource(fsLineColor_src, 0);
		fs_lineColor.compile();

	string log;

	m_pgm_gaussianBandW.addShader(vs_BandW);
	m_pgm_gaussianBandW.addShader(gs);
	m_pgm_gaussianBandW.addShader(fs);
	m_pgm_gaussianBandW.linkShaders(log);
	m_pgm_gaussianBandW.useProgram(0);

	m_pgm_gaussianColor.addShader(vs_Color);
	m_pgm_gaussianColor.addShader(gs);
	m_pgm_gaussianColor.addShader(fs);
	m_pgm_gaussianColor.linkShaders(log);

	m_pgm_lineColor.addShader(vs_lineColor);
	m_pgm_lineColor.addShader(fs_lineColor);
	m_pgm_lineColor.linkShaders(log);

	m_pgm_lineBandW.addShader(vs_lineBandW);
	m_pgm_lineBandW.addShader(fs_lineColor);
	m_pgm_lineBandW.linkShaders(log);

	m_gaussian_texture = gstd::genGaussianTexture(vec2(256,256), 0.2);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	m_glWidget = glWidget;
	m_rendering_mode = GAUSSIAN_BANDW;

	m_scaling_matrix = mat4(1,0,0,0,
							0,1,0,0,
							0,0,1,0,
							0,0,0,1);

	m_square_size = vec2(4.0f,4.0f);

	m_lineWidth = 1;
	m_spotIntensity = 1.0;
	m_lineIntensity = 1.0;

	m_min_length = 0;
	m_max_length = 10000;

	m_tiffMovie.setIsYInverted(false);
	m_tiffMovie.setTexCoordinates();
}

void TracePlayerModel::setLUT(const string& lut_path)
{
	m_lut.loadLUT(lut_path);
	updateRenderingPipeline();
}

void TracePlayerModel::setTimeStep(float timestep)
{
	//modified data...
	// timestep, d, instant d,
	TracePlayerModel::m_timestep = timestep;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}

void TracePlayerModel::setPixelSize(float pixelSize)
{
	//modified data...
	// timestep, d, instant d, spotSize
	TracePlayerModel::m_pixel_size = pixelSize;
	TracePlayerModel::computeMSDsAndDs(m_pixel_size, m_timestep);
	TracePlayerModel::filter();
	TracePlayerModel::updateRenderingPipeline();
}


void TracePlayerModel::setTraces(std::vector<Trace> &traces)
{
	m_traces = traces;
	computeMSDsAndDs(m_pixel_size, m_timestep);
	filter();

	setCurrentPlane(1);

	nbTracesChanged(m_traces.size()); //to keep empty plane...
	planeRangedChanged();

	minLengthChanged(0);
	maxLengthChanged(getMaxLength(m_traces));
	m_maxLength_slider.setValue(m_maxLength_slider.maximum());

	updateNbEventsWord();
	updateNbTracesWord();
}

void TracePlayerModel::setTracesFromBinaryFile(string path)
{

	m_trace_path = path;
	vector<Trace> traces = loadTracesAsBinary(path);
	setTraces(traces);
}

void TracePlayerModel::setTracesFromStringFile(string path)
{

	m_trace_path = path;

	QPoint center_pos = this->mapToGlobal(m_mainWindow->centralWidget()->pos() + QPoint(m_mainWindow->centralWidget()->size().width()/2.0f, m_mainWindow->centralWidget()->size().height()/2.0f)
						- QPoint(m_progressWdgt->size().width()/2.0f, m_progressWdgt->size().height()/2.0f));
	m_progressWdgt->setText("Importing Trajectories...");
	m_progressWdgt->move(center_pos);
	m_progressWdgt->show();
	m_main_app->processEvents();

	vector<Trace> traces = loadTracesAsString2(path, m_progressWdgt->getProgressBar());

	m_progressWdgt->hide();

	setTraces(traces);
}

void TracePlayerModel::clearTraces()
{
	m_trace_path = "";
	m_traces.clear();
	filter();

	updateRenderingPipeline();

	nbTracesChanged(m_traces.size()); //to keep empty plane...
	planeRangedChanged();

	minLengthChanged(0);
	maxLengthChanged(0);
	m_maxLength_slider.setValue(m_maxLength_slider.maximum());

	updateCurrentPlaneWord();
	updateNbEventsWord();
	updateNbTracesWord();
}

void TracePlayerModel::setTracesDsFromStringFile(string path)
{
	loadDAsString(m_traces, path);
}

void TracePlayerModel::setMovieFromStringFile(string path)
{
	if(QString(path.data()).endsWith(".tiff")
	   || QString(path.data()).endsWith(".tif")
	   || QString(path.data()).endsWith(".stk"))
	{
	   m_tiffMovie.openTiffMovie(path);
	   m_tiffMovie.getRect().setBottomLeft({0,0});

		   int height = m_tiffMovie.getTiffParameters().height;
		   int width = m_tiffMovie.getTiffParameters().width;
	   cout<<"texture size : {"<<width<<","<<height<<"};\n";
	   m_tiffMovie.getRect().setTopRight({width, height});
	   m_tiffMovie.loadTiffPage(m_current_plane);

	   int nb_directories = m_tiffMovie.getTiffParameters().numberDirectory;
	   if(nb_directories > 1)
	   {
		   planeRangedChanged();
		   m_tiffMovie.loadTiffPage(m_current_plane);
	   }
	   if(nb_directories == 1)
	   {
		   planeRangedChanged();
		   m_tiffMovie.loadTiffPage(0);
	   }
	}
}

void TracePlayerModel::clearMovie()
{
	m_tiffMovie.closeTiffMovie();
	planeRangedChanged();
	updateCurrentPlaneWord();
}


void TracePlayerModel::setCurrentPlane(long plane)
{
	m_current_plane = plane;
	updateCurrentPlaneWord();
}

long TracePlayerModel::getCurrentPlane()
{
	return m_current_plane;
}

void TracePlayerModel::updateRenderingPipeline()
{
	vector<vec2> r_events_v;
	vector<vec4> color_events_v;
	vector<vec2> r_trajectories_v;
	vector<vec4> color_trajectories_v;

	//***Color Mode***
	switch(m_colorMode)
	{
		case BANDW_COLOR_MODE :
		{
			m_pgm_events = m_pgm_gaussianBandW;
			m_pgm_trajectories = m_pgm_lineBandW;
		}
		break;

		case RANDOM_COLOR_MODE :
		{
			m_pgm_events = m_pgm_gaussianColor;
			m_pgm_trajectories = m_pgm_lineColor;

			setTracesColorsRandom(m_filteredTraces);
		}
		break;

		case D_COLOR_MODE :
		{
			m_pgm_events = m_pgm_gaussianColor;
			m_pgm_trajectories = m_pgm_lineColor;

			setTracesColorsUsingDs(m_filteredTraces, m_lut, m_lut_minValue, m_lut_maxValue);
		}
		break;


		case DINST_COLOR_MODE :
		{
			m_pgm_events = m_pgm_gaussianColor;
			m_pgm_trajectories = m_pgm_lineColor;

			setTracesColorsUsingDInsts(m_filteredTraces, m_lut, m_lut_minValue, m_lut_maxValue);
		}
		break;

		case LENGTH_COLOR_MODE :
		{
			m_pgm_events = m_pgm_gaussianColor;
			m_pgm_trajectories = m_pgm_lineColor;

			setTracesColorsUsingLengths(m_filteredTraces, m_lut);
		}
		break;
	}

	//***Trajectories Mode***
	switch(m_trajectories_renderingMode)
	{
		case NONE_TRAJECTORIES_RENDERING_MODE :
		{
		}
		break;

		case CURRENT_TRAJECTORIES_RENDERING_MODE :
		{
			getTracesCoordsInPlane(m_current_plane, m_filteredTraces, r_trajectories_v, color_trajectories_v);
		}
		break;

		case ALL_TRAJECTORIES_RENDERING_MODE :
		{
			getTracesCoords(m_filteredTraces, r_trajectories_v, color_trajectories_v);
		}
		break;
	}

	//***Events Mode***
	switch(m_events_renderingMode)
	{
		case NONE_EVENTS_RENDERING_MODE :
		{
		}
		break;

		case CURRENT_EVENTS_RENDERING_MODE :
		{
			getEventCoordsInPlane(m_current_plane, m_filteredTraces, r_events_v, color_events_v);
		}
		break;

		case ALL_EVENTS_RENDERING_MODE :
		{
			getEventsCoords(m_filteredTraces, r_events_v, color_events_v);
		}
		break;
	}

	m_gR_traces.clear();
	m_gColor_traces.clear();
	m_gR.clear();
	m_gColor.clear();

	m_gR_traces.insert(0, r_trajectories_v);
	m_gColor_traces.insert(0, color_trajectories_v);
	m_gR.insert(0, r_events_v);
	m_gColor.insert(0, color_events_v);

}

void TracePlayerModel::render(myGLWidget* window)
{
	window->makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glLineWidth(m_lineWidth);
	glDisable(GL_LINE_SMOOTH);

	window->setClearColor({0.0,0.0,0.0,1});
	window->clearColorBuffer();


	//***Trajectories rendering***

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_LINE_SMOOTH);
	m_tiffMovie.render(window);

	mat4 m = window->getWorldToExtendedHomMatrix();

	if(m_lineWidth != 0)
	{
        m_pgm_trajectories.useProgram(true);
            gstd::myConnector<vec2>::connect(m_pgm_lineColor, "R", m_gR_traces);
            gstd::myConnector<vec4>::connect(m_pgm_lineColor, "color_in", m_gColor_traces);
            gstd::connectUniform(m_pgm_lineColor, "scaling_matrix",  &m,1);
            gstd::connectUniform(m_pgm_lineColor, "line_intensity", &m_lineIntensity, 1);
        glDrawArrays(GL_LINES, 0, m_gR_traces.size());
	}

    ////	//***Events Rendering***
    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	m_pgm_events.useProgram(true);
		gstd::myConnector<vec2>::connect(m_pgm_gaussianColor, "R", m_gR);
		gstd::myConnector<vec4>::connect(m_pgm_gaussianColor, "color_in", m_gColor);
		gstd::connectUniform(m_pgm_gaussianColor, "square_size", &m_square_size,1);
		gstd::connectUniform(m_pgm_gaussianColor, "scaling_matrix", &m,1);
		gstd::connectUniform(m_pgm_gaussianColor, "spot_intensity", &m_spotIntensity,1);
		gstd::connectTexture(m_pgm_gaussianColor, "texture_id", m_gaussian_texture, 0);
	glDrawArrays(GL_POINTS, 0, m_gR.size());

	glBlendFunc(GL_ONE, GL_ZERO);

	m_currentPlane_word.draw(window);
	m_numberTraces_word.draw(window);
	m_numberEvents_word.draw(window);
}

void TracePlayerModel::captureScreen(myGLWidget* window, string& file_path)
{
// RENDERING WITHOUT TEXT
	window->makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glLineWidth(m_lineWidth);
	glDisable(GL_LINE_SMOOTH);

	window->setClearColor({0.0,0.0,0.0,1});
	window->clearColorBuffer();

	//***Trajectories rendering***
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	m_tiffMovie.render(window);

	mat4 m = window->getWorldToExtendedHomMatrix();

			m_pgm_trajectories.useProgram(true);
				gstd::myConnector<vec2>::connect(m_pgm_lineColor, "R", m_gR_traces);
				gstd::myConnector<vec4>::connect(m_pgm_lineColor, "color_in", m_gColor_traces);
				gstd::connectUniform(m_pgm_lineColor, "scaling_matrix",  &m,1);
				gstd::connectUniform(m_pgm_lineColor, "line_intensity", &m_lineIntensity, 1);
			glDrawArrays(GL_LINES, 0, m_gR_traces.size());

    //***Events Rendering***
	m_pgm_events.useProgram(true);
		gstd::myConnector<vec2>::connect(m_pgm_gaussianColor, "R", m_gR);
		gstd::myConnector<vec4>::connect(m_pgm_gaussianColor, "color_in", m_gColor);
		gstd::connectUniform(m_pgm_gaussianColor, "square_size", &m_square_size,1);
		gstd::connectUniform(m_pgm_gaussianColor, "scaling_matrix", &m,1);
		gstd::connectUniform(m_pgm_gaussianColor, "spot_intensity", &m_spotIntensity,1);
		gstd::connectTexture(m_pgm_gaussianColor, "texture_id", m_gaussian_texture, 0);
	glDrawArrays(GL_POINTS, 0, m_gR.size());

	glBlendFunc(GL_ONE, GL_ZERO);


// COPY FRAMEBUFFER IN TEXTURE
	gstd::gTexture screen_text(GL_TEXTURE_2D);
	window->captureScreenBackBuffer(screen_text);

	vector<uint8> text_data;
	screen_text.getTextureData(text_data);

// SAVE DATA IN A .BMP image file
    vec2 screen_size = screen_text.getSize();
	QImage screen_image(text_data.data(), screen_size.x, screen_size.y, QImage::Format_RGBA8888);
	screen_image = screen_image.mirrored(false, true);

	screen_image.save(file_path.data());
}

void TracePlayerModel::captureScreen2(myGLWidget* window)
{

	myTiff tiff;
	tiff.open("test.tiff", myTiff::WRITE_MODE);

	for(int plane_idx = m_min_plane; plane_idx <= m_max_plane; plane_idx++)
	{
// RENDERING WITHOUT TEXT
		setCurrentPlane(plane_idx);
		updateRenderingPipeline();

		window->makeCurrent();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glLineWidth(m_lineWidth);
		glDisable(GL_LINE_SMOOTH);

		window->setClearColor({0.0,0.0,0.0,1});
		window->clearColorBuffer();

		//***Trajectories rendering***

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		m_tiffMovie.render(window);

		mat4 m = window->getWorldToExtendedHomMatrix();

				m_pgm_trajectories.useProgram(true);
					gstd::myConnector<vec2>::connect(m_pgm_lineColor, "R", m_gR_traces);
					gstd::myConnector<vec4>::connect(m_pgm_lineColor, "color_in", m_gColor_traces);
					gstd::connectUniform(m_pgm_lineColor, "scaling_matrix",  &m,1);
					gstd::connectUniform(m_pgm_lineColor, "line_intensity", &m_lineIntensity, 1);
				glDrawArrays(GL_LINES, 0, m_gR_traces.size());

        //***Events Rendering***

		m_pgm_events.useProgram(true);
			gstd::myConnector<vec2>::connect(m_pgm_gaussianColor, "R", m_gR);
			gstd::myConnector<vec4>::connect(m_pgm_gaussianColor, "color_in", m_gColor);
			gstd::connectUniform(m_pgm_gaussianColor, "square_size", &m_square_size,1);
			gstd::connectUniform(m_pgm_gaussianColor, "scaling_matrix", &m,1);
			gstd::connectUniform(m_pgm_gaussianColor, "spot_intensity", &m_spotIntensity,1);
			gstd::connectTexture(m_pgm_gaussianColor, "texture_id", m_gaussian_texture, 0);
		glDrawArrays(GL_POINTS, 0, m_gR.size());

		glBlendFunc(GL_ONE, GL_ZERO);


// COPY FRAMEBUFFER IN TEXTURE

		gstd::gTexture screen_text(GL_TEXTURE_2D);
		window->captureScreenBackBuffer(screen_text);

		vector<uint8> text_data;
		screen_text.getTextureData(text_data);

		tiff.setBytesPerSample(4);
		tiff.setSamplesPerPixel(1);
		tiff.setTiffSize(screen_text.getSize());
//		tiff.addPage(text_data);
	}

	tiff.close();
	tiff.open("test.tiff", myTiff::READ_MODE);

//	//* SAVE DATA IN A .BMP image file *

//	myTiff tiff;
//	tiff.open("test.tiff", myTiff::WRITE_MODE);
//	tiff.setBytesPerSample(1);
//	tiff.setSamplesPerPixel(3);

//	tiff.addPage();

//		vec2 screen_size = screen_text.getSize();
//	QImage screen_image(text_data.data(), screen_size.x, screen_size.y, QImage::Format_RGBA8888);
//	screen_image = screen_image.mirrored(false, true);

//	screen_image.save("screen_capture.bmp");
}

void TracePlayerModel::filter()
{
	m_filteredTraces.clear();
	if(m_traces.size() == 0) return; //->

	auto trace = m_traces.begin();
	while(trace != m_traces.end())
	{
		if(trace->getLength() <= m_max_length && trace->getLength() >= m_min_length)
		{
			float logD = log10(trace->getD());

			if(trace->isDCalculated() &&
			   logD >= TracePlayerModel::m_minLogD &&
			   logD <= TracePlayerModel::m_maxLogD)
			{
				m_filteredTraces.push_back(*trace);
			}
		}
		trace++;
	}

	nbTracesChanged(m_traces.size()); //to keep empty plane...
	planeRangedChanged();

	updateCurrentPlaneWord();
	updateNbEventsWord();
	updateNbTracesWord();
}



void TracePlayerModel::computeMSDsAndDs(float pixel_size, float dt)
{
	computeMSDs(m_traces, m_MSD_maxDPplane);
	computeDs(m_traces, pixel_size, dt, 1, m_D_nbPointsMSDFit);
	computeDInst(m_traces, pixel_size, dt, m_DInst_nbPointsBeforeMSDFit, m_DInst_nbPointsAfterMSDFit);
}

/*virtual*/ void TracePlayerModel::nbTracesChanged(int new_nb_traces){}
/*virtual*/ void TracePlayerModel::maxPlaneChanged(int new_max_plane){}
/*virtual*/ void TracePlayerModel::minPlaneChanged(int new_min_plane){}
/*virtual*/ void TracePlayerModel::planeRangedChanged(){}
/*virtual*/ void TracePlayerModel::maxLengthChanged(int new_maxLength){}
/*virtual*/ void TracePlayerModel::minLengthChanged(int new_maxLength){}


void TracePlayerModel::updateCurrentPlaneWord()
{
	string planeIdx_str = "PLANE :";
	planeIdx_str += floatToString(m_current_plane);
	planeIdx_str += '/';
	planeIdx_str += floatToString(TracePlayerModel::m_max_plane);
	m_currentPlane_word.setText(planeIdx_str);
	m_currentPlane_word.setDirection(vec2(1,0));
	m_currentPlane_word.setColor(vec4(1,1,1,1));
	m_currentPlane_word.setLetterSizePxl(6);
	m_currentPlane_word.setPositionPxl(vec2(10,60));
}

void TracePlayerModel::updateNbTracesWord()
{
	string nbTraces_str = "NB_TRACES :";
	nbTraces_str += floatToString(m_filteredTraces.size());
	m_numberTraces_word.setText(nbTraces_str);
	m_numberTraces_word.setDirection(vec2(1,0));
	m_numberTraces_word.setColor(vec4(0,1,0,1));
	m_numberTraces_word.setLetterSizePxl(6);
	m_numberTraces_word.setPositionPxl(vec2(10,40));
}

void TracePlayerModel::updateNbEventsWord()
{
	string nbEvents_str = "NB_EVENTS :";
	nbEvents_str += floatToString(getNbEvents(m_filteredTraces));
	m_numberEvents_word.setText(nbEvents_str);
	m_numberEvents_word.setDirection(vec2(1,0));
	m_numberEvents_word.setColor(vec4(0.3,0.3,1,1));
	m_numberEvents_word.setLetterSizePxl(6);
	m_numberEvents_word.setPositionPxl(vec2(10,20));
}
