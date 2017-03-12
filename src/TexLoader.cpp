#include "TexLoader.h"
#include <windows.h> //TO DO: gl.h needs this, annnoyingly
#include <gl/gl.h>



/* Takes simple 8-bit luminance data, and generates a texture with alpha data. */
int CreateAlphaTextureFromLuminanceData (unsigned char* tex_data, int width, int height)
{

	unsigned int TextureNo;
	glGenTextures(1,(GLuint*) &TextureNo);
	glBindTexture(GL_TEXTURE_2D, TextureNo);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, tex_data);

	return TextureNo;

}