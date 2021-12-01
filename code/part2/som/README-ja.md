# 10. 自己組織化マップ (SOM)

## ファイル一覧
- `som.c`: SOMのコード
- `mnist.c`: MNISTのデータを読み書きするための関数群
- `mnist.h`: そのためのヘッダファイル
- `./mnist/`: MNISTのデータを置くためのフォルダ
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コードの説明
いわゆるSOMをスパイキングネットワークで実装した。Time-to-First-Spike (TFS) を説明するためのものである。

MNISTのデータは同梱されていないので、まずYann LeCunの[MNIST page](http://yann.lecun.com/exdb/mnist/)にある
```
train-images-idx3-ubyte.gz:  training set images (9912422 bytes)
train-labels-idx1-ubyte.gz:  training set labels (28881 bytes)
t10k-images-idx3-ubyte.gz:   test set images (1648877 bytes)
t10k-labels-idx1-ubyte.gz:   test set labels (4542 bytes)
```
を全てダウンロードし`./mnist/`以下に置く。

## コンパイル方法
`make`する。

## 実行方法
`./som`を実行すると、ステップ番号とそのときに呈示された数字が表示され、60,000ステップ完了すると
計算が終了する。参照ベクトルを可視化したものが`w.png`として生成されるので、これを表示すると図10.2が得られる。
