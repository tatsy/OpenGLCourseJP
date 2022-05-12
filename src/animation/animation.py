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
import glfw
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

fps = 30.0  # FPS
theta = 0.0

# 立方体の頂点位置
# Vertex positions of a cube
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
# yapf: enable

# 立方体の面の色
# Face colors of a cube
# yapf: disable
colors = [
    [ 1.0, 0.0, 0.0 ],  # 赤
    [ 0.0, 1.0, 0.0 ],  # 緑
    [ 0.0, 0.0, 1.0 ],  # 青
    [ 1.0, 1.0, 0.0 ],  # イエロー
    [ 0.0, 1.0, 1.0 ],  # シアン
    [ 1.0, 0.0, 1.0 ],  # マゼンタ
]
# yapf: enable

# 立方体の面となる三角形の定義
# Vertex indices to form triangles of a cube
# yapf: disable
faces = [
    [ 7, 4, 1 ], [ 7, 1, 6 ],
    [ 2, 4, 7 ], [ 2, 7, 5 ],
    [ 5, 7, 6 ], [ 5, 6, 3 ],
    [ 4, 2, 0 ], [ 4, 0, 1 ],
    [ 3, 6, 1 ], [ 3, 1, 0 ],
    [ 2, 5, 3 ], [ 2, 3, 0 ],
]
# yapf: enable


# OpenGLの初期化関数
# OpenGL initialization
def initializeGL():
    # 背景色の設定 (黒)
    # Background color (black)
    glClearColor(0.0, 0.0, 0.0, 1.0)

    # 深度テストの有効化
    # Enable depth testing
    glEnable(GL_DEPTH_TEST)


# キューブの描画
# Draw a cube
def drawCube():
    glBegin(GL_TRIANGLES)
    for face in range(6):
        glColor3fv(colors[face])
        for i in range(3):
            glVertex3fv(positions[faces[face * 2 + 0][i]])

        for i in range(3):
            glVertex3fv(positions[faces[face * 2 + 1][i]])
    glEnd()


# OpenGLの描画関数
# OpenGL drawing function
def paintGL():
    # 背景色の描画
    # Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 投影変換行列
    # Specify projection matrix
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    # 透視投影
    # Perspective projection
    gluPerspective(45.0, WIN_WIDTH / WIN_HEIGHT, 1.0, 10.0)

    # モデルビュー変換行列
    # Specify model-view matrix
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    # yapf: disable
    gluLookAt(3.0, 4.0, 5.0,  # 視点の位置
              0.0, 0.0, 0.0,  # 見ている先
              0.0, 1.0, 0.0)  # 視界の上方向
    # yapf: enable

    # 1つ目のキューブ
    # First cube
    glPushMatrix()
    glTranslatef(-1.0, 0.0, 0.0)
    glRotatef(theta, 0.0, 1.0, 0.0)
    glRotatef(theta * 0.5, 1.0, 0.0, 0.0)
    glScalef(0.5, 0.5, 0.5)

    drawCube()
    glPopMatrix()

    # 2つ目のキューブ
    # Second cube
    glPushMatrix()
    glTranslatef(1.0, 0.0, 0.0)
    glRotated(2.0 * theta, 0.0, 1.0, 0.0)
    glRotatef(theta, 1.0, 0.0, 0.0)
    glScalef(0.5, 0.5, 0.5)

    drawCube()
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

    # ウィンドウのリサイズを扱う関数の登録
    # Register a callback function for window resizing
    glfw.set_window_size_callback(window, resizeGL)

    # ユーザ指定の初期化
    # User-specified initialization
    initializeGL()

    # メインループ
    # Main loop
    prevTime = glfw.get_time()
    while glfw.window_should_close(window) == glfw.FALSE:
        # 現在の時刻を取得
        # Acquire a current time
        currentTime = glfw.get_time()

        # 経過時間が 1 / FPS 以上なら描画する
        # Perform drawing if elapsed time is more than one over FPS
        if currentTime - prevTime > 1.0 / fps:
            # タイトルにFPSを表示
            # Show FPS in title
            realFps = 1.0 / (currentTime - prevTime)
            winTitle = "%s (%.3f)" % (WIN_TITLE, realFps)
            glfw.set_window_title(window, winTitle)

            # 描画
            # Draw
            paintGL()

            # アニメーション
            # Animation
            animate()

            # 描画用バッファの切り替え
            # Swap drawing target buffers
            glfw.swap_buffers(window)
            glfw.poll_events()

            # 前回描画時間の更新
            # Update latest time of drawing
            prevTime = currentTime

    # 後処理
    # Postprocess
    glfw.destroy_window(window)
    glfw.terminate()


if __name__ == "__main__":
    main()
