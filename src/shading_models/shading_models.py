# MacOS Big Sur以降で動かすための設定
# Special settings for working on MacOS Big Sur or later
import platform
import ctypes.util

uname = platform.uname()
if uname.system == "Darwin" and uname.release >= "20.":
    _find_library = ctypes.util.find_library

    def find_library(name):
        if name in ["OpenGL"]:
            return "/System/Library/Frameworks/{0}.framework/{0}".format(name)
        return _find_library(name)

    ctypes.util.find_library = find_library

# 必要なパッケージのインポート
# Import required packages
import os
import sys
import glfw
import pyrr
import numpy as np
import ctypes
import open3d as o3d
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

# シェーダ言語のソースファイル / Shader source files
VERT_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.vert")
FRAG_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.frag")

# メッシュモデルのファイル
# Mesh model file
MESH_FILE = os.path.join(os.path.dirname(__file__), "data", "bunny.obj")

# 頂点番号配列の大きさ
# Length of index array buffer
indexBufferSize = 0

# バッファを参照する番号
# Indices for vertex/index buffers
vaoId = 0
vertexBufferId = 0
indexBufferId = 0

# シェーダプログラムを参照する番号
# Index for a shader program
programId = 0

# 立方体の回転角度
# Rotation angle for animating a cube
theta = 0

# シェーディングのためのマテリアル情報
# Material parameters for shading
# Gold (http://www.barradeau.com/nicoptere/dump/materials.html)
lightPos = [5.0, 5.0, 5.0]
diffColor = [0.75164, 0.60648, 0.22648]
specColor = [0.628281, 0.555802, 0.366065]
ambiColor = [0.24725, 0.1995, 0.0745]
shininess = 51.2


# VAOの初期化
# Initialize VAO
def initVAO():
    global vaoId, vertexBufferId, indexBufferId, indexBufferSize

    # メッシュファイルの読み込み
    # Load mesh file
    mesh = o3d.io.read_triangle_mesh(MESH_FILE)
    vertices = np.array(mesh.vertices, dtype='float32')
    if mesh.vertex_normals is not None:
        normals = np.array(mesh.vertex_normals, dtype='float32')
    else:
        normals = np.zeros_like(vertices)
    indices = np.array(mesh.triangles, dtype='uint32')

    # 頂点配列の作成
    # Prepare vertex array
    vertices = np.concatenate([vertices, normals], axis=1)

    # 配列を1次元配列に変換しておく
    # Convert each array to 1D array
    vertices = np.array(vertices, dtype='float32').reshape((-1))
    indices = np.array(indices, dtype='uint32').reshape((-1))

    # VAOの作成
    # Create VAO
    vaoId = glGenVertexArrays(1)
    glBindVertexArray(vaoId)

    # 頂点バッファオブジェクトの作成
    # Create vertex buffer object
    vertexBufferId = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId)
    glBufferData(GL_ARRAY_BUFFER, len(vertices.tobytes()), vertices.tobytes(), GL_STATIC_DRAW)

    # 頂点バッファに対する属性情報の設定
    # Setup attributes for vertex buffer object
    glEnableVertexAttribArray(0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * 3 * 2, ctypes.c_void_p(0))

    glEnableVertexAttribArray(1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * 3 * 2, ctypes.c_void_p(4 * 3))

    # 頂点番号バッファオブジェクトの作成
    # Create index buffer object
    indexBufferId = glGenBuffers(1)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, len(indices.tobytes()), indices.tobytes(), GL_STATIC_DRAW)

    # 頂点バッファのサイズを変数に入れておく
    # Store size of index array buffer
    indexBufferSize = len(indices)

    # VAOをOFFにしておく
    # Temporarily disable VAO
    glBindVertexArray(0)


# シェーダのソースファイルをコンパイルする
# Compile a shader source
def compileShader(filename, type):
    # シェーダの作成
    # Create a shader
    shaderId = glCreateShader(type)

    # ファイルをすべて読んで変数に格納
    # Load entire contents of a file and store to a string variable
    with open(filename, 'r', encoding='utf-8') as f:
        code = f.read()

    # コードのコンパイル
    # Compile a source code
    glShaderSource(shaderId, code)
    glCompileShader(shaderId)

    # コンパイルの成否を判定する
    # Check whther compile is successful
    compileStatus = glGetShaderiv(shaderId, GL_COMPILE_STATUS)
    if compileStatus == GL_FALSE:
        # エラーメッセージの長さを取得する
        # Get length of error message
        errMsg = glGetShaderInfoLog(shaderId)

        # エラーメッセージとソースコードの出力
        # Print error message and corresponding source code
        sys.stderr.write("[ ERROR ] %s\n" % errMsg)
        sys.stderr.write("%s\n" % code)

        # コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        # Terminate with error message if compilation failed
        raise Exception("Failed to compile a shader!")

    return shaderId


# シェーダプログラムのビルド (=コンパイル＋リンク)
# Build a shader program (build = compile + link)
def buildShaderProgram(vShaderFile, fShaderFile):
    # 各種シェーダのコンパイル
    # Compile shader files
    vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER)
    fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER)

    # シェーダプログラムへのリンク
    # Link shader objects to the program
    programId = glCreateProgram()
    glAttachShader(programId, vertShaderId)
    glAttachShader(programId, fragShaderId)
    glLinkProgram(programId)

    # リンクの成否を判定する
    # Check whether link is successful
    linkState = glGetProgramiv(programId, GL_LINK_STATUS)
    if linkState == GL_FALSE:
        # エラーメッセージを取得する
        # Get error message
        errMsg = glGetProgramInfoLog(programId)

        # エラーメッセージを出力する
        # Print error message
        sys.stderr.write("[ ERROR ] %s\n" % errMsg)

        # リンクに失敗したらエラーメッセージを表示して終了
        # Terminate with error message if link is failed
        raise Exception("Failed to link shaders!")

    # シェーダを無効化した後にIDを返す
    # Disable shader program and return its ID
    glUseProgram(0)
    return programId


# シェーダの初期化
# Initialization related to shader programs
def initShaders():
    global programId
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE)


# ユーザ定義のOpenGLの初期化
# User-define OpenGL initialization
def initializeGL():
    # 深度テストの有効化
    # Enable depth testing
    glEnable(GL_DEPTH_TEST)

    # 背景色の設定 (黒)
    # Background color (black)
    glClearColor(0.0, 0.0, 0.0, 1.0)

    # VAOの初期化
    # Initialize VAO
    initVAO()

    # シェーダの用意
    # Prepare shader program
    initShaders()


def paintGL():
    # 背景色の描画
    # Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 座標の変換
    # Coordinate transformation
    projMat = pyrr.matrix44.create_perspective_projection(45.0, WIN_WIDTH / WIN_HEIGHT, 0.1, 1000.0)
    viewMat = pyrr.matrix44.create_look_at(
        [3.0, 4.0, 5.0],  # 視点の位置
        [0.0, 0.0, 0.0],  # 見ている先
        [0.0, 1.0, 0.0])  # 視界の上方向
    modelMat = pyrr.matrix44.create_from_axis_rotation([0.0, 1.0, 0.0], np.radians(theta))

    # pyrrの行列はglmと要素の格納順序が逆なので注意
    mvMat = np.dot(modelMat, viewMat)
    mvpMat = np.dot(modelMat, np.dot(viewMat, projMat))
    normMat = np.linalg.inv(mvMat.T)
    lightMat = viewMat

    # シェーダの有効化
    # Enable shader program
    glUseProgram(programId)

    # Uniform変数の転送
    # Transfer uniform variables
    uid = glGetUniformLocation(programId, "u_mvMat")
    glUniformMatrix4fv(uid, 1, GL_FALSE, mvMat)
    uid = glGetUniformLocation(programId, "u_mvpMat")
    glUniformMatrix4fv(uid, 1, GL_FALSE, mvpMat)
    uid = glGetUniformLocation(programId, "u_normMat")
    glUniformMatrix4fv(uid, 1, GL_FALSE, normMat)
    uid = glGetUniformLocation(programId, "u_lightMat")
    glUniformMatrix4fv(uid, 1, GL_FALSE, lightMat)

    uid = glGetUniformLocation(programId, "u_lightPos")
    glUniform3fv(uid, 1, lightPos)
    uid = glGetUniformLocation(programId, "u_diffColor")
    glUniform3fv(uid, 1, diffColor)
    uid = glGetUniformLocation(programId, "u_specColor")
    glUniform3fv(uid, 1, specColor)
    uid = glGetUniformLocation(programId, "u_ambiColor")
    glUniform3fv(uid, 1, ambiColor)
    uid = glGetUniformLocation(programId, "u_shininess")
    glUniform1f(uid, shininess)

    # VAOの有効化
    # Enable VAO
    glBindVertexArray(vaoId)

    # 三角形の描画
    # Draw triangles
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, ctypes.c_void_p(0))

    # VAOの無効化
    # Disable VAO
    glBindVertexArray(0)

    # シェーダの無効化
    # Disable shader program
    glUseProgram(0)


# ウィンドウサイズ変更のコールバック関数
# Callback function for window resizing
def resizeGL(window, width, height):
    global WIN_WIDTH, WIN_HEIGHT

    # ユーザ管理のウィンドウサイズを変更
    # Update user-managed window size
    WIN_WIDTH = width
    WIN_HEIGHT = height

    # GLFW管理のウィンドウサイズを変更
    # Update GLFW-managed window size
    glfw.set_window_size(window, WIN_WIDTH, WIN_HEIGHT)

    # 実際のウィンドウサイズ (ピクセル数) を取得
    # Get actual window size by pixels
    renderBufferWidth, renderBufferHeight = glfw.get_framebuffer_size(window)

    # ビューポート変換の更新
    # Update viewport transform
    glViewport(0, 0, renderBufferWidth, renderBufferHeight)


# アニメーションのためのアップデート
# Update for animating object
def animate():
    global theta
    theta += 1.0  # 1度だけ回転 / Rotate by 1 degree of angle


def main():
    # OpenGLを初期化する
    # OpenGL initialization
    if glfw.init() == glfw.FALSE:
        raise Exception("Initialization failed!")

    # OpenGLのバージョン設定 (Macの場合には必ず必要)
    # Specify OpenGL version (mandatory for Mac)
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

    # Windowの作成
    # Create a window
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, None, None)
    if window is None:
        glfw.terminate()
        raise Exception("Window creation failed!")

    # OpenGLの描画対象にwindowを指定
    # Specify window as an OpenGL context
    glfw.make_context_current(window)

    # OpenGLのバージョンをチェック
    # Check OpenGL version
    print(glGetString(GL_VERSION).decode('ascii'))

    # ウィンドウのリサイズを扱う関数の登録
    # Register a callback function for window resizing
    glfw.set_window_size_callback(window, resizeGL)

    # ユーザ指定の初期化
    # User-specified initialization
    initializeGL()

    # メインループ
    # Main loop
    while glfw.window_should_close(window) == glfw.FALSE:
        # 描画 / Draw
        paintGL()

        # アニメーション / Animation
        animate()

        # 描画用バッファの切り替え
        # Swap drawing target buffers
        glfw.swap_buffers(window)
        glfw.poll_events()

    # 後処理 / Postprocess
    glfw.destroy_window(window)
    glfw.terminate()


if __name__ == "__main__":
    main()
