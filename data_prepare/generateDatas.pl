#!/usr/bin/perl -w
%newsgroup = ('flag','1','address','data/20_newsgroups/','mode','even','num','2000','repeat',1);
foreach my $size(6000,8000) {#(500,1000,3000,4000,5000) {
	$newsgroup{num} = $size;
	generate(\%newsgroup);	
}
#generate(\%san);
#generate(\%reuters);
sub generate{
	%info = %{$_[0]};
	return 0 if(!$info{flag});
	system ("data_prepare/getStatics.pl $info{address}docs");
	foreach(1..$info{repeat}) {
		my $feature = $info{mode}.'_'.$info{num}.'_'.$_;
		system ("data_prepare/getRandomFileMat.pl $info{mode} $info{num} $info{address}");
		my $matAddress = $info{address}.$feature;
		system ("rm -rf $matAddress");
		system ("mkdir $matAddress");
		#system (":>$info{address}doc_$feature");
		#system (":>$info{address}mat_$feature");
		#system (":>$info{address}classInfo_$feature");
		system ("mv $info{address}docMat       ${matAddress}/doc");
		system ("mv $info{address}docClass     ${matAddress}/classInfo");
		system ("mv $info{address}label_bottom ${matAddress}/label_bottom");
		system ("mv $info{address}label_middle ${matAddress}/label_middle");
		system ("mv $info{address}label_top   ${matAddress}/label_top");
		system ("data_prepare/doc2mat -nlskip=1 -skipnumeric=1 ${matAddress}/doc ${matAddress}/mat");
	}
}