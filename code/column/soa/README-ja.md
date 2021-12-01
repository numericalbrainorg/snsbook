# コラム: AoS vs. SoA

## ファイル一覧
- `aos.cu`: AoSで書かれたバージョン
- `soa.cu`: SoAで書かれたバージョン
- `hh.cu`: ゲート変数の関数定義
- `hh.h`: そのヘッダファイル
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コンパイル方法
CUDAがインストールされたGPUマシンで`make`すると、実行ファイル`aos`, `soa`が作成される。
Compute Capabilityの指定は`Makefile`の先頭にある。現在はV100にあわせて
```
--generate-code arch=compute_70,code=sm_70
```
としているので、これを自分のGPUにあわせて適切に設定する。なおユニファイドメモリを使って
いるので、Compute Capability 6以上のGPUが必要。

## 実行方法
`aos`, `soa`を実行すると、それぞれ計算時間を出力して停止する。また、メインループの中で
```
kernel_gpu <<< gs, BLOCK_SIZE >>> ( neuron );
//kernel_cpu ( neuron );
```
としてあるので、`kernel_cpu`の方を有効にすれば、CPU版として動作する。
