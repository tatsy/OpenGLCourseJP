*********************************
第4回 座標変換 |source_code|
*********************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/004_coordinate_transformation/004_coordinate_transformation.cpp

.. raw:: html

  <p><span style="color: #ff0000;">
    注) この回は、全ての回の中で最も重要な回です。非常に難しい部分ですので、めげずに何度も復習しましょう。
  </span></p>


二次元の座標変換
-------------------------

ここまでの回では、二次元座標上に三角形を描画していました。「 :doc:`../index/002` 」でご紹介したとおり、
画面上の座標系は :math:`(-1, 1) \times (-1, 1)` のようになっています。

ですが、実用的な場面では、当然、この座標系を変更したい場面というのが出てきます。
それでは、この座標系は変換できるのでしょうか？

結論から言えば、この座標系を変換することはできません。
ですが、描画する物体のサイズを変更することで擬似的に画面の座標系を変換することはできます。

例えば、今、画面の座標系を :math:`(-2, 2) \times (-2, 2)` のようにしたいとします。
とすれば、描画する全ての物体のサイズを2分の1にしておけば、これは :math:`(-1, 1) \times (-1, 1)`
の画面上にあたかも :math:`(-2, 2) \times (-2, 2)` の座標系を持つ画面上に物体を描画することができます。

それでは早速試してみましょう。

物体のサイズを変更する関数は ``glScalef`` です。この関数は引数を3つ取り、それぞれ
x軸、y軸、z軸の方向の拡大縮小比を表します。今回はx軸方向とy軸方向を2分の1に縮小したいので、

.. code-block:: cpp

  glScalef(0.5f, 0.5f, 1.0f);

のように書けばOKです。これをどこに書くのか、という部分についてですが、ここには少し工夫が必要です。

上記の ``glScalef`` はOpenGLの内部で管理されている座標変換行列に対して、
スケールを変更する行列を掛け算する、という処理をします。

具体的にはOpenGLの内部に保存されている座標変換行列は4×4の行列になっていて、
``glScalef(sx, sy, sz)`` のように呼び出したとすると、

.. math::

  \begin{pmatrix}
    sx & 0 & 0 & 0 \\
    0 & sy & 0 & 0 \\
    0 & 0 & sz & 0 \\
    0 & 0 & 0 & 1 \\
  \end{pmatrix}

という行列を掛け算することになります。

この行列をOpenGLの管理する行列に掛け算するには以下のように記述する必要があります。

.. code-block:: cpp
  :linenos:

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(0.5f, 0.5f, 1.0f);

これを使って、前回作成した描画関数を次のように書き換えます。

.. code-block:: cpp
  :linenos:

  // OpenGLの描画関数
  void paintGL() {
      // 背景色の描画
      glClear(GL_COLOR_BUFFER_BIT);

      // 座標変換
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glScalef(0.5f, 0.5f, 1.0f);

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

コードを書き換えて実行すると、左側の図のような画像が出てるはずです。
右側に比較用の元画像を表示してあります。

.. csv-table::
   :widths: 5, 5

   |after_scale|, |before_scale|
   **座標変換後**, 座標変換前


.. |after_scale| image:: ./figures/after_scale.jpg
  :width: 250px

.. |before_scale| image:: ./figures/before_scale.jpg
  :width: 250px

ソースコードでは、特に頂点の座標を変換していませんが、実際に表示される三角形は縮小されており、
擬似的に座標系が :math:`(-2, 2) \times (-2, 2)` になっていることが分かります。

練習1
^^^^^^^^^^^^

``glTranslatef`` 関数は頂点を平行移動させる関数です。この関数を使って、画面の擬似的な座標系を
:math:`(0, 4) \times (0, 4)` にしてみてください。

----

OpenGLが管理する座標変換行列
-------------------------------------

先ほど、OpenGLは座標変換のための行列を管理しており ``glScalef`` はその行列に対して、
拡大縮小の行列を掛ける関数であると説明しました。

実は、OpenGLが管理している行列には射影変換行列とモデルビュー変換行列の2種類があり、
上記のコードではモデルビュー変換行列に対して掛け算をしています。

その掛け算をする対象となる行列を指定する関数が、先ほど使った ``glMatrixMode`` 関数です。
``glMatrixMode`` 関数には ``GL_MODELVIEW`` と ``GL_PERSPECTIVE`` のどちらかを
指定することができて、前者がモデルビュー変換行列を後者が射影変換行列を指定するための定数です
(実際にはあと2種類 ``GL_TEXTURE`` と ``GL_COLOR`` が指定できますが、通常あまり使いません)。

モデルビュー変換行列の役割
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



射影変換行列の役割
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


ここまで説明をしておいて、申し訳ないのですが、実はこれらの行列は利便性の観点から
使い分けができるようになっているだけで、実際にはどちらか一方の行列だけを変換しても大丈夫です。

しかしながら、これらの行列の種類を理解することが、このあとに説明する三次元物体の描画において非常に重要になります。


三次元物体表示のための座標系
----------------------------

ワールド座標系
^^^^^^^^^^^^^^^^^^^^^^

カメラ座標系
^^^^^^^^^^^^^^^^^^^^^^

スクリーン座標系
^^^^^^^^^^^^^^^^^^^^^^


立方体の表示
----------------------------

.. code-block:: cpp
  :linenos:

  // 立方体の頂点位置
  static const float positions[8][3] = {
      { -1.0f, -1.0f, -1.0f },
      {  1.0f, -1.0f, -1.0f },
      { -1.0f,  1.0f, -1.0f },
      { -1.0f, -1.0f,  1.0f },
      {  1.0f,  1.0f, -1.0f },
      { -1.0f,  1.0f,  1.0f },
      {  1.0f, -1.0f,  1.0f },
      {  1.0f,  1.0f,  1.0f }
  };

  // 立方体の面の色
  static const float colors[6][3] = {
      { 1.0f, 0.0f, 0.0f },  // 赤
      { 0.0f, 1.0f, 0.0f },  // 緑
      { 0.0f, 0.0f, 1.0f },  // 青
      { 1.0f, 1.0f, 0.0f },  // イエロー
      { 0.0f, 1.0f, 1.0f },  // シアン
      { 1.0f, 0.0f, 1.0f },  // マゼンタ
  };

  // 立方体の面となる三角形の定義
  static const unsigned int indices[12][3] = {
      { 1, 6, 7 }, { 1, 7, 4 },
      { 2, 5, 7 }, { 2, 7, 4 },
      { 3, 5, 7 }, { 3, 7, 6 },
      { 0, 1, 4 }, { 0, 4, 2 },
      { 0, 1, 6 }, { 0, 6, 3 },
      { 0, 2, 5 }, { 0, 5, 3 }
  };


.. code-block:: cpp
  :linenos:

  // OpenGLの描画関数
  void paintGL() {
      // 背景色の描画
      glClear(GL_COLOR_BUFFER_BIT);

      // 座標の変換
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
                0.0f, 0.0f, 0.0f,     // 見ている先
                0.0f, 1.0f, 0.0f);    // 視界の上方向

      // 立方体の描画
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


.. image:: ./figures/strange_cube.jpg
  :width: 250px
