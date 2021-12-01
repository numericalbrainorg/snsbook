`-*- coding: utf-8 -*-`

# README-ja.md

このフォルダの内容は、書籍

> 山﨑 匡, 五十嵐 潤. はじめての神経回路シミュレーション. 森北出版. 2021
[(出版社の紹介ページへのリンク)](https://www.morikita.co.jp/books/mid/085631)

で使用されたソースコード一式である。

## ファイル構成

テキストの付録A.3にも記載がある。このファイルの末尾に一覧を添付する。また、各フォルダはそれぞれ`README`と`Makefile`を含んでいる。

疑似乱数として、[SIMD-oriented Fast Mersenne Twister (SFMT)](http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/index-jp.html) のバージョン 1.5.1 を使っている。`./{column, part1, part2, part3}/misc/SFMT-1.5.1` 以下がそれである。

## 使い方

テキスト中の説明を読み、各フォルダの下の`README-ja.md`を参照のこと。

## ライセンス

- SFMTのライセンスは修正BSDライセンスである。詳細は`SFMT-1.5.1`以下に同梱されている`LICENSE.txt`もしくはSFMTのホームページ上の[`LICENSE.txt`](http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/LICENSE.txt)を確認のこと。
- `./{column, part1, part2, part3}/misc/timer.c`のライセンスはPublic Domainである。
- それ以外のソースコードのライセンスは修正BSDライセンスである。`LICENSE.txt`を確認のこと。

## ファイル一覧

```
part1/ : 第I部のソースコード
part1/hh/ : ホジキン・ハクスレー型モデル
part1/hh/hh.c : ホジキン・ハクスレーモデル
part1/hh/sfa.c : 発火頻度適応のモデル
part1/hh/ia.c : Type Iニューロンのモデル
part1/lif/ : 積分発火型モデル
part1/lif/lif.c : 1個のモデル
part1/lif/lif_alt.c : 1個のモデルで別のバージョン
part1/lif/lif_refr.c : 不応期を考慮した1個のモデル
part1/lif/lif2.c : 2個のモデル
part1/lif/network.c : 2個のモデルを繋いだネットワーク
part1/lif/network_delay.c : 不応期とシナプス遅延を考慮したネットワーク
part1/random/ : ランダムネットワーク
part1/random/random.c : ランダムネットワークのモデル
part1/multi/ : マルチコンパートメントモデル
part1/multi/traub.c : 海馬CA3モデル
part1/numeric/ : 常微分方程式の数値解法
part1/numeric/exp.c : 初期値問題を解いてexp(1)を求めるコード
part2/ : 第II部のソースコード (第II部のフォルダは複数のファイルを含む)
part2/od/ : 眼優位性マップの自己組織化
part2/ec/ : 瞬目反射条件づけ
part2/bg/ : ゴール探索
part2/hopfield/ : 連想記憶
part2/cpg/ : 中枢パターン生成器 (CPG)
part2/biped/ : 二足歩行
part2/som/ : 自己組織化マップ (SOM)
part2/sudoku/ : ナンプレ
part3/ : 第III部のソースコード
part3/random/random.c : オリジナルのランダムネットワークのコード
part3/random/random_ell.c : 結合行列をELLに変更したコード
part3/random/random_omp.c : OpenMPによる並列化
part3/random/random_mpi.c : MPIによる並列化
part3/random/random_mpi_ws.c : 
part3/random/random_hyb.c : ハイブリッド並列化
part3/random/random_gpu.cu : GPUによる並列化
part3/random/random_gpu_reduction.cu : シナプスの計算も含めた並列化
part3/random/hostfile : MPI版で用いたホストファイル
column/ : コラムのソースコード
column/make/ : Makefileの書き方
column/rng/ : 疑似乱数
column/soa/ : SoA vs AoS
appendix/ : 付録のソースコード
appendix/ode/ : 高校生にもわかる数値シミュレーション
appendix/ode/car.c : 等速度運動のシミュレーション
appendix/ode/car2.c : 等加速度運動のシミュレーション
```
