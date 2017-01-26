#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

// ユーザ定義のOpenGLの初期化
void initializeOpenGL() {
    // 背景色の設定
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

// ユーザ定義のOpenGL描画
void drawOpenGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);

    // 初期化
    initializeOpenGL();

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        drawOpenGL();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
