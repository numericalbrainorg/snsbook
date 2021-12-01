#!/usr/bin/ruby

l1 = 0.5
l2 = 0.6

open("stick.dat", "w"){|o|
  n = 0
  IO.foreach("ms.dat"){|l|
    if n % 5 == 0 
      x = l.chomp.split
      axr = x[9].to_f + (l2/2)*Math.cos(x[11].to_f)
      ayr = x[10].to_f - (l2/2)*Math.sin(x[11].to_f)
      axl = x[12].to_f + (l2/2)*Math.cos(x[14].to_f)
      ayl = x[13].to_f - (l2/2)*Math.sin(x[14].to_f)

      kxr = x[3].to_f + (l1/2)*Math.cos(x[5].to_f)
      kyr = x[4].to_f - (l1/2)*Math.sin(x[5].to_f)
      kxl = x[6].to_f + (l1/2)*Math.cos(x[8].to_f)
      kyl = x[7].to_f - (l1/2)*Math.sin(x[8].to_f)

      hx = x[1].to_f
      hy = x[2].to_f

      o.puts "#{hx} #{hy}"
      o.puts "#{kxr} #{kyr}"
      o.puts "#{axr} #{ayr}"
      o.puts "\n"
      o.puts "#{hx} #{hy}"
      o.puts "#{kxl} #{kyl}"
      o.puts "#{axl} #{ayl}"
      o.puts "\n"
    end
    n += 1
  }
}
