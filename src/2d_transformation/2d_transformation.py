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

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title


# ユーザ定義のOpenGLの初期化
# User-defined OpenGL initialization
def initializeGL():
    # 背景色の設定
    # Specify background color
    glClearColor(0.0, 0.0, 0.0, 1.0)


# ユーザ定義のOpenGL描画
# User-defined OpenGL drawing
def paintGL():
    # 背景色の描画
    # Background color
    glClear(GL_COLOR_BUFFER_BIT)

    # 座標変換の指定
    # Specify coordinate transformation
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    # glScalef(0.5, 0.5, 1.0);  # 拡大・縮小 / Scaling
    # glTranslatef(0.25, 0.25, 0.0);  # 平行移動 / Translation
    glRotatef(45.0, 0.0, 0.0, 1.0)  # 回転 / Rotation

    # 三角形の描画
    # Draw a triangle
    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.0, 0.0)  # 赤 / Red
    glVertex2f(-0.5, -0.5)
    glColor3f(0.0, 1.0, 0.0)  # 緑 / Green
    glVertex2f(-0.5, 0.5)
    glColor3f(0.0, 0.0, 1.0)  # 青 / Blue
    glVertex2f(0.5, -0.5)
    glEnd()


def main():
    # OpenGLを初期化する
    # Initialize OpenGL
    if glfw.init() == glfw.FALSE:
        raise Exception("Initialization failed")

    # Windowsの作成
    # Create a window
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, None, None)
    if window is None:
        glfw.terminate()
        raise Exception("Window creation failed!")

    # OpenGLの描画対象にwindowを指定
    # Specify window as an OpenGL context
    glfw.make_context_current(window)

    initializeGL()

    # メインループ
    # Main loop
    while glfw.window_should_close(window) == glfw.FALSE:
        # 描画
        # Draw
        paintGL()

        # 描画用バッファの切り替え
        # Swap drawing target buffers
        glfw.swap_buffers(window)
        glfw.poll_events()

    # 後処理
    # Postprocess
    glfw.destroy_window(window)
    glfw.terminate()


if __name__ == "__main__":
    main()
