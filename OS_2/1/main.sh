#!/bin/bash
firefox http://www.lostfilm.tv/&
sleep 15
scrot $HOME/Desktop/site.png
perl find.pl | { read x y; xdotool mousemove $x $y click 1;}
sleep 15
scrot $HOME/Desktop/site2.bmp
exit