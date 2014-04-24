Haisyo
======

- Windowsデスクトップ環境のダイアログリソースを微妙に拝承化します。
  仕事してる気分になりますよね。

![Haisyo](http://zuse.jp/misc/haisyo-example.png)

- タスクバーにトレイが出ます。

![Haisyo](http://zuse.jp/misc/haisyo-tray.png)


動作環境
--------

以下のOSで動作確認しました

    * Windows 2000 sp4
    * Windows XP sp3
    * Windows 7 sp1 (32bit)
    * Windows Server 2012 (64bit) (バージョン1.3.0から)

ライセンス
----------
[GNU GENERAL PUBLIC LICENSE Version 3](http://www.gnu.org/licenses/gpl.html)でライセンスされた[自由ソフトウェア](http://www.gnu.org/philosophy/free-sw.ja.html)として提供されます。

![GPLv3](http://zuse.jp/misc/gplv3.png)

スクリーンショット
------------------

![Windows 2000](http://zuse.jp/misc/haisyo-win2000.png)

![Windows Server 2012 64bit](http://zuse.jp/misc/haisyo-winserver2012.png)


ダウンロード
------------

MinGWでビルドしました。

- [haisyo-latest.zip](http://zuse.jp/misc/haisyo-latest.zip)
- [haisyo-latest.tar.gz](http://zuse.jp/misc/haisyo-latest.tar.gz)


ビルド方法
----------

### MinGWでビルド

* MinGW環境を作ります。
  Debianだと以下のようにしてクロスコンパイル環境を作ります。

  ```
  $ sudo apt-get install build-dep mingw-w64
  ```

  Mac/Cygwin/MSYS環境でもビルドできると思います。
  できなかったらissueをたててください。


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


* 2014-04-19 64bit Windows に対応しました。

  x86_64なmingwが見つかると、64bit版フックDLL(haisyo64.dll)と
  サロゲート用実行ファイル(haisyo64surrogete.exe)を作成するようになります。

  64bitコンパイラが見つからないと32bit版だけ作ろうとするので、
  明示的にこれを指定したい場合は、例えば

  ```
  $ CC=i686-w64-mingw32-gcc \
    RC=i686-w64-mingw32-windres \
    CC64=x86_64-w64-mingw32-gcc \
    RC64=x86_64-w64-mingw32-windres ./configure
  ```

  のようにして下さい。


### MSVCでビルド (64bit非対応)

* MSVC環境を作ります。

* nmake します。

```
$ nmake -f msvc.mk
```

* haisyo.exe と haisyo.dll ができあがります。
  適当な場所に配置して下さい。


### Cygwin + MSVCでビルド (64bit非対応)

* Cygwinをインストールします。makeを入れておいて下さい。

* Visual C++ 2008 を入れます。(Expressでいいです)

* make します。

```
$ make -f cygwin-msvc.mk dist
```

  環境により、ちょっとcygwin-msvcをいじる必要があるかもしれません。


