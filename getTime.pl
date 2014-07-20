#!/usr/bin/perl -w 
$dir = $ARGV[0];
my @dir_vec = `ls $dir`;
open TIME,">time.txt";
foreach (@dir_vec) {
	chomp;
	print $_."\n";
	my $new_dir = "${dir}/${_}/time";
	my @temp_vec = `ls $new_dir`;
	print TIME "${dir}_${_}\n";
	foreach(@temp_vec) {
		chomp;
		my $filepath ="${new_dir}/${_}";
		open TEMP_FILE, "<$filepath";
		my $temp_str = <TEMP_FILE>;
		my ($time) = $temp_str =~ m{real\s+(\S+)};
		defined time or die 'cant parse';
		print TIME $time."\n";
		close TEMP_FILE;
	}
}
close TIME;