#!/bin/zsh

for i in ./animate/*.eps; do
  echo $i
  j=$i:r
  convert -density 100 -flatten $i $j.jpg
done

ffmpeg -r 10 -i ./animate/%05d.jpg -an -vcodec libx264 -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" animate.mp4
