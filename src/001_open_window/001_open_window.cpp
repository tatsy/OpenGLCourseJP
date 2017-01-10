/******************************************************************************
実験
(1) ウィンドウの幅や高さを変更したときの変化を調べる
(2) ウィンドウのタイトルを変更したときの変化を調べる
(3) glClearColorに渡されている色を別の色に変更して、変化を調べる
(4) glClearをコメントアウトしたときに背景色がどうなるかを調べる
******************************************************************************/

#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

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

    // 背景色の設定
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 背景色の描画
        glClear(GL_COLOR_BUFFER_BIT);

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
