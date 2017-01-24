#include <cstdio>
#include <GLFW/glfw3.h>

int main(int argc, char **argv) {
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }
}
