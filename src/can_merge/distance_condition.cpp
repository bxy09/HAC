#include "hc_tree.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
static ifstream fp_distance;
extern int *doc_in_cluster;
int *cluster_labeled_size;
int *cluster_first_label_doc_id;
int *distance_of_label_doc;
static bool read_distance();
bool read_merge_condition(int argc, char* argv[]){
	if(argc < 1) {
		cout<<"parameter error!"<<endl;
		return -1;
	}
	fp_distance.open(argv[0]);
	if(fp_distance == NULL)
	{
		cout << "File distance("<<argv[0]<<") cannot open!";
		return -1;
	}
	cluster_first_label_doc_id = new int[doc_num*2];
	cluster_labeled_size = new int[doc_num*2];
	distance_of_label_doc = new int[doc_num*doc_num];
	return read_distance();
}
static bool read_distance(){
	int doc_id1;
	int doc_id2;
	int doc_distance;
	
	for(int i = 0; i < doc_num; i++){
		cluster_first_label_doc_id[i] = doc_num;
		cluster_labeled_size[i] = 0;
	}
	for(int i = 0; i < doc_num*doc_num; i++){
		distance_of_label_doc[i] = 0;
	}
	while(fp_distance >> doc_id1){
		fp_distance >> doc_id2;
		fp_distance >> doc_distance;
		cluster_first_label_doc_id[doc_id1] = doc_id1;
		cluster_first_label_doc_id[doc_id2] = doc_id2;
		cluster_labeled_size[doc_id1] = 1;
		cluster_labeled_size[doc_id2] = 1;
		if(doc_id2 < doc_id1) {
			int temp = doc_id1;
			doc_id1 = doc_id2;doc_id2 = temp;
		}
		distance_of_label_doc[doc_id1*doc_num+doc_id2] = doc_distance;
	}
	return 1;
}
void when_merge(int node1, int node2, int endNode){
	cluster_first_label_doc_id[endNode] = cluster_first_label_doc_id[node1];
	if(cluster_first_label_doc_id[node2] < cluster_first_label_doc_id[endNode]) {
		cluster_first_label_doc_id[endNode] = cluster_first_label_doc_id[node2];
	}
	cluster_labeled_size[endNode] = cluster_labeled_size[node1] +  cluster_labeled_size[node2];
}
bool can_merge(int node1, int node2){
	int label_num1 = cluster_labeled_size[node1];
	int label_num2 = cluster_labeled_size[node2];
	if(label_num1 == 0 ||
		label_num2 == 0) {
		return 1;
	}
	int doc_id1 = cluster_first_label_doc_id[node1];
	int doc_id2 = cluster_first_label_doc_id[node2];
	if(doc_id1 == doc_id2) {
		cout<<"Error:wtf in can_merge under distance condition"<<endl;
		exit(0);
	}
	if(doc_id1 > doc_id2) {
		int  temp = doc_id1;doc_id1 = doc_id2;doc_id2 = temp;
	}
	if(label_num1 + label_num2 >= distance_of_label_doc[doc_id1*doc_num+doc_id2]) {//could change to >
		return 1;
	}
	return 0;
}
