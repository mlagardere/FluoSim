
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

#ifndef GPU_MYFILEHANDLER_H
#define GPU_MYFILEHANDLER_H

#include "iostream"
#include <fstream>

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>

using namespace std;


namespace gstd
{

class myFileHandler
{
public:

    myFileHandler(QWidget* parent=0)
    {
        m_parent = parent;
    }

    static bool getFileLocation(string &file_dir, const char* working_directory = "\0")
    {
        QFileDialog dialog;

            dialog.setFileMode(QFileDialog::AnyFile);
            if(strlen(working_directory)!=0) dialog.setDirectory(QString(working_directory));
            dialog.exec();

            QStringList file_directory_qstrL = dialog.selectedFiles();
            if (file_directory_qstrL.size()==0)
            {
                cout<<"No file selected!\n";
                return 0;
            }
            QString file_directory_qstr = file_directory_qstrL[0];

        QByteArray file_directory_8barray = file_directory_qstr.toLocal8Bit();

        file_dir.clear();
        file_dir = string(file_directory_8barray.data());

        return 1;
    }

    bool getFileLocation(char** p_file_directory, const char* working_directory = "\0")
    {
        QFileDialog dialog;

            dialog.setFileMode(QFileDialog::AnyFile);
            if(strlen(working_directory)!=0) dialog.setDirectory(QString(working_directory));
            dialog.exec();

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

    bool getTrcFileLocation(char** p_file_directory, const char* working_directory = "\0")
    {
        QFileDialog dialog;

            dialog.setNameFilter("(*.trc)");
            if(strlen(working_directory)!=0) dialog.setDirectory(QString(working_directory));
            dialog.exec();

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

    static bool getFileContent(char* file_directory, char** file_str)
    {
        ifstream myfile(file_directory, ifstream::binary);
        if(myfile.is_open())
        {
                myfile.seekg(0, myfile.end);
                int file_lgth=myfile.tellg();
                myfile.seekg(0, myfile.beg);
                *file_str= new char[file_lgth+1];
                myfile.read(*file_str, file_lgth);
                (*file_str)[file_lgth] = '\0';
            myfile.close();
            return 1;
        }
        else
        {
            return 0;
        }
    }

    static bool getFileContent(string file_directory, string& file_content_str)
    {
        ifstream myfile(file_directory.data(), ifstream::binary);
        if(myfile.is_open())
        {
            char* file_content_cstr;

                myfile.seekg(0, myfile.end);
                int file_lgth=myfile.tellg();
                myfile.seekg(0, myfile.beg);
                file_content_cstr = new char[file_lgth+1];
                myfile.read(file_content_cstr, file_lgth);
                file_content_cstr[file_lgth] = '\0';
            myfile.close();

            file_content_str = string(file_content_cstr);
            return 1;
        }
        else
        {
            return 0;
        }
    }



private:

    QWidget* m_parent;

};

}

#endif // GPU_MYFILEHANDLER_H
