# 5. 第1次視覚野の眼優位性マップ形成のシミュレーション

## ファイル一覧
- `od.c`: 眼優位性マップのモデル
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コンパイル方法
5.3.3項の通り`make`する。テキストでは`make`しただけでは実行はされないが、こちらは実行も
同時にされるようにした (他のコードとの一貫性のため)。もしコンパイルだけしたければ
```
node00:~/snsbook/code/part2/od$ make od
```
とすればよい。

## 実行方法
基本的にテキスト通りに実行して、`before.dat`, `after.dat`をプロットすればよい。図5.6が得られる。

単眼遮蔽のシミュレーションは、配列 `activity` の値を例えば
```
//float activity [ N_PRE ] = { 1.0, 1.0 };
float activity [ N_PRE ] = { 1.0, 0.5 };
```
とすればよい。0.5にすれば図5.7aが、0.1にすれば図5.7bが得られる。