*****************************************************
第3回 メソッドによるコードの整理 |source_code|
*****************************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/003_use_of_methods/003_use_of_methods.cpp


前回までの内容で無事、三角形が表示できるようになりましたが、ここからプログラムを書き足していく上で
大切になってくるのがコードの整理です。より専門的な言葉では「リファクタリング」と呼びます。

今までのコードは ``main`` 関数の中に全ての処理が書かれていましたが、この処理の一部を
別の関数 (=メソッド) として、外側に定義し、 ``main`` 関数ではそれらの関数を呼び出して処理を行います。

今回、外側に分ける処理は **OpenGLの初期化** と **描画処理** です。

----

コードの整理
-----------------

とは言うものの、実際にすることはとても単純です。以下の整理前のコードと整理後のコードを比較してみてください。

**整理前**

.. code-block:: cpp
  :linenos:

  #include <cstdio>
  #include <GLFW/glfw3.h>

  static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
  static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
  static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

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

      // 背景色の設定 (黒)
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

      // メインループ
      while (glfwWindowShouldClose(window) == GL_FALSE) {
          // 背景色の描画
          glClear(GL_COLOR_BUFFER_BIT);

          // 三角形の描画
          glBegin(GL_TRIANGLES);
          glColor3f(1.0f, 0.0f, 0.0f);    // 赤
          glVertex2f(-0.5f, -0.5f);
          glColor3f(0.0f, 1.0f, 0.0f);    // 緑
          glVertex2f(-0.5f,  0.5f);
          glColor3f(0.0f, 0.0f, 1.0f);    // 青
          glVertex2f( 0.5f, -0.5f);
          glEnd();

          // 描画用バッファの切り替え
          glfwSwapBuffers(window);
          glfwPollEvents();
      }
  }



**整理後**

.. code-block:: cpp
  :linenos:

  #include <cstdio>
  #include <GLFW/glfw3.h>

  static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
  static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
  static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

  // OpenGLの初期化関数
  void initializeGL() {
      // 背景色の設定 (黒)
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  }

  // OpenGLの描画関数
  void paintGL() {
      // 背景色の描画
      glClear(GL_COLOR_BUFFER_BIT);

      // 三角形の描画
      glBegin(GL_TRIANGLES);
      glColor3f(1.0f, 0.0f, 0.0f);    // 赤
      glVertex2f(-0.5f, -0.5f);
      glColor3f(0.0f, 1.0f, 0.0f);    // 緑
      glVertex2f(-0.5f,  0.5f);
      glColor3f(0.0f, 0.0f, 1.0f);    // 青
      glVertex2f( 0.5f, -0.5f);
      glEnd();
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

      // OpenGLを初期化
      initializeGL();

      // メインループ
      while (glfwWindowShouldClose(window) == GL_FALSE) {
          // 描画
          paintGL();

          // 描画用バッファの切り替え
          glfwSwapBuffers(window);
          glfwPollEvents();
      }
  }

違いが確認できるでしょうか？

整理後のコードでは、新しく ``initializeGL`` という関数と ``paintGL`` という関数が
新しく作られて、それらの中に初期化の処理、描画の処理がそれぞれ移動されています。

それでは、今まで初期化と描画の処理が書かれていた場所はどうかというと、それらの場所は
関数の呼び出し1行だけに置き換わっています。

----

コードを整理することのメリット
-------------------------------------

それでは、上記のようにコードを整理することのメリットは何でしょうか？

最大のメリットは、やはりコードの見通しが良くなるという部分だと思います。
もちろん対応する部分の直前にコメントを入れて、その部分がどういう処理をする部分なのかを
明示するというのもコードを見やすくするための一つの手段ですが、
ある意味を持ったまとまりを、適切な名前を持った関数に置き換えれば、コードが短く簡潔になります。

またその処理がどういう処理なのかを確認するときも、ある役割を名前とするメソッドの中身を
見ることでその役割が何をしているのかを知ることができます。

このように考える単位を切り分けられるというのがコードを整理することのメリットだと思います。
