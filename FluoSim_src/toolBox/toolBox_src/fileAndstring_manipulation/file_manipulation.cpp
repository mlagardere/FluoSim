
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


#include "file_manipulation.h"

using namespace std;



bool getFileLocation(char** p_file_directory, const char* working_directory, QWidget* parent)
{
	QFileDialog dialog(parent);

		dialog.setFileMode(QFileDialog::ExistingFile);
		if(strlen(working_directory)!=0) dialog.setDirectory(QString(working_directory));
		if(dialog.exec() == 0) return 0; //->

		QStringList file_directory_qstrL = dialog.selectedFiles();
		if (file_directory_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0;
		}
		QString file_directory_qstr = file_directory_qstrL[0];

	QByteArray file_directory_8barray = file_directory_qstr.toLocal8Bit();
	*p_file_directory = new char[strlen(file_directory_8barray.data())+1];
	memcpy(*p_file_directory, file_directory_8barray.data(), strlen(file_directory_8barray.data())+1);

	return 1;
}

bool getFileLocation(string& file_directory, const string working_directory, QWidget* parent)
{
	file_directory.clear();

	QFileDialog dialog(parent);
	//nb file
		dialog.setFileMode(QFileDialog::ExistingFile);
	//working directory
		if(working_directory.size() !=0) dialog.setDirectory(QString(working_directory.data()));
		if(dialog.exec() == 0) return 0; //->
	//file selection
		QStringList file_directory_qstrL = dialog.selectedFiles();
		if (file_directory_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0;
		}
		file_directory = string(file_directory_qstrL[0].toLocal8Bit().data());

	return 1;
}





bool getSaveLocationFiltered(string& file_path,
							 vector<pair<string, string> > typeGroupName_filteredTypes_v,
							 const string default_caption,
							 const string working_directory,
							 QWidget* parent)
{

	file_path.clear();

	QFileDialog dialog(parent);
	//nb file
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.selectFile(default_caption.data());
	//working directory
		if(working_directory.size() !=0) dialog.setDirectory(QString(working_directory.data()));

		QStringList list;
			string temp_filteredTypesList;
				string temp_filteredType;
				int pos = 0;
		auto filter_pair = typeGroupName_filteredTypes_v.begin();
		while (filter_pair != typeGroupName_filteredTypes_v.end())
		{
			pos = 0;
			temp_filteredTypesList.clear();
			temp_filteredTypesList += filter_pair->first + " ";
			temp_filteredTypesList += "(";
				while(getWord(filter_pair->second, temp_filteredType, pos, pos))
				{
					temp_filteredTypesList += "*.";
					temp_filteredTypesList += temp_filteredType;
					temp_filteredTypesList += " ";
					temp_filteredType.clear();
				}
			temp_filteredTypesList += ")";
			list.push_back(temp_filteredTypesList.data());
			filter_pair++;
		}

		dialog.setNameFilters(list);

		if(dialog.exec() == 0) return 0; //->
	//file selection
		QStringList file_directory_qstrL = dialog.selectedFiles();
		if (file_directory_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0;
		}
		file_path = string(file_directory_qstrL[0].toLocal8Bit().data());
		cout<<file_path<<"\n";

	return 1;

}

bool getFileLocations(vector<string>& file_directory, const string working_directory, QWidget* parent)
{
	file_directory.clear();

	QFileDialog dialog(parent);
	//nb file
		dialog.setFileMode(QFileDialog::ExistingFiles);
	//working directory
		if(working_directory.size() !=0) dialog.setDirectory(QString(working_directory.data()));
		if(dialog.exec() == 0) return 0; //->

	//file selection
		QStringList file_directory_qstrL = dialog.selectedFiles();
		if (file_directory_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0;
		}

		auto path_qstr = file_directory_qstrL.begin();
		while(path_qstr != file_directory_qstrL.end())
		{
			file_directory.push_back(string(path_qstr->toLocal8Bit().data()));
			path_qstr++;
		}

	return 1;
}


bool getFileLocationFiltered(string& file_directory,  vector<string> typeFiltered_v, const string working_directory, QWidget* parent)
{
	file_directory.clear();

	QFileDialog dialog(parent);
		dialog.setAcceptMode(QFileDialog::AcceptOpen);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setViewMode(QFileDialog::Detail);
		if(working_directory.size() !=0) dialog.setDirectory(QString(working_directory.data()));

		QStringList list;
		auto typeFiltered = typeFiltered_v.begin();
		while(typeFiltered != typeFiltered_v.end())
		{
			list.push_front(string("(*." + *typeFiltered + ")").data());
			typeFiltered++;
		}
		dialog.setNameFilters(list);
	if(dialog.exec() == 0) return 0; //->

	//file selection
		QStringList file_directory_qstrL = dialog.selectedFiles();
		if (file_directory_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0;
		}
		file_directory = string(file_directory_qstrL[0].toLocal8Bit().data());
		cout<<file_directory<<"\n";

	return 1;
}

bool getFileLocationFiltered(string& file_directory,
							 vector<pair<string, string> > typeGroupName_filteredTypes_v,
							 const string working_directory, QWidget* parent)
{
	file_directory.clear();

	QFileDialog dialog(parent);
		dialog.setAcceptMode(QFileDialog::AcceptOpen);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setViewMode(QFileDialog::Detail);
		if(working_directory.size() !=0) dialog.setDirectory(QString(working_directory.data()));

		QStringList list;
			string temp_filteredTypesList;
				string temp_filteredType;
				int pos = 0;
		auto filter_pair = typeGroupName_filteredTypes_v.begin();
		while (filter_pair != typeGroupName_filteredTypes_v.end())
		{
			pos = 0;
			temp_filteredTypesList.clear();
			temp_filteredTypesList += filter_pair->first + " ";
			temp_filteredTypesList += "(";
				while(getWord(filter_pair->second, temp_filteredType, pos, pos))
				{
					temp_filteredTypesList += "*.";
					temp_filteredTypesList += temp_filteredType;
					temp_filteredTypesList += " ";
					temp_filteredType.clear();
				}
			temp_filteredTypesList += ")";
			list.push_back(temp_filteredTypesList.data());
			filter_pair++;
		}

		dialog.setNameFilters(list);
		if(dialog.exec() == 0) return 0; //->

		//file selection
			QStringList file_directory_qstrL = dialog.selectedFiles();
			if (file_directory_qstrL.size()==0)
			{
				cout<<"No file selected!\n";
				return 0;
			}
			file_directory = string(file_directory_qstrL[0].toLocal8Bit().data());

	return 1;
}

bool getFolderLocation(std::string& folder_directory,
					   const std::string working_directory,
					   bool areFilesVisible, QWidget* parent)
{
	QFileDialog dialog(parent);

		dialog.setFileMode(QFileDialog::Directory);
		dialog.setOption(QFileDialog::ShowDirsOnly, !areFilesVisible);

		if(working_directory.size()!=0) dialog.setDirectory(QString(working_directory.data()));
		if(dialog.exec() == 0) return 0; //->

		QStringList folder_directories_qstrL = dialog.selectedFiles();
		if (folder_directories_qstrL.size()==0)
		{
			cout<<"No file selected!\n";
			return 0; //->
		}
		QString folder_directory_qstr = folder_directories_qstrL[0];
		folder_directory = string(folder_directory_qstr.toLocal8Bit().data());

	return 1;
}

bool getFileContent(char* file_directory, char** file_str, int* strlen)
{
	ifstream myfile;
	myfile.open(file_directory, ifstream::binary);
	if(myfile.is_open())
	{
        myfile.seekg(0, myfile.end);
        int file_lgth=myfile.tellg();
        myfile.seekg(0, myfile.beg);
        *file_str= new char[file_lgth+1];
        myfile.read(*file_str, file_lgth);
        (*file_str)[file_lgth] = '\0';
		myfile.close();
		if(strlen != 0) *strlen = file_lgth;
		return 1;

	}
	else
	{
		return 0;
	}
}

bool getFileContent(string file_directory, string& file_str)
{
	file_str.clear();

	ifstream myfile(file_directory.data(), ifstream::binary);
	if(myfile.is_open())
	{
		char* temp_file;
        myfile.seekg(0, myfile.end);
        int file_lgth=myfile.tellg();
        myfile.seekg(0, myfile.beg);
        temp_file = new char[file_lgth+1];
        myfile.read(temp_file, file_lgth);
        temp_file[file_lgth] = '\0';
		myfile.close();

		file_str = string(temp_file);
		return 1;
	}
	else
	{
		return 0;
	}
}

