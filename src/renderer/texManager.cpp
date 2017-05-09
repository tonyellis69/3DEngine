
#include <iostream>
using namespace std;

#include "texManager.h"
#include "..\SOIL.h"

GLuint CTextureManagerOGL::getTexture(std::string filename) {

	if (textures.empty() || textures.find(filename) == textures.end()) { //not found
		CTextureOGL newTexture;
		unsigned char* data = SOIL_load_image(filename.c_str(), &newTexture.width, &newTexture.height, &newTexture.channels, SOIL_LOAD_AUTO);

		int flags = SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y;
		if (data) {
			newTexture.handle = SOIL_create_OGL_texture(data, newTexture.width, newTexture.height, newTexture.channels, SOIL_CREATE_NEW_ID, flags); //then turn it into an OpenGL texture
			SOIL_free_image_data(data);
		}
		else {
			cerr << "\nFailed to load texture file " << filename;
			return NULL;
		}

		textures[filename] = newTexture;
	}

	return textures[filename].handle;
}
