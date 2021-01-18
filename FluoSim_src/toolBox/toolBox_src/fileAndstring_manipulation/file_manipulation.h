
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

#ifndef FILE_MANIPULATION_H
#define FILE_MANIPULATION_H

#include "toolbox_library_global.h"
#include "fileAndstring_manipulation/string_manipulation.h"
#include "iostream"
#include <fstream>

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>

TOOLBOXSHARED_EXPORT bool getFileLocation(char** p_file_directory,
											const char* working_directory = "\0",
											QWidget* parent = 0);

TOOLBOXSHARED_EXPORT bool getFileLocation(std::string& file_directory,
											const std::string working_directory = "\0",
											QWidget* parent = 0);


TOOLBOXSHARED_EXPORT bool getSaveLocationFiltered(std::string& file_directory,
													std::vector<std::pair<std::string, std::string> > typeGroupName_filteredTypes_v,
													const std::string default_caption,
													const std::string working_directory = "\0",
													QWidget* parent = 0);

TOOLBOXSHARED_EXPORT bool getFileLocationFiltered(std::string& file_directory,
													std::vector<std::string> typeFiltered_v,
													const std::string working_directory = "\0",
													QWidget* parent = 0);

TOOLBOXSHARED_EXPORT bool getFileLocationFiltered(std::string& file_directory,
													std::vector<std::pair<std::string, std::string> > typeGroupName_filteredTypes_v,
													const std::string working_directory = "\0",
													QWidget* parent = 0);

TOOLBOXSHARED_EXPORT bool getFileLocations(std::vector<std::string>& file_directory,
											 const std::string working_directory = "\0",
											 QWidget* parent = 0);

TOOLBOXSHARED_EXPORT bool getFolderLocation(std::string& folder_directory,
											  const std::string working_directory = "\0",
											  bool areFilesVisible = true, QWidget* parent = 0);


TOOLBOXSHARED_EXPORT bool getFileContent(char* file_directory, char** file_str, int* strlen = 0);

TOOLBOXSHARED_EXPORT bool getFileContent(std::string file_directory, std::string& file_str);




#endif // FILE_MANIPULATION_H
