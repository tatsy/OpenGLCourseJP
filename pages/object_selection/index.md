************************************************
オブジェクトの選択 |source_code|
************************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/object_selection


安直なやり方
"""""""""""""""""""""""""""""

.. code-block:: cpp
  :linenos:

  // バッファ全体を読み込んで, そのクリック位置のラベルを見る
  unsigned char *bytes = new unsigned char[WIN_WIDTH * WIN_HEIGHT * 4];
  glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
  printf("Mouse position: %d %d\n", cx, cy);
  printf("Select object %d\n", bytes[((WIN_HEIGHT - cy - 1) * WIN_WIDTH + cx) * 4]);


より良いやり方
"""""""""""""""""""""""""""""

.. code-block:: cpp
  :linenos:

  // クリック位置の情報だけを読み込んで, そのラベルを見る
  char byte[4];
  glReadPixels(cx, WIN_HEIGHT - cy - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &byte);
  printf("Mouse position: %d %d\n", cx, cy);
  printf("Select object %d\n", byte[0]);
