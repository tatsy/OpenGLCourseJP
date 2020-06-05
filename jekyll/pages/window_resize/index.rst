******************************************************
ウィンドウサイズの変更 |source_code|
******************************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/005_window_resizing/main.cpp

単純な設定
------------------

.. code-block:: cpp
  :linenos:

  void resizeGL(GLFWwindow *window, int width, int height) {
      // ユーザ管理のウィンドウサイズを変更
      WIN_WIDTH = width;
      WIN_HEIGHT = height;

      // GLFW管理のウィンドウサイズを変更
      glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

      // ビューポート変換の更新
      glViewport(0, 0, renderBufferWidth, renderBufferHeight);
  }

より高度な設定
------------------

.. code-block:: cpp
  :linenos:

  void resizeGL(GLFWwindow *window, int width, int height) {
      // ユーザ管理のウィンドウサイズを変更
      WIN_WIDTH = width;
      WIN_HEIGHT = height;

      // GLFW管理のウィンドウサイズを変更
      glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

      // 実際のウィンドウサイズ (ピクセル数) を取得
      int renderBufferWidth, renderBufferHeight;
      glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

      // ビューポート変換の更新
      glViewport(0, 0, renderBufferWidth, renderBufferHeight);
  }
