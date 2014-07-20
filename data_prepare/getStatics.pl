#!/usr/bin/perl -w
use Data::Dumper;
my $currentDir = `pwd`;
scalar @ARGV == 0 or chdir($ARGV[0]);
my @folders = `ls -d */`;
print scalar @folders;
open DOCNUM,">docNum";
my $docNumSum = 0;
my $docNumHash;
foreach (@folders) {
	chomp;
	my @Docfiles =  `ls $_|grep doc`;
	scalar @Docfiles == 1 or die '$_  get .doc file false';
	my $docPath = $_.$Docfiles[0];
	chomp $docPath;
	my $docNum = `wc -l <$docPath`;
	chomp $docNum;
	$docNumSum += $docNum;
	my $className = substr $_,0,length($_)-1;
	my $ref = \$docNumHash;
	foreach (split /\./,$className) {
		my %newHash;
		defined $$ref->{$_} or $$ref->{$_} = \%newHash;
		$ref = \($$ref->{$_});
	}
	my @temp;
	$$ref = \@temp;
	$$$ref [0] = $className;
	$$$ref [1] = $docNum
}
print DOCNUM $docNumSum."\t".scalar @folders."\n";
my @refStack;
my @nameStack;
push @refStack,$docNumHash;
push @nameStack,'';
while(@refStack!=0) {
	my $currentRef = pop @refStack;
	my $currentName = pop @nameStack;
	if(ref($currentRef) ne "HASH") {
		$currentName = substr $currentName,1,(length($currentName) - 1);
		print DOCNUM $currentName."\t".@{$currentRef}[0]."\t".@{$currentRef}[1]."\n";
		next;
	}
	if(scalar keys(%{$currentRef}) == 1) {
		my($useless, $value) = each %{$currentRef};
		$currentRef = $value;
		push @refStack,$currentRef;
		push @nameStack,$currentName;
		next;
	}
	foreach my $key (reverse sort keys %{$currentRef}) {
		push @refStack,$currentRef->{$key};
		push @nameStack,$currentName.'.'.$key;
	}
}
close DOCNUM;