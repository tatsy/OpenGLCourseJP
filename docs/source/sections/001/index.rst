***********************
第1回 ウィンドウを開く
***********************

|source_code|

.. |source_code| raw:: html

  <a href="https://github.com/tatsy/OpenGLCourseJP/blob/master/src/001_open_window/001_open_window.cpp" target="_blank">ソースコード</a>


OpenGLの初期化
-------------------

GLFWを通してOpenGLを使用するには初期化処理が必要です。初期化には以下のソースコードが対応します。

.. code-block:: cpp
  :linenos:

  // OpenGLを初期化する
  if (glfwInit() == GL_FALSE) {
      fprintf(stderr, "Initialization failed!\n");
      return 1;
  }

上記のコードの中で ``glfwInit()`` メソッドが初期化処理をします。もし初期化が成功すれば
``GL_TRUE`` を、失敗すれば ``GL_FALSE`` を返します。

ここでは初期化が失敗し ``GL_FALSE`` が返されたときにエラーメッセージを表示して、プログラムを停止しています。
少し難しくなりますが ``fprintf`` 関数の第1引数に ``stderr`` と書くとエラーメッセージとして、
第2引数に渡された文字列である *Initialization failed* が出力されます。

ウィンドウの作成
--------------------

ウィンドウを作成するには ``glfwCreateWindow`` 関数を使います。この関数は、必ず上記の初期化の後で呼び出してください。
対応するソースコードは以下のようになります。

.. code-block:: cpp
  :linenos:

  // Windowの作成
  GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                        NULL, NULL);
  if (window == NULL) {
      fprintf(stderr, "Window creation failed!");
      glfwTerminate();
      return 1;
  }
