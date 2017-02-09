#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <deque>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
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

#include "common.h"

static int WIN_WIDTH   = 800;                       // ウィンドウの幅
static int WIN_HEIGHT  = 600;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const std::string AIRCRAFT_OBJFILE = std::string(DATA_DIRECTORY) + "aircraft.obj";
static const std::string AIRCRAFT_TEXFILE = std::string(DATA_DIRECTORY) + "aircraft_diff.png";

static const std::string BALLOON_OBJFILE = std::string(DATA_DIRECTORY) + "balloon.obj";
static const std::string BULLET_OBJFILE = std::string(DATA_DIRECTORY) + "bullet.obj";

static const std::string SKY_OBJFILE = std::string(DATA_DIRECTORY) + "sky.obj";
static const std::string SKY_TEXFILE = std::string(DATA_DIRECTORY) + "sky_diff.png";

static const std::string RENDER_SHADER = std::string(SHADER_DIRECTORY) + "render";

static const glm::vec3 cameraPos = glm::vec3(0.0f, 100.0f, 0.0f);
static const glm::vec3 eyeTo = glm::vec3(0.0f, 0.0f, 0.0f);
static const glm::vec3 upVec = glm::vec3(0.0f, 0.0f, -1.0f);
static const glm::vec3 lightPos = glm::vec3(0.0f, 50.0f, 0.0f);

std::deque<glm::vec3> bulletPos;
std::deque<glm::vec3> balloonPos;

struct Vertex {
    Vertex()
        : position(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 0.0f, 0.0f)
        , texcoord(0.0f, 0.0f) {
    }
    
    Vertex(const glm::vec3 &pos, const glm::vec3 &norm, const glm::vec2 &uv)
        : position(pos)
        , normal(norm)
        , texcoord(uv) {
    }
    
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct Camera {
    glm::mat4 viewMat;
    glm::mat4 projMat;
};

Camera camera;

struct RenderObject {
    GLuint programId;
    GLuint vaoId;
    GLuint vboId;
    GLuint iboId;
    GLuint textureId;
    int bufferSize;

    glm::mat4 modelMat;
    glm::vec3 ambiColor;
    glm::vec3 diffColor;
    glm::vec3 specColor;
    float shininess;
    
    void initialize() {
        programId = 0u;
        vaoId = 0u;
        vboId = 0u;
        iboId = 0u;
        textureId = 0u;
        bufferSize = 0;
        
        ambiColor = glm::vec3(0.0f, 0.0f, 0.0f);
        diffColor = glm::vec3(1.0f, 1.0f, 1.0f);
        specColor = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
    void buildShader(const std::string &basename) {
        const std::string vertShaderFile = basename + ".vert";
        const std::string fragShaderFile = basename + ".frag";
    
        // シェーダの用意
        GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        
        // ファイルの読み込み (Vertex shader)
        std::ifstream vertFileInput(vertShaderFile, std::ios::in);
        if (!vertFileInput.is_open()) {
            fprintf(stderr, "Failed to load vertex shader: %s\n", vertShaderFile.c_str());
            exit(1);
        }
        std::istreambuf_iterator<char> vertDataBegin(vertFileInput);
        std::istreambuf_iterator<char> vertDataEnd;
        const std::string vertFileData(vertDataBegin,vertDataEnd);
        const char *vertShaderCode = vertFileData.c_str();

        // ファイルの読み込み (Fragment shader)
        std::ifstream fragFileInput(fragShaderFile, std::ios::in);
        if (!fragFileInput.is_open()) {
            fprintf(stderr, "Failed to load fragment shader: %s\n", fragShaderFile.c_str());
            exit(1);
        }
        std::istreambuf_iterator<char> fragDataBegin(fragFileInput);
        std::istreambuf_iterator<char> fragDataEnd;
        const std::string fragFileData(fragDataBegin,fragDataEnd);
        const char *fragShaderCode = fragFileData.c_str();
        
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
                
                delete[] errmsg;
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
                
                delete[] errmsg;
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
            fprintf(stderr, "Failed to link shaders!\n");
            
            GLint logLength;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                GLsizei length;
                char *errmsg = new char[logLength + 1];
                glGetProgramInfoLog(programId, logLength, &length, errmsg);
                
                std::cerr << errmsg << std::endl;
                delete[] errmsg;
            }
            
            exit(1);
        }
    }
    
    void loadOBJ(const std::string &filename) {
        // Load OBJ file.
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
        if (!err.empty()) {
            std::cerr << "[WARNING] " << err << std::endl;
        }

        if (!success) {
            std::cerr << "Failed to load OBJ file: " << filename << std::endl;
            exit(1);
        }
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for (int s = 0; s < shapes.size(); s++) {
            const tinyobj::shape_t &shape = shapes[s];
            for (int i = 0; i < shape.mesh.indices.size(); i++) {
                const tinyobj::index_t &index = shapes[s].mesh.indices[i];
                
                Vertex vertex;
                if (index.vertex_index >= 0) {
                    vertex.position = glm::vec3(
                        attrib.vertices[index.vertex_index * 3 + 0],
                        attrib.vertices[index.vertex_index * 3 + 1],
                        attrib.vertices[index.vertex_index * 3 + 2]
                    );
                }
                
                if (index.normal_index >= 0) {
                    vertex.normal = glm::vec3(
                        attrib.normals[index.normal_index * 3 + 0],
                        attrib.normals[index.normal_index * 3 + 1],
                        attrib.normals[index.normal_index * 3 + 2]
                    );
                }
                
                if (index.texcoord_index >= 0) {
                    vertex.texcoord = glm::vec2(
                        attrib.texcoords[index.texcoord_index * 2 + 0],
                        1.0f - attrib.texcoords[index.texcoord_index * 2 + 1]
                    );
                }
                
                indices.push_back(vertices.size());
                vertices.push_back(vertex);
            }
        }
        
        // Prepare VAO.
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
                     vertices.data(), GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
        
        glGenBuffers(1, &iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);
        bufferSize = indices.size();
        
        glBindVertexArray(0);
    }
    
    void loadTexture(const std::string &filename) {
        int texWidth, texHeight, channels;
        unsigned char *bytes = stbi_load(filename.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
        if (!bytes) {
            fprintf(stderr, "Failed to load image file: %s\n", filename.c_str());
            exit(1);
        }
        
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        stbi_image_free(bytes);
    }
    
    void draw(const Camera &camera) {
        glUseProgram(programId);
        
        GLuint location;
        location = glGetUniformLocation(programId, "u_ambColor");
        glUniform3fv(location, 1, glm::value_ptr(ambiColor));
        location = glGetUniformLocation(programId, "u_diffColor");
        glUniform3fv(location, 1, glm::value_ptr(diffColor));
        location = glGetUniformLocation(programId, "u_specColor");
        glUniform3fv(location, 1, glm::value_ptr(specColor));
        location = glGetUniformLocation(programId, "u_shininess");
        glUniform1f(location, shininess);

        glm::mat4 mvMat, mvpMat, normMat;
        mvMat = camera.viewMat * modelMat;
        mvpMat = camera.projMat * mvMat;
        normMat = glm::transpose(glm::inverse(mvMat));
        
        location = glGetUniformLocation(programId, "u_lightPos");
        glUniform3fv(location, 1, glm::value_ptr(lightPos));
        location = glGetUniformLocation(programId, "u_lightMat");
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(camera.viewMat));
        location = glGetUniformLocation(programId, "u_mvMat");
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(mvMat));
        location = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvpMat));
        location = glGetUniformLocation(programId, "u_normMat");
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(normMat));
        
        if (textureId != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
            location = glGetUniformLocation(programId, "u_isTextured");
            glUniform1i(location, 1);
            location = glGetUniformLocation(programId, "u_texture");
            glUniform1i(location, 0);
        } else {
            location = glGetUniformLocation(programId, "u_isTextured");
            glUniform1i(location, 0);
        }
        
        glBindVertexArray(vaoId);
        glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glUseProgram(0);
    }
};

RenderObject aircraft;
RenderObject bullet;
RenderObject balloon;
RenderObject sky;

void initializeGL() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    aircraft.initialize();
    aircraft.loadOBJ(AIRCRAFT_OBJFILE);
    aircraft.buildShader(RENDER_SHADER);
    aircraft.loadTexture(AIRCRAFT_TEXFILE);
    aircraft.modelMat = glm::translate(glm::vec3(0.0f, 0.0f, 30.0f));
    
    balloon.initialize();
    balloon.loadOBJ(BALLOON_OBJFILE);
    balloon.buildShader(RENDER_SHADER);
    balloon.diffColor = glm::vec3(1.0f, 0.0f, 0.0f);

    bullet.initialize();
    bullet.loadOBJ(BULLET_OBJFILE);
    bullet.buildShader(RENDER_SHADER);
    bullet.diffColor = glm::vec3(0.5f, 0.5f, 0.0f);
    bullet.specColor = glm::vec3(0.5f, 0.5f, 0.5f);

    sky.initialize();
    sky.loadOBJ(SKY_OBJFILE);
    sky.buildShader(RENDER_SHADER);
    sky.loadTexture(SKY_TEXFILE);
    sky.modelMat = glm::scale(glm::vec3(200.0f, 200.0f, 200.0f));
    
    float aspect = WIN_WIDTH / (float)WIN_HEIGHT;
    camera.projMat = glm::ortho(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, 0.1f, 1000.0f);
    camera.viewMat = glm::lookAt(cameraPos, eyeTo, upVec);

    // 風船の配置
    srand((unsigned long)time(0));
    for (int i = 0; i < 10; i++) {
        float rx = (rand() / (float)RAND_MAX) * 60.0f - 30.0f;
        float rz = (rand() / (float)RAND_MAX) * 30.0f - 50.0f;
        balloonPos.push_back(glm::vec3(rx, 0.0f, rz));
    }
}

void paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

    sky.draw(camera);
    
    aircraft.draw(camera);

    std::deque<glm::vec3>::iterator it;
    for (it = bulletPos.begin(); it != bulletPos.end(); ++it) {
        bullet.modelMat = glm::translate(*it);
        bullet.draw(camera);
    }

    for (it = balloonPos.begin(); it != balloonPos.end(); ++it) {
        balloon.modelMat = glm::translate(*it);
        balloon.draw(camera);
    }
}

void resizeGL(GLFWwindow *window, int width, int height) {
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

    float aspect = WIN_WIDTH / (float)WIN_HEIGHT;
    camera.projMat = glm::ortho(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, 0.1f, 1000.0f);
}

void update() {
    std::deque<glm::vec3>::iterator it;
    std::deque<glm::vec3>::iterator jt;

    // 球の位置のアップデート
    for (it = bulletPos.begin(); it != bulletPos.end(); ++it) {
        it->z += -2.0f;
    }

    // 風船とのあたり判定
    bool hit = true;
    while(hit) {
        hit = false;
        for (it = bulletPos.begin(); it != bulletPos.end() && !hit; ++it) {
            for (jt = balloonPos.begin(); jt != balloonPos.end() && !hit; ++jt) {
                glm::vec3 diff = *it - *jt;
                if (glm::length(*it - *jt) <= 1.0f) {
                    bulletPos.erase(it);
                    balloonPos.erase(jt);
                    hit = true;
                }
            }
        }
    }

    // 画面外の弾を削除
    while (!bulletPos.empty()) {
        if (bulletPos.begin()->z <= -50.0f) {
            bulletPos.pop_front();
            continue;
        }
        break;
    }

}

void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    int state;
    
    // 左
    state = glfwGetKey(window, GLFW_KEY_LEFT);
    if (state == GLFW_PRESS) {
        aircraft.modelMat = glm::translate(aircraft.modelMat, glm::vec3(-0.5f, 0.0f, 0.0f));
    }

    // 右
    state = glfwGetKey(window, GLFW_KEY_RIGHT);
    if (state == GLFW_PRESS) {
        aircraft.modelMat = glm::translate(aircraft.modelMat, glm::vec3(0.5f, 0.0f, 0.0f));
    }

    // Enter
    state = glfwGetKey(window, GLFW_KEY_ENTER);
    if (state == GLFW_PRESS) {
        printf("Bullets: %zu\n", bulletPos.size());
        if (bulletPos.size() < 5) {
            glm::vec4 pos = aircraft.modelMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            pos = pos / pos.w;
            bulletPos.push_back(glm::vec3(pos.x, pos.y, pos.z));
        }
    }
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }
    
    // OpenGLのバージョン指定
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
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

    // キーボードコールバック関数の登録
    glfwSetKeyCallback(window, keyboard);
    
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
