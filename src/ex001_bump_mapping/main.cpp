#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#define GLFW_INCLUDE_GLU
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
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

// モデルのファイル
static const std::string BUNNY_FILE = std::string(DATA_DIRECTORY) + "head.obj";

//　テクスチャファイル
static const std::string DIFFUSE_TEX_FILE = std::string(DATA_DIRECTORY) + "lambertian.png";
static const std::string BUMP_TEX_FILE = std::string(DATA_DIRECTORY) + "normal-lowRes.png";

// 頂点番号配列の大きさ
static size_t indexBufferSize = 0;

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec2 &texcoord_,
           const glm::vec3 &normal_ = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 &tangent_ = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 &binormal_ = glm::vec3(0.0f, 0.0f, 0.0f))
        : position(position_)
        , texcoord(texcoord_) 
        , normal(normal_)
        , tangent(tangent_)
        , binormal(binormal_) {
    }

    bool operator==(const Vertex & other) const {
        return position == other.position && texcoord == other.texcoord;
    }

    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 binormal;
};

namespace std {
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex &vertex) const {
            return hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.texcoord) << 1);
        }
    };
}

// バッファを参照する番号
GLuint vaoId;
GLuint vertexBufferId;
GLuint indexBufferId;

// シェーダを参照する番号
GLuint vertShaderId;
GLuint fragShaderId;
GLuint programId;

// 立方体の回転角度
static float theta = 0.0f;

// バンプマップを有効にするかどうか
static bool enableBump = true;

// シェーディングのための情報
static const glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
GLuint diffTexId;
GLuint bumpTexId;

// VAOの初期化
void initVAO() {
    // モデルのロード
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, BUNNY_FILE.c_str());
    if (!err.empty()) {
        std::cerr << "[WARNING] " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load OBJ file: " << BUNNY_FILE << std::endl;
        exit(1);
    }

    // Vertex配列の作成
    std::unordered_map<Vertex, int> uniqueVertices;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int s = 0; s < shapes.size(); s++) {
        const tinyobj::mesh_t &mesh = shapes[s].mesh;
        for (int i = 0; i < mesh.indices.size(); i++) {
            const tinyobj::index_t &index = mesh.indices[i];
            
            glm::vec3 position;
            glm::vec2 texcoord;

            if (index.vertex_index >= 0) {
                position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0],
                                     attrib.vertices[index.vertex_index * 3 + 1],
                                     attrib.vertices[index.vertex_index * 3 + 2]);
            }

            if (index.texcoord_index >= 0) {
                texcoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2 + 0],
                                     attrib.texcoords[index.texcoord_index * 2 + 1]);
            }

            Vertex vertex(position, texcoord);
            
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = vertices.size();
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
    indexBufferSize = indices.size();

    // 接ベクトル及び従法線ベクトルの初期化
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].tangent = glm::vec3(0.0f, 0.0f, 0.0f);
        vertices[i].binormal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // 接ベクトルおよび従法線ベクトルを求める
    for (int face = 0; face < indices.size(); face += 3) {
        const glm::vec3 & v0 = vertices[indices[face + 0]].position;
        const glm::vec3 & v1 = vertices[indices[face + 1]].position;
        const glm::vec3 & v2 = vertices[indices[face + 2]].position;
        const glm::vec2 &uv0 = vertices[indices[face + 0]].texcoord;
        const glm::vec2 &uv1 = vertices[indices[face + 1]].texcoord;
        const glm::vec2 &uv2 = vertices[indices[face + 2]].texcoord;
        
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;
        glm::vec2 deltaUv1 = uv1 - uv0;
        glm::vec2 deltaUv2 = uv2 - uv0;

        float det = deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x;
        if (det != 0.0f) {
            glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUv2.y - deltaPos2 * deltaUv1.y) / det);
            glm::vec3 binormal = glm::normalize((deltaPos2 * deltaUv1.x - deltaPos1 * deltaUv2.x) / det);

            vertices[indices[face + 0]].tangent += tangent;
            vertices[indices[face + 0]].binormal += binormal;
            vertices[indices[face + 1]].tangent += tangent;
            vertices[indices[face + 1]].binormal += binormal;
            vertices[indices[face + 2]].tangent += tangent;
            vertices[indices[face + 2]].binormal += binormal;
        }
    }

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].tangent = glm::normalize(vertices[i].tangent);
        vertices[i].binormal = glm::normalize(vertices[i].binormal);
        vertices[i].normal = glm::cross(vertices[i].tangent, vertices[i].binormal);
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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, binormal));

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);

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

            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }

    return shaderId;
}

// シェーダの初期化
void initShaders() {
    // シェーダの作成
    vertShaderId = compileShader(RENDER_VSHADER_FILE, GL_VERTEX_SHADER);
    fragShaderId = compileShader(RENDER_FSHADER_FILE, GL_FRAGMENT_SHADER);

    // シェーダプログラムのリンク
    programId = glCreateProgram();
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

    // シェーダの無効化しておく
    glUseProgram(0);
}

void initTexture() {
    int texWidth, texHeight, channels;
    unsigned char *bytes = NULL;
    
    // 拡散反射テクスチャの設定
    bytes = stbi_load(DIFFUSE_TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", DIFFUSE_TEX_FILE.c_str());
        exit(1);
    }

    glGenTextures(1, &diffTexId);
    glBindTexture(GL_TEXTURE_2D, diffTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(bytes);    

    // バンプマップテクスチャの設定
    bytes = stbi_load(BUMP_TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load file: %s\n", BUMP_TEX_FILE.c_str());
        exit(1);
    }

    glGenTextures(1, &bumpTexId);
    glBindTexture(GL_TEXTURE_2D, bumpTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(bytes);
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

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.5f, 0.1f, 0.5f),   // 視点の位置
                                    glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                    glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

    glm::mat4 modelMat = glm::rotate(theta, glm::vec3(0.0f, 1.0f, 0.0f)); 

    glm::mat4 mvMat = viewMat * modelMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat;
    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = viewMat;

    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // Uniform変数の転送
    GLuint uid;
    uid = glGetUniformLocation(programId, "u_mvMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
    uid = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
    uid = glGetUniformLocation(programId, "u_normMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(normMat));
    uid = glGetUniformLocation(programId, "u_lightMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));
    uid = glGetUniformLocation(programId, "u_lightPos");
    glUniform3fv(uid, 1, glm::value_ptr(lightPos));
    uid = glGetUniformLocation(programId, "u_enableBump");
    glUniform1i(uid, enableBump ? 1 : 0);

    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTexId);
    uid = glGetUniformLocation(programId, "u_diffTexture");
    glUniform1i(uid, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bumpTexId);
    uid = glGetUniformLocation(programId, "u_bumpTexture");
    glUniform1i(uid, 1);

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);

    // テクスチャの無効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void resizeGL(GLFWwindow *window, int width, int height) {
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int modes) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        enableBump = !enableBump;
    }
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

    // キーボードイベントを扱う関数の登録
    glfwSetKeyCallback(window, keyboardCallback);

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
