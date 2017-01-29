/******************************************************************************
実験
(1) 三角形の頂点の位置を変えるとどうなるかを調べる
(2) 三角形の頂点の色を変えるとどうなるかを調べる
(3) 三角形の頂点の色を部分的にコメントアウトするとどうなるかを調べる
******************************************************************************/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#define GLFW_INCLUDE_GLU
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const char *VERT_SHADER_FILE = "glsl.vert";
static const char *FRAG_SHADER_FILE = "glsl.frag";

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec3 &color_)
        : position(position_)
        , color(color_) {
    }

    glm::vec3 position;
    glm::vec3 color;
};

// 頂点情報
Vertex vertices[] = {
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))
};

// 面を構成する頂点の番号
unsigned int indices[] = {
    0, 1, 2
};

// バッファを参照する番号
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// シェーダを参照する番号
GLuint vertShaderId;
GLuint fragShaderId;
GLuint programId;

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // VAOの作成
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, vertices, GL_STATIC_DRAW);

    // 頂点バッファの有効化
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3,
                 indices, GL_STATIC_DRAW);

    // VAOをOFFにしておく
    glBindVertexArray(0);

    // シェーダの用意
    vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // ファイルの読み込み
    std::ifstream vertShaderFile(VERT_SHADER_FILE, std::ios::in);
    if (!vertShaderFile.is_open()) {
        fprintf(stderr, "Failed to load vertex shader: %s\n", VERT_SHADER_FILE);
        exit(1);
    }

    std::string line;

    std::string vertShaderBuffer;
    while (!vertShaderFile.eof()) {
        std::getline(vertShaderFile, line);
        vertShaderBuffer += line + "\r\n";
    }
    const char *vertShaderCode = vertShaderBuffer.c_str();
    
    std::ifstream fragShaderFile(FRAG_SHADER_FILE, std::ios::in);
    if (!fragShaderFile.is_open()) {
        fprintf(stderr, "Failed to load fragment shader: %s\n", FRAG_SHADER_FILE);
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
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT);

    // 座標の変換
    glm::mat4 projMat = glm::perspective(45.0f,
        (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glm::mat4 lookAt = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
                                   glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                   glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

    glm::mat4 mvpMat = projMat * lookAt;

    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // Uniform変数の転送
    GLuint mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

    // 三角形の描画
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);
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

    // GLEWを初期化する (glfwMakeContextCurrentの後でないといけない)
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed!\n");
        return 1;
    }

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
