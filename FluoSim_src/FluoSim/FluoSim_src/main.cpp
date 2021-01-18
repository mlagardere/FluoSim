
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


#include "stdlib.h"
#include "time.h"

#include "QApplication.h"
#include "QDesktopWidget"

#include "FluoSim.h"

using namespace std;


int main(int argc, char* argv[])
{
    QApplication main_app(argc, argv);
    QLocale::setDefault(QLocale::C);

    QSplashScreen splashScreen(QPixmap("Resources/Icons/logo.png"));
    splashScreen.show();

    srand (time(NULL));
    string projectFile_path;
    string destination_path;
    if(argc > 1)
    {
        projectFile_path = argv[1];
        if(QFile(projectFile_path.data()).exists() == false) projectFile_path.clear();

        if(argc > 2)
        {
            destination_path = argv[2];
            if(QFile(destination_path.data()).exists() == false) destination_path.clear();
        }
    }

    myDropMenu::m_darkTheme = true;
    FluoSim FluoSim_simulator(&main_app);

    if(projectFile_path.size() != 0)
    {
        FluoSim_simulator.loadProject(projectFile_path);
        FluoSim_simulator.loadProject(projectFile_path);
        if(destination_path.size() != 0)
        {
            FluoSim_simulator.setDestinationPath(destination_path);
        }
    }

    splashScreen.hide();
    FluoSim_simulator.runSimulator();

    QTimer::singleShot(10, &main_app, SLOT(quit()));
    return main_app.exec();
}


