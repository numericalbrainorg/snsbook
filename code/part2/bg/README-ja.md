# 7. ゴール探索のシミュレーション

## ファイル一覧
- `gw.c`: ゴール探索のコード
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コードの説明
このコードは、Fr\'{e}maux et al. (2013) を再実装したものである。元の論文ではニューロンモデルとしてSRM0を用いていたが
(Gerstnerのところの論文なので)、ここではテキストにあわせてLIFモデルを用いた。論文はオープンアクセスでPDFをダウンロード
できるので、詳細はそちらを参照していただきたい。
[Fr\'{e}maux et al. (2013) Reinforcement Learning Using a Continuous Time Actor-Critic Framework with Spiking Neurons. PLoS Comput Biol 9(4): e1003024.](https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1003024)

## コンパイル方法
7.3節の通り`make`する。テキストでは`make`しただけでは実行はされないが、こちらは実行も同時にされるようにした
(他のコードとの一貫性のため)。もしコンパイルだけしたければ
```
node00:~/snsbook/code/part2/bg$ make gw
```
とすればよい。

## 実行方法
テキスト通りに実行すればよい。`pos.dat`を表示すれば図7.4aが、`td.dat`を表示すれば図7.4bが、`raster.dat`を表示すれば
図7.4c, dがそれぞれ得られる。

## 謝辞
このコードは𠮷村英幸さんに作成・提供していただいた。
