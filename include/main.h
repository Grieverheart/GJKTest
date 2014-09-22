#ifndef MAIN_H
#define MAIN_H


#include <GL/glew.h>
#include <cstdio>
#include <string>


inline void glError(const char *file, int line) {
	GLenum err (glGetError());

	while(err!=GL_NO_ERROR) {
		std::string error;

		switch(err) {
			case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
		}

        fprintf(stderr, "GL_%s - %s:%d\n", error.c_str(), file, line);
		err=glGetError();
	}
}


#endif