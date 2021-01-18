
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

#include "gpu_string_manipulation.h"


using namespace std;



namespace gstd
{

//see C-str for tokens...
bool getWord(string &considered_string,
             string &word, int beg_pos,
             int &end_pos,
             string separating_char, string unaccepted_startingChar)
{
    word.clear();

    if(beg_pos >= considered_string.size()-1)
    {
        end_pos = -1;
        return 0;
    }

    else
    {
//        string unaccepted_startingChar = (" \n\t/");
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
                for(int char_idx = 0; char_idx <= unaccepted_startingChar.size()-1; char_idx++)
                {
                    if(considered_string[substring_idx] == unaccepted_startingChar[char_idx])
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



bool getSubWord(string& considered_str,
                string& subWord,
                string left_separators,
                string right_separators)
{
    int  left_separtor_idx = -1;
    int  right_separtor_idx = -1;

    left_separtor_idx = considered_str.find_first_of(left_separators, 0);
    right_separtor_idx = considered_str.find_last_of(right_separators, considered_str.size()-1);

    subWord.clear();

    if(left_separtor_idx == string::npos ||
       right_separtor_idx == string::npos ||
       left_separtor_idx >= right_separtor_idx)
    {
        return 0;
    }

    subWord.insert(subWord.begin(),
                   considered_str.begin()+left_separtor_idx+1,
                   considered_str.begin()+right_separtor_idx);
    return 1;

}

bool getLine(string &considered_string, string &line, int beg_pos, int &end_pos)
{
    line.clear();

    if(beg_pos > considered_string.size()-1) //on autorise des string de 1 char
    {
        end_pos = -1;
        return 0;
    }


    string removed_endingChar = (" \n\t");

    //we look for the end of line (could be \n of file_end)
    int current_idx = beg_pos;
    bool endLine_reached = (considered_string[beg_pos]=='\n') || (current_idx == considered_string.size()-1);


    while(!endLine_reached)
    {
        current_idx++;
        endLine_reached = (considered_string[current_idx]=='\n') || (current_idx == considered_string.size()-1);
    }
    int endLine_idx = current_idx;

    //we return the position of the next element
    if(endLine_idx == considered_string.size()-1)
    {
        end_pos = -1;
    }
    else
    {
        end_pos = endLine_idx+1;
    }

    line = considered_string.substr(beg_pos, endLine_idx-beg_pos+1);


    //we remove the unwanted characters
    current_idx = line.size()-1; // il y a moins un élément

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


}
