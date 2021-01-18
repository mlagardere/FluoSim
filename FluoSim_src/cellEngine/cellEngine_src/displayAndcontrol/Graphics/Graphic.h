
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

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "QApplication"

#include "cellEngine_library_global.h"
    #include "Axis.h"
    #include "Measure/Signal.h"

#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/glGUI/glWidgets/myGLScreen/myGLScreen.h"


class CELLENGINE_LIBRARYSHARED_EXPORT Graphic : public myGLWidget
{
public :

	enum GRAPHIC_MODE {FITTEST_MODE, ORIGIN_VISIBLE_MODE};


    Graphic(QApplication* app, QWidget* parent = 0, QGLWidget* sharedContext_glWdgt = 0);
    ~Graphic();

//    void initializeScreen(QWidget* parent = 0);
    void addSignal(Signal* glObject);
    void removeSignal(Signal* signal);
	void clearSignals();
	void renderSignals();

	void setXAxisText(string text);
	void setYAxisText(string text);

    void drawScreen();
    void clearScreen();

private :

	GRAPHIC_MODE m_graphicMode;

	gstd::gMultiVector<glm::vec2> m_axisGeom_gMV;
	Axis* m_XAxis;
	Axis* m_YAxis;

    vector<Signal*> m_signals_v;
    QApplication* m_app;
};

#endif // GRAPHIC_H
