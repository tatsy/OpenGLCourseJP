#include <cmath>
#include <iostream>
#include <string>
#include <format>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要 / Required to use GLU
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION  // 画像のロードに必要 / Required to load images
#include "stb_image.h"

// 画像のパスなどが書かれた設定ファイル
// Config file storing image locations etc.
#include "common.h"

static int WIN_WIDTH = 500;                            // ウィンドウの幅 / Window width
static int WIN_HEIGHT = 500;                           // ウィンドウの高さ / Window height
static const std::string WIN_TITLE = "OpenGL Course";  // ウィンドウのタイトル / Window title

static const double PI = 4.0 * std::atan(1.0);  // 円周率の定義 / Circular constant
static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "checker.png";
static GLuint textureId = 0u;

// clang-format off
static const float positions[4][3] = {
    { -1.0f,  0.0f, -1.0f },
    {  1.0f,  0.0f, -1.0f },
    { -1.0f,  0.0f,  1.0f },
    {  1.0f,  0.0f,  1.0f },
};

static const float texcoords[4][2] = {
    {  0.0f,  0.0f },
    {  1.0f,  0.0f },
    {  0.0f,  1.0f },
    {  1.0f,  1.0f }
};

static const unsigned int faces[2][3] = {
    { 0, 1, 3 }, { 0, 3, 2 }
};
// clang-format on

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    // Background color (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 深度テストの有効化
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // テクスチャの有効化
    // Enable texture mapping
    glEnable(GL_TEXTURE_2D);

    // テクスチャの設定
    // Setup texture
    int texWidth, texHeight, channels;
    uint8_t *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        std::cerr << std::format("Failed to load image file: {}", TEX_FILE) << std::endl;
        std::exit(1);
    }

    // テクスチャの生成と有効化
    // Generate and bind texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // 単純なテクスチャの転送
    // Simple texture transfer
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
    //              0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    // MIP mapを用いたテクスチャの転送
    // Texture transfer with MIP mapping
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    // テクスチャの画素値参照方法の設定 (MIP mapなし)
    // Texture filtering operations (w/o MIP map)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャの画素値参照方法の設定 (MIP mapあり)
    // Texture filtering operation (with MIP map)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // テクスチャ境界の折り返し設定
    // Texture wrapping (to process pixels outside border)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャの無効化
    // Disable texture binding
    glBindTexture(GL_TEXTURE_2D, 0);

    // ロードした画素情報の破棄 (CPU側のデータを破棄するだけなのでGPU上のテクスチャには影響しない)
    // Free image data (it's on CPU side, so it does not affect texture on GPU)
    stbi_image_free(bytes);
}

// OpenGLの描画関数
// OpenGL drawing function
void paintGL() {
    // 背景色と深度値のクリア
    // Clear background color and depth values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    // Projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 100.0f);

    // モデルビュー行列
    // Model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 3.0f, 5.0f,   // 視点の位置 / Eye position
              0.0f, 0.0f, 0.0f,   // 見ている先 / Looking direction
              0.0f, 1.0f, 0.0f);  // 視界の上方向 / Upward direction

    // 前後に移動 / Move back and forth
    glTranslatef(0.0f, 0.0f, std::cos(PI * theta / 180.0f) * 4.0f - 4.0f);
    // Y軸中心にthetaだけ回転 / Rotate by theta around Y axis
    glRotatef(theta, 0.0f, 1.0f, 0.0f);

    // 平面の描画
    // Draw a flat plane
    glBindTexture(GL_TEXTURE_2D, textureId);  // テクスチャの有効化 / Enable texture mapping
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 3; i++) {
        glTexCoord2fv(texcoords[faces[0][i]]);
        glVertex3fv(positions[faces[0][i]]);
    }

    for (int i = 0; i < 3; i++) {
        glTexCoord2fv(texcoords[faces[1][i]]);
        glVertex3fv(positions[faces[1][i]]);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);  // テクスチャの無効化 / Disable texture mapping
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

    // OpenGLの描画対象にWindowを追加
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
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // 描画 / Draw
        paintGL();

        // アニメーション / Animation
        animate();

        // 描画用バッファの切り替え
        // Swap drawing target buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 後処理 / Postprocess
    glfwDestroyWindow(window);
    glfwTerminate();
}
