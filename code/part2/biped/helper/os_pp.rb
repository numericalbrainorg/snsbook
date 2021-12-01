#!/usr/bin/ruby

NOS = 12+1
Idx = [0, 1, 2, 5, 6, 9, 10, 3, 4, 7, 8, 11, 12]

open("os_pp.dat", "w"){|o|
  IO.foreach("os.dat"){|l|
    a = l.chomp.split
    (1...NOS).each{|i|
      a[i] = a[i].to_f + 2*i - 15
    }
    Idx.each{|i|
      o.printf('%f ', a[i])
    }
    o.puts("\n")
  }
}
