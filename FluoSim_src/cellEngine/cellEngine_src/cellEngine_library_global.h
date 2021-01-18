
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

#ifndef CELLENGINE_LIBRARY_GLOBAL_H
#define CELLENGINE_LIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CELLENGINE_LIBRARYSHARED)
#  define CELLENGINE_LIBRARYSHARED_EXPORT Q_DECL_EXPORT
#elif defined(CELLENGINE_LIBRARYSTATIC)
#  define CELLENGINE_LIBRARYSHARED_EXPORT
#else
#  define CELLENGINE_LIBRARYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MYBIOLOGICAL_LIBRARY_GLOBAL_H

