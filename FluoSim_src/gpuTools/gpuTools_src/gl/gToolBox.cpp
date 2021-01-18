
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




#include "gToolBox.h"


namespace gstd {

bool connectUniform(gProgram program, string entry_point_str, void const* data_ptr, int vector_size)
{
    entryPoint entry_point = program.getEntryPoint(entry_point_str);

    if(entry_point.location == -1 || entry_point.type != UNIFORM_POINT) return 0;

    //we get the dataType of entry_point
    gTypeDescriptor gtype_descriptor = getgTypeDescription(entry_point.dataType);

    if(entry_point.isArray == false)
    {
        switch(gtype_descriptor.component_atomicType)
        {        
			case GL_INT :
            {
                switch (gtype_descriptor.nb_components)
                {
                    case 1:
                    {
                        glUniform1i(entry_point.location,
                                    *((int*) data_ptr));
                    }
                    break;

                    case 2:
                    {
                        glUniform2i(entry_point.location,
                                    *((int*) data_ptr),
                                    *((int*) data_ptr+1));
                    }
                    break;

                    case 3:
                    {
                        glUniform3i(entry_point.location,
                                    *((int*) data_ptr),
                                    *((int*) data_ptr+1),
                                    *((int*) data_ptr+2));
                    }
                    break;

                    case 4:
                    {
                        glUniform4i(entry_point.location,
                                    *((int*)data_ptr),
                                    *((int*)data_ptr+1),
                                    *((int*)data_ptr+2),
                                    *((int*)data_ptr+3));
                    }
                    break;
                }
            }
            break;

			case GL_UNSIGNED_INT :
            {
				switch (gtype_descriptor.nb_components)
				{
					case 1:
					{
						glUniform1ui(entry_point.location,
									*((unsigned int*) data_ptr));
					}
					break;

					case 2:
					{
						glUniform2ui(entry_point.location,
									*((unsigned int*) data_ptr),
									*((unsigned int*) data_ptr+1));
					}
					break;

					case 3:
					{
						glUniform3ui(entry_point.location,
									*((unsigned int*) data_ptr),
									*((unsigned int*) data_ptr+1),
									*((unsigned int*) data_ptr+2));
					}
					break;

					case 4:
					{
						glUniform4ui(entry_point.location,
									*((unsigned int*)data_ptr),
									*((unsigned int*)data_ptr+1),
									*((unsigned int*)data_ptr+2),
									*((unsigned int*)data_ptr+3));
					}
					break;
			}
            }
            break;

			case GL_FLOAT :
            {
					switch (gtype_descriptor.nb_components)
					{
						case 1:
						{
							glUniform1f(entry_point.location,
										*((float*) data_ptr));
						}
						break;

						case 2:
						{
							glUniform2f(entry_point.location,
										*((float*) data_ptr),
										*((float*) data_ptr+1));
						}
						break;

						case 3:
						{
							glUniform3f(entry_point.location,
										*((float*) data_ptr),
										*((float*) data_ptr+1),
										*((float*) data_ptr+2));
						}
						break;

						case 4:
						{
							if(gtype_descriptor.type == VEC4)
							{
							glUniform4f(entry_point.location,
										*((float*)data_ptr),
										*((float*)data_ptr+1),
										*((float*)data_ptr+2),
										*((float*)data_ptr+3));
							}

							if(gtype_descriptor.type == MAT2)
							{

							glUniformMatrix2fv(entry_point.location, 1, false, (float*)data_ptr);

							}
						}
						break;

						case 9:
						{

							glUniformMatrix3fv(entry_point.location, 1, false, (float*)data_ptr);

						}
						break;

						case 16:
						{

							glUniformMatrix4fv(entry_point.location, 1, false, (float*)data_ptr);

						}
						break;




                    }//end_switch_components
			}//end_case_FLOAT
			break;


            default :
            {
				cout<<"In : connect(void* data_ptr, gProgram program, string entry_point_str) : dataType not recognised\n";
            }
            break;

		}//end_switch_TYPE
	}//end_if_NOT_ARRAY
	else //******is vector******
	{
		if(entry_point.arraySize != -1) vector_size = entry_point.arraySize;

		switch(gtype_descriptor.component_atomicType)
		{
			case GL_INT :
			{
				switch (gtype_descriptor.nb_components)
				{
					case 1:
					{
						glUniform1iv(entry_point.location, vector_size, (int*) data_ptr);

					}
					break;

					case 2:
					{
						glUniform2iv(entry_point.location, vector_size, (int*) data_ptr);
					}
					break;

					case 3:
					{
						glUniform3iv(entry_point.location, vector_size, (int*) data_ptr);
					}
					break;

					case 4:
					{
						glUniform4iv(entry_point.location, vector_size, (int*) data_ptr);
					}
					break;
				}
			}
			break;

			case GL_UNSIGNED_INT :
			{
				switch (gtype_descriptor.nb_components)
				{
					case 1:
					{
						glUniform1uiv(entry_point.location, vector_size, (unsigned int*) data_ptr);
					}
					break;

					case 2:
					{
						glUniform2uiv(entry_point.location, vector_size, (unsigned int*) data_ptr);
					}
					break;

					case 3:
					{
						glUniform3uiv(entry_point.location, vector_size, (unsigned int*) data_ptr);
					}
					break;

					case 4:
					{
						glUniform4uiv(entry_point.location, vector_size, (unsigned int*) data_ptr);
					}
					break;
			}
			}
			break;

			case GL_FLOAT :
			{
					switch (gtype_descriptor.nb_components)
					{
						case 1:
						{
							glUniform1fv(entry_point.location, vector_size, (float*) data_ptr);

						}
						break;

						case 2:
						{
							glUniform2fv(entry_point.location, vector_size, (float*) data_ptr);
						}
						break;

						case 3:
						{
							glUniform3fv(entry_point.location, vector_size, (float*) data_ptr);
						}
						break;

						case 4:
						{
							if(gtype_descriptor.type == VEC4)
							{
								glUniform4fv(entry_point.location, vector_size, (float*) data_ptr);
							}

							if(gtype_descriptor.type == MAT2)
							{

								glUniformMatrix2fv(entry_point.location, vector_size, false, (float*)data_ptr);

							}
						}
						break;

						case 9:
						{

							glUniformMatrix3fv(entry_point.location, vector_size, false, (float*)data_ptr);

						}
						break;

						case 16:
						{

							glUniformMatrix4fv(entry_point.location, vector_size, false, (float*)data_ptr);

						}
						break;

					}//end_switch_components
			}//end_case_FLOAT
			break;


			default :
			{
				cout<<"In : connect(void* data_ptr, gProgram program, string entry_point_str) : dataType not recognised\n";
			}
			break;

		}//end_switch_TYPE
	}//end_else = IS_ARRAY

    return 1;
}



bool connectTexture(gProgram program, string entry_point_str, gTexture texture, uint textureUnit_idx)
{
	entryPoint entry_point = program.getEntryPoint(entry_point_str);

	if((entry_point.dataType != "sampler2D" && entry_point.dataType != "sampler1D") ||
	   entry_point.type != UNIFORM_POINT ||
	   entry_point.location == -1) return 0;

	uint a = textureUnit_idx;
	texture.bindTexture(textureUnit_idx);
	connectUniform(program, entry_point_str, &a);

	return 1;
}


}
