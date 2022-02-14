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
import glfw
import numpy as np
from PIL import Image
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

theta = 0.0

TEX_FILE = os.path.join(os.path.dirname(__file__), "data/checker.png")
textureId = 0

# yapf: disable
positions = [
    [ -1.0,  0.0, -1.0 ],
    [  1.0,  0.0, -1.0 ],
    [ -1.0,  0.0,  1.0 ],
    [  1.0,  0.0,  1.0 ],
]

texcoords = [
    [  0.0,  0.0 ],
    [  1.0,  0.0 ],
    [  0.0,  1.0 ],
    [  1.0,  1.0 ]
]

faces = [
    [ 0, 1, 3 ], [ 0, 3, 2 ]
]
# yapf: enable


# OpenGLの初期化関数
def initializeGL():
    global textureId

    # 背景色の設定 (黒)
    # Background color (black)
    glClearColor(0.0, 0.0, 0.0, 1.0)

    # 深度テストの有効化
    # Enable depth testing
    glEnable(GL_DEPTH_TEST)

    # テクスチャの有効化
    # Enable texture mapping
    glEnable(GL_TEXTURE_2D)

    # テクスチャの設定
    # Setup texture
    image = np.asarray(Image.open(TEX_FILE), dtype="uint8")
    texHeight, texWidth, _ = image.shape

    # テクスチャの生成と有効化
    # Generate and bind texture
    textureId = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, textureId)

    # 単純なテクスチャの転送
    # Simple texture transfer
    # glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.tobytes())

    # MIP mapを用いたテクスチャの転送
    # Texture transfer with MIP mapping
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, image.tobytes())

    # テクスチャの画素値参照方法の設定 (MIP mapなし)
    # Texture filtering operations (w/o MIP map)
    # glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    # glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)

    # テクスチャの画素値参照方法の設定 (MIP mapあり)
    # Texture filtering operation (with MIP map)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)

    # テクスチャ境界の折り返し設定
    # Texture wrapping (to process pixels outside border)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)

    # テクスチャの無効化
    # Disable texture binding
    glBindTexture(GL_TEXTURE_2D, 0)


# OpenGLの描画関数
# OpenGL drawing function
def paintGL():
    # 背景色と深度値のクリア
    # Clear background color and depth values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 投影変換行列
    # Projection matrix
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, WIN_WIDTH / WIN_HEIGHT, 1.0, 100.0)

    # モデルビュー行列
    # Model-view matrix
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    # yapf: disable
    gluLookAt(0.0, 3.0, 5.0,   # 視点の位置 / Eye position
              0.0, 0.0, 0.0,   # 見ている先 / Looking direction
              0.0, 1.0, 0.0)  # 視界の上方向 / Upward direction
    # yapf: enable

    # 前後に移動 / Move back and forth
    glTranslatef(0.0, 0.0, np.cos(np.pi * theta / 180.0) * 4.0 - 4.0)
    # Y軸中心にthetaだけ回転 / Rotate by theta around Y axis
    glRotatef(theta, 0.0, 1.0, 0.0)

    # 平面の描画
    # Draw a flat plane
    glBindTexture(GL_TEXTURE_2D, textureId)  # テクスチャの有効化 / Enable texture mapping
    glBegin(GL_TRIANGLES)
    for i in range(3):
        glTexCoord2fv(texcoords[faces[0][i]])
        glVertex3fv(positions[faces[0][i]])

    for i in range(3):
        glTexCoord2fv(texcoords[faces[1][i]])
        glVertex3fv(positions[faces[1][i]])

    glEnd()
    glBindTexture(GL_TEXTURE_2D, 0)  # テクスチャの無効化 / Disable texture mapping


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

    # OpenGLの描画対象にWindowを追加
    # Specify window as an OpenGL context
    glfw.make_context_current(window)

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
