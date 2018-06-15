#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "common.h"
#include "wave_equation.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "hue.png";

static const std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "glsl.vert";
static const std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "glsl.frag";

// VAO関連の変数
GLuint vaoId;
GLuint vboId;
GLuint iboId;

// シェーダを参照する番号
GLuint vertShaderId;
GLuint fragShaderId;
GLuint programId;

// テクスチャ
GLuint textureId;

// 波動方程式の計算に使うパラメータ
WaveEquation waveEqn;
static const int xCells = 100;
static const int yCells = 100;
static const double speed = 0.5;
static const double dx = 0.05;
static const double dt = 0.05;

// 頂点のデータ
std::vector<glm::vec3> positions;

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);

    // 波動方程式シミュレーションの初期化
    waveEqn.setParams(xCells, yCells, speed, dx, dt);

    // 頂点データの初期化
    for (int y = 0; y < yCells; y++) {
        for (int x = 0; x < xCells; x++) {
            double vx = (x - xCells / 2) * dx;
            double vy = (y - yCells / 2) * dx;
            positions.push_back(glm::vec3(vx, vy, 0.0));

            waveEqn.set(x, y, 2.0 * exp(-5.0 * (vx * vx + vy * vy)));
        }
    }

    waveEqn.start();

    // VAOの用意
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(),
                 &positions[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    std::vector<unsigned int> indices;
    for (int y = 0; y < yCells - 1; y++) {
        for (int x = 0; x < xCells - 1; x++) {
            const int i0 = y * xCells + x;
            const int i1 = y * xCells + (x + 1);
            const int i2 = (y + 1) * xCells + x;
            const int i3 = (y + 1) * xCells + (x + 1);

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i0);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * indices.size(),
                 &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    // テクスチャの用意
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
        exit(1);
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_1D, textureId);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, texWidth,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_1D, 0);

    stbi_image_free(bytes);

    // シェーダの用意
    vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // ファイルの読み込み
    std::ifstream vertShaderFile(VERT_SHADER_FILE.c_str(), std::ios::in);
    if (!vertShaderFile.is_open()) {
        fprintf(stderr, "Failed to load vertex shader: %s\n", VERT_SHADER_FILE.c_str());
        exit(1);
    }

    std::string line;

    std::string vertShaderBuffer;
    while (!vertShaderFile.eof()) {
        std::getline(vertShaderFile, line);
        vertShaderBuffer += line + "\r\n";
    }
    const char *vertShaderCode = vertShaderBuffer.c_str();
    
    std::ifstream fragShaderFile(FRAG_SHADER_FILE.c_str(), std::ios::in);
    if (!fragShaderFile.is_open()) {
        fprintf(stderr, "Failed to load fragment shader: %s\n", FRAG_SHADER_FILE.c_str());
        exit(1);
    }

    std::string fragShaderBuffer;
    while (!fragShaderFile.eof()) {
        std::getline(fragShaderFile, line);
        fragShaderBuffer += line + "\n";
    }
    const char *fragShaderCode = fragShaderBuffer.c_str();

    // シェーダのコンパイル
    GLint compileStatus;
    glShaderSource(vertShaderId, 1, &vertShaderCode, NULL);
    glCompileShader(vertShaderId);
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        fprintf(stderr, "Failed to compile vertex shader!\n");
        
        GLint logLength;
        glGetShaderiv(vertShaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei length;
            char *errmsg = new char[logLength + 1];
            glGetShaderInfoLog(vertShaderId, logLength, &length, errmsg);

            std::cerr << errmsg << std::endl;
            fprintf(stderr, "%s", vertShaderCode);
        }
    }

    glShaderSource(fragShaderId, 1, &fragShaderCode, NULL);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        fprintf(stderr, "Failed to compile fragment shader!\n");

        GLint logLength;
        glGetShaderiv(fragShaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei length;
            char *errmsg = new char[logLength + 1];
            glGetShaderInfoLog(fragShaderId, logLength, &length, errmsg);

            std::cerr << errmsg << std::endl;
            fprintf(stderr, "%s", vertShaderCode);
        }
    }

    // シェーダプログラムの用意
    programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);

    GLint linkState;
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE) {
        fprintf(stderr, "Failed to compile shader!\n");

        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei length;
            char *errmsg = new char[logLength];
            glGetProgramInfoLog(programId, logLength, &length, errmsg);

            std::cerr << errmsg << std::endl;
            delete[] errmsg;
        }

        exit(1);
    }
}

// OpenGLの描画関数
void paintGL() {
    // 背景色と深度値のクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 座標の変換
    glm::mat4 projMat = glm::perspective(45.0f,
        (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glm::mat4 lookAt = glm::lookAt(glm::vec3(5.0f, 5.0f, 3.0f),   // 視点の位置
                                   glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                   glm::vec3(0.0f, 0.0f, 1.0f));  // 視界の上方向

    glm::mat4 mvpMat = projMat * lookAt;

    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // テクスチャの有効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, textureId);

    // Uniform変数の転送
    GLuint mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

    GLuint texLocId = glGetUniformLocation(programId, "u_texture");
    glUniform1i(texLocId, 0);

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, 3 * (yCells - 1) * (xCells - 1) * 2, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);

    // テクスチャの無効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    
    // GLFW管理のウィンドウサイズを変更
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
    
    // 実際のウィンドウサイズ (ピクセル数) を取得
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);
    
    // ビューポート変換の更新
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

// アニメーションのためのアップデート
void update() {
    // 波動データの更新
    waveEqn.step();

    for (int y = 0; y < yCells; y++) {
        for (int x = 0; x < xCells; x++) {
            positions[y * xCells + x].z = waveEqn.get(x, y);
        }
    }

    glBindVertexArray(vaoId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * positions.size(), &positions[0]);
    glBindVertexArray(0);
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }
    
    // OpenGLのバージョン指定
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
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
    
    // GLEWを初期化する (glfwMakeContextCurrentの後でないといけない)
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed!\n");
        return 1;
    }

    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);

    // OpenGLを初期化
    initializeGL();

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();

        // アニメーション
        update();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
