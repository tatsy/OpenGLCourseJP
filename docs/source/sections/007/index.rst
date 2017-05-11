
************************************
第7回 アニメーション |source_code|
************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/007_animation/main.cpp


アニメーションその1
-----------------------

:doc:`code_0071`

.. code-block:: cpp
  :linenos:

  // OpenGLの描画関数
  void paintGL() {
      // 背景色と深度値のクリア
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // ビューポート変換の指定
      glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

      // 座標の変換
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
                0.0f, 0.0f, 0.0f,     // 見ている先
                0.0f, 1.0f, 0.0f);    // 視界の上方向

      glRotatef(theta, 0.0f, 1.0f, 0.0f);

      // キューブの描画
      drawCube();
  }


.. raw:: html

  <iframe src="https://player.vimeo.com/video/201603287" width="320" height="336" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe></p>


アニメーション その2
-----------------------------

:doc:`code_0072`

.. code-block:: cpp
  :linenos:

  // キューブの描画
  void drawCube() {
      glBegin(GL_TRIANGLES);
      for (int face = 0; face < 6; face++) {
          glColor3fv(colors[face]);
          for (int i = 0; i < 3; i++) {
              glVertex3fv(positions[indices[face * 2 + 0][i]]);
          }

          for (int i = 0; i < 3; i++) {
              glVertex3fv(positions[indices[face * 2 + 1][i]]);
          }
      }
      glEnd();
  }


.. code-block:: cpp
  :linenos:

  // OpenGLの描画関数
  void paintGL() {
      // 背景色と深度値のクリア
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // ビューポート変換の指定
      glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

      // 座標の変換
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
                0.0f, 0.0f, 0.0f,     // 見ている先
                0.0f, 1.0f, 0.0f);    // 視界の上方向

      // 1つ目のキューブ
      glPushMatrix();
      glTranslatef(1.0f, 0.0f, 0.0f);
      glRotatef(theta, 0.0f, 1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);

      drawCube();
      glPopMatrix();

      // 2つ目のキューブ
      glPushMatrix();
      glTranslatef(-1.0f, 0.0f, 0.0f);
      glRotated(2.0f * theta, 0.0f, 1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);

      drawCube();
      glPopMatrix();
  }
