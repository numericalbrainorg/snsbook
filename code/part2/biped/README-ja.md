# 9.2. 二足歩行のシミュレーション

## ファイル一覧
- `main.c`: main関数
- `os.c`: CPGのコード
- `ms.c`: 筋骨格系のコード
- `mat.c`: 行列演算のコード
- `misc.c`: モデルに含まれるいくつかの汎用的な関数
- `header.h`: パラメータの定義
- `healper/`: データの編集・可視化と動画の作成を行うスクリプト群
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コードの説明
このコードは、Taga et al. (1991) をスパイキングニューロンで再実装したものである。
論文はSemantic ScholarからPDFをダウンロードできるので、詳細はそちらを参照していただきたい。
なお、この論文から実装を読み解くのは結構大変である。
[Taga et al. (1991) Self-organized control of bipedal locomotion by neural oscillators in unpredictable environment. Biol Cybern 65: 147--159.](https://api.semanticscholar.org/CorpusID:5262616)

歩行中の動画を作成するために、[ffmpeg](https://www.ffmpeg.org/)と[zsh](https://zsh.prg)を必要とする。事前にインストールしておくこと。
また、スクリプト群は全てRubyで書いてあるので、可視化にはRubyも必要である。

## コンパイル方法
`make`すればよい。実行ファイル`main`ができる。

## 実行方法
`make run`を実行する。コードが実行され、かつ`helper/`以下のスクリプト群が実行されて、
以下のデータが生成される。
- `ms.dat`: 筋骨格系の姿勢
- `os.dat`: CPGの活動
- `os_pp.dat`: `os.dat`を可視化用に調整したもの (`helper/os_pp.rb`で生成)
- `sp.dat`: CPGの膜電位
- `sp_pp.dat`: `sp.dat`を可視化用に調整したもの (`helper/sp_pp.rb`で生成)
- `ankle.dat`: 足先の位置 (左足の(x, y)座標と右足の(x, y)座標)
- `stick.dat`: `ms.dat`から生成した歩行中のスティックダイアグラム (`helper/stick.rb`で生成)
- `aminate.mp4`: 歩行中の動画 (`stick.dat`から`helper/animate.rb`と`helper/animate_post.zsh`で生成)

## 可視化方法

gnuplotで
```
gnuplot> load 'helper/os_pp.gp'
```
を実行すると、CPGの活動が一覧で表示される。これは、Taga et al. (1991) の図6.Aに相当する。また、
```
gnuplot> load 'helper/sp_pp.gp'
```
を実行すると、CPGの各ニューロンの膜電位が表示される。さらに、
```
gnuplot> plot 'stick.dat'
```
を実行すると、歩行中のスティックダイアグラム (図9.4に相当) が得られる。

`animate.mp4`を再生すると歩行中の様子を動画で見ることができる。

## 謝辞
このコードは市村大輔さんに作成・提供していただいた。
