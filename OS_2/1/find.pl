#!/usr/bin/perl -l
use GD;

open SHOT, "site.png" or die "Can not open site.png: $!";
my $screenShot = GD::Image->newFromPng(SHOT);
my ($i, $j, $flag) = (0, 0, 1);
my ($r, $g, $b, $pixel, $x, $y);
my ($x, $y) = (-1, -1);
while ($i < 1004) {
    $j = 0;
    while ($j < 1980) {
        $pixel = $screenShot->getPixel($i, $j);
        ($r, $g, $b) = $screenShot->rgb($pixel);

        if (($r > 235) && ($g < 50) && ($b < 50)) {
            for my $m (1..8) {
                for my $n (1..5) {
                    $pixel = $screenShot->getPixel($i+$m, $j+$n);
                    ($r, $g, $b) = $screenShot->rgb($pixel);
                    
                    $flag = 0 if(($r <= 235) || ($g >= 50) || ($b >= 50));
                   
                    ($x, $y) = ($i, $j) if(($flag != 0) && ($m == 8) && ($n == 5));
                }
                
            }
            # if(($x != -1) && ($y != -1)){
            #     last;
            #     last;
            # }  
        }
        $j++;
    }
    $i++;
}

print "$x $y";