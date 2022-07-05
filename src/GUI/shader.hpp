#ifndef __GUI_SHADER_HPP__
#define __GUI_SHADER_HPP__

#include <GL/glew.h>

#include <vector>
#include <fstream>
#include <sstream>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
