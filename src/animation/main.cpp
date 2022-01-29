#include <cstdio>
#include <cmath>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

static int WIN_WIDTH = 500;                      // ウィンドウの幅
static int WIN_HEIGHT = 500;                     // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル

static const double fps = 30.0;  // FPS
static float theta = 0.0f;

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
static const unsigned int indices[12][3] = {
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

    // 深度テストの有効化
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

// キューブの描画
// Draw a cube
void drawCube() {
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        glColor3fv(colors[face]);
        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 0][i]]);
        }

        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 1][i]]);
        }
    }
    glEnd();
}

// OpenGLの描画関数
// OpenGL drawing function
void paintGL() {
    // 背景色と深度値のクリア
    // Clear background color and depth values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 座標の変換
    // Coordinate transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,   // 視点の位置
              0.0f, 0.0f, 0.0f,   // 見ている先
              0.0f, 1.0f, 0.0f);  // 視界の上方向

    // 1つ目のキューブ
    // First cube
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(theta, 0.0f, 1.0f, 0.0f);
    glRotatef(theta * 0.5f, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);

    drawCube();
    glPopMatrix();

    // 2つ目のキューブ
    // Second cube
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    glRotated(2.0f * theta, 0.0f, 1.0f, 0.0f);
    glRotatef(theta, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);

    drawCube();
    glPopMatrix();
}

// ウィンドウサイズ変更のコールバック関数
// Callback function for window resizing
void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    // Update user-managed window size
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    // GLFW管理のウィンドウサイズを変更
    // Update GLFW-managed window size
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

    // 実際のウィンドウサイズ (ピクセル数) を取得
    // Get actual window size by pixels
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

    // ビューポート変換の更新
    // Update viewport transform
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

// アニメーションのためのアップデート
// Update for animating object
void animate() {
    theta += 1.0f;  // 1度だけ回転 / Rotate by 1 degree of angle
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    // OpenGL initialization
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Windowの作成
    // Create a window
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にwindowを指定
    // Specify window as an OpenGL context
    glfwMakeContextCurrent(window);

    // ウィンドウのリサイズを扱う関数の登録
    // Register a callback function for window resizing
    glfwSetWindowSizeCallback(window, resizeGL);

    // ユーザ指定の初期化
    // User-specified initialization
    initializeGL();

    // メインループ
    // Main loop
    double prevTime = glfwGetTime();
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // 現在の時刻を取得
        // Acquire a current time
        double currentTime = glfwGetTime();

        // 経過時間が 1 / FPS 以上なら描画する
        // Perform drawing if elapsed time is more than one over FPS
        if (currentTime - prevTime >= 1.0 / fps) {
            // タイトルにFPSを表示
            // Show FPS in title
            double realFps = 1.0 / (currentTime - prevTime);
            char winTitle[256];
            sprintf(winTitle, "%s (%.3f)", WIN_TITLE, realFps);
            glfwSetWindowTitle(window, winTitle);

            // 描画
            // Draw
            paintGL();

            // アニメーション
            // Animation
            animate();

            // 描画用バッファの切り替え
            // Swap drawing target buffers
            glfwSwapBuffers(window);
            glfwPollEvents();

            // 前回描画時間の更新
            // Update latest time of drawing
            prevTime = currentTime;
        }
    }

    // 後処理
    // Postprocess
    glfwDestroyWindow(window);
    glfwTerminate();
}
