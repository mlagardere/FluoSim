
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



#include "myFont.h"

using namespace glm;
using namespace std;


charDescription null_desc = charDescription{'\0', -1, -1, -1, -1, -1, -1, -1};
charDescription exclamationMark_desc = charDescription{'!', 193, 73, 21, 135, 26, 134, 18};
charDescription diese_desc = charDescription{'#', 282, 81, 93, 128, 3, 127, 3};
charDescription dolar_desc = charDescription{'$', 432, 59, 82, 172, 9, 148, 10};
charDescription perCent_desc = charDescription{'%', 577, 77, 128, 134, 10, 132, 7};
charDescription and_desc = charDescription{'&', 768, 75, 119, 134, 7, 130, 7};
charDescription apostroph_desc = charDescription{'"', 953, 70, 33, 50, 13, 137, 4};
charDescription leftParenthesis_desc = charDescription{'(', 1047, 68, 36, 173, 12, 139, 11};
charDescription rightParenthesis_desc = charDescription{')', 1153, 68, 36, 173, 12, 139, 12};
charDescription starProduct_desc = charDescription{'*', 1262,68,69,74,15,138,14};
charDescription plus_desc = charDescription{'+',1398,106,87,92,6,102,6};
charDescription coma_desc = charDescription{',',1540,185,33,49,4,22,13};
charDescription minus_desc = charDescription{'-',1637,151,49,12,6,56,6};
charDescription dot_desc = charDescription{'.',1752,185,22,22,14,21,14};
charDescription slash_desc = charDescription{'/',1833,63,75,170,0,143,1};
charDescription zero_desc = charDescription{'0',130,320,88,130,7,128,6};
charDescription one_desc = charDescription{'1',286,320,74,126,16,126,10};
charDescription two_desc = charDescription{'2',427,319,81,128,11,128,9};
charDescription three_desc = charDescription{'3',573,319,80,130,10,128,10};
charDescription four_desc = charDescription{'4',713,321,91,127,4,126,4};
charDescription five_desc = charDescription{'5',865,321,80,128,9,127,11};
charDescription six_desc = charDescription{'6',1012,319,84,130,9,128,8};
charDescription seven_desc = charDescription{'7',1158,321,84,127,10,126,4};
charDescription eight_desc = charDescription{'8',1304,319,84,130,9,128,7};
charDescription nine_desc = charDescription{'9',1450,319,83,130,7,128,10};
charDescription colon_desc = charDescription{':',1607,357,21,91,18,90,14};
charDescription semiColon_desc = charDescription{';',1693,356,32,118,6,90,15};
charDescription leftChevron_desc = charDescription{'<',1791,343,84,94,5,103,11};
charDescription equal_desc = charDescription{'=',132,605,82,51,9,81,9};
charDescription rightChevron_desc = charDescription{'>',277,583,82,94,9,103,5};
charDescription questionMark_desc = charDescription{'?',425,552,71,135,12,134,8};
charDescription at_desc =charDescription{'@',566,558,145,153,15,129,18};
charDescription A_desc = charDescription{'A',778,559,110,129,3,128,4};
charDescription B_desc = charDescription{'B',951,561,83,126,15,126,9};
charDescription C_desc = charDescription{'C',1098,559,93,130,9,128,6};
charDescription D_desc = charDescription{'D',1257,561,98,125,15,125,10};
charDescription E_desc = charDescription{'E',1426,561,70,126,16,126,10};
charDescription F_desc = charDescription{'F',1568,561,67,126,16,125,9};
charDescription G_desc = charDescription{'G',1698,559,103,130,8,128,14};
charDescription H_desc = charDescription{'H',1877,560,92,128,16,127,16};
charDescription I_desc = charDescription{'I',275,1040,17,127,16,126,16};
charDescription J_desc = charDescription{'J',123,800,48,129,0,128,17};
charDescription K_desc = charDescription{'K',248,800,84,128,16,127,3};
charDescription L_desc = charDescription{'L',397,800,66,126,16,125,1};
charDescription M_desc = charDescription{'M',526,801,138,126,15,125,17};
charDescription N_desc = charDescription{'N',743,800,97,127,16,126,15};
charDescription O_desc = charDescription{'O',910,799,113,131,9,128,9};
charDescription P_desc = charDescription{'P',1095,801,79,126,16,125,8};
charDescription Q_desc = charDescription{'Q',1236,799,128,146,8,127,-3};
charDescription R_desc = charDescription{'R',1423,801,85,126,16,125,7};
charDescription S_desc = charDescription{'S',1567,799,78,130,6,128,7};
charDescription T_desc = charDescription{'T',1699,801,95,126,1,125,1};
charDescription U_desc = charDescription{'U',1856,800,95,129,15,127,17};
charDescription V_desc = charDescription{'V',357,1040,106,127,2,126,3};
charDescription W_desc = charDescription{'W',519,1040,165,127,6,126,6};
charDescription X_desc = charDescription{'X',741,1040,94,127,5,126,5};
charDescription Y_desc = charDescription{'Y',888,1040,91,127,3,126,3};
charDescription Z_desc = charDescription{'Z',1033,1041,84,125,4,125,3};
charDescription bra_desc = charDescription{'[',1185,1029,35,168,18,137,8};
//charDescription antiSlash_desc = charDescription{'\',127,1024,76,167,1,142,1};
charDescription ket_desc = charDescription{']',1404,1029,35,168,8,137,17};
//charDescription ket_desc = charDescription{']',1404,1029,35,168,8,31,17};
charDescription pow_desc = charDescription{'^',1512,1040,79,72,9,127,12};
charDescription underBar_desc = charDescription{'_',1646,1190,100,12,-1,-23,-1};
charDescription antiApostrophe_desc = charDescription{'`',1804,1030,33,30,12,136,13};
charDescription a_desc = charDescription{'a',132,1312,72,97,9,95,14};
charDescription b_desc = charDescription{'b',278,1271,82,138,13,136,10};
charDescription c_desc = charDescription{'c',423,1312,70,97,8,95,6};
charDescription d_desc = charDescription{'d',553,1271,82,138,9,136,15};
charDescription e_desc = charDescription{'e',703,1312,81,97,9,95,10};
charDescription f_desc = charDescription{'f',842,1270,61,138,3,137,-3};
charDescription g_desc = charDescription{'g',950,1312,83,131,4,95,5};
charDescription h_desc = charDescription{'h',1099,1271,77,136,14,135,14};
charDescription i_desc = charDescription{'i',1247,1277,21,130,12,129,13};
charDescription j_desc = charDescription{'j',1321,1277,40,166,-5,130,13};
charDescription k_desc = charDescription{'k',1433,1270,73,137,14,136,4};
charDescription l_desc = charDescription{'l',1569,1271,18,137,14,136,14};
charDescription m_desc = charDescription{'m',1660,1312,132,96,13,95,14};
charDescription n_desc = charDescription{'n',1865,1312,77,95,13,94,14};
charDescription o_desc = charDescription{'o',265,1550,91,99,8,97,8};
charDescription p_desc = charDescription{'p',423,1552,82,131,14,95,10};
charDescription q_desc = charDescription{'q',569,1552,82,131,10,95,14};
charDescription r_desc = charDescription{'r',724,1552,53,96,14,95,2};
charDescription s_desc = charDescription{'s',831,1552,63,97,7,95,8};
charDescription t_desc = charDescription{'t',951,1530,57,119,3,117,6};
charDescription u_desc = charDescription{'u',1074,1553,77,96,14,94,15};
charDescription v_desc = charDescription{'v',1214,1553,84,95,4,94,3};
charDescription w_desc = charDescription{'w',1351,1533,133,114,4,114,5};
charDescription x_desc = charDescription{'x',1539,1553,78,95,4,94,4};
charDescription y_desc = charDescription{'y',1669,1553,84,129,3,93,3};
charDescription z_desc = charDescription{'z',1810,1554,63,93,8,93,8};
charDescription curvedBra_desc = charDescription{'{',900,1749,49,169,8,138,6};
charDescription vBar_desc = charDescription{'|',1038,1747,16,176,37,140,37};
charDescription curvedKet_desc = charDescription{'}',1148,1749,49,169,11,138,3};
charDescription wave_desc = charDescription{'~',1250,1776,89,36,5,111,5};


vector<charDescription> test_desc{null_desc,  exclamationMark_desc,  diese_desc,  dolar_desc,  perCent_desc,
								  and_desc,	  apostroph_desc,  leftParenthesis_desc,  rightParenthesis_desc,
								  starProduct_desc,	  plus_desc,  coma_desc,  minus_desc,  dot_desc,
								  slash_desc,
								  one_desc,  two_desc,  three_desc,  four_desc,  five_desc,	  six_desc,	  seven_desc,
								  eight_desc,  nine_desc, zero_desc,
								  colon_desc,  semiColon_desc,  leftChevron_desc,  equal_desc,  rightChevron_desc,
								  questionMark_desc,  at_desc,
								  A_desc, B_desc,  C_desc,  D_desc,  E_desc,  F_desc,  G_desc,  H_desc,  I_desc,  J_desc,
								  K_desc, L_desc,  M_desc,	N_desc,  O_desc,  P_desc,  Q_desc,  R_desc,  S_desc,  T_desc,
								  U_desc, V_desc,  W_desc,  X_desc,  Y_desc,  Z_desc,
								  bra_desc,
//								  antiSlash_desc,
								  ket_desc,  pow_desc,	  underBar_desc,   antiApostrophe_desc,
								  a_desc,  b_desc,	  c_desc,  d_desc,	  e_desc,  f_desc,  g_desc,	  h_desc,  i_desc,
								  j_desc,  k_desc,    l_desc,  m_desc,	  n_desc,  o_desc,  p_desc,   q_desc,  r_desc,
								  s_desc,  t_desc,	  u_desc,  v_desc,    w_desc,  x_desc,  y_desc,   z_desc,
								  curvedBra_desc,	vBar_desc,  curvedKet_desc,	  wave_desc};

myFont::myFont()
{
	m_fontTexture = new gstd::gTexture(GL_TEXTURE_2D);
	m_textureLoaded = 0;
	m_descriptionLoaded = 0;
}

myFont::myFont(string texture_dir, vector<charDescription> charDescriptions_v)
{
	m_fontTexture = new gstd::gTexture(GL_TEXTURE_2D);

	QImage image;
	if(image.load(QString(texture_dir.data())) == false)
	{
		m_textureLoaded = false;
		return; //->
	}

	int size_x = image.size().width();
	int size_y = image.size().height();

	QImage image_argb32 = image.convertToFormat(QImage::Format_ARGB32);

	uint8* data_8;
	data_8 = new uint8[4*size_x*size_y];
	data_8 = (uint8*) convertBGRA32toRGBA32(image_argb32.bits(), 4*size_x*size_y);

	m_fontTexture->loadTexture(size_x, size_y, GL_RGBA,
							   GL_RGBA, GL_UNSIGNED_BYTE, data_8);

	delete [] data_8;

	m_textureLoaded = true;

//	m_textureLoaded = m_fontTexture->loadTexture(texture_dir);
	m_fontTexture->genMipMap();
	m_fontTexture->enableMipMap(1);

	m_charDescriptions_v = charDescriptions_v;
	m_descriptionLoaded = 1;


}

gstd::gTexture myFont::getTexture()
{
	return *m_fontTexture;
}

charDescription myFont::getCharacterDescription(char a)
{
	bool found = 0;
	auto description = m_charDescriptions_v.begin();
	while(description != m_charDescriptions_v.end() && !found)
	{
		if((*description).character == a)
		{
			found = 1;
		}
		else
		{
			description++;
		}
	}
	if(found == 0) return null_desc;
	else return *description;
}


vec2 myFont::getTextureSize()
{
	return m_fontTexture->getSize();
}

bool myFont::isLoaded()
{
	return m_textureLoaded && m_descriptionLoaded;
}
