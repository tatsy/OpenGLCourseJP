#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

// ユーザ定義のOpenGLの初期化
void initializeGL() {
    // 背景色の設定
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// ユーザ定義のOpenGL描画
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT);

    // 見る範囲の指定
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);

    // 三角形の描画
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);    // 赤
    glVertex2f(-1.0f, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);    // 緑
    glVertex2f(-1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);    // 青
    glVertex2f(1.0f, -1.0f);
    glEnd();
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
    initializeGL();

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
