#include "hc_tree.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <algorithm>
using namespace std;
struct cluster_guide{
	int head;
	int size;
	bool fulfill;
};

static ifstream fp_distance;
extern int *doc_in_cluster;
extern void next_level();
int *cluster_labeled_size;
int *cluster_first_label_doc_id;
int *distance_of_label_doc;
static FILE* fp_guide;
vector<cluster_guide>* guide;
vector<int> current_head;
static int cluster_meet;
static int current_level;
static int guide_level_num;

static bool read_distance();
static int read_guide(char* filePath);
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
	int ret =  read_distance();
	if(ret < 0) return -1;
	ret = read_guide(argv[1]);
	if(ret < 0) return -1;
	return 2;
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
static int read_guide(char* filePath) {
	fp_guide = fopen(filePath,"r");
	if(fp_guide == NULL) {
		cerr<<"can't open "<<filePath<<endl;
		return -1;
	}
	string str;
	int ret = fscanf(fp_guide,"guide level num:%d",&guide_level_num);
	if(ret == 0) {
		cerr<<"can't parse the file!!"<<endl;
		return -1;
	}
	guide = new vector<cluster_guide>[guide_level_num];
	for(int i = 0; i <guide_level_num; i++) {
		int cluster_num;
		ret = fscanf(fp_guide,"\ncluster num:%d detail:",&cluster_num);
		if(ret == 0) {
			cerr<<"can't parse the file!!"<<endl;
			return -1;
		}
		for(int j = 0; j < cluster_num; j++) {
			cluster_guide temp;
			int head,size;
			ret = fscanf(fp_guide,"(%d,%d)",&head,&size);
			if(ret == 0) {
				cerr<<"can't parse the file!!"<<endl;
				return -1;
			}
			temp.head = head;
			temp.size = size;
			temp.fulfill = 0;
			guide[i].push_back(temp);
		}
	}
	current_head.clear();
	cluster_meet = 0;
	for(int i = 0; i < guide[0].size();i++) {
		current_head.push_back(guide[0][i].head);
		if(guide[0][i].size == 1) {cluster_meet ++;guide[0][i].fulfill = 1;}
	}
	sort(current_head.begin(),current_head.end());
	current_level = 0;
}
extern void clear_sim();
static void if_meet_guide(int endNode){
	if(binary_search(current_head.begin(),current_head.end(),cluster_first_label_doc_id[endNode])) {
		int flag = 0;
		for(int i = 0; i < guide[current_level].size();i++) {
			if(guide[current_level][i].head == cluster_first_label_doc_id[endNode]) {
				flag = 1;
				if(cluster_labeled_size[endNode] > guide[current_level][i].size) {
					cerr<<"Error:wtf3 in can_merge under distance level"<<endl;
					exit(0);
				}else if(cluster_labeled_size[endNode] == guide[current_level][i].size) {
					if(!guide[current_level][i].fulfill){
						cluster_meet ++;
						guide[current_level][i].fulfill = 1;
					}
				}
				break;
			}
		}
		if(flag != 1) {
			cerr<<"Error:wtf2 in can_merge under distance level"<<endl;
			exit(0);
		}
	}
}
static void to_next_level() {
	next_level();
	cluster_meet = 0;
	current_level ++;
	current_head.clear();
	for(int i = 0; i < guide[current_level].size();i++) {
		current_head.push_back(guide[current_level][i].head);
	}
	sort(current_head.begin(),current_head.end());
	//clear the similarity,and re_init_it!!;
	clear_sim();
	for(int i = 0; i < doc_num-1; i++)
	{
		for(int j = i+1; j < doc_num; j++)
		{
			int k = doc_num*i - (i+3)*i/2 + j - 1;
			doc_similarity[k] = calc_doc_sim(i, j);
		}
	}
	for(int i = 0; i < cluster_queue.size();i++){
		insertNodeInSimilarity(cluster_queue[i]);
		if_meet_guide(cluster_queue[i]);
	}
	cout<<"current_guide cluster size:"<<guide[current_level].size()<<endl;
	if(cluster_meet == guide[current_level].size()) {
		if(current_level != guide_level_num) {
			to_next_level();
		} else {
			if(cluster_queue.size()!=1) {
				cerr<<"Error:wtf4 in can_merge under distance level"<<endl;
				exit(0);
			}
		}
	}
}

void when_merge(int node1, int node2, int endNode){
	cluster_first_label_doc_id[endNode] = cluster_first_label_doc_id[node1];
	if(cluster_first_label_doc_id[node2] < cluster_first_label_doc_id[endNode]) {
		cluster_first_label_doc_id[endNode] = cluster_first_label_doc_id[node2];
	}
	cluster_labeled_size[endNode] = cluster_labeled_size[node1] +  cluster_labeled_size[node2];
	if_meet_guide(endNode);
	if(cluster_meet == guide[current_level].size()) {
		if(current_level != guide_level_num - 1) {
			to_next_level();
		}
	}
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
		cout<<"Error:wtf in can_merge under distance level"<<endl;
		exit(0);
	}
	if(doc_id1 > doc_id2) {
		int  temp = doc_id1;doc_id1 = doc_id2;doc_id2 = temp;
	}
	if(binary_search(current_head.begin(),current_head.end(),doc_id1)) {
		int flag = 0;
		for(int i = 0; i < guide[current_level].size();i++) {
			if(guide[current_level][i].head == doc_id1) {
				flag = 1;
				if(label_num1 + label_num2 > guide[current_level][i].size) {
					return 0;
				}
				break;
			}
		}
		if(flag != 1) {
			cerr<<"Error:wtf2 in can_merge under distance level"<<endl;
			exit(0);
		}
	}

	if(label_num1 + label_num2 >= distance_of_label_doc[doc_id1*doc_num+doc_id2]) {//could change to >
		return 1;
	}
	return 0;
}
