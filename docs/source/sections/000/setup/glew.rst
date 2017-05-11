***************************************
GLEWとGLMの設定 (第10回以降に必要)
***************************************

第10回以降のソースコードを動かすためには, 最近のOpenGLの機能を扱うためのライブラリが必要です。
このコースではGLEW (OpenGL Extension Wrangler Library) を使ってプログラムを作ります。

合わせて、ベクトルや行列の計算を簡単に扱うためのライブラリとしてGLM (OpenGL Mathematics) も導入しておきます。

----

Windowsの場合
-------------------------

GLEWのインストール
^^^^^^^^^^^^^^^^^^^^^^^^

GLEWの公式ページからコンパイル済みのライブラリをダウンロードします。

| **GLEWのダウンロードページ**
| http://glew.sourceforge.net/
|

.. image:: ../figures/glew_download_page.jpg

上記のウェブページを開いて、中央にある「Binaries」という文字の右側にある
「Windows 32-bit and 64-bit」というリンクから、ZIPファイルをダウンロードします。
ダウンロードしたファイルは「glew-2.0.0-win32.zip」という名前になっているはずです。

ダウンロードが完了したら、ZIPファイルを解凍し、GLFWの時と同様に適当なディレクトリに配置します。
この講義では、 **C:\\Libraries\\opengl** のようなディレクトリを作成して、
その中に置くことを推奨しています。

GLMのインストール
^^^^^^^^^^^^^^^^^^^^^^^^

GLMも公式のウェブページからライブラリをダウンロードします。

| **GLMのダウンロードページ**
| http://glm.g-truc.net
|

.. image:: ../figures/glm_top_page.jpg

上記のウェブページの左側にある「Downloads」というリンクをクリックします。
するとダウンロードページに移動するので、その中から最新版である0.9.8.4をダウンロードします (2017年5月11日現在)。
ダウンロードしたファイルは「glm-0.9.8.4.zip」という名前になっているはずです。

.. image:: ../figures/glm_download_page.jpg

ダウンロードが完了したら、上記と同様にZIPファイルを解凍して適当なディレクトリに配置します。


プロパティシートの更新
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

初回の準備の際に作成したプロパティシートを更新して、GLEWおよびGLMを使用できるようにします。

設定方法に関しては、:doc:`glfw` を参照してください。設定項目は以下の通りです。

VC++ ディレクトリ
""""""""""""""""""""""""""""""""""""

| **インクルードディレクトリ**
| C:\\Libraries\\opengl\\glew-2.0.0\\include  (GLEW)
| C:\\Libraries\\opengl\\glm-0.9.8.4  (GLM)
|
| **ライブラリディレクトリ**
| C:\\Libraries\\opengl\\glew-2.0.0\\lib\\Release\\Win32 (GLEW, 32bitの場合)
| C:\\Libraries\\opengl\\glew-2.0.0\\lib\\Release\\x64 (GLEW, 64bitの場合)
|

リンカー
""""""""""""""""""""""""""""""""""""

| **追加の依存ファイル**
| glew32.lib (GLEW, 32bit, 64bitとも同じ名前)
|


サンプルプログラムの実行
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

サンプルプログラムは「第12回シェーダの利用」のソースコードを使用してください。

https://github.com/tatsy/OpenGLCourseJP/blob/master/src/012_hello_shader

なお、このソースコードを実行する際にはシェーダを書いておく必要がありますので、注意してください。
(シェーダに関しては、上記のリンクをクリックしたのち **shader** フォルダ内を参照してください)。

上手く実行できると、色付きの立方体が描画されるはずです。

.. image:: ../figures/shader_cube.jpg
  :width: 300px


----


Macの場合
-------------------------

Macを使用する場合には、GLEW、GLMともにGitを経由してダウンロードし、GNU Makeあるいは
CMakeを使用してビルド、インストールを行います。

GLEWのインストール
^^^^^^^^^^^^^^^^^^^^^^^^

`GLEWのGitHub <https://github.com/nigels-com/glew.git>`_ からソースコードを
ダウンロードして、ビルドとインストールを行います。ターミナルを開いて、次のコマンドを実行してください。

.. code-block:: shell
  :linenos:

  git clone https://github.com/nigels-com/glew.git
  cd glew
  make
  sudo make install

これだけでインストールは完了です。


GLMのインストール
^^^^^^^^^^^^^^^^^^^^^^^^

`GLMのGitHub <https://github.com/g-truc/glm.git>`_ からソースコードを
ダウンロードして、ビルドとインストールを行います。GLEWの時と同様に、
次のコマンドをターミナルで実行してください。

.. code-block:: shell
  :linenos:

  git clone https://github.com/g-truc/glm.git
  cd glm
  mkdir build && cd build
  cmake ..
  make
  sudo make install

これだけでインストールは完了です。

Xcodeの設定
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Xcodeで設定が必要な項目は1箇所で、GLEWのライブラリをリンカフラグに設定するだけです。

Xcodeの画面左側にあるプロジェクト名 (今回の場合は「OpenGLCourse」) をクリックすると、
設定画面が現れるので、その設定画面の「Build Settings」を開きます。

.. image:: ../figures/project_setting_xcode.jpg

この画面中段くらいにある「Linking」という項目の中から「Other Linker Flag」という項目を選び、
そこに「-lglew」という文字列を追加します。

これでXcode側の設定は完了です。

サンプルプログラムの実行
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

サンプルプログラムは「第12回シェーダの利用」のソースコードを使用してください。

https://github.com/tatsy/OpenGLCourseJP/blob/master/src/012_hello_shader

なお、このソースコードを実行する際にはシェーダを書いておく必要がありますので、注意してください。
(シェーダに関しては、上記のリンクをクリックしたのち **shader** フォルダ内を参照してください)。

上手く実行できると、色付きの立方体が描画されるはずです。

.. image:: ../figures/shader_cube.jpg
  :width: 300px
