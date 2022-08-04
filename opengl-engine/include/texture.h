#ifndef TEXTURE_H
#define TEXTURE_H

#include "pch.h"

class Texture {
  private:
    int width, height, nrChannels;
  public:
    unsigned int id;
    Texture(unsigned int texId) {
        id = texId;
    }
    Texture (const char* texPath, int channel_type = GL_RGB, bool flip_vertical = true) {
      stbi_set_flip_vertically_on_load(flip_vertical);
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);

      if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, channel_type, width, height, 0, channel_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      else {
        std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
      }
      stbi_image_free(data);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // sets the texture wrap behaviour
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // sets how the filtering will work
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      id = texture;
    }
    void bind2D() {
      glBindTexture(GL_TEXTURE_2D, id);
    }
    void activate(int texIndex) {
      glActiveTexture(GL_TEXTURE0 + texIndex);
      bind2D();
    }
};
#endif