#include <cstdio>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 700;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 700;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

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

static const float colors[6][3] = {
    { 1.0f, 0.0f, 0.0f },  // 赤
    { 0.0f, 1.0f, 0.0f },  // 緑
    { 0.0f, 0.0f, 1.0f },  // 青
    { 1.0f, 1.0f, 0.0f },  // イエロー
    { 0.0f, 1.0f, 1.0f },  // シアン
    { 1.0f, 0.0f, 1.0f },  // マゼンタ
};

static const unsigned int indices[12][3] = {
    { 7, 4, 1 }, { 7, 1, 6 },
    { 2, 4, 7 }, { 2, 7, 5 },
    { 5, 7, 6 }, { 5, 6, 3 },
    { 4, 2, 0 }, { 4, 0, 1 },
    { 3, 6, 1 }, { 3, 1, 0 },
    { 2, 5, 3 }, { 2, 3, 0 }
};

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);
}

// OpenGLの描画関数
void paintGL() {
    // 背景色と深度値のクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ビューポート変換の指定 (MacのRetinaディスプレイだと変になる)
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    
    // 座標の変換
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 平行投影
    // glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    // 透視投影
    // glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
    // 透視投影
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 10.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
              0.0f, 0.0f, 0.0f,     // 見ている先
              0.0f, 1.0f, 0.0f);    // 視界の上方向

    // 立方体の描画
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        //glColor3fv(colors[face]);
        for (int i = 0; i < 3; i++) {
            glColor3fv(colors[face]);
            glVertex3fv(positions[indices[face * 2 + 0][i]]);
        }

        for (int i = 0; i < 3; i++) {
            glColor3fv(colors[face]);
            glVertex3fv(positions[indices[face * 2 + 1][i]]);
        }
    }
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

    // OpenGLを初期化
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
