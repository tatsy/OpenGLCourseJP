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
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

# シェーダ言語のソースファイル / Shader source files
VERT_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.vert")
FRAG_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.frag")

# 立方体のパラメータ
# Cube parameters
# yapf: disable
positions = [
    [ -1.0, -1.0, -1.0 ],
    [  1.0, -1.0, -1.0 ],
    [ -1.0,  1.0, -1.0 ],
    [ -1.0, -1.0,  1.0 ],
    [  1.0,  1.0, -1.0 ],
    [ -1.0,  1.0,  1.0 ],
    [  1.0, -1.0,  1.0 ],
    [  1.0,  1.0,  1.0 ],
]

colors = [
    [ 1.0, 0.0, 0.0 ],  # 赤
    [ 0.0, 1.0, 0.0 ],  # 緑
    [ 0.0, 0.0, 1.0 ],  # 青
    [ 1.0, 1.0, 0.0 ],  # イエロー
    [ 0.0, 1.0, 1.0 ],  # シアン
    [ 1.0, 0.0, 1.0 ],  # マゼンタ
]

faces = [
    [ 7, 4, 1 ], [ 7, 1, 6 ],
    [ 2, 4, 7 ], [ 2, 7, 5 ],
    [ 5, 7, 6 ], [ 5, 6, 3 ],
    [ 4, 2, 0 ], [ 4, 0, 1 ],
    [ 3, 6, 1 ], [ 3, 1, 0 ],
    [ 2, 5, 3 ], [ 2, 3, 0 ],
]
# yapf: enable

# バッファを参照する番号
# Indices for vertex/index buffers
vaoId = 0
vertexBufferId = 0
indexBufferId = 0

# シェーダプログラムを参照する番号
# Index for a shader program
programId = 0

# マウスドラッグ中かどうか
# Flag to check mouse is dragged or not
isDragging = False

# マウスのクリック位置
# Mouse click position
oldPos = [0, 0]
newPos = [0, 0]

# 操作の種類
# Type of control
ARCBALL_MODE_NONE = 0x00
ARCBALL_MODE_TRANSLATE = 0x01
ARCBALL_MODE_ROTATE = 0x02
ARCBALL_MODE_SCALE = 0x04

# 座標変換のための変数
# Variables for coordinate transformation
arcballMode = ARCBALL_MODE_NONE
viewMat = projMat = None
acRotMat = acTransMat = acScaleMat = None
acScale = 1.0


# VAOの初期化
# Initialize VAO
def initVAO():
    global vaoId, vertexBufferId, indexBufferId

    # 頂点配列の作成
    # Prepare vertex array
    vertices = []
    indices = []
    idx = 0
    for face in range(6):
        for i in range(3):
            vertices.extend([positions[faces[face * 2 + 0][i]], colors[face]])
            indices.append(idx)
            idx += 1

        for i in range(3):
            vertices.extend([positions[faces[face * 2 + 1][i]], colors[face]])
            indices.append(idx)
            idx += 1

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
    global projMat, viewMat, acRotMat, acTransMat, acScaleMat

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

    # カメラの姿勢を決定する変換行列の初期化
    # Initialize transformation matrices for camera pose
    projMat = pyrr.matrix44.create_perspective_projection(45.0, WIN_WIDTH / WIN_HEIGHT, 0.1, 1000.0)
    viewMat = pyrr.matrix44.create_look_at(
        (3.0, 4.0, 5.0),  # 視点の位置 / Eye position
        (0.0, 0.0, 0.0),  # 見ている先 / Looking position
        (0.0, 1.0, 0.0))  # 視界の上方向 / Upward vector

    # アークボール操作のための変換行列を初期化
    # Initialize transformation matrices for arcball control
    acRotMat = np.eye(4)
    acTransMat = np.eye(4)
    acScaleMat = np.eye(4)


def paintGL():
    # 背景色の描画
    # Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 座標の変換. アークボール操作からモデル行列を決定する
    # Coordinate transformation. Model matrix is determined by arcball control
    modelMat = np.dot(acScaleMat, np.dot(acRotMat, acTransMat))
    mvpMat = np.dot(modelMat, np.dot(viewMat, projMat))

    # シェーダの有効化
    # Enable shader program
    glUseProgram(programId)

    # Uniform変数の転送
    # Transfer uniform variables
    mvpMatLocId = glGetUniformLocation(programId, "u_mvpMat")
    glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, mvpMat)

    # VAOの有効化
    # Enable VAO
    glBindVertexArray(vaoId)

    # 三角形の描画
    # Draw triangles
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, ctypes.c_void_p(0))

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


# マウスのクリックを処理するコールバック関数
# Callback for mouse click events
def mouseEvent(window, button, action, mods):
    global isDragging, newPos, oldPos, arcballMode

    # クリックしたボタンで処理を切り替える
    # Switch following operation depending on a clicked button
    if button == glfw.MOUSE_BUTTON_LEFT:
        arcballMode = ARCBALL_MODE_ROTATE
    elif button == glfw.MOUSE_BUTTON_MIDDLE:
        arcballMode = ARCBALL_MODE_SCALE
    elif button == glfw.MOUSE_BUTTON_RIGHT:
        arcballMode = ARCBALL_MODE_TRANSLATE

    # クリックされた位置を取得
    # Acquire a click position
    px, py = glfw.get_cursor_pos(window)

    # マウスドラッグの状態を更新
    # Update state of mouse dragging
    if action == glfw.PRESS:
        if not isDragging:
            isDragging = True
            oldPos = [px, py]
            newPos = [px, py]
    else:
        isDragging = False
        oldPos = [0, 0]
        newPos = [0, 0]
        arcballMode = ARCBALL_MODE_NONE


# スクリーン上の位置をアークボール球上の位置に変換する関数
# Convert screen-space coordinates to a position on the arcball sphere
def getVector(x, y):
    # 円がスクリーンの長辺に内接していると仮定
    # Assume a circle contacts internally with longer edges
    shortSide = min(WIN_WIDTH, WIN_HEIGHT)
    pt = np.array([2.0 * x / shortSide - 1.0, -2.0 * y / shortSide + 1.0, 0.0])

    # z座標の計算
    # Calculate Z coordinate
    xySquared = pt[0]**2 + pt[1]**2
    if xySquared <= 1.0:
        # 単位円の内側ならz座標を計算
        # Calculate Z coordinate if a point is inside a unit circle
        pt[2] = np.sqrt(1.0 - xySquared)
    else:
        # 外側なら球の外枠上にあると考える
        # Suppose a point is on the circle line if the click position is outside the unit circle
        pt /= np.linalg.norm(pt)

    return pt


# 回転成分の更新
# Update rotation matrix
def updateRotate():
    global acRotMat

    # マウスクリック位置をアークボール球上の座標に変換
    # Convert click positions to those on the arcball sphere
    u = getVector(oldPos[0], oldPos[1])
    v = getVector(newPos[0], newPos[1])

    # カメラ座標における回転量 (=世界座標における回転量)
    # Amount of rotation in camera space (= that in world space)
    angle = np.arccos(np.clip(np.dot(u, v), -1.0, 1.0))

    # カメラ空間における回転軸
    # Rotation axis in camera space
    rotAxis = np.cross(u, v)

    # カメラ座標の情報を世界座標に変換する行列
    # Transformation matrix from camera space to world space
    c2wMat = np.linalg.inv(viewMat)

    # 世界座標における回転軸
    # Rotation axis in world space
    rotAxis = np.concatenate([rotAxis, [0.0]])
    rotAxisWorldSpace = np.dot(rotAxis, c2wMat)[:3]

    # 回転行列の更新
    # Update rotation matrix
    acRotMat = np.dot(acRotMat, pyrr.matrix44.create_from_axis_rotation(rotAxisWorldSpace, 4.0 * angle))


# 平行移動量成分の更新
# Update translation matrix
def updateTranslate():
    global acTransMat

    # NOTE:
    # この関数では物体が世界座標の原点付近にあるとして平行移動量を計算する
    # This function assumes the object locates near to the world-space origin and computes the amount of translation

    # 世界座標の原点のスクリーン座標を求める
    # Calculate screen-space coordinates of the world-space origin
    originScreenSpace = np.dot(np.dot(np.array([0.0, 0.0, 0.0, 1.0]), viewMat), projMat)
    originScreenSpace /= originScreenSpace[3]

    # スクリーン座標系におけるマウス移動の視点と終点の計算. これらの位置はスクリーン座標系のZ座標に依存することに注意する
    # Calculate the start and end points of mouse motion, which depend Z coordinate in screen space
    newPosScreenSpace = np.array(
        [2.0 * newPos[0] / WIN_WIDTH - 1.0, -2.0 * newPos[1] / WIN_HEIGHT + 1.0, originScreenSpace[2], 1.0])
    oldPosScreenSpace = np.array(
        [2.0 * oldPos[0] / WIN_WIDTH - 1.0, -2.0 * oldPos[1] / WIN_HEIGHT + 1.0, originScreenSpace[2], 1.0])

    # スクリーン座標の情報を世界座標座標に変換する行列 (= MVP行列の逆行列)
    # Transformation from screen space to world space (= inverse of MVP matrix)
    invMvpMat = np.linalg.inv(np.dot(viewMat, projMat))

    # スクリーン空間の座標を世界座標に変換
    # Transform screen-space positions to world-space positions
    newPosObjSpace = np.dot(newPosScreenSpace, invMvpMat)
    oldPosObjSpace = np.dot(oldPosScreenSpace, invMvpMat)
    newPosObjSpace /= newPosObjSpace[3]
    oldPosObjSpace /= oldPosObjSpace[3]

    # 世界座標系で移動量を求める
    # Calculate the amount of translation in world space
    transWorldSpace = (newPosObjSpace - oldPosObjSpace)[:3]

    # 行列に変換
    # Calculate translation matrix
    acTransMat = np.dot(acTransMat, pyrr.matrix44.create_from_translation(transWorldSpace))


# 物体の拡大縮小率を更新
# Update object scale
def updateScale():
    global acScaleMat
    acScaleMat = pyrr.matrix44.create_from_scale([acScale, acScale, acScale])


# 変換行列の更新. マウス操作の内容に応じて更新対象を切り替える
# Update transformation matrices, depending on type of mouse interaction
def updateTransform():
    if arcballMode == ARCBALL_MODE_ROTATE:
        updateRotate()
    elif arcballMode == ARCBALL_MODE_TRANSLATE:
        updateTranslate()
    elif arcballMode == ARCBALL_MODE_SCALE:
        acScale += (oldPos[1] - newPos[1]) / WIN_HEIGHT
        updateScale()


# マウスの動きを処理するコールバック関数
# Callback for mouse move events
def motionEvent(window, xpos, ypos):
    global isDragging, newPos, oldPos

    if isDragging:
        # マウスの現在位置を更新
        # Update current mouse position
        newPos = [xpos, ypos]

        # マウスがあまり動いていない時は処理をしない
        # Update transform only when mouse moves sufficiently
        dx = newPos[0] - oldPos[0]
        dy = newPos[1] - oldPos[1]
        length = dx * dx + dy * dy
        if length < 2.0 * 2.0:
            return
        else:
            updateTransform()
            oldPos = [xpos, ypos]


# マウスホイールを処理するコールバック関数
# Callback for mouse wheel event
def wheelEvent(window, xpos, ypos):
    global acScale

    acScale += ypos / 10.0
    updateScale()


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

    # マウスのイベントを処理する関数を登録
    # Register a callback function for mouse click events
    glfw.set_mouse_button_callback(window, mouseEvent)

    # マウスの動きを処理する関数を登録
    # Register a callback function for mouse move events
    glfw.set_cursor_pos_callback(window, motionEvent)

    # マウスホイールを処理する関数を登録
    # Register a callback function for mouse wheel
    glfw.set_scroll_callback(window, wheelEvent)

    # ユーザ指定の初期化
    # User-specified initialization
    initializeGL()

    # メインループ
    # Main loop
    while glfw.window_should_close(window) == glfw.FALSE:
        # 描画 / Draw
        paintGL()

        # 描画用バッファの切り替え
        # Swap drawing target buffers
        glfw.swap_buffers(window)
        glfw.poll_events()

    # 後処理 / Postprocess
    glfw.destroy_window(window)
    glfw.terminate()


if __name__ == "__main__":
    main()
