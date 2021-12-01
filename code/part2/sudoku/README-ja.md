# 11. ナンプレ

## ファイル一覧
- `sudoku.c`: ナンプレのコード
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コードの説明
特になし。

## コンパイル方法
テキスト通りに`make`する。

## 実行方法
テキスト通りに実行する。計算によって得られた解が画面に表示される。

もし図11.3のラスタープロットを表示したければ、コード中の
```
//if ( s [ i ] ) { printf ( "%d %d\n", nt, i ); } // print spikes for raster plots
```
を有効にし、コンパイルし直して
```
node00:~/snsbook/code/part2/sudoku$ ./sudoku > sudoku.dat
```
と実行する。あとは得られた`sudoku.dat`をgnuplotで表示すればよい。なお、ナンプレの解は標準エラー出力に出力されている。
