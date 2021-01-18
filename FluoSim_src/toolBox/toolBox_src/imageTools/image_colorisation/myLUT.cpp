
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


#include "myLUT.h"





using namespace std;

myLUT::myLUT()
{
//	string file_content;
//	string line; int line_pos = 0;
//	string word; int word_pos = 0;
//	glm::vec4 color;
//	if(getFileContent("ncl_default.rgb", file_content) != true) return;

//	while(getLine(file_content, line, line_pos, line_pos))
//	{
//		word_pos = 0;
//		getWord(line, word, word_pos, word_pos);
//		color.x = strtod(word.data(), 0);
//		getWord(line, word, word_pos, word_pos);
//		color.y = strtod(word.data(), 0);
//		getWord(line, word, word_pos, word_pos);
//		color.z = strtod(word.data(), 0);
//		color[3] = 1.0f;

//		m_colors.push_back(color);
//	}
}

void myLUT::loadLUT(const std::string &path)
{
	enum LUT_EXT {DOT_LUT, DOT_RGB};
	map<string, LUT_EXT> lutExtensions_map = {{"lut", DOT_LUT},
											  {"LUT", DOT_LUT},
											  {"rgb", DOT_RGB},
											  {"RGB", DOT_RGB}};
	//get extension
	uint ext_pos = path.find_last_of(".");
	if(ext_pos == path.npos) return; //file has no extension ->
	string path_ext = path.substr(ext_pos+1);

	auto lut_it = lutExtensions_map.find(path_ext);
	if(lut_it == lutExtensions_map.end()) return; //file extension is not recognized;

	LUT_EXT lut_ext = lut_it->second;
	switch(lut_ext)
	{
		case DOT_LUT :
		{
			ifstream myfile;
			myfile.open(path, ios_base::binary); //!
			if(myfile.is_open() == false) return; //->

			myfile.seekg(0, myfile.end);
			uint file_length = myfile.tellg();

			//test if the file is a binary file
			if(file_length == 800 || file_length == 768)
			{
				char* data_cstr;
				uint dataChunk_length;
				if(file_length == 800)
				{
                    //we jump the header
					dataChunk_length = (file_length-32)/3;
					data_cstr = new char[file_length-32];
					myfile.seekg(32, myfile.beg);
					myfile.read(data_cstr, file_length-32);
				}
				if(file_length == 768)
				{
					dataChunk_length = file_length/3;
					data_cstr = new char[file_length];
					myfile.seekg(0, myfile.beg);
					myfile.read(data_cstr, file_length);
				}

				m_colors.clear();
				m_colors.assign(dataChunk_length,{0,0,0,1.0f});

				glm::vec4 color;
				for(uint char_offset = 0; char_offset < dataChunk_length; char_offset++)
				{
					color[0] = *((uchar*) data_cstr + char_offset)/256.0f;
					color[1] = *((uchar*) data_cstr + char_offset + dataChunk_length)/256.0f;
					color[2] = *((uchar*) data_cstr + char_offset + 2*dataChunk_length)/256.0f;
					color[3] = 1.0f;
					m_colors[char_offset] = color;
				}

				delete[] data_cstr;
				myfile.close();//!
			}
			else //file is a text file
			{
				myfile.close();//!
				string file_content;
				string line; int line_pos = 0;
				string word; int word_pos = 0;
				glm::vec4 color;
				if(getFileContent(path, file_content) != true) return;

				m_colors.clear();

				//discriminate type of text file
				getLine(file_content, line, line_pos, line_pos);
				if(line.find("Index") != line.npos) line_pos = 0; //we need to jump the first line
				while(getLine(file_content, line, line_pos, line_pos))
				{
					word_pos = 0;
					getWord(line, word, word_pos, word_pos); //index
//					color.x = strtod(word.data(), 0);
					getWord(line, word, word_pos, word_pos); //red
					color.x = strtod(word.data(), 0)/256.0f;
					getWord(line, word, word_pos, word_pos); //green
					color.y = strtod(word.data(), 0)/256.0f;
					getWord(line, word, word_pos, word_pos); //blue
					color.z = strtod(word.data(), 0)/256.0f;
					color[3] = 1.0f;

					m_colors.push_back(color);
				}
			}
		}
		break;

		case DOT_RGB :
		{
			string file_content;
			string line; int line_pos = 0;
			string word; int word_pos = 0;
			glm::vec4 color;
			if(getFileContent(path, file_content) != true) return;

			m_colors.clear();
			while(getLine(file_content, line, line_pos, line_pos))
			{
				word_pos = 0;
				getWord(line, word, word_pos, word_pos);
				color.x = strtod(word.data(), 0);
				getWord(line, word, word_pos, word_pos);
				color.y = strtod(word.data(), 0);
				getWord(line, word, word_pos, word_pos);
				color.z = strtod(word.data(), 0);
				color[3] = 1.0f;

				m_colors.push_back(color);
			}
		}
		break;
	}
}

glm::vec4 myLUT::getColor(float normalized_value)
{
	if(normalized_value<0) normalized_value = 0;
	if(normalized_value>1.0) normalized_value = 1.0;

	return m_colors[(int) (normalized_value*253)];
}

std::vector<glm::vec4> myLUT::getColors()
{
	return m_colors;
}
