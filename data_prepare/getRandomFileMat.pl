#!/usr/bin/perl -w
use Data::Dumper;
my $currentDir = `pwd`;
my $workMode = $ARGV[0];
$workMode eq "even" or $workMode eq "random" or die 'Error:mode error';
my $selectDocNum = $ARGV[1];
scalar @ARGV == 2 or chdir($ARGV[2]);
open DOCNUM,"<docs/docNum" or die 'Error:can\'t open docNum file';
my $tempStr = <DOCNUM>;
chomp $tempStr;
my ($docNumSum,$classNum) = $tempStr =~ m{(\d+)\s+(\d+)};
$selectDocNum < $docNumSum or die 'Error:select doc num is bigger than doc sum!!';
my $docNumHash;
my $leastNumInClass = 10000000;
my $classDepth = 0;
my %selectDocs;
my $newNum;
srand();
if($workMode eq 'random') {
	my $DownCount = $selectDocNum;
	while($DownCount != 0) {
		$newNum = int(rand($docNumSum));
		next if(defined $selectDocs{$newNum});
		$selectDocs{$newNum} = 1;
		$DownCount --;
	}
}
my @selectDocs = sort{-($a<=>$b)} keys %selectDocs;
my $docNumForClassInEven = int($selectDocNum/$classNum);
my $cumulateDocNum = 0;
while(<DOCNUM>) {
	my($className,$classFolder,$docNum) = m{(\S+)\s+(\S+)\s+(\d+)};
	my $ref = \$docNumHash;
	my @classDocs;
	if($workMode eq 'random') {
		my $ceil = $cumulateDocNum + $docNum;
		while( scalar @selectDocs and $selectDocs[scalar @selectDocs - 1] < $ceil) {
			push @classDocs, (pop @selectDocs) - $cumulateDocNum;
		}
	} else {
		my %selectDocsInClass;
		$docNum > $docNumForClassInEven or 
			die "Error:class $classFolder don't have $docNumForClassInEven docs!!";
		my $DownCount = $docNumForClassInEven;
		while($DownCount != 0) {
			$newNum = int(rand($docNum));
			next if(defined $selectDocsInClass{$newNum});
			$selectDocsInClass{$newNum} = 1;
			$DownCount --;
		}
		@classDocs = sort{$a<=>$b} keys %selectDocsInClass;
	}
	$cumulateDocNum += $docNum;
	next if(scalar @classDocs == 0);
	foreach (split /\./,$className) {
		my %newHash;
		defined $$ref->{$_} or $$ref->{$_} = \%newHash;
		$ref = \($$ref->{$_});
	}
	my @temp;
	$$ref = \@temp;
	$$$ref [0] = $classFolder;
	$$$ref [1] = $docNum;
	$$$ref [2] = \@classDocs;

	my $currentClassDepth = scalar split /\./,$className;
	$currentClassDepth < $classDepth or $classDepth = $currentClassDepth;
}
my @ref;
my $rootNode = {name => '',ref => $docNumHash};
push @ref,$rootNode;
foreach(1..$classDepth) {
	my @newRef;
	my $count = 0;
	foreach(@ref) {
		if(ref($_->{ref}) ne "HASH") {
			$count ++;
			my $node = {name => $_->{name}." $count",ref => $_->{ref}};
			push @newRef, $node;
			next;
		}
		foreach my $key (sort keys %{$_->{ref}}) {
			$count ++;
			my $node = {name => $_->{name}." $count",ref=> $_->{ref}->{$key}};
			push @newRef, $node;
		}
	}
	@ref = @newRef;
}
open DOCMAT,">docMat" or die "Error:Can\'t open docMat";
open DOCCLASS,">docClass" or die "Error:Can\'t open docClass";
#only print the lowest 3 level label
open BOTTOM_LABEL,">label_bottom" or die "Can\'t open label_bottom";
open MIDDLE_LABEL,">label_middle" or die "Can\'t open label_middle";
open HIGH_LABEL,">label_top" or die "Can\'t open label_top";
$selectDocNum = $docNumForClassInEven * scalar @ref if ($workMode eq 'even');
print DOCCLASS "ClassDepth:".$classDepth."\tClassNum:".
	scalar @ref."\tDocNumInSum:".$selectDocNum."\n";
foreach $classRef (@ref) {
	my @class = @{$classRef->{ref}};
	print $class[0].":\n\t".$classRef->{name}."\n";
	print "\tchoose ".scalar scalar @{$class[2]}." docs\n";
	my @DocPath =  `ls docs/$class[0]|grep doc`;
	scalar @DocPath == 1 or die 'Error $class[0]  get .doc file false';
	my $docPath = $class[0].'/'.$DocPath[0];
	open DOC,"<docs/$docPath" or die "Error:Can\'t open $docPath";
	my $count = 0;
	my $index = 0;
	$classRef->{name} = substr $classRef->{name},1,length($classRef->{name})-1;
	my @nameInfo = split /\s/,$classRef->{name};
	@nameInfo = reverse @nameInfo;
	while(scalar @nameInfo < 3) {
		push @nameInfo, @nameInfo[(scalar @nameInfo) - 1];
	}
	while(<DOC>) {
		if($count == $class[2][$index]) {
			print BOTTOM_LABEL $nameInfo[0]."\n";
			print MIDDLE_LABEL $nameInfo[1]."\n";
			print HIGH_LABEL $nameInfo[2]."\n";
			print DOCMAT $_;
			$index ++;
		}
		last if($index == scalar @{$class[2]});
		$count ++;
	}
	close DOC;
	print DOCCLASS $classRef->{name};
	print DOCCLASS "\tDocNum:".scalar scalar @{$class[2]}."\n";
}
close DOCMAT;
close DOCCLASS;
close BOTTOM_LABEL;
close MIDDLE_LABEL;
close HIGH_LABEL;