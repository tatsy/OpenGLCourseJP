---
layout: page
title: GLMのインストール
permalink: /setup/glm/
---

## GLMのインストール (Windowsの場合)

GLMも公式のウェブページからライブラリをダウンロードします。

**GLMのダウンロードページ** <br/>
[http://glm.g-truc.net](http://glm.g-truc.net)

{% include lightbox.html src="/assets/images/setup/glm_top_page.jpg" data="image-1" %}

上記のウェブページの左側にある「Downloads」というリンクをクリックします。するとダウンロードページに移動するので、その中から最新版である0.9.9.5をダウンロードします (2019年4月1日現在) 。ダウンロードしたファイルは「`glm-0.9.9.5.zip`」という名前になっているはずです。

{% include lightbox.html src="/assets/images/setup/glm_download_page.jpg" data="image-1" %}

ダウンロードが完了したら、ZIPファイルを展開して適当なディレクトリに配置します。


### プロパティシートの更新

初回の準備の際に作成したプロパティシートを更新して、GLEWおよびGLMを使用できるようにします。設定方法に関しては「[GLFWの設定]({% link pages/glfw.md %})」を参照してください。設定項目は以下の通りです。

**インクルードディレクトリ** <br/>
`C:/Libraries/opengl/glm-0.9.x.x`

---

## GLMのインストール (Macの場合)

[GLMのGitHub](https://github.com/g-truc/glm.git) からソースコードをダウンロードして、ビルドとインストールを行います。次のコマンドをターミナルで実行してください。

{% highlight shell linenos %}
git clone https://github.com/g-truc/glm.git
cd glm
mkdir build && cd build
cmake -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=OFF -DGLM_TEST_ENABLE=OFF ..
make
sudo make install
{% endhighlight %}

これだけでインストールは完了です。

*2020年4月現在のGLMの最新版を使うと上記のCMakeを使う方法ではヘッダをインストールできないようです。代替案としてバージョン0.9.9.5を使うか[Homebrew](https://brew.sh/index){:target="_blank"}などを使ってインストールすることもできます。*

### Xcodeの設定

Macの場合、上記のコマンドでGLMは`/usr/local/include`というシステム全体から見える場所にインストールされるため、Xcodeでインクルード・ディレクトリ等を設定する必要ありません。
