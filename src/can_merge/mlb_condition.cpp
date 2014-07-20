#include "hc_tree.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
static ifstream fp_constraints;
static ofstream fout;
extern int *doc_in_cluster;
struct constraint{
	int doc_id1;
	int doc_id2;
	int doc_id3;
};
static vector<struct constraint> constraints;
int *constrait_index_for_doc;
int *constraint_num_for_doc;
static bool read_constraints();
bool read_merge_condition(int argc, char* argv[]){
	if(argc < 1) {
		cout<<"parameter error!"<<endl;
		return -1;
	}
	fp_constraints.open(argv[0]);
	if(fp_constraints == NULL)
	{
		cout << "File constraints("<<argv[0]<<") cannot open!";
		return -1;
	}
	constrait_index_for_doc = new int[doc_num];
	constraint_num_for_doc = new int[doc_num];
	return read_constraints();
}
static bool read_constraints(){
	constraints.clear();
	int doc_id1;
	int doc_id2;
	int doc_id3;
	struct constraint temp_constraint;
	int temp_doc_id3 = -1; 
	for(int i = 0; i < doc_num; i++){
		constrait_index_for_doc[i] = -1;
		constraint_num_for_doc[i] = 0;
	}
	while(fp_constraints >> temp_constraint.doc_id1){
		fp_constraints >> temp_constraint.doc_id2;
		fp_constraints >> temp_constraint.doc_id3;
		if(temp_constraint.doc_id3 != temp_doc_id3 || temp_doc_id3 == -1){
			temp_doc_id3 = temp_constraint.doc_id3;
			constrait_index_for_doc[temp_constraint.doc_id3] = constraints.size();
			if(constraint_num_for_doc[temp_constraint.doc_id3] != 0) {
				cout<<"constraints id3 is not sorted!"<<endl;
				return -1;
			}
		}
		constraint_num_for_doc[temp_constraint.doc_id3] ++;
		constraints.push_back(temp_constraint);
	}
	return 1;
}
static bool is_contain(int id, struct cluster_node& cluster_node1){
	return binary_search(cluster_node1.id.begin(),cluster_node1.id.end(),id);
}
static bool NotHaveBCA2(int node1, int node2) {
	int p2 = 0;
	int p2_end = 0;
	int k;

	for(int p1 = 0; p1 < cluster_tree[node1].id.size(); p1++){
		p2 = constrait_index_for_doc[cluster_tree[node1].id[p1]];
		int p2_start = p2;
		if(p2 != -1){
			p2_end = p2 + constraint_num_for_doc[cluster_tree[node1].id[p1]];
			p2 = p2_end - 1;

			while(p2 >= p2_start){
				int cluster_id1 = doc_in_cluster[constraints[p2].doc_id1];
				int cluster_id2 = doc_in_cluster[constraints[p2].doc_id2];
				if(cluster_id1 == cluster_id2) {
					constraints[p2] = constraints[p2_end - 1];
					constraint_num_for_doc[cluster_tree[node1].id[p1]] --;
					p2_end --;
					p2 --;continue;
				} else if(cluster_id1 == node2 || cluster_id2 == node2) {
					return false;
				}
				p2--;
			}
		}
	}
	return true;
}
static bool NotHaveBCA(int node1, int node2) {
	int p2 = 0;
	int p2_end = 0;
	int k;
	for(int p1 = 0; p1 < cluster_tree[node1].id.size(); p1++){
		p2 = constrait_index_for_doc[cluster_tree[node1].id[p1]];
		if(p2 != -1){
			p2_end = p2 + constraint_num_for_doc[cluster_tree[node1].id[p1]];
			while(p2 < p2_end){
				int cluster_id1 = doc_in_cluster[constraints[p2].doc_id1];
				int cluster_id2 = doc_in_cluster[constraints[p2].doc_id2];
				if(cluster_id1 == node2 && cluster_id2 != node2 || cluster_id2 == node2 && cluster_id1 != node2) {
					return false;
				}
				else{
					p2++;
				}
			}
		}
	}
	return true;
}

bool can_merge(int node1, int node2){
	return NotHaveBCA2(node1,node2) && NotHaveBCA2(node2,node1);
}
void when_merge(int node1, int node2, int endNode){}