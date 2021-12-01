#!/usr/bin/ruby
# coding: utf-8

N = 4000 # ニューロン数
T = 1000 # 時間ステップ数 (ミリ秒)
B = 10 # PSTHのビン幅 (ミリ秒)

h = Array.new(T/B+1){|x| 0.0}
IO.foreach("spike.dat"){|l|
  t, n = l.chomp.split.map{|x| x.to_i}
  h[t/B] += 1.0
}
open("popavg.dat", "w"){|o|
  h.each_with_index{|x, i|
    o.puts "#{i * B} #{1000.0 * x / (B * N).to_f}"
  }
}
