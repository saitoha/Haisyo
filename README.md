Haisyo
======

- Windowsデスクトップ環境のダイアログリソースを微妙に拝承化します。
  仕事してる気分になりますよね。

![Haisyo](http://zuse.jp/misc/haisyo-example.png)

- タスクバーにトレイが出ます。

![Haisyo](http://zuse.jp/misc/haisyo-tray.png)


動作環境
--------

Windows XP sp3 と Windows 7 sp1 で動作確認しました。

ライセンス
----------
GNU GENERAL PUBLIC LICENSE Version 3


ダウンロード
------------

MinGWでビルドしました。
[haisyo-0.2.0.zip](http://zuse.jp/misc/haisyo-0.2.0.zip)


ビルド方法
----------

### MinGWでビルド

* MinGW環境を作ります。
  Debianだと以下のようにしてクロスコンパイル環境を作ります。

```
$ sudo apt-get install build-dep mingw-w64
```

  MacやCygwinでもコンパイルできます。


* ./configure します。

```
$ ./configure
```

* make します。

```
$ make dist
```

* zip パッケージができます。
  Windows環境にもっていってください。


### MSVCでビルド

* MSVC環境を作ります。

* nmake します。

```
$ nmake -f msvc.mk
```

* haisyo.exe と haisyo.dll ができあがります。
  適当な場所に配置して下さい。


### Cygwin + MSVCでビルド

* Cygwinをインストールします。makeを入れておいて下さい。

* Visual C++ 2008 を入れます。(Expressでいいです)

* make します。

```
$ make -f cygwin-msvc.mk dist
```

  環境により、ちょっとcygwin-msvcをいじる必要があるかもしれません。


