************************************
シェーダの利用 |source_code|
************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/hello_shader


最も単純な頂点シェーダ
""""""""""""""""""""""""""""""""""""

.. code-block:: cpp
  :linenos:

  #version 330

  // CPUから受け取る頂点位置 (頂点バッファの中の1つ)
  layout(location = 0) in vec3 in_position;

  // どの頂点にも共通に使う変数
  uniform mat4 u_mvpMat;

  void main(void) {
      // 座標変換により、スクリーン上の位置を決定 (-1から1の範囲が描画される)
      gl_Position = u_mvpMat * vec4(in_position, 1.0);
  }

最も単純なフラグメントシェーダ
""""""""""""""""""""""""""""""""""""

.. code-block:: cpp
  :linenos:

  #version 330

  // out識別子のついた1つ目の変数がディスプレイに表示される
  out vec4 out_color;

  void main(void) {
      // 単に黄色い色を出力する
      out_color = vec4(1.0, 1.0, 0.0, 1.0);
  }
