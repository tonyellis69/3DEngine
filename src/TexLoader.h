#ifndef TEXTURE_LOADER_LIBRARY
#define TEXTURE_LOADER_LIBRARY

//#include "soil.h"

/// Simple wrapper for the SOIL library that includes an extra function. 

int CreateAlphaTextureFromLuminanceData (unsigned char* tex_data, int width, int height);


#endif