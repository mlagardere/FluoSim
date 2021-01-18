
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

#ifndef SCREENHANDLER_H
#define SCREENHANDLER_H

#include "time.h"

#include "cellEngine_library_global.h"
    #include "biologicalWorld/Particle.h"
    #include "biologicalWorld/Region_gpu.h"
    #include "biologicalWorld/Particle.h"
    #include "Screen.h"


class CELLENGINE_LIBRARYSHARED_EXPORT ScreenHandler : public QObject
{
Q_OBJECT

public :
signals :

	void rgnHasBeenDrawn_signal(std::vector<glm::vec2> r_v);

public :

    ScreenHandler(QApplication* app);
    ~ScreenHandler();

    void initializeScreen(QWidget* parent =0);

	//bioWorld Data
		void addRegions(std::list<Region> &regions, int primitive_type);
		void addParticles(std::vector<Particle>& particles, float pointingAccuracy_px = -1);
		void addParticles(std::list<Particle>& particles, float pointingAccuracy_px = -1);
		void addPoints(std::vector<glm::vec2>& points);
		void addPoints(std::vector<glm::vec2>& points_R, std::vector<glm::vec4>& points_color);

	//Rendering Data
		//rendering params
        void setRenderingMode(Screen::RENDERING_MODE mode);
		void setPointSize(int spotSize);
		void setPointIntensity(float intensity);
		void setSpotSize(glm::vec2 spotSize);
		void setSpotIntensity(float intensity);
        void setIsAutoscale(bool isAutoscale);
        void setAutoscaleFactor(float autoscale_factor);
        void setLUT(string lut);
		void setClearColor(glm::vec4 color);
		//camera params
		void setCameraField(glm::vec2& bottomLeft,glm::vec2& topRight);
		void setCameraDefinition(glm::vec2& definition);
        void setCameraPrePoissonOffset(float offset);
        void setCameraIsUsingPoissonNoise(bool);
        void setCameraGain(float gain);
        void setCameraIsBypassingPoissonAndNoise(bool bypassing);
        void setCameraPostGainOffset(float offset);
        void setCameraReadoutNoiseSigma(float sigma);
        void setPixelView(glm::vec2 r, glm::vec2 delta_r);

    void clearScreen();
	void clearCamera();
    void removeRenderable();
    void drawBuffer(Screen::RENDERING_TARGET target = Screen::SCREEN_FRAMEBUFFER);
    void renderBeingDrawnRegion();
	void applyPoissonNoise();
    void applyOffsetsAndNoises();
	void refreshScreen();
	myGLScreen* getRenderWindow();
    void showScreen();

	gstd::gTexture& getCameraTexture();

	void drawRegion(GEOMETRY geometry);

protected slots :

	void rgnHasBeenDrawn();

private :

    bool m_is_initialized;
    QApplication* m_app;
    Screen m_screen;

	myMultiVector<glm::vec2> m_geom_mV;
	myMultiVector<glm::vec2> m_contour_mV;

	gstd::gMultiVector<glm::vec2> m_geom_gmV;
	gstd::gMultiVector<glm::vec2> m_contour_gmV;

	RandomNumberGenerator m_gaussianFactory;
};



#endif // SCREENHANDLER_H
