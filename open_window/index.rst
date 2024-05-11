<!DOCTYPE html>
<html lang="en-us">

<head>
  <link href="http://gmpg.org/xfn/11" rel="profile">
  <meta http-equiv="content-type" content="text/html; charset=utf-8">

  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1">

  <title>
    
      ウィンドウを開く &middot; OpenGL Course (JP)
    
  </title>

  
  <link rel="canonical" href="https://tatsy.github.io/OpenGLCourseJP/open_window/">
  

  <link rel="stylesheet" href="https://tatsy.github.io/OpenGLCourseJP/public/css/poole.css">
  <link rel="stylesheet" href="https://tatsy.github.io/OpenGLCourseJP/public/css/syntax.css">
  <link rel="stylesheet" href="https://tatsy.github.io/OpenGLCourseJP/public/css/lanyon.css">
  <link rel="stylesheet" href="https://tatsy.github.io/OpenGLCourseJP/public/css/main.css">
  <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=PT+Serif:400,400italic,700%7CPT+Sans:400">

  <link rel="apple-touch-icon-precomposed" sizes="144x144" href="https://tatsy.github.io/OpenGLCourseJP/public/apple-touch-icon-precomposed.png">
  <link rel="shortcut icon" href="https://tatsy.github.io/OpenGLCourseJP/public/favicon.ico">

  <script type="text/javascript" src="/OpenGLCourseJP/public/js/jquery.min.js"></script>
  <script type="text/javascript" src="/OpenGLCourseJP/public/js/lightbox.min.js"></script>

  <link rel="alternate" type="application/rss+xml" title="RSS" href="https://tatsy.github.io/OpenGLCourseJP/atom.xml">

  
</head>


<body>

  <!-- Target for toggling the sidebar `.sidebar-checkbox` is for regular
     styles, `#sidebar-checkbox` for behavior. -->
<input type="checkbox" class="sidebar-checkbox" id="sidebar-checkbox">

<!-- Toggleable sidebar -->
<div class="sidebar" id="sidebar">
  <div class="sidebar-item">
    <p>Welcome to OpenGL programming course!</p>
  </div>

  <nav class="sidebar-nav">
    <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/">Home</a>

    

    
    
      
        
          <a class="sidebar-nav-item active" href="https://tatsy.github.io/OpenGLCourseJP/open_window/">ウィンドウを開く</a>
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/depth_testing/">深度テスト</a>
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/coordinate_transform/">座標変換</a>
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/hello_shader/">シェーダの利用</a>
        
      
    
      
        
      
    
      
        
      
    
      
    
      
        
      
    
      
        
      
    
      
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/hello_triangle/">三角形を描く</a>
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/setup/">環境設定</a>
        
      
    
      
        
          <a class="sidebar-nav-item" href="https://tatsy.github.io/OpenGLCourseJP/window_resize/">ウィンドウサイズの変更</a>
        
      
    
      
        
          <a class="sidebar-nav-item active" href="https://tatsy.github.io/OpenGLCourseJP/open_window/">ウィンドウを開く</a>
        
      
    
      
    
      
        
      
    
      
    
      
    
      
    

    <a class="sidebar-nav-item" href="https://github.com/tatsy/OpenGLCourseJP/archive/v1.0.0.zip">Download</a>
    <a class="sidebar-nav-item" href="https://github.com/tatsy/OpenGLCourseJP">GitHub project</a>
    <span class="sidebar-nav-item">Currently v1.0.0</span>
  </nav>

  <div class="sidebar-item">
    <p>
      &copy; 2024. All rights reserved.
    </p>
  </div>
</div>


  <!-- Wrap is the content to shift when toggling the sidebar. We wrap the content to avoid any CSS collisions with our real content. -->
  <div class="wrap">
    <div class="masthead">
      <div class="container">
        <h3 class="masthead-title">
          <a href="/OpenGLCourseJP/" title="Home">OpenGL Course (JP)</a>
          <small></small>
        </h3>
      </div>
    </div>

    <div class="container content">
      <div class="page">
  <h1 class="page-title">
    ウィンドウを開く
    


  </h1>
  ************************************

.. |source_code| image:: ../../images/octcat.png
  :width: 24px
  :target: https://github.com/tatsy/OpenGLCourseJP/blob/master/src/001_open_window/main.cpp


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

実際にウィンドウを作成する関数は ``glfwCreateWindow`` 関数です。こちらは5つの引数を取ります。
重要なのは最初の3つで、先頭から順に「ウィンドウの幅」、「ウィンドウの高さ」、「ウィンドウの名前」です。

残りの4つ目の引数はフルスクリーンモードにするときに使用する引数、5つ目の引数は別のウィンドウを描画内容を
共有するときに使う引数なのですが、このコースでは使用しないので、基本は ``NULL`` を渡しておけば大丈夫です。

この ``glfwCreateWindow`` 関数はウィンドウを表す ``GLFWwindow`` 型のポインタを返します。
ウィンドウの作成に失敗した場合には、このポインタが ``NULL`` になるので、もし ``NULL`` であれば、
エラーメッセージを出力してプログラムを停止します。

この時、 ``glfwInit`` 関数を呼び出したあとであれば、必ず ``glfwTerminate`` 関数を呼ぶ必要があるので注意してください。

コンテキストの生成
-------------------

OpenGLでは描画の状態を保存するためにコンテキストと呼ばれるオブジェクトを作る必要があります。
これを今の時点で正確に説明するのは難しいのですが、概ね、OpenGLで扱う描画の状態等を保存するためのものだと思ってください。

このコンテキストを生成するためのコードは以下の行になります。

.. code-block:: cpp
  :linenos:

  // OpenGLの描画対象にWindowを追加
  glfwMakeContextCurrent(window);


背景色の設定
-------------

今回のプログラムでは、背景に色をつけて画面を表示します。
その背景の描画色を決定する関数が ``glClearColor`` 関数です。
対応するソースコードは以下の部分です。

.. code-block:: cpp
  :linenos:

  // 背景色の設定
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);


``glClearColor`` 関数は4つの引数を持ち、それぞれが赤、緑、青、アルファ値に対応します。
各色ちゃんねるの値は0から1までの値を取り、例えば(1, 1, 1, 1)が与えられると白色に、
(1, 0, 0, 0.5)が与えられると透明度が50%の赤色になります。


描画ループの設定
-------------------

.. code-block:: cpp
  :linenos:

  // メインループ
  while (glfwWindowShouldClose(window) == GL_FALSE) {
      // 描画
      drawOpenGL();

      // 描画用バッファの切り替え
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

この部分ではウィンドウが表示されている限り続くwhileループが書かれています。
まず、whileの終了判定に使われている ``glfwWindowShouldClose`` 関数は、
現在ウィンドウが開かれているのか閉じられているのかを調べます。
閉じられているときには ``GL_TRUE`` が返ってくるので、そうでない場合、
すなわち ``GL_FALSE`` が返ってきている間は描画を継続します。

描画部分は ``drawOpenGL`` 関数の中で処理を記述することにします。
現在のこの関数の定義は、このようになっています。

.. code-block:: cpp
  :linenos:

  // ユーザ定義のOpenGL描画
  void drawOpenGL() {
      // 背景色の描画
      glClear(GL_COLOR_BUFFER_BIT);
  }

ここでは、先ほど ``glClearColor`` 関数で設定した色を使って画面を塗りつぶす処理を書いています。
その役割を担うのが ``glClear`` 関数です。この関数の引数には現在 ``GL_COLOR_BUFFER_BIT`` という
定数が渡されていますが、この定数が渡されているときには ``glClear`` 関数により画面の色が塗りつぶされます。


描画結果
------------

全てのソースコードが上手くかけていれば、以下のような赤一色で塗りつぶされた画面が表示されるはずです。

.. image:: ./figures/001_opengl_window.jpg
  :width: 250px

練習
------

1. ウィンドウの幅や高さを変更したときの変化を調べる
2. ウィンドウのタイトルを変更したときの変化を調べる
3. ``glClearColor`` に渡されている色を別の色に変更して、変化を調べる
4. ``glClear`` をコメントアウトしたときに背景色がどうなるかを調べる


  
</div>



    </div>
  </div>

  <label for="sidebar-checkbox" class="sidebar-toggle"></label>

  <footer class="site-footer h-card">
    <data class="u-url" href="//OpenGLCourseJP/"></data>

    <div class="wrapper">
      <div class="container content">
        <div class="footer-col-wrapper">
          <div class="footer-col footer-col-1">
            <ul class="contact-list">
              <li class="p-name">OpenGL Course (JP)</li>
              <li><a class="u-email" href="mailto:tatsy.mail@gmail.com">tatsy.mail@gmail.com</a></li>
            </ul>
          </div>

          <div class="footer-col footer-col-2">
            <ul class="social-media-list">
              <li>
                <a href="https://github.com/tatsy">
                  <svg class="svg-icon">
                    <use xlink:href="/OpenGLCourseJP/public/minima-social-icons.svg#github"></use>
                  </svg>
                  <span class="username">tatsy</span>
                </a>
              </li>
            </ul>
          </div>

          <div class="footer-col footer-col-3">
            <p>Welcome to OpenGL programming course!</p>
          </div>
        </div>
      </div>
    </div>
  </footer>

  <script src='/OpenGLCourseJP/public/js/script.js'></script>

</body>

</html>