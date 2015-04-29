#!/usr/bin/perl -w
my ${basic_path}=$ARGV[0];	
#${group_gap}=$ARGV[1];
#mercury size = 1800;
#20news = 2000;
#audit = 288;
#reuters1 = 1059;
#reuters2 = 900;
my %newsgroup = (basic_path=>'data/20_newsgroups',basic_result_path=>'result_20news',tag=>0);
my %mercury = (basic_path=>'data/mercury',basic_result_path=>'result_mercury',tag=>0);
my %reuters1 = (basic_path=>'data/reuters1',basic_result_path=>'result_reuters1',tag=>0);
my %reuters2 = (basic_path=>'data/reuters2',basic_result_path=>'result_reuters2',tag=>0);
my %audit = (basic_path=>'data/audit',basic_result_path=>'result_audit',tag=>1);
foreach my $num(5794,6594) {
	;#run($reuters1{basic_path},$reuters1{basic_result_path},$num);
}
foreach my $num(4553,5353,6153) {
	;#run($reuters2{basic_path},$reuters2{basic_result_path},$num);
}
foreach my $num(6384) {#(900,1800,2620,4607,5243) {
	;#run($mercury{basic_path},$mercury{basic_result_path},$num);
}
foreach my $num(2000) {#8000(500,1000,2000,3000,4000,5000) {
	;#run($newsgroup{basic_path},$newsgroup{basic_result_path},$num);
}

run($audit{basic_path},$audit{basic_result_path},288);
sub run{
	my ${basic_path} = $_[0];
	my ${basic_result_path} = $_[1];
	my ${data_size} = $_[2];
	foreach $sample_mode('random') {
		my @ratio_vec = ('0.1','0.05','0.15');
		foreach $ratio(@ratio_vec) {
			my ${result_path} = "${basic_result_path}/result_${data_size}_${sample_mode}_${ratio}";
			-e ${basic_result_path} or `mkdir ${basic_result_path}`;
			`rm -r ${result_path}` if -e ${result_path};
			`mkdir ${result_path}`;
			`mkdir ${result_path}/bottom_level`;
			`mkdir ${result_path}/middle_level`;
			`mkdir ${result_path}/top_level`;
			`mkdir ${result_path}/tree`;
			`mkdir ${result_path}/label_marked`;
			`mkdir ${result_path}/time`;
			foreach $data_selection_index(1..1) {
				my $path = $basic_path."/even_".$data_size."_".$data_selection_index;
				my ${data_path}="${path}/mat";
				my ${label_bottom_path}="${path}/label_bottom";
				my ${label_middle_path} = "${path}/label_middle";
				my ${label_top_path} = "${path}/label_top";
				foreach $sample_index(1..1) {
					my ${sample_path} = "${path}/sample_${sample_mode}_${ratio}_${sample_index}";
					my ${constraints_path}="${sample_path}/constraints";
					my ${distance_path}="${sample_path}/distance";
					my ${weight_path}="${sample_path}/weight";
					my $guide_path="${sample_path}/guide";

					my ${cluster_tree_path}="./cluster_tree";
					my ${label_marked_path}="./cluster_tree_label";

					my $tra_tag = 1;
					my $mlb_tag = 1;
					my $rt_tag = 1;
					my $tra_m_tag = 1;
					my $annan_m_tag = 1;
					my $mlb_m_tag = 1;
					my $rt_m_tag = 1;

					my @level_name = qw 'bottom_level middle_level top_level';
					my $path_map;
					$path_map -> {"bottom_level"} = "${path}/label_bottom";
					$path_map -> {"middle_level"} = "${path}/label_middle";
					$path_map -> {"top_level"} = "${path}/label_top";

					print "Start! $constraints_path\n";
					if($tra_tag){
						print "tra\n";
						system("(time -p ./bin/hc ${data_path} ${label_bottom_path}) 2>> ${result_path}/time/traditional");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/traditional");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/traditional");
						system("mv $label_marked_path ${result_path}/label_marked/traditional");
					}
					if($tra_m_tag){
						print "tra_m\n";
						system("(time -p ./bin/hc_m ${data_path} ${label_bottom_path} $weight_path) 2>> ${result_path}/time/traditional_m");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/traditional_m");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/traditional_m");
						system("mv $label_marked_path ${result_path}/label_marked/traditional_m");
					}
					if($annan_m_tag){
						print "annan_m\n";
						system("(time -p ./bin/hc_m ${data_path} ${label_bottom_path} ${sample_path}/weight_annan) 2>> ${result_path}/time/annan_m");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/annan_m");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/annan_m");
						system("mv $label_marked_path ${result_path}/label_marked/annan_m");
					}
					if($rt_tag){
						print("distance\n");
						system("(time -p ./bin/rt ${data_path} ${label_bottom_path} $distance_path) 2>> ${result_path}/time/distance");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/distance");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/distance");
						system("mv $label_marked_path ${result_path}/label_marked/distance");
					}
					if($rt_m_tag){
						print("distance_m\n");
						system("(time -p ./bin/rt_m_g ${data_path} ${label_bottom_path} $weight_path $distance_path $guide_path) 2>> ${result_path}/time/distance_m");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/distance_m");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/distance_m");
						system("mv $label_marked_path ${result_path}/label_marked/distance_m");
					}
					if($rt_m_tag){
						print("distance_g\n");
						system("(time -p ./bin/rt_g ${data_path} ${label_bottom_path} $distance_path $guide_path) 2>> ${result_path}/time/distance_g");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/distance_g");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/distance_g");
						system("mv $label_marked_path ${result_path}/label_marked/distance_g");
					}
					if($mlb_tag){
						print("mlb\n");
						system("(time -p ./bin/mlb ${data_path} ${label_bottom_path} $constraints_path) 2>> ${result_path}/time/mlb");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/mlb");
							system("rm ./fscore");
						}
						#system("mv $cluster_tree_path ${result_path}/tree/mlb");
						#system("mv $label_marked_path ${result_path}/label_marked/mlb");
					}

					if($mlb_m_tag){
						print("mlb_m\n");
						system("(time -p ./bin/mlb_m ${data_path} ${label_bottom_path} $weight_path $constraints_path) 2>> ${result_path}/time/mlb_m");
						foreach(@level_name) {
							system("./bin/fscore $path_map->{$_} ${cluster_tree_path}");
							system("cat ./fscore >> ${result_path}/$_/mlb_m");
							system("rm ./fscore");
						}
						system("mv $cluster_tree_path ${result_path}/tree/mlb_m");
						system("mv $label_marked_path ${result_path}/label_marked/mlb_m");
					}
				}
			}
		}
	}
}
