my $dir = '/home/';# откуда всё начинается
&recur($dir);

my %file_info = ();#

sub recur{
    my $dir = shift;
    opendir DIR, $dir or return;
    my @contents = map "$dir/$_", sort grep !/^\.\.?$/, readdir DIR;
    closedir DIR;
    foreach (@contents){
        if (!-l && -d){#-d Файл является каталогом, -l Файл является символической ссылкой
            recur($_);
        }
        else {

		print $_."\n";

		($file_info{dev},$file_info{ino},$file_info{mode},$file_info{nlink},
		$file_info{uid},$file_info{gid},$file_info{rdev},$file_info{size},$file_info{atime},
		$file_info{mtime},$file_info{ctime},$file_info{blksize},
		$file_info{blocks}) = stat($_);
		print $file_info{nlink}."\n\n";
        }
    }
}
