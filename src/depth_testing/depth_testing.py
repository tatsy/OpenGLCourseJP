import glfw
from OpenGL.GL import *
from OpenGL.GLU import *

WIN_WIDTH = 500  # ウィンドウの幅 / Window width
WIN_HEIGHT = 500  # ウィンドウの高さ / Window height
WIN_TITLE = "OpenGL Course"  # ウィンドウのタイトル / Window title

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
indices = [
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


# OpenGLの描画関数
# OpenGL drawing function
def paintGL():
    # 背景色の描画
    # Fill background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # ビューポート変換の指定
    # Viewport transform
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT)

    # 投影変換行列
    # Specify projection matrix
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    # 平行投影 / Orthographic projection
    # glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f)
    # 透視投影 / Perspective projection with view frustum
    # glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f)
    # 透視投影 / Perspective projection
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

    # 立方体の描画
    # Draw a cube
    glBegin(GL_TRIANGLES)
    for face in range(6):
        # 面の色
        # Face color
        glColor3fv(colors[face])

        # 1つの面(四角形)は2つの三角形から成る
        # A square face of a cube consists of two triangles
        for i in range(3):
            glVertex3fv(positions[indices[face * 2 + 0][i]])

        for i in range(3):
            glVertex3fv(positions[indices[face * 2 + 1][i]])

    glEnd()


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

    # ユーザ指定の初期化
    # User-specified initialization
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
