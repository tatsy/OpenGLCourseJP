#include <iostream>
#include <string>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要 / Required to use GLU
#include <GLFW/glfw3.h>

static const int WIN_WIDTH = 500;                      // ウィンドウの幅 / Window width
static const int WIN_HEIGHT = 500;                     // ウィンドウの高さ / Window height
static const std::string WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title

// 立方体の頂点位置
// Vertex positions of a cube
// clang-format off
static const float positions[8][3] = {
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f }
};
// clang-format on

// 立方体の面の色
// Face colors of a cube
// clang-format off
static const float colors[6][3] = {
    { 1.0f, 0.0f, 0.0f },  // 赤
    { 0.0f, 1.0f, 0.0f },  // 緑
    { 0.0f, 0.0f, 1.0f },  // 青
    { 1.0f, 1.0f, 0.0f },  // イエロー
    { 0.0f, 1.0f, 1.0f },  // シアン
    { 1.0f, 0.0f, 1.0f },  // マゼンタ
};
// clang-format on

// 立方体の面となる三角形の定義
// Vertex indices to form triangles of a cube
// clang-format off
static const unsigned int faces[12][3] = {
    { 7, 4, 1 }, { 7, 1, 6 },
    { 2, 4, 7 }, { 2, 7, 5 },
    { 5, 7, 6 }, { 5, 6, 3 },
    { 4, 2, 0 }, { 4, 0, 1 },
    { 3, 6, 1 }, { 3, 1, 0 },
    { 2, 5, 3 }, { 2, 3, 0 }
};
// clang-format on

// OpenGLの初期化関数
// OpenGL initialization
void initializeGL() {
    // 背景色の設定 (黒)
    // Background color (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// OpenGLの描画関数
// OpenGL drawing function
void paintGL() {
    // 背景色の描画
    // Fill background color
    glClear(GL_COLOR_BUFFER_BIT);

    // ビューポート変換の指定
    // Viewport transform
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

    // 投影変換行列
    // Specify projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 平行投影 / Orthographic projection
    // glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    // 透視投影 / Perspective projection with view frustum
    // glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
    // 透視投影 / Perspective projection
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 10.0f);

    // モデルビュー変換行列
    // Specify model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,   // 視点の位置 / Eye position
              0.0f, 0.0f, 0.0f,   // 見ている先 / Looking position
              0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

    // 立方体の描画
    // Draw a cube
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        // 面の色
        // Face color
        glColor3fv(colors[face]);

        // 1つの面(四角形)は2つの三角形から成る
        // A square face of a cube consists of two triangles
        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[faces[face * 2 + 0][i]]);
        }

        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[faces[face * 2 + 1][i]]);
        }
    }
    glEnd();
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    // OpenGL initialization
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "Initialization failed!" << std::endl;
        std::exit(1);
    }

    // Windowの作成
    // Create a window
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!" << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    // OpenGLの描画対象にwindowを指定
    // Specify window as an OpenGL context
    glfwMakeContextCurrent(window);

    // ユーザ指定の初期化
    // User-specified initialization
    initializeGL();

    // メインループ
    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // 描画
        // Draw
        paintGL();

        // 描画用バッファの切り替え
        // Swap drawing target buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 後処理
    // Postprocess
    glfwDestroyWindow(window);
    glfwTerminate();
}
