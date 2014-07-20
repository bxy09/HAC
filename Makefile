all: new util old
new: hc mlb rt hc_m mlb_m rt_g rt_m_g
util: weight generate_distance fscore generate_ha_constraints generate_constraints
old: old_hc old_mlb old_rt
.PHONY:all,new,old,util,clean,tar
hc_lib_dir =  ./src/hc_tree_lib
old_l =  ./src/old
can_merge_dir = ./src/can_merge
prepare_data_dir = ./data_prepare
weight_dir = ./src/weight
i_rbt_dir = -I./src/rbt/
i_hc_lib_dir = -I./src/hc_tree_lib/
#new hac
hc:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/blank.o $(weight_dir)/blank.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/blank.o $(weight_dir)/blank.o ./src/hac_main.o -o ./bin/hc
./src/hac_main.o: ./src/hac_main.cpp
	g++ $(i_rbt_dir) $(i_hc_lib_dir) ./src/hac_main.cpp -c -o ./src/hac_main.o
$(hc_lib_dir)/hc_tree.o:$(hc_lib_dir)/hc_tree.cpp
	g++ $(i_rbt_dir) $(hc_lib_dir)/hc_tree.cpp -c -o $(hc_lib_dir)/hc_tree.o
$(can_merge_dir)/blank.o: $(can_merge_dir)/blank.cpp
	g++ $(can_merge_dir)/blank.cpp -c -o $(can_merge_dir)/blank.o
$(weight_dir)/blank.o: $(weight_dir)/blank.cpp
	g++ $(weight_dir)/blank.cpp -c -o $(weight_dir)/blank.o
#new mlb
mlb:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/mlb_condition.o $(weight_dir)/blank.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/mlb_condition.o $(weight_dir)/blank.o ./src/hac_main.o -o ./bin/mlb
$(can_merge_dir)/mlb_condition.o:$(can_merge_dir)/mlb_condition.cpp
	g++ $(i_rbt_dir) $(i_hc_lib_dir) $(can_merge_dir)/mlb_condition.cpp -c -o $(can_merge_dir)/mlb_condition.o
#new rt
rt:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/distance_condition.o $(weight_dir)/blank.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/distance_condition.o $(weight_dir)/blank.o ./src/hac_main.o -o ./bin/rt
$(can_merge_dir)/distance_condition.o:$(can_merge_dir)/distance_condition.cpp
	g++ $(i_rbt_dir) $(i_hc_lib_dir) $(can_merge_dir)/distance_condition.cpp -c -o $(can_merge_dir)/distance_condition.o
#new hac+metric
hc_m:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/blank.o $(weight_dir)/average_weight.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/blank.o $(weight_dir)/average_weight.o ./src/hac_main.o -o ./bin/hc_m
$(weight_dir)/average_weight.o: $(weight_dir)/average_weight.cpp
	g++ $(weight_dir)/average_weight.cpp -c -o $(weight_dir)/average_weight.o
#new mlb+metric
mlb_m:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/mlb_condition.o $(weight_dir)/average_weight.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/mlb_condition.o $(weight_dir)/average_weight.o ./src/hac_main.o -o ./bin/mlb_m
#new rt+guide
rt_g:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/distance_level.o $(weight_dir)/blank.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/distance_level.o $(weight_dir)/blank.o ./src/hac_main.o -o ./bin/rt_g
#new rt+guide+metric
rt_m_g:$(hc_lib_dir)/hc_tree.o ./src/hac_main.o $(can_merge_dir)/distance_level.o $(weight_dir)/level_weight.o
	g++ $(hc_lib_dir)/hc_tree.o $(can_merge_dir)/distance_level.o $(weight_dir)/level_weight.o ./src/hac_main.o -o ./bin/rt_m_g
$(weight_dir)/level_weight.o: $(weight_dir)/level_weight.cpp
	g++ $(weight_dir)/level_weight.cpp -c -o $(weight_dir)/level_weight.o
$(can_merge_dir)/distance_level.o:$(can_merge_dir)/distance_level.cpp
	g++ $(i_rbt_dir) $(i_hc_lib_dir) $(can_merge_dir)/distance_level.cpp -c -o $(can_merge_dir)/distance_level.o

#old source
old_hc:$(old_l)/traditional_hc.cpp
	g++ $(old_l)/traditional_hc.cpp -o ./bin/old_hc
old_mlb: $(old_l)/mlb.cpp
	g++ $(old_l)/mlb.cpp -o ./bin/old_mlb
old_rt: $(old_l)/rt.cpp
	g++ $(old_l)/rt.cpp -o ./bin/old_rt
#prepare data
generate_distance: $(prepare_data_dir)/my_generate_distance.o $(hc_lib_dir)/hc_tree.o $(weight_dir)/blank.o
	g++ $(i_rbt_dir) $(prepare_data_dir)/my_generate_distance.o $(hc_lib_dir)/hc_tree.o $(weight_dir)/blank.o -o ./bin/generate_distance
$(prepare_data_dir)/my_generate_distance.o: $(prepare_data_dir)/my_generate_distance.cpp
	g++ $(i_rbt_dir) $(i_hc_lib_dir) \
	 $(prepare_data_dir)/my_generate_distance.cpp -c -o $(prepare_data_dir)/my_generate_distance.o
weight: ./data_prepare/mlb_weight.cpp
	g++ ./data_prepare/mlb_weight.cpp -o ./bin/weight
generate_ha_constraints: ./data_prepare/generate_hierarchy_constraints.cpp
	g++ ./data_prepare/generate_hierarchy_constraints.cpp -o ./bin/generate_ha_constraints
generate_constraints: ./data_prepare/generate_constraints.cpp
	g++ ./data_prepare/generate_constraints.cpp -o ./bin/generate_constraints
#utils
fscore: ./src/fscore/fscore.cpp
	g++ ./src/fscore/fscore.cpp -o ./bin/fscore
clear:
	-rm ./bin/*
	-find . -name '*.o' -type f -print -exec rm -rf {} \;
tar:
	tar cvzf hc.tar.gz src
