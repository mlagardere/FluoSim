
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



#include "ScreenHandler.h"

using namespace std;
using namespace glm;

ScreenHandler::ScreenHandler(QApplication* app) :

	m_screen()

{
		m_app = app;
		m_is_initialized = 0;
}

ScreenHandler::~ScreenHandler()
{

	if(m_is_initialized)
	{
	}

}

void ScreenHandler::initializeScreen(QWidget* parent)

{
	if(m_is_initialized)
	{
		cout<<"ScreenHandler has already been initialized!\n";
		return;
	}
		uint size[2] = {600,600};
//		m_screen = new Screen();
		m_screen.intializeScreen(size,parent);
		m_is_initialized=1;
}


void ScreenHandler::addRegions(list<Region> &regions, int primitive_type)
{
	int region_size;
	vec4 region_color_temp;

	int n_region = regions.size();
	vector<vec2> r_region;
	vector<vec4> color;
	vector<vec2> r_region_temp;

	switch (primitive_type)
	{
		case s_POINT :
		{
			std::list<Region>::iterator it_rgn = regions.begin();
			for(int region_idx = 0; region_idx<=n_region-1; region_idx++)
			{
				it_rgn = regions.begin();
				advance(it_rgn, region_idx);

				r_region.clear();
				color.clear();

				region_size = (*it_rgn).getSize();
				color.push_back(vec4(0,1.0,0,0));

					for(int i=1; i<=region_size-1; i++) // i=1 since the first color (i=0) is set to green!
					{
						color.push_back(vec4(0,0,1.0,1.0));
					}

			   (*it_rgn).getRegionSubData(&r_region,0, region_size);
				m_screen.bufferData(r_region, color, 0);
			}
		}
		break;

		case s_LINE :
		{
			std::list<Region>::iterator it_rgn = regions.begin();
			for(int region_idx = 0; region_idx<=n_region-1; region_idx++)
			{
				it_rgn = regions.begin();
				advance(it_rgn, region_idx);

				r_region.clear();
				color.clear();
				r_region_temp.clear();

					region_size = (*it_rgn).getSize();
					region_color_temp = (*it_rgn).getColor();
					(*it_rgn).getRegionSubData(&r_region_temp, 0, region_size);

					for(int i=0; i<=region_size-1; i++)
					{
						color.push_back(region_color_temp);
						color.push_back(region_color_temp);

						if(i!=region_size-1)
						{
							//r
							r_region.push_back(r_region_temp[i]);
							r_region.push_back(r_region_temp[i+1]);
						}
						else
						{
							//r
							r_region.push_back(r_region_temp[i]);
							r_region.push_back(r_region_temp[0]);
						}
					}
					m_screen.bufferData(r_region, color, 1);
			}
		}
		break;
	}
}




void ScreenHandler::addParticles(vector<Particle>& particles, float pointingAccuracy_px)
{
	vector<vec2> r;
	vector<vec4> color;

	if(pointingAccuracy_px <= 0)
	{
		for(Particle& prtl : particles)
		{
			if(prtl.getIntensity() != 0.0)
			{
				r.push_back(prtl.getR());
				color.push_back(prtl.getColor());
			}
		}
	}
	else
	{
		vec2 dr;
		for(Particle& prtl : particles)
		{
			if(prtl.getIntensity() != 0.0)
			{
				dr = pointingAccuracy_px*vec2(m_gaussianFactory.gaussianRandomNumber(false),
											  m_gaussianFactory.gaussianRandomNumber(false));
				r.push_back(prtl.getR() + dr);
				color.push_back(prtl.getColor());
			}
		}
	}

	m_screen.bufferData(r, color, 0);
}

void ScreenHandler::addParticles(list<Particle>& particles,  float pointingAccuracy_px)
{
	vector<vec2> r;
	vector<vec4> color;

	if(pointingAccuracy_px <= 0)
	{
		for(Particle& prtl : particles)
		{
			if(prtl.getIntensity() != 0.0)
			{
				r.push_back(prtl.getR());
				color.push_back(prtl.getColor());
			}
		}
	}
	else
	{
		vec2 dr;
		for(Particle& prtl : particles)
		{
			if(prtl.getIntensity() != 0.0)
			{
				dr = pointingAccuracy_px*vec2(m_gaussianFactory.gaussianRandomNumber(false),
											  m_gaussianFactory.gaussianRandomNumber(false));
				r.push_back(prtl.getR() + dr);
				color.push_back(prtl.getColor());
			}
		}
	}

	m_screen.bufferData(r, color, 0);

}

void ScreenHandler::setSpotSize(glm::vec2 spotSize)
{
	m_screen.setSpotSize(spotSize);
}

void ScreenHandler::setSpotIntensity(float intensity)
{
	m_screen.setSpotIntensity(intensity);
}

void ScreenHandler::setIsAutoscale(bool isAutoscale)
{
    m_screen.setIsAutoscale(isAutoscale);
}

void ScreenHandler::setAutoscaleFactor(float autoscale_factor)
{
    m_screen.setAutoscaleFactor(autoscale_factor);
}
void ScreenHandler::setLUT(string lut)
{
	m_screen.setLUT(lut);
}

void ScreenHandler::setPointSize(int spotSize)
{
	m_screen.setPointSize(spotSize);
}

void ScreenHandler::setPointIntensity(float intensity)
{
	m_screen.setPointIntensity(intensity);
}


void ScreenHandler::addPoints(vector<vec2>& points_R, vector<vec4>& points_color)
{
	m_screen.bufferData(points_R, points_color,0);
}

void ScreenHandler::addPoints(vector<vec2>& points_R)
{
	vector<vec4> color;
	vector<vec2>::iterator it_pt;

	for(it_pt = points_R.begin(); it_pt != points_R.end(); ++it_pt)
	{
			color.push_back(vec4(0,1,0,0));
	}
	m_screen.bufferData(points_R, color, 0);
}

void ScreenHandler::drawRegion(GEOMETRY geometry)
{
	myGLScreenLoopedGeometry* looped_geom = m_screen.getRenderWindow()->drawLoopedGeometricObject(m_geom_mV, m_geom_gmV,
														   m_contour_mV, m_contour_gmV,
														   geometry);

	connect(looped_geom, SIGNAL(hasBeenDrawn_sig()), this, SLOT(rgnHasBeenDrawn()));
}

void ScreenHandler::setPixelView(vec2 r, vec2 delta_r)
{
	m_screen.getRenderWindow()->fitQuadToScreen(r, r+delta_r);
}


void ScreenHandler::drawBuffer(Screen::RENDERING_TARGET target)
{
	m_screen.drawBuffer(target);
}

void ScreenHandler::renderBeingDrawnRegion()
{
    m_screen.getRenderWindow()->renderObjects();
}


void ScreenHandler::applyPoissonNoise()
{
//	m_screen.applyPoissonNoise();
}

void ScreenHandler::applyOffsetsAndNoises()
{
    m_screen.applyOffsetsAndNoises();
}

void ScreenHandler::refreshScreen()
{
	m_screen.getRenderWindow()->swapBuffers();
}

void ScreenHandler::clearScreen()
{
	m_screen.getRenderWindow()->clearColorBuffer();
}

void ScreenHandler::clearCamera()
{
	m_screen.clearCamera();
}

void ScreenHandler::removeRenderable()
{
	m_screen.clearBuffer(0);
	m_screen.clearBuffer(1);
}


myGLScreen* ScreenHandler::getRenderWindow()
{
	return m_screen.getRenderWindow();
}


void ScreenHandler::showScreen()
{
	m_screen.showScreen();
}

void ScreenHandler::setRenderingMode(Screen::RENDERING_MODE mode)
{
	m_screen.setRenderingMode(mode);
}


void ScreenHandler::rgnHasBeenDrawn()
{
	myGLScreenLoopedGeometry* looped_geom = (myGLScreenLoopedGeometry*) sender();
	emit(rgnHasBeenDrawn_signal(looped_geom->getGeometry()));

	m_screen.getRenderWindow()->removeObject(looped_geom);
}

void ScreenHandler::setCameraField(glm::vec2& bottomLeft,glm::vec2& topRight)
{
	m_screen.setCameraField(bottomLeft, topRight);
}

void ScreenHandler::setCameraDefinition(glm::vec2& definition)
{
	m_screen.setCameraDefinition(definition);
}

void ScreenHandler::setCameraPrePoissonOffset(float bckg)
{
    m_screen.setCameraPrePoissonOffset(bckg);
}

void ScreenHandler::setCameraIsUsingPoissonNoise(bool isUsingPoissonNoise)
{
    m_screen.setCameraIsUsingPoissonNoise(isUsingPoissonNoise);
}

void ScreenHandler::setCameraGain(float gain)
{
    m_screen.setCameraGain(gain);
}

void ScreenHandler::setCameraIsBypassingPoissonAndNoise(bool isBypassing)
{
    m_screen.setCameraIsBypassingPoissonAndGain(isBypassing);
}

void ScreenHandler::setCameraPostGainOffset(float offset)
{
    m_screen.setCameraPostGainOffset(offset);
}

void ScreenHandler::setCameraReadoutNoiseSigma(float sigma)
{
    m_screen.setCameraReadoutNoiseSigma(sigma);
}

gstd::gTexture& ScreenHandler::getCameraTexture()
{
	return m_screen.getCameraTexture();
}

