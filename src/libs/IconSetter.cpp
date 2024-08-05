#include "IconSetter.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "GLFW/glfw3.h"
#include <Windows.h>

void setWindowIcon(GLFWwindow* window) {
    int image_width = 0;
    int image_height = 0;

    HRSRC info = FindResourceA(nullptr, "iconpng", "PNGIMAGE");
    if(!info) return;

    HGLOBAL res = LoadResource(nullptr, info);
    auto bres = LockResource(res);
    if(!res) return;
    DWORD size = SizeofResource(nullptr, info);
    GLFWimage images[1];
    images[0].pixels = stbi_load_from_memory((unsigned char*)bres, static_cast<int>(size), &image_width, &image_height, nullptr, 4);
    images[0].width = image_width;
    images[0].height = image_height;
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);
    UnlockResource(res);
    FreeResource(res);
}
