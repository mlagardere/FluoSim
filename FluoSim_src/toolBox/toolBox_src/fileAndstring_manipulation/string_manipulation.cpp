
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



#include "string_manipulation.h"

using namespace std;

//see C-str for tokens...

bool getWord(string &considered_string,
			 string &word, int beg_pos,
			 int &end_pos,
			 string separating_char)
{
	word.clear();

	if(beg_pos >= considered_string.size()-1)
	{
		end_pos = -1;
		return 0;
	}

	else
	{
		string unaccpeted_startingChar = (" \r\n\t/");
		bool word_started=0;

		int substring_idx = beg_pos;
		bool word_ended = 0;
		for(int char_idx = 0; char_idx <= separating_char.size()-1; char_idx++)
		{
			word_ended = word_ended || considered_string[substring_idx] == separating_char[char_idx];
		}

		while(
			  ((!word_started) ||  !word_ended)  &&

			  substring_idx<= considered_string.size()-1)
		{
			if(!word_started) // is the word started?
			{
				word_started = 1;
				for(int char_idx = 0; char_idx <= unaccpeted_startingChar.size()-1; char_idx++)
				{
					if(considered_string[substring_idx] == unaccpeted_startingChar[char_idx])
					{
						word_started = 0;
					}
				}
				if(word_started)   //if the word started
				{
					beg_pos = substring_idx; // to know where the word actually starts!
					word += considered_string[substring_idx];
				}
			}
			else
			{
				word += considered_string[substring_idx];
			}

			substring_idx++;
			word_ended = 0;
			for(int char_idx = 0; char_idx <= separating_char.size()-1; char_idx++)
			{
				word_ended = word_ended || considered_string[substring_idx] == separating_char[char_idx];
			}

		}

		if(substring_idx >= considered_string.size()-1) end_pos = -1;
		else   end_pos = beg_pos + word.size();

		return 1;
	}
}

bool getLine(string &considered_string, string &line, int beg_pos, int &end_pos)
{
	line.clear();

    if(beg_pos > considered_string.size()-1) //one char string are allowed
	{
		end_pos = -1;
		return 0;
	}


	string removed_endingChar = (" \n\t\r");

    //we look for the end of the line (could be either \n or file_end)
	int current_idx = beg_pos;
	bool endLine_reached = (considered_string[beg_pos]=='\n') || (current_idx == considered_string.size()-1);


	while(!endLine_reached)
	{
		current_idx++;
		endLine_reached = (considered_string[current_idx]=='\n') || (current_idx == considered_string.size()-1);
	}
	int endLine_idx = current_idx;

    //we find the position of the next element
	if(endLine_idx == considered_string.size()-1)
	{
		end_pos = -1;
	}
	else
	{
		end_pos = endLine_idx+1;
	}

	line = considered_string.substr(beg_pos, endLine_idx-beg_pos+1);


    //we delete the unwanted chars...
    current_idx = line.size()-1; //there is at least one element

	bool character_toBe_Removed = 0;
	bool weCan_decrement_idx = 0;

	for(int char_idx = 0; char_idx <= removed_endingChar.size()-1; char_idx++)
	{
		character_toBe_Removed = character_toBe_Removed || (line[current_idx] == removed_endingChar[char_idx]);
	}
	if(current_idx>0) weCan_decrement_idx = 1;


	while(character_toBe_Removed && weCan_decrement_idx)
	{
		current_idx--;

		character_toBe_Removed = 0;
		for(int char_idx = 0; char_idx <= removed_endingChar.size()-1; char_idx++)
		{
			character_toBe_Removed = character_toBe_Removed || (line[current_idx] == removed_endingChar[char_idx]);
		}

		weCan_decrement_idx = 0;
		if(current_idx>0) weCan_decrement_idx = 1;
	}

	int nb_char_to_delete = line.size()-1 - current_idx;
	if(weCan_decrement_idx==0)  nb_char_to_delete++;

	line.erase(line.size() - nb_char_to_delete, nb_char_to_delete);

	return 1;


}

long getNumberWords(string content, string separators)
{
	string line;
	string word;
	int file_pos = 0;
	int line_pos = 0;

	long nb_words = 0;

	while(getLine(content, line, file_pos, file_pos))
	{
		line_pos = 0;
		while(getWord(line, word, line_pos, line_pos, separators))
		{
			nb_words++;
		}
	}

	return nb_words;
}


long getNumberLines(string content)
{
	string line;

	int file_pos = 0;
	long nb_lines = 0;

	while(getLine(content, line, file_pos, file_pos))
	{
		nb_lines++;
	}

	return nb_lines;
}


