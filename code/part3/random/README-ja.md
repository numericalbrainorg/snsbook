# 第III部 スパコンを上手に使う

## ファイル一覧
- `random.c`: ランダムネットワークのコード (`code/part1/random/random.c`と同じもの)
- `random_ell.c`: ELL化したコード (リスト12.1, 12.2)
- `random_omp.c`: OpenMPで並列化したコード (リスト13.2, 13.3)
- `random_mpi.c`: MPIで並列化したコード (リスト14.1--17)
- `random_mpi_ws.c`: 弱スケーリング (図14.4) 測定用のコード
- `random_hyb.c`: OpenMPとMPIでハイブリッド並列したコード
- `random_gpu.cu`: GPUで並列化したコード (リスト15.1--13)
- `random_gpu_reduction.cu`: シナプスの計算も含めて並列化したコード (リスト15.14--21)
- `hostfile`: MPI版で用いたホストファイル (リスト14.1)
- `Makefile`: Makefile
- `README-ja.md`: 日本語の説明 (このファイル)

## コードの説明
`random.c`を様々な手法で並列化し、計算時間を比較する。
- `random_omp.c`のコンパイルには、OpenMPをサポートするコンパイラ (gccであればバージョン4.9以降) が必要である。
- `random_mpi.c`のコンパイルには、MPIが必要である。テキストでは[mvapich2](https://mvapich.cse.ohio-state.edu/)を利用している。
- `random_hyb.c`のコンパイルには、OpenMPとMPIの両方が必要である。
- `random_gpu.cu`, `random_gpu_reduction.cu`のコンパイルには、Compute Capability 6以上のGPUとそれにあわせたCUDAが必要である。

## コンパイル方法
`xxxxx.c`をコンパイルするには、`make xxxxx`とすればよい。例えば`random_ell.c`をコンパイルするなら`make random_ell`である。

## 実行方法

`random_ell`, `random_omp`はそのまま実行すれば良い。`random_omp`は
```
node01:~/snsbook/code/part3/random$ OMP_NUM_THREADS=24 ./random_omp
# of omp threads = 24
Elapsed time = 0.445473 sec.
```
のようにして、利用するスレッド数を指定することもできる。

`random_mpi`は、テキスト通り
```
node00:~/snsbook/code/part3/random$ mpirun -hostfile hostfile -np 192 ./random_mpi
```
とする。`-np`の後の数値でプロセス数を指定できるので、1, 2, 4, 8, ... と変えていけば図14.3が得られる。

`random_mpi_ws`は少し煩雑で、先頭のニューロン数を例えば
```
//#define N       ( 4000 )            // # of neurons
//#define N_E     ( 3200 )            // # of excitatory neurons
#define N       ( 8 * 4000 )            // # of neurons
#define N_E     ( 8 * 3200 )            // # of excitatory neurons
```
このように変えながら、プロセス数を変えて実行する必要がある。図14.4は、ニューロン数が4000個の時に
8プロセスで計算し、ニューロン数を8000, 16000, 32000と増やすのにあわせてプロセス数も16, 32, 64と
増やす。

`random_hyb`も、テキスト通り
```
node00:~/snsbook/code/part3/random$ mpirun -hostfile hostfile -np 16 ./random_hyb
```
とする。

`random_gpu`はコンパイルができていればそのまま実行すれば良い。

`random_gpu_reduction`は、デフォルトでは`calculateSynapticInputs_a`が有効になっている。
```
calculateSynapticInputs_a <<< GRID_SIZE, BLOCK_SIZE >>> ( n );
//calculateSynapticInputs_b <<< N, NTHSYN >>> ( n );
```
そのまま実行すると`random_gpu`と同じ計算時間になるので、シナプスの並列計算を試す場合は、`calculateSynapticInputs_b`を有効にすること。
