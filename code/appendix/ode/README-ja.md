`-*- coding: utf-8 -*-`

# A.1 高校生にもわかる数値シミュレーション

## ファイル一覧

- `car.c`: 等速度運度のシミュレーション (リストA.1)
- `car2.c`: 等加速度運動のシミュレーション (リストA.2)
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コンパイル方法
テキスト通りにコンパイルする。
```
node00:~/snsbook/code/part1/appendix/ode$ gcc -O3 -std=gnu11 -Wall -o car car.c
node00:~/snsbook/code/part1/appendix/ode$ gcc -O3 -std=gnu11 -Wall -o car2 car2.c
```

もしくは`make`すると全て自動的にコンパイルされ実行される。
```
node00:~/snsbook/code/part1/appendix/ode$ make
gcc -O3 -std=gnu11 -Wall -c car.c
gcc -O3 -std=gnu11 -Wall -o car car.o
gcc -O3 -std=gnu11 -Wall -c car2.c
gcc -O3 -std=gnu11 -Wall -o car2 car2.o
./car > car.dat
./car2 > car2.dat
node00:~/snsbook/code/part1/appendix/ode$
```
シミュレーション結果は`car.dat`, `car2.dat`にそれぞれ出力される。

## 実行方法
`car`, `car2`ともにテキスト通りに実行する。
