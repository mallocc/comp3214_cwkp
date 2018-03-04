#pragma once

#include "opengl.h"

struct image_data
{
	unsigned char * data;
	int w, h, n;
};

GLuint load_texture_from_image(const char *fname);


image_data get_data(const char * filename);