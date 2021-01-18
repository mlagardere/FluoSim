
/*
 *  FluoSim (simulation program of live cell fluorescence microscopy experiments)
 *  Copyright (C) 2020 Matthieu Lagardère
 *
 *	FluoSim is free software: you can redistribute it and/or modify
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

#ifndef GPUTOOLS_LIBRARY_GLOBAL_H
#define GPUTOOLS_LIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GPUTOOLS_LIBRARYSHARED) //building a shared library (.dll), Q_DECL_EXPORT function declaration flag needed
#  define GPUTOOLS_LIBRARYSHARED_EXPORT Q_DECL_EXPORT
#elif defined(GPUTOOLS_LIBRARYSTATIC) //building and/or linking a satic library (.a), no function declaration flags needed. Need to be defined when building and linking !
#  define GPUTOOLS_LIBRARYSHARED_EXPORT
#else //used by default : for linking to the built shared library
#	define GPUTOOLS_LIBRARYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MYGSTD_LIBRARY_GLOBAL_H
