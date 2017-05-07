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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// ディレクトリの設定ファイル
#include "common.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * std::atan(1.0);

// シェーダファイル
static const std::string RENDER_VSHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static const std::string RENDER_FSHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";
static const std::string BKG_VSHADER_FILE = std::string(SHADER_DIRECTORY) + "background.vert";
static const std::string BKG_FSHADER_FILE = std::string(SHADER_DIRECTORY) + "background.frag";

// モデルのファイル
static const std::string OBJECT_FILE = std::string(DATA_DIRECTORY) + "teapot.obj";
static const std::string BIG_CUBE_FILE = std::string(DATA_DIRECTORY) + "cube.obj";

// キューブマップのテクスチャ
static const std::string CUBEMAP_FILE = std::string(DATA_DIRECTORY) + "stpeters_cross.hdr";

// 頂点番号配列の大きさ
static size_t objectIboSize = 0;
static size_t bkgIboSize = 0;

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
GLuint objectVaoId;
GLuint bkgVaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// シェーダを参照する番号
GLuint renderProgId;
GLuint bkgProgId;

// 立方体の回転角度
static float theta = 0.0f;

// シェーディングのための情報
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
GLuint textureId;

// VAOの作成
GLuint prepareVAO(const std::string &objFile, size_t *iboSize) {
    // モデルのロード
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFile.c_str());
    if (!err.empty()) {
        std::cerr << "[WARNING] " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load OBJ file: " << objFile << std::endl;
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
    *iboSize = indices.size();

    // VAOの作成
    GLuint vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // 頂点バッファの有効化
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);

    // VAOをOFFにしておく
    glBindVertexArray(0);

    return vaoId;
}

// VAOの初期化
void initVAO() {
    objectVaoId = prepareVAO(OBJECT_FILE, &objectIboSize);
    bkgVaoId = prepareVAO(BIG_CUBE_FILE, &bkgIboSize);
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
        fprintf(stderr, "Failed to load vertex shader: %s\n", filename.c_str());
        exit(1);
    }

    // ファイルをすべて読んで変数に格納 (やや難)
    reader.seekg(0, std::ios::end);             // ファイル読み取り位置を終端に移動 
    codeSize = reader.tellg();                  // 現在の箇所(=終端)の位置がファイルサイズ
    code.resize(codeSize);                      // コードを格納する変数の大きさを設定
    reader.seekg(0);                            // ファイルの読み取り位置を戦闘に移動
    reader.read(&code[0], codeSize);            // 先頭からファイルサイズ分を読んでコードの変数に格納

    // ファイルを閉じる
    reader.close();

    // コードのコンパイル
    GLint compileStatus;
    const char *codeChars = code.c_str();
    glShaderSource(shaderId, 1, &codeChars, NULL);
    glCompileShader(shaderId);
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE) {
        // コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        fprintf(stderr, "Failed to compile vertex shader!\n");

        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

            fprintf(stderr, "[ ERROR] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }

    return shaderId;
}

// シェーダのビルド
GLuint buildShader(const std::string &vShaderFile, const std::string &fShaderFile) {
    // シェーダの作成
    GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
    GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);

    // シェーダプログラムのリンク
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);

    GLint linkState;
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE) {
        // リンクに失敗したらエラーメッセージを表示して終了
        fprintf(stderr, "Failed to link shaders!\n");

        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
        }
        exit(1);
    }
    glUseProgram(0);

    return programId;
}

// シェーダの初期化
void initShaders() {
    renderProgId = buildShader(RENDER_VSHADER_FILE, RENDER_FSHADER_FILE);
    bkgProgId = buildShader(BKG_VSHADER_FILE, BKG_FSHADER_FILE);
}

void initTexture() {
    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(CUBEMAP_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", CUBEMAP_FILE.c_str());
        exit(1);
    }

    const int faceWidth = texWidth / 3;
    const int faceHeight = texHeight / 4;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    unsigned char *faceBytes = new unsigned char[faceWidth * faceHeight * 4];

    const int startX[6] = { faceWidth, 0, faceWidth, 2 * faceWidth, faceWidth, 2 * faceWidth - 1 };
    const int startY[6] = { 0, faceHeight, faceHeight, faceHeight, 2 * faceHeight, 4 * faceHeight - 1 };
    const int deltaX[6] = { 1, 1, 1, 1, 1, -1 };
    const int deltaY[6] = { 1, 1, 1, 1, 1, -1 };
    const int targetFace[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    for (int i = 0; i < 6; i++) {
        for (int y = 0; y < faceHeight; y++) {
            for (int x = 0; x < faceWidth; x++) {
                const int px = startX[i] + x * deltaX[i];
                const int py = startY[i] + y * deltaY[i];
                for (int c = 0; c < 4; c++) {
                    faceBytes[(y * faceWidth + x) * 4 + c] = bytes[(py * texWidth + px) * 4 + c];
                }
            }
        }        
        glTexImage2D(targetFace[i], 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, faceBytes);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    stbi_image_free(bytes);    
    delete[] faceBytes;
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

    // テクスチャの初期化
    initTexture();
}

// OpenGLの描画関数
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ビューポート変換の指定
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

    // 座標の変換
    glm::mat4 projMat = glm::perspective(45.0f,
        (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glm::vec3 cameraPos = glm::vec3(5.0f, 2.0f, 5.0f);
    glm::mat4 viewMat = glm::lookAt(cameraPos,                     // 視点の位置
                                    glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                    glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

    glm::mat4 modelMat = glm::rotate(theta, glm::vec3(0.0f, 1.0f, 0.0f)); 

    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;
    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = projMat * viewMat;

    // 背景の描画
    {
        glBindVertexArray(bkgVaoId);
        glUseProgram(bkgProgId);
        
        GLuint uid;
        uid = glGetUniformLocation(bkgProgId, "u_lightMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
        uid = glGetUniformLocation(bkgProgId, "u_texture");
        glUniform1i(uid, 0);

        glDrawElements(GL_TRIANGLES, bkgIboSize, GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    // オブジェクトの描画
    {
        // VAOの有効化
        glBindVertexArray(objectVaoId);

        // シェーダの有効化
        glUseProgram(renderProgId);

        // Uniform変数の転送
        GLuint uid;
        uid = glGetUniformLocation(renderProgId, "u_modelMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
        uid = glGetUniformLocation(renderProgId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
        uid = glGetUniformLocation(renderProgId, "u_cameraPos");
        glUniform3fv(uid, 1, glm::value_ptr(cameraPos));

        // テクスチャの有効化とシェーダへの転送
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
        uid = glGetUniformLocation(renderProgId, "u_texture");
        glUniform1i(uid, 0);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, objectIboSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);

        // テクスチャの無効化
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}

void resizeGL(GLFWwindow *window, int width, int height) {
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
}

// アニメーションのためのアップデート
void animate() {
    theta += 2.0f * PI / 360.0f;  // 10分の1回転
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
