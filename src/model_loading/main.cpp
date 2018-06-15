#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLFW_INCLUDE_GLU
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// ディレクトリの設定ファイル
#include "common.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * std::atan(1.0);

// シェーダファイル
static const std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static const std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";

// モデルのファイル
static const std::string OBJECT_FILE = std::string(DATA_DIRECTORY) + "bunny.obj";

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec3 &normal_)
        : position(position_)
        , normal(normal_) {
    }

    glm::vec3 position;
    glm::vec3 normal;
};

// バッファを参照する番号
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// インデックスバッファのサイズ (glDrawElementsで使用)
size_t indexBufferSize;

// シェーダを参照する番号
GLuint programId;

// 立方体の回転角度
static float theta = 0.0f;

// VAOの初期化
void initVAO() {
    // モデルのロード
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, OBJECT_FILE.c_str());
    if (!err.empty()) {
        std::cerr << "[WARNING] " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load OBJ file: " << OBJECT_FILE << std::endl;
        exit(1);
    }

    // Vertex配列の作成
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int s = 0; s < shapes.size(); s++) {
        const tinyobj::mesh_t &mesh = shapes[s].mesh;
        for (int i = 0; i < mesh.indices.size(); i++) {
            const tinyobj::index_t &index = mesh.indices[i];
            
            glm::vec3 position, normal;

            if (index.vertex_index >= 0) {
                position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0],
                                     attrib.vertices[index.vertex_index * 3 + 1],
                                     attrib.vertices[index.vertex_index * 3 + 2]);
            }

            if (index.normal_index >= 0) {
                normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0],
                                   attrib.normals[index.normal_index * 3 + 1],
                                   attrib.normals[index.normal_index * 3 + 2]);
            }
            
            const unsigned int vertexIndex = vertices.size();
            vertices.push_back(Vertex(position, normal));
            indices.push_back(vertexIndex);
        }
    }

    // VAOの作成
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // 頂点バッファの有効化
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
    
    // 頂点バッファのサイズを変数に入れておく
    indexBufferSize = indices.size();

    // VAOをOFFにしておく
    glBindVertexArray(0);
}

GLuint compileShader(const std::string &filename, GLuint type) {
    // シェーダの作成
    GLuint shaderId = glCreateShader(type);
    
    // ファイルの読み込み
    std::ifstream reader;
    size_t codeSize;
    std::string code;

    // ファイルを開く
    reader.open(filename.c_str(), std::ios::in);
    if (!reader.is_open()) {
        // ファイルを開けなかったらエラーを出して終了
        fprintf(stderr, "Failed to load a shader: %s\n", VERT_SHADER_FILE.c_str());
        exit(1);
    }

    // ファイルをすべて読んで変数に格納 (やや難)
    reader.seekg(0, std::ios::end);             // ファイル読み取り位置を終端に移動 
    codeSize = reader.tellg();                  // 現在の箇所(=終端)の位置がファイルサイズ
    code.resize(codeSize);                      // コードを格納する変数の大きさを設定
    reader.seekg(0);                            // ファイルの読み取り位置を先頭に移動
    reader.read(&code[0], codeSize);            // 先頭からファイルサイズ分を読んでコードの変数に格納

    // ファイルを閉じる
    reader.close();

    // コードのコンパイル
    const char *codeChars = code.c_str();
    glShaderSource(shaderId, 1, &codeChars, NULL);
    glCompileShader(shaderId);

    // コンパイルの成否を判定する
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        // コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        fprintf(stderr, "Failed to compile a shader!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

            // エラーメッセージとソースコードの出力
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }

    return shaderId;
}

GLuint buildShaderProgram(const std::string &vShaderFile, const std::string &fShaderFile) {
    // シェーダの作成
    GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
    GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);
    
    // シェーダプログラムのリンク
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    
    // リンクの成否を判定する
    GLint linkState;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE) {
        // リンクに失敗したらエラーメッセージを表示して終了
        fprintf(stderr, "Failed to link shaders!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

            // エラーメッセージを出力する
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
        }
        exit(1);
    }
    
    // シェーダを無効化した後にIDを返す
    glUseProgram(0);
    return programId;
}

// シェーダの初期化
void initShaders() {
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

// OpenGLの初期化関数
void initializeGL() {
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);

    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // VAOの初期化
    initVAO();

    // シェーダの用意
    initShaders();
}

// OpenGLの描画関数
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 座標の変換
    glm::mat4 projMat = glm::perspective(45.0f,
        (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
                                    glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                    glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

    glm::mat4 modelMat = glm::rotate(glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f)); 

    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;

    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // Uniform変数の転送
    GLuint uid;
    uid = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);
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
void animate() {
    theta += 1.0f;  // 10分の1回転
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // OpenGLのバージョン設定 (Macの場合には必ず必要)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    glewExperimental = true;
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
        animate();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
