#!/usr/bin/perl

%table_com = (
  0 => [0, 0, 0, 0],
  17 => [0, 1, 1, 0], # 11
  21 => [1, 1 ,1 ,0], # 15
  2 => [0, 2, 0, 0], # 02
  33 => [0, 1, 2, 0], # 21
  37 => [1, 1, 2, 0], # 25
  49 => [0, 1, 3, 0], # 31
  254 => [0, 4, 15, 1], #FE
  240 => [0, 4, 15, [0,1]], #F0
  241 => [0, 4, 15, [0,1]], #F1
  244 => [0, 4, 15, [0,1]], #F4
  245 => [0, 4, 15, [0,1]], #F5
  255 => [0, 4, 15, 0] #FF
  );
  %command = ('00' => 1, '11' => 1, '15' => 1, '02' => 1, '21' => 1, '25' => 1, '31' => 1, 'FE' => 1, 'F0' => 1, 'F1' => 1, 'F4' => 1, 'F5' => 1, 'FF' => 1);
 
if (@ARGV > 0){
  for $i(0..@ARGV-1){
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
 
@memory = ();

open F, '<', "k.txt" or die "Ñan't open file!";
chomp(@proga = <F>);#v proga legat stroki nashego faila
close F;

@clear_proga = ();
@data = ();
for $i(0..(@proga-1)) {
	($kop, $addr) = split(" ", $proga[$i]);
	if ($command{$kop}){push (@clear_proga, [$kop, $addr]);}
	else{push (@data, $kop);}
}

for $i(0..(@clear_proga-1)){
	$clear_proga[$i][0] = hex($clear_proga[$i][0]);
	push (@memory, $clear_proga[$i][0]);
	my @mas = $clear_proga[$i][1] =~ /.{1,2}/g;
	foreach $i(0..$#mas){
		$mas[$i] = hex($mas[$i]);
	}
	push (@memory, @mas);
}
for $i(0..(@data-1)){
	my @mas = $data[$i] =~ /.{1,2}/g;
	foreach $i(0..$#mas){
		$mas[$i] = hex($mas[$i]);
	}
	push (@memory, @mas);
}
$length = @memory;

for $i(0..((2**16)-1-$length)){ 
	push (@memory, 0);
}

%REG_RON = ('pr'=> 0, 'zn' => 0);
$REG_IR = 0;
$REG_UKKOM = 0;

$pusk = 1;
our $flag_F = 0;
($zapp, $zam1, $zam2, $vzap1, $vib, $chist, $adrkom, $kom, $p_kop, $ind, $p_a, $p_ia, $p_sum, $p_rez1, $p_pr) = (0,0,0,0,0,0,1,1,1,1,1,1,1,1,1);
setConstants();
while($pusk){	
	if($adrkom) {$my_reg_ukkom = $REG_UKKOM;} else  {$my_reg_ukkom = 0;}
	if($adrkom) {$my_reg_ukkom_in_memory = $my_reg_ukkom * 3;} else {$my_reg_ukkom_in_memory = 0;}
	if($kom) {$my_kop = @memory[$my_reg_ukkom_in_memory];} else {$my_kop = 0;}	
	# print "my_kop = ", $my_kop, "\n";
	if($kom) {$my_fbyte = @memory[$my_reg_ukkom_in_memory + 1];} else{$my_fbyte = 0;}
	if($kom) {$my_sbyte = @memory[$my_reg_ukkom_in_memory + 2];} else {$my_sbyte = 0;}
	if($kom) {$my_addr = $my_fbyte * 256 + $my_sbyte;} else{$my_addr = 0;}
	our $pereh = -2;
	if ($p_kop) {dekkom($my_kop);}
	setConstants();
	if($ind) {$my_ir = $REG_IR;} else{$my_ir = 0;}
	if($p_a) {$my_addr2 = $my_addr;} else{$my_addr2 = 0;}
	if($p_ia){$my_i_addr = $my_ir + $my_addr2;} else{$my_i_addr = 0;}
	$t0 = multipl(($memory[$my_i_addr] * 256 + $memory[$my_i_addr + 1]), $my_i_addr, $vib);
	if($p_sum){$t1 = $REG_RON{'zn'};} else{$t1 = 0;} 
	if($p_rez1){$alu_res = alu($t0, $t1, $my_kop);} else{$alu_res = 0;}
	$REG_RON{'zn'} = $alu_res if ($zam1 == 1);
	if ($zam1 == 1){
		if($p_pr){$REG_RON{'pr'} = alu_pr($alu_res);} else{$REG_RON{'pr'} = 0;} 
	}
	$t = multipl($alu_res, 0, $chist);
	$REG_IR = $t if ($zam2 == 1);		
	if ($zapp == 1){
		my @mas = ${$clear_proga[$REG_UKKOM]}[1] =~ /.{1,2}/g;
		foreach $i(0..$#mas){
			$mas[$i] = hex($mas[$i]);
		}		
		$addr_for_write = $mas[0]*256 + $mas[1];
		$hex_REG_RON = sprintf "%x",$REG_RON{'zn'};
		$hex_REG_RON = ('0' x (4 - length($hex_REG_RON))).$hex_REG_RON;
		my @mas = $hex_REG_RON =~ /.{1,2}/g;
		foreach $i(0..$#mas){
			$mas[$i] = hex($mas[$i]);
		}
		$memory[$addr_for_write] = $mas[0];
		$memory[$addr_for_write + 1] = $mas[1];
	} 
	
	if ($pereh != -2){$new_pereh = $pereh;}
	else {$new_pereh = $table_com{$my_kop}[3];}
	$REG_UKKOM = multipl(($my_reg_ukkom + 1), $my_i_addr, $new_pereh) if ($pusk == 1);
}

$, = "  ";
print "REG_IR = ", (sprintf "%x", $REG_IR), "\n";
print "REG_UKKOM = ", (sprintf "%x", $REG_UKKOM), "\n";
print "REG_RON = ",(sprintf "%x", $REG_RON{'zn'}), "\n";
$count = 0;
for (@memory){
	$hex_mem = sprintf "%x",$_;
	$hex_mem = ('0' x (2 - length($hex_mem))).$hex_mem; 
	print $hex_mem." ";
	$count++;
	print "\n" if($count % 16 == 0 && $count != 0);
	last if($count == 160);
}

sub alu_pr{
	$res = shift;
	return '01' if($res == 0);
	return '10' if($res > 0);
	return '11' if($res < 0);
}

sub alu{
	($arg0, $arg1, $kop) = (shift, shift, shift);
	$op = ${$table_com{$kop}}[2];
	return my_module($arg1) if (!$op); 
	return my_module($arg0) if ($op == 1);
	return my_module($arg0 + $arg1) if ($op == 2);
	return my_module($arg1 - $arg0) if ($op == 3);
}

sub my_module{
	$number = shift;
	$norm = $number % 2**16;
	$norm = $norm - 2 ** 16 if ($norm >= 2 ** 15);
	return $norm;
}

sub multipl{
	@args = @_;
	return $args[$args[@args-1]];
}

sub dekkom(){
	$com1 = shift;
	$p = ${$table_com{$com1}}[1];
	if ($p == 0) {$zapp = 1} else {$zapp = 0;}
	if ($p == 1){$zam1 = 1; } else {$zam1 = 0;}
	if ($p == 3) {$vzap1 = 1;}  else {$vzap1 = 0;}
	if ($p != 3) {$zam2 = 1;}  else {$zam2 = 0;}
	if ($p != 2 and $p != 3) {$chist = 1;}  else {$chist = 0;}
	if ($com1 == 255) {$pusk = 0;}  else {$pusk = 1;}
	$vib = ${$table_com{$com1}}[0];
	# print "zapp = " . $zapp ." zam1 = ".$zam1. " vzap1 = ".$vzap1." zam2 = ".$zam2." chist = ".$chist." pusk = " .$pusk." vib = ".$vib."\n";
	$priznak = $REG_RON{'pr'};
	if ($com1 == 240) {	
		# $pereh = 1 - (int( $priznak / 16 ));
		$pereh = int( $priznak / 16 );
	}
	if ($com1 == 241) {	$pereh = $priznak % 16;}
	if ($com1 == 244) {	$pereh = 1 - $flag_F;}
	if ($com1 == 245) {	$pereh = $flag_F;}
}


sub setConstants(){ 
  %num_hash = ('zapp' => 0, 'zam1' => 1, 'vzap1' => 2,
    'zam2'=> 3, 'chist' => 4, 'pusk' => 5, 'vib' => 6, 'adrkom' => 7, 'kom' => 8, 'p_kop' => 9, 'ind' => 10, 'p_a' => 11, 'p_ia' => 12, 'p_sum' =>13, 'p_rez1'=>14, 'p_pr' => 15);
  if (@ARGV > 0){
    for $i(0..@ARGV-1){
      $broken = $ARGV[$i];
      if (($broken eq 'zapp') or ($broken eq 'zam1') or ($broken eq 'vzap1') or ($broken eq 'zam2') or ($broken eq 'chist') or ($broken eq 'pusk') or ($broken eq 'vib') or ($broken eq 'adrkom') or ($broken eq 'kom') or ($broken eq 'p_kop') or ($broken eq 'ind')or ($broken eq 'p_a')or ($broken eq 'p_ia') or ($broken eq 'p_sum') or ($broken eq 'p_rez1') or ($broken eq 'p_pr')){
        $val = $num_hash{$broken};
        #idut v takom por'adke: ($zapp, $zam1, $vzap1, $zam2, $chist, $pusk, $vib)
        @a = ('-1', '-1', '-1', '-1', '-1', '-1', '-1', '-1', '-1', '-1', '-1','-1', '-1', '-1', '-1', '-1');
        $a[$val] = 0;
        $zapp=$a[0] if $a[0] ne '-1';
        $zam1=$a[1] if $a[1] ne '-1';
        $vzap1=$a[2] if $a[2] ne '-1';
        $zam2=$a[3] if $a[3] ne '-1';
        $chist=$a[4] if $a[4] ne '-1';
        $pusk=$a[5] if $a[5] ne '-1';
        $vib=$a[6] if $a[6] ne '-1';
		$adrkom=$a[7] if $a[7] ne '-1';
		$kom=$a[8] if $a[8] ne '-1';
		$p_kop=$a[9] if $a[9] ne '-1';
		$ind=$a[10] if $a[10] ne '-1';
		$p_a=$a[11] if $a[11] ne '-1';
		$p_ia=$a[12] if $a[12] ne '-1';
		$p_sum=$a[13] if $a[13] ne '-1';
		$p_rez1=$a[14] if $a[14] ne '-1';
		$p_pr=$a[15] if $a[15] ne '-1';
      }
    }
  }
}


