
//#ifndef GSHADER_H
//#define GSHADER_H

//#include "stdlib.h"
//#include "string"
//#include "vector"
//#include "glew.h"
//#include "gl.h"
//#include "autres/myFileHandler.h"
//#include "autres/string_manipulation.h"
//#include "iostream"
//#include "iomanip"

//#include "gEnum.h"

//using namespace std;


//namespace gstd
//{

////    NULL_ENTRYPOINT.name = "-1";
////    NULL_ENTRYPOINT.type = NONE_POINT;
////    NULL_ENTRYPOINT.dataType = "-1";
////    NULL_ENTRYPOINT.location = -1;

////enum SHADER_TYPE {NONE_SHADER, VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER};

//class gShader
//{
//public :

//    gShader();
//    gShader(SHADER_TYPE type);
//    gShader(string shader_fileDir, SHADER_TYPE type, bool binaryOrFile);
//    ~gShader();

//    bool isSourced();
//    bool isCompiled();

//    void setSource(string shader_str, bool binaryOrFile); // in both memory
//    string getSource();
//    int compile();

//    void findEntryPoints();
//    entryPoint getEntryPoint(int ep_idx);
//    vector<entryPoint> getEntryPoints_v();
//    void coutEntryPoints_v();

//    GLuint getShader();
//    SHADER_TYPE getType();



//private :

//    string m_source;
//    bool m_isSourced;
//    bool m_isCompiled;

//    SHADER_TYPE m_type;
//    GLuint m_shader;

//    vector<entryPoint> m_entryPoints_v;

//};



//}

//#endif // GSHADER_H
