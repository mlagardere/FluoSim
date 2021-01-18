
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

#ifndef MYFONT_H
#define MYFONT_H

#include "vector"
#include "glm.hpp"
#include "gl/gTexture.h"

#include "toolbox_library_global.h"
#include "otherFunctions/otherFunctions.h"


struct TOOLBOXSHARED_EXPORT charDescription
{
	//character name
	char character;

	//texture aspects
	float x;
	float y;

	float width;
	float height;

	//character aspects
	float xOffset;
	float yOffset;
	float xAdvance;
};

extern charDescription null_desc;
extern charDescription exclamationMark_desc;
extern charDescription diese_desc;
extern charDescription dolar_desc;
extern charDescription perCent_desc;
extern charDescription and_desc;
extern charDescription apostroph_desc;
extern charDescription leftParenthesis_desc;
extern charDescription rightParenthesis_desc;
extern charDescription starProduct_desc;
extern charDescription plus_desc;
extern charDescription coma_desc;
extern charDescription minus_desc;
extern charDescription dot_desc;
extern charDescription slash_desc;
extern charDescription zero_desc;
extern charDescription one_desc;
extern charDescription two_desc;
extern charDescription three_desc;
extern charDescription four_desc;
extern charDescription five_desc;
extern charDescription six_desc;
extern charDescription seven_desc;
extern charDescription eight_desc;
extern charDescription nine_desc;
extern charDescription zero_desc;
extern charDescription colon_desc;
extern charDescription semiColon_desc;
extern charDescription leftChevron_desc;
extern charDescription equal_desc;
extern charDescription rightChevron_desc;
extern charDescription questionMark_desc;
extern charDescription at_desc;
extern charDescription A_desc;
extern charDescription B_desc;
extern charDescription C_desc;
extern charDescription D_desc;
extern charDescription E_desc;
extern charDescription F_desc;
extern charDescription G_desc;
extern charDescription H_desc;
extern charDescription I_desc;
extern charDescription J_desc;
extern charDescription K_desc;
extern charDescription L_desc;
extern charDescription M_desc;
extern charDescription N_desc;
extern charDescription O_desc;
extern charDescription P_desc;
extern charDescription Q_desc;
extern charDescription R_desc;
extern charDescription S_desc;
extern charDescription T_desc;
extern charDescription U_desc;
extern charDescription V_desc;
extern charDescription W_desc;
extern charDescription X_desc;
extern charDescription Y_desc;
extern charDescription Z_desc;
extern charDescription bra_desc;
//extern charDescription antiSlash_desc;
extern charDescription ket_desc;
extern charDescription pow_desc;
extern charDescription underBar_desc;
extern charDescription antiApostrophe_desc;
extern charDescription a_desc;
extern charDescription b_desc;
extern charDescription c_desc;
extern charDescription d_desc;
extern charDescription e_desc;
extern charDescription f_desc;
extern charDescription g_desc;
extern charDescription h_desc;
extern charDescription i_desc;
extern charDescription j_desc;
extern charDescription k_desc;
extern charDescription l_desc;
extern charDescription m_desc;
extern charDescription n_desc;
extern charDescription o_desc;
extern charDescription p_desc;
extern charDescription q_desc;
extern charDescription r_desc;
extern charDescription s_desc;
extern charDescription t_desc;
extern charDescription u_desc;
extern charDescription v_desc;
extern charDescription w_desc;
extern charDescription x_desc;
extern charDescription y_desc;
extern charDescription z_desc;
extern charDescription curvedBra_desc;
extern charDescription vBar_desc;
extern charDescription curvedKet_desc;
extern charDescription wave_desc;


TOOLBOXSHARED_EXPORT extern std::vector<charDescription> test_desc;


class TOOLBOXSHARED_EXPORT myFont
{
public :

	myFont();
	myFont(std::string texture_dir, std::vector<charDescription> charDescriptions_v);
	charDescription getCharacterDescription(char a);
	glm::vec2 getTextureSize();
	gstd::gTexture getTexture();

	bool isLoaded();

private:

	std::vector<charDescription> m_charDescriptions_v;
	gstd::gTexture* m_fontTexture;

//	bool m_isLoaded;
		bool m_textureLoaded;
		bool m_descriptionLoaded;

};




#endif // MYFONT_H
