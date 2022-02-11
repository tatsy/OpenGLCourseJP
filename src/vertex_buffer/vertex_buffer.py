# MacOS Big Sur以降で動かすための設定
# Special settings for working on MacOS Big Sur or later
import os
import ctypes.util

try:
    uname = os.uname()
    if uname.sysname == "Darwin" and uname.release >= "20.":
        _find_library = ctypes.util.find_library

        def find_library(name):
            if name in ["OpenGL"]:
                return "/System/Library/Frameworks/{0}.framework/{0}".format(name)
            return _find_library(name)

        ctypes.util.find_library = find_library
except:
    pass

# 必要なパッケージのインポート
# Import required packages
import glfw
import numpy as np
import ctypes
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

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
vertexBufferId = 0
indexBufferId = 0

# 立方体の回転角度
# Rotation angle for animating a cube
theta = 0


# ユーザ定義のOpenGLの初期化
# User-define OpenGL initialization
def initializeGL():
    global vertexBufferId, indexBufferId

    # 深度テストの有効化
    # Enable depth testing
    glEnable(GL_DEPTH_TEST)

    # 背景色の設定 (黒)
    # Background color (black)
    glClearColor(0.0, 0.0, 0.0, 1.0)

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

    # 頂点バッファオブジェクトの作成
    # Create vertex buffer object
    vertexBufferId = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId)
    glBufferData(GL_ARRAY_BUFFER, len(vertices.tobytes()), vertices.tobytes(), GL_STATIC_DRAW)

    # 頂点番号バッファオブジェクトの作成
    # Create index buffer object
    indexBufferId = glGenBuffers(1)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, len(indices.tobytes()), indices.tobytes(), GL_STATIC_DRAW)


def paintGL():
    # 背景色の描画
    # Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 座標の変換
    # Coordinate transformation
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, WIN_WIDTH / WIN_HEIGHT, 0.1, 1000.0)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    # yapf: disable
    gluLookAt(3.0, 4.0, 5.0,   # 視点の位置 / Eye position
              0.0, 0.0, 0.0,   # 見ている先 / Looking position
              0.0, 1.0, 0.0)   # 視界の上方向 / Upward direction
    # yapf: enable

    # 回転行列の設定
    # Setup rotation matrix
    glPushMatrix()
    glRotatef(theta, 0.0, 1.0, 0.0)

    # 頂点バッファの有効化
    # Enable vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId)

    # 頂点情報の詳細を設定
    # Setup details for vertex array
    glEnableClientState(GL_VERTEX_ARRAY)
    glVertexPointer(3, GL_FLOAT, 4 * 3 * 2, ctypes.c_void_p(0))

    glEnableClientState(GL_COLOR_ARRAY)
    glColorPointer(3, GL_FLOAT, 4 * 3 * 2, ctypes.c_void_p(4 * 3))

    # 三角形の描画
    # Draw triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, ctypes.c_void_p(0))

    # 頂点バッファの無効化
    # Disable vertex buffer object
    glDisableClientState(GL_VERTEX_ARRAY)
    glDisableClientState(GL_COLOR_ARRAY)

    # 回転行列の破棄
    # Dispose of rotation matrix
    glPopMatrix()


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
