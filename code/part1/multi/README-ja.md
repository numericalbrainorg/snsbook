# 3.7 ニューロンの形状まで考慮したシミュレーション

## ファイル一覧
- `traub.c`: 海馬CA3のモデル (リスト3.8)
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コンパイル方法
テキスト通り`make`する。テキストでは`make`しただけでは実行はされないが、こちらは実行も
同時にされるようにした (他のコードとの一貫性のため)。もしコンパイルだけしたければ
```
node00:~/snsbook/code/part1/multi$ make multi
```
とすればよい。

## 実行方法
基本的にテキスト通りに実行して、`traub.dat`をプロットすればよい。テキスト通りgnuplotで
```
gnuplot> plot 'traub.dat' using 1:10 with lines
```
とすれば図3.20aが得られる。

図3.20bを得るためにはコードに手を入れる必要がある。まず、
```
//#define T  ( 200   ) // ms
//#define NT ( 20000 ) // T / DT
#define T  ( 2000   ) // ms
#define NT ( 200000 ) // T / DT
```
としてシミュレーション時間を200ミリ秒から2000ミリ秒に延長し、外部電流の与え方を
```
//i_inj [ 8 ] = 1.e-4 / area [ 8 ];
i_inj [ 8 ] = (  1000 <= t && t < 1001 ) ? 1.e-2 / area [ 8 ] : 0.;
```
と変更して1000ミリ秒の時点で1ミリ秒間だけ与えるようにする。これでコードを実行し、
```
gnuplot> plot [1000:1050] 'traub.dat' using 1:9 w l, 'traub.dat' using 1:12 w l, 'traub.dat' using 1:15 w l, 'traub.dat' using 1:18 w l
```
として1000ミリ秒から50ミリ秒間の膜電位をプロットすればよい。
