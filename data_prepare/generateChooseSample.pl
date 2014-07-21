#!/usr/bin/perl -w
$ifGenerateWeight = 1;
%newsgroup = ('flag','0','address','data/20_newsgroups/','sample_mode','even',
	'num','2000','sample_ratio','0.15','repeat_for_each_mat',1,'mat_count',1);
%mercury = ('flag','0','address','data/mercury/','sample_mode','even',
	'num','1800','sample_ratio','0.15','repeat_for_each_mat',1,'mat_count',1);
%reuters1 = ('flag','0','address','data/reuters1/','sample_mode','even',
	'num','1059','sample_ratio','0.1','repeat_for_each_mat',1,'mat_count',1);
%reuters2 = ('flag','0','address','data/reuters2/','sample_mode','even',
	'num','900','sample_ratio','0.1','repeat_for_each_mat',1,'mat_count',1);
%audit = ('flag','1','address','data/audit/','sample_mode','even',
	'num','288','sample_ratio','0.1','repeat_for_each_mat',1,'mat_count',1);

foreach $ratio(0.05,0.15,0.1) {
	foreach my $mode('random'){

		foreach my $num(5794,6594) {#(550,1059,2059,3059,4059) {
			$reuters1{num} = $num;
			$reuters1{sample_ratio} = $ratio;
			$reuters1{sample_mode} = $mode;
			generate_for_reuters(\%reuters1);
		}
		foreach my $num(4553,5353,6153) {#(450,900,1753,2553,3353) {
			$reuters2{num} = $num;
			$reuters2{sample_ratio} = $ratio;
			$reuters2{sample_mode} = $mode;
			generate_for_reuters(\%reuters2);
		}
		foreach my $num(6384) {#(900,2620,4607,5243) {!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!6384
			$mercury{num} = $num;
			$mercury{sample_ratio} = $ratio;
			$mercury{sample_mode} = $mode;
			generate(\%mercury);
		}
		foreach my $num(6000) {#(500,1000,3000,4000,5000) {
			$newsgroup{num} = $num;
			$newsgroup{sample_ratio} = $ratio;
			$newsgroup{sample_mode} = $mode;
			generate(\%newsgroup);
		}
		$audit{sample_ratio} = $ratio;
		$audit{sample_mode} = $mode;
		generate(\%audit);
	}
}

sub generate {
	%info = %{$_[0]};
	return 0 if(!$info{flag});
	foreach my $mat_index(1..$info{mat_count}) {
		my $matAddress = $info{address}.'even_'.$info{num}.'_'.$mat_index;
		foreach my $sample_index(1..$info{repeat_for_each_mat}) {
			my $sampleAddress = $matAddress."/sample_".$info{sample_mode}.'_'.$info{sample_ratio}.'_'.$sample_index;
			print "generate $sampleAddress\n";
			system ("rm -rf $sampleAddress");
			system ("mkdir $sampleAddress");
			#label_marked
			system ("data_prepare/chooseSample.pl $info{sample_mode} $info{sample_ratio} $matAddress/classInfo ".
				"$sampleAddress/label_marked");
			#weight
			if($ifGenerateWeight) {
				system ("bin/generate_ha_constraints $sampleAddress/label_marked");
				system ("rm ./weight") if(-e './weight');
				system ("bin/weight $matAddress/mat ./mlb_constraints 10");
				system ("rm ./mlb_constraints");
				system ("mv ./weight $sampleAddress/weight");
				system ("python data_prepare/annan/learnWeights1228byfour.py $matAddress/mat $sampleAddress/label_marked $sampleAddress/weight_annan")
			}
			#constraint
			system ("bin/generate_constraints $sampleAddress/label_marked");
			system ("sort -k 3 -k 1 -k 2 -u -n constraints > constraints_temp");
			system ("rm ./constraints");
			system ("mv ./constraints_temp $sampleAddress/constraints");
			#distance and  metric_rt_guide
			system ("bin/generate_distance $matAddress/mat $sampleAddress/label_marked");
			system ("mv ./distance $sampleAddress/distance");
			system ("mv ./guide $sampleAddress/guide");
		}
	}
}
sub generate_for_reuters {
	%info = %{$_[0]};
	return 0 if(!$info{flag});
	foreach my $mat_index(1..$info{mat_count}) {
		my $matAddress = $info{address}.'even_'.$info{num}.'_'.$mat_index;
		system ("data_prepare/doc2mat -nlskip=1 -skipnumeric=1 ${matAddress}/doc_mat ${matAddress}/mat");
		foreach my $sample_index(1..$info{repeat_for_each_mat}) {
			my $sampleAddress = $matAddress."/sample_".$info{sample_mode}.'_'.$info{sample_ratio}.'_'.$sample_index;
			print "generate $sampleAddress\n";
			#system ("rm -rf $sampleAddress");
			#system ("mkdir $sampleAddress");
			#label_marked
			#system ("data_prepare/chooseSample.pl $info{sample_mode} $info{sample_ratio} $matAddress/classInfo ".
			#	"$sampleAddress/label_marked");
			#weight
			if($ifGenerateWeight) {
				system ("bin/generate_ha_constraints $sampleAddress/label_marked");
				system ("rm ./weight") if(-e './weight');
				system ("bin/weight $matAddress/mat ./mlb_constraints 10");
				system ("rm ./mlb_constraints");
				system ("mv ./weight $sampleAddress/weight");
				system ("python data_prepare/annan/learnWeights1228byfour.py $matAddress/mat $sampleAddress/label_marked $sampleAddress/weight_annan")
			}
			#constraint
			system ("bin/generate_constraints $sampleAddress/label_marked");
			system ("sort -k 3 -k 1 -k 2 -u -n constraints > constraints_temp");
			system ("rm ./constraints");
			system ("mv ./constraints_temp $sampleAddress/constraints");
			#distance and  metric_rt_guide
			system ("bin/generate_distance $matAddress/mat $sampleAddress/label_marked");
			system ("mv ./distance $sampleAddress/distance");
			system ("mv ./guide $sampleAddress/guide");
		}
	}
}
