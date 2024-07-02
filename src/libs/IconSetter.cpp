#include "IconSetter.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "GLFW/glfw3.h"
#include "romfs/romfs.hpp"

void setWindowIcon(GLFWwindow* window) {
    int image_width = 0;
    int image_height = 0;
    romfs::Resource image = romfs::get("icon.png");
    GLFWimage images[1];
    images[0].pixels = stbi_load_from_memory((unsigned char*) image.data(), image.size(),
                                                      &image_width, &image_height, nullptr, 4);
    images[0].height = image_height;
    images[0].width = image_width;
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);
}

