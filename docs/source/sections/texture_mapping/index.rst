********************************************
テクスチャ・マッピング |source_code|
********************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/007_texture_mapping

境界の折り返し設定
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

UV座標が[0, 1]の範囲外にあるときの挙動を決定する

GL_CLAMP
""""""""""""""""""""""""""""""

テクスチャ境界上の最も近い画素の色とユーザ設定の境界色の平均を取る

.. code-block:: cpp
  :linenos:

  float borderColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

.. image:: ./figures/clamp.png


GL_CLAMP_TO_EDGE
""""""""""""""""""""""""""""""

テクスチャ境界上の最も近い画素の色を使う

.. code-block:: cpp
  :linenos:

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

.. image:: ./figures/clamp_to_edge.png


GL_CLAMP_TO_BORDER
""""""""""""""""""""""""""""""

ユーザ設定の境界色を常に使う

.. code-block:: cpp
  :linenos:

  float borderColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


.. image:: ./figures/clamp_to_border.png


GL_REPEAT
""""""""""""""""""""""""""""""

テクスチャを単純に繰り返す

.. code-block:: cpp
  :linenos:

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

.. image:: ./figures/repeat.png


GL_MIRRORED_REPEAT
""""""""""""""""""""""""""""""

テクスチャを反転しながら繰り返す

.. code-block:: cpp
  :linenos:

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

.. image:: ./figures/mirrored_repeat.png
