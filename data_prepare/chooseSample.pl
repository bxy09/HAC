#!/usr/bin/perl -w
use Data::Dumper;
my $workMode = $ARGV[0];
my $sampleRatio = $ARGV[1];

my $classInfoPath = $ARGV[2];
my $labelMarkedPath = $ARGV[3];
$workMode eq "even" or $workMode eq "random" or die 'Error:mode error';
open FIN,"<$classInfoPath" or die "Error:can\'t open $classInfoPath";
open MARKOUT,">$labelMarkedPath" or die "Error:can\'t open $labelMarkedPath";
my $tempStr = <FIN>;
my ($classDepth,$classNum,$docNumInSum) = $tempStr =~ m{ClassDepth:(\d+)\s+ClassNum:(\d+)\s+DocNumInSum:(\d+)};
my $sampleNum = int($sampleRatio*$docNumInSum);
my @classInfo;
my @chooseSample;
my $SampleNumInClassForEven = int($sampleNum/$classNum);
if($workMode eq "random") {
	my $DownCount = $sampleNum;
	my %chooseSample;
	while($DownCount != 0) {
		$newNum = int(rand($docNumInSum));
		next if(defined $chooseSample{$newNum});
		$chooseSample{$newNum} = 1;
		$DownCount --;
	}
	@chooseSample = sort{-($a<=>$b)} keys %chooseSample;
}else {
	$sampleNum = 0;
}
#print "Dumper:".Dumper(@chooseSample)."\n";
my $cumulateDocNum = 0;
while(<FIN>) {
	my $info;
	my @temp = split /\s+/,$_;
	my $docNumStr = pop @temp;
	my ($docNum) = $docNumStr =~ m{DocNum:(\d+)};

	my @classDocs;
	if($workMode eq 'random') {
		my $ceil = $cumulateDocNum + $docNum;
		while( scalar @chooseSample and $chooseSample[scalar @chooseSample - 1] < $ceil) {
			push @classDocs, (pop @chooseSample);
		}
	} else {
		my %selectDocsInClass;
		my $selectDocNumInClass = int($docNum*$sampleRatio);
		$selectDocNumInClass = 1 if($selectDocNumInClass == 0);
		$sampleNum += $selectDocNumInClass;
		my $DownCount = $selectDocNumInClass;
		while($DownCount != 0) {
			$newNum = int(rand($docNum)) + $cumulateDocNum;
			next if(defined $selectDocsInClass{$newNum});
			$selectDocsInClass{$newNum} = 1;
			$DownCount --;
		}
		@classDocs = sort{$a<=>$b} keys %selectDocsInClass;
	}
	$cumulateDocNum += $docNum;
	$info = {docNum=>$docNum,classInfo=>\@temp,classDocs=>\@classDocs};
	push @classInfo,$info;
}
close FIN;
print MARKOUT $classDepth."\n";
print MARKOUT $sampleNum;
foreach(1..$classDepth) {
	print MARKOUT " ".$classInfo[scalar @classInfo - 1]->{classInfo}[$classDepth - $_];
}
print MARKOUT "\n";
foreach $class(@classInfo) {
	my @docs = @{$class->{classDocs}};
	foreach(@docs) {
		print MARKOUT $_;
		foreach(1..$classDepth) {
			print MARKOUT " ".$class->{classInfo}[$classDepth - $_];
		}
		print MARKOUT "\n";
	}
}
close MARKOUT;
