#!/usr/bin/ruby
# coding: utf-8

N = 8 # PSTHを計算する顆粒細胞の個数
T = 50 # 50試行分のデータを収集する
B = 10 # PSTHのビン幅 (ミリ秒)

#
# 先頭のN個の顆粒細胞に対して、試行毎のスパイクデータを収集する
#

o = Array.new
(0...N).each{|i| o[i] = open("gr.trial.#{i}", "w") }

(0...T).each{|tr|
  IO.foreach("gr.spk.#{tr}"){|l|
    t, n = l.chomp.split.map{|x| x.to_i}
    o[n].puts "#{t - 250} #{tr}" if n < N
  }
}

(0...N).each{|i| o[i].close }

#
# PSTHをビン幅 B (ミリ秒) で計算する
#

(0...N).each{|n|
  open("gr.psth.#{n}", "w"){|o|
    h = Array.new(T){|x| 0.0}
    IO.foreach("gr.trial.#{n}"){|l|
      t, tr = l.chomp.split.map{|x| x.to_i}
      h[t/B] += 1.0 if 0 <= t && t < 500
    }
    h.each_with_index{|x, i|
      o.puts "#{i * B} #{1000.0 * x / (B * T).to_f}"
    }
  }
}
