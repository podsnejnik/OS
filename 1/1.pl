#!/usr/bin/perl
use locale; 
use vars '$RON, $RVV, $zapp, $zam1, $vzap1, $zam2, $chist, $pusk, $vib, $table_com';
%table_com = (
	'00' => [1, 0, 0, 0],
	'11' => [0, 1, 1, 0],
	'15' => [1, 1 ,1 ,0],
	'02' => [0, 2, 0, 0],
	'21' => [0, 1, 2, 0],
	'25' => [1, 1, 2, 0],
	'31' => [0, 1, 3, 0],
	'FE' => [-1, 4, 15, 1],
	'F0' => [-1, 4, 15, 0],
	'F1' => [-1, 4, 15, 0],
	'F4' => [-1, 4, 15, 1],
	'F5' => [-1, 4, 15, 0],
	'FF' => [-1, 4, 15, 0]
	);

($zapp1, $vzap1, $zam1, $op, $chist, $RON, $IP, $ADDR, $IR) = (0,0,0,0,0,0,0, 0, 0);
$RVV = 0;
$vib = 0;
$COP = "";
$zam2 = 0;
($RON, $RVV, $zapp, $zam1, $vzap1, $zam2, $chist, $pusk, $vib, $table_com) = (0,0,0,0,0,0,0,0,0,0);
die "Usage: program filename [zapp|zam1|vzap1|zam2|chist|pusk|vib|op|pereh]" if (@ARGV < 1);
open F, '<', $ARGV[0] or die "can't open file";
chomp(@proga = <F>);#v proga legat stroki nashego faila
close F;
if (@ARGV > 1){

	for $i(1..@ARGV-1){
		$broken = $ARGV[$i];
		if ($broken eq 'op'){
			for $i (keys %table_com){
				${$table_com{$i}}[2] = 0;			
			}
		}
		if ($broken eq 'pereh'){
			foreach $i (keys %table_com){
				${$table_com{$i}}[3] = 0;	
			}
			
		}
	}		
			

}

for $i(0..@proga-1){
	($ip, $com) = split(":", $proga[$i]);
	($cop, $arg) = split(" ", $com);
	$proga[$i] = [$ip, $cop, $arg];# v proga teper' legat stroki faila v vide massiva
}

$pusk = 1;
$IP = 0;
$IR = 0;

while (1){
	#${$table_com{'F0'}}[3]  = ${$table_com{'F1'}}[3] = 1 - $RON;#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	${$table_com{'F0'}}[3]  = 1 - $RON;#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	${$table_com{'F1'}}[3]  = $RON;#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	our $COP;
	$COP = uc($proga[$IP][1]);#priveli k verhnemu registru
	$ADDR = $proga[$IP][2];
	dekkom($COP);#vistavlyet flagi v zavisimosti ot instrukcii
	setConstants();#smotrit, kakie provoda slomani
	$IA = $t1 = $ADDR + $IR;
	$t0 = $proga[$t1][2];
	$t2 = $RVV;
	$t0 = multipl($t0, $t1, $t2, $vib);
	$t1 = $RON;
	$t0 = alu($t1, $t0);
	# IR
	$t = multipl($t0, 0, $chist);
	$IR = $t if ($zam2 == 1);
	# RON
	$RON = $t0 if ($zam1 == 1);
	# memory
	$proga[$IP+$IA][2] = $t0 if ($zapp == 1);
	# next command
	$t = multipl($IP+1, $IA, ${$table_com{$COP}}[3]);
	if ($pusk == 1){
		$IP = $t;
	}
	else{
		last;
	}
}

#print finish state
print "Instruction pointer: " . $IP ."\n";
print "General-purpose register: " . $RON."\n";
print "Index register: " . $IR."\n";
#print memory
print "Memory:"."\n";
for $i (0..@proga-1){
	print $i . ':' . $proga[$i][2]."\n";
}

sub dekkom(){
	$com1 = shift;
	# print "com1 = ".$com1."\n";
	$p = ${$table_com{$com1}}[1];
	if ($p == 0) {$zapp = 1} else {$zapp = 0;}
	if ($p == 1){$zam1 = 1; } else {$zam1 = 0;}
	if ($p == 3) {$vzap1 = 1;}  else {$vzap1 = 0;}
	if ($p != 3) {$zam2 = 1;}  else {$zam2 = 0;}
	if ($p !=2 and $p !=3) {$chist = 1;}  else {$chist = 0;}
	if ($com1 eq 'FF') {$pusk = 0;}  else {$pusk = 1;}
	$vib = ${$table_com{$com1}}[0];
	# print "zapp = " . $zapp ." zam1 = ".$zam1. " vzap1 = ".$vzap1." zam2 = ".$zam2." chist = ".$chist." pusk = " .$pusk." vib = ".$vib."\n";
}

sub alu(){
	($arg0,$arg1) = (shift,shift);
	$op = ${$table_com{$COP}}[2];
	#switch (op):
	return $arg0 if (!$op); 
	return $arg1 if ($op == 1);
	return $arg0 + $arg1 if ($op == 2);
	return $arg1 - $arg0 if ($op == 3);
	return $RON if ($op == 15);
	return -1;
}


sub multipl{
	@args = @_;
	return $args[$args[@args-1]];
}
sub setConstants(){
	%num_hash = ('zapp' => 0, 'zam1' => 1, 'vzap1' => 2,
		'zam2'=> 3, 'chist' => 4, 'pusk' => 5, 'vib' => 6);
	if (@ARGV > 1){
		for $i(1..@ARGV-1){
			$broken = $ARGV[$i];
			if (($broken eq 'zapp') or ($broken eq 'zam1') or ($broken eq 'vzap1') or ($broken eq 'zam2') or ($broken eq 'chist') or ($broken eq 'pusk') or ($broken eq 'vib')){
				$val = $num_hash{$broken};
				#idut v takom por'adke: ($zapp, $zam1, $vzap1, $zam2, $chist, $pusk, $vib)
				@a = ('-1', '-1', '-1', '-1', '-1', '-1', '-1');
				$a[$val] = 0;
				$zapp=$a[0] if $a[0] ne '-1';
				$zam1=$a[1] if $a[1] ne '-1';
				$vzap1=$a[2] if $a[2] ne '-1';
				$zam2=$a[3] if $a[3] ne '-1';
				$chist=$a[4] if $a[4] ne '-1';
				$pusk=$a[5] if $a[5] ne '-1';
				$vib=$a[6] if $a[6] ne '-1';
			}
		}
	}
}	


