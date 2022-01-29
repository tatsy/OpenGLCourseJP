import glfw
from OpenGL.GL import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title


# ユーザ定義のOpenGLの初期化
# User-define OpenGL initialization
def initializeGL():
    # 背景色の設定
    # Specify background color
    glClearColor(1.0, 0.0, 0.0, 1.0)


# ユーザ定義のOpenGL描画
# User-defined OpenGL drawing
def paintGL():
    # 背景色の描画
    # Background color
    glClear(GL_COLOR_BUFFER_BIT)


def main():
    # OpenGLを初期化する
    # Initialize OpenGL
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

    # 初期化
    # Initialization
    initializeGL()

    # メインループ
    # Main loop
    while glfw.window_should_close(window) == glfw.FALSE:
        # 描画
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
