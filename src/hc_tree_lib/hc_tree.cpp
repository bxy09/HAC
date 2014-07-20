#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <list>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <set>
#include "memory_pool_impl.h"
#include "rbt_impl.h"
#include "hc_tree.h"
#define FIRST_TERM_ID 1
using namespace std;

memory_pool<similarity_node> similarity_node::mem = memory_pool<similarity_node>();
memory_pool<reference_node> reference_node::mem = memory_pool<reference_node>();

vecint2 nodeID_distance;//todo :improve the hierarchy of code,move the function about generate_distance to suitable file.
int doc_num;
int term_num;
static int total_num;
static ifstream fp_data;
static ifstream fp_label;
static ifstream fp_label_marked;
static ofstream fp_cluster_tree;
static ofstream fp_cluster_tree_label;
static ofstream fp_distance;
static ofstream fp_guide;
static int* label;
static int* doc_frequency;
static float* term_frequency;
static int* doc;
static int* term_id;
float* term_weight;
float* doc_similarity;
vector<int> cluster_queue;
struct cluster_node* cluster_tree;
static int cluster_tree_num = 0;
static similarity_node* cluster_similarity = 0;
static reference_node* similarity_search_root = 0;
extern float get_Weight(int term_id);
extern int read_weight(int argc, char* argv[]);

extern int end_weight();
extern int use_weight;
void clear_sim() {
	cluster_similarity = 0;
	similarity_search_root = 0;
	reference_node::mem.clear_pool();
	similarity_node::mem.clear_pool();
	
}
int *doc_in_cluster;
int similarity_num = 0;
int cluster_queue_size() {
	return cluster_queue.size();
}
void read_data();
void read_label();
void read_label_marked();

void weight();
void init_cluster();
void init_similarity();
void fprint_cluster_tree();
void fprintf_distance();
void fprintf_rt_guide();

bool insert_in_sim(int node1, int node2, float val);
int init_hc(int argc, char* argv[]) {
	if(argc < 2) {
		cout<< "parameter error!";
		return -1;
	}
	fp_data.open(argv[0]);
	if(fp_data == NULL)
	{
		cout << "File data("<<argv[0]<<") cannot open!";
		return -1;
	}
	fp_label.open(argv[1]);
	if(fp_label == NULL)
	{
		cout << "File label cannot open!";
		return -1;
	}
	fp_cluster_tree.open("cluster_tree");
	if(fp_cluster_tree == NULL)
	{
		cout << "File cluster_tree cannot open!";
		return -1;
	}
	fp_cluster_tree_label.open("cluster_tree_label");
	if(fp_cluster_tree_label == NULL)
	{
		cout << "File cluster_tree_label cannot open!";
		return -1;
	}
	//init constrait
	fp_data>>doc_num>>term_num>>total_num;
	doc = new int[doc_num + 1];
	term_id = new int[total_num];
	term_frequency = new float[total_num];
	doc_frequency = new int[FIRST_TERM_ID + term_num];
	term_weight = new float[total_num];
	label = new int[doc_num];
	doc_similarity = new float[(doc_num-1)*doc_num/2];
	cluster_tree = new struct cluster_node[doc_num*2-1];
	doc_in_cluster = new int[doc_num];
	read_data();
	read_label();
	int weight_ret = read_weight(argc - 2,argv + 2);
	if(weight_ret < 0) {
		return -1;
	}
    weight();
	init_cluster();
	for(int i = 0; i < doc_num; i++) {
		cluster_queue.push_back(i);
	}
	init_similarity();
	return 2 + weight_ret;
}
int init_gen_distance(int argc, char* argv[]) {
	if(argc < 2) {
		cout<< "parameter error!";
		return -1;
	}
	fp_data.open(argv[0]);
	if(fp_data == NULL)
	{
		cout << "File data("<<argv[0]<<") cannot open!";
		return -1;
	}
	fp_label_marked.open(argv[1]);
	if(fp_label_marked == NULL)
	{
		cout << "File label_marked cannot open!";
		return -1;
	}
	fp_distance.open("distance");
	fp_guide.open("guide");
	//init constrait
	fp_data>>doc_num>>term_num>>total_num;
	doc = new int[doc_num + 1];
	term_id = new int[total_num];
	term_frequency = new float[total_num];
	doc_frequency = new int[FIRST_TERM_ID + term_num];
	term_weight = new float[total_num];
	label = new int[doc_num];
	doc_similarity = new float[(doc_num-1)*doc_num/2];
	cluster_tree = new struct cluster_node[doc_num*2-1];
	doc_in_cluster = new int[doc_num];
	read_data();
	cout<<"going to read_label_marked"<<endl;
	read_label_marked();
	int weight_ret = read_weight(argc - 2,argv + 2);
	if(weight_ret < 0) {
		return -1;
	}
    weight();
    init_cluster();
	return 2 + weight_ret;
}
int end_hc() {
	fprint_cluster_tree();
	int end_weight();
	delete [] doc;
	delete [] term_id;
	delete [] term_frequency;
	delete [] doc_frequency;
	delete [] doc_similarity;
	delete [] term_weight;
	delete [] label;
	delete [] cluster_tree;
	delete [] doc_in_cluster;
}
int end_gen_distance() {
	fprintf_distance();
	fprintf_rt_guide();
	int end_weight();
	delete [] doc;
	delete [] term_id;
	delete [] term_frequency;
	delete [] doc_frequency;
	delete [] doc_similarity;
	delete [] term_weight;
	delete [] label;
	delete [] cluster_tree;
	delete [] doc_in_cluster;
}

void read_data()
{
	int t_id = 0;
	string strin;
	int loc = 0;
	for(int i = 0; i < term_num + FIRST_TERM_ID; i++)
	{
		doc_frequency[i] = 0;
	}
	getline(fp_data, strin);
	for(int i = 0; i < doc_num; i++)
	{
		getline(fp_data,strin);
		stringstream strline(strin);
		doc[i] = loc;
		while(strline >> t_id)
		{
			doc_frequency[t_id]++;
			term_id[loc] = t_id;
			strline >> term_frequency[loc];
			loc++;
		}
		
	}
	doc[doc_num] = total_num;
}

void read_label()
{
	for(int i = 0; i < doc_num; i++)
	 {
		 fp_label >> label[i];
	 }
}
vecint3 label_marked;
void read_label_marked() {
	int level_num;
	fp_label_marked >> level_num;
	label_marked = vecint3(level_num + 1);
	vecint1 lol_o;
	label_marked[level_num].push_back(lol_o);
	//first level -- different level;
	//second level-- different cluster;
	//third level -- different id;
	int *classNum = new int[level_num + 1];
	int *lastIndex = new int[level_num + 1];
	int *currentIndex = new int[level_num + 1];
	int *storeIndex = new int[level_num + 1];
	for(int i = 0; i < level_num + 1; i++) 	{
		fp_label_marked >> classNum[i];
		lastIndex[i] = 0;
		currentIndex[i] = 0;
		storeIndex[i] = 0;
	}

	for(int i = 0; i < classNum[0]; i++) {
		int hightest_change_level = -1;
		for (int  j = 0; j < level_num + 1; j++) {
			fp_label_marked >> currentIndex[j];
			if(currentIndex[j] != lastIndex[j]) {
				hightest_change_level = j;
			}
			lastIndex[j] = currentIndex[j];
		}
		for (int j = hightest_change_level; j >0; j--) {
			label_marked[j].back().push_back(storeIndex[j]++);
			vecint1 lol;
			label_marked[j - 1].push_back(lol);
		}
		label_marked[0].back().push_back(currentIndex[0]);
	}
	delete [] classNum;
	delete [] lastIndex;
	delete [] currentIndex;
	delete [] storeIndex;
}
void weight()
{
	for(int i = 0; i < doc_num; i++)
	{
		for(int j = doc[i]; j < doc[i+1]; j++)
		{
			term_weight[j] = term_frequency[j] * log( (float)doc_num/(float)doc_frequency[term_id[j]] );
		}
	}

}
void init_cluster()
{
	for(int i = 0; i < doc_num; i++)
	{
		struct cluster_node& temp = cluster_tree[cluster_tree_num++];
		temp.id.push_back(i);
		temp.parent = -1;
		temp.lchild = -1;
		temp.rchild = -1;
		doc_in_cluster[i] = i;
	}
		
}
float calc_doc_sim(int i, int  j) {
	float temp1 = 0;
	float temp2 = 0;
	int b1 = doc[i];
	int b2 = doc[j];
	float doc_similarity_ret = 0;
	if(use_weight) {
		while(b1 < doc[i+1] && b2 < doc[j+1])
		{
			if(term_id[b1] == term_id[b2])
			{
				doc_similarity_ret += term_weight[b1] * term_weight[b2] * get_Weight(term_id[b2]);
				temp1 += term_weight[b1] * term_weight[b1] * get_Weight(term_id[b1]);
				temp2 += term_weight[b2] * term_weight[b2] * get_Weight(term_id[b2]);
				b1++;
				b2++;
			}
			else if(term_id[b1] < term_id[b2])
			{
				temp1 += term_weight[b1] * term_weight[b1] * get_Weight(term_id[b1]);
				b1++;
			}
			else
			{
				temp2 += term_weight[b2] * term_weight[b2] * get_Weight(term_id[b2]);
				b2++;
			}
		}
		while(b1 < doc[i+1])
		{
			temp1 += term_weight[b1] * term_weight[b1] * get_Weight(term_id[b1]);
			b1++;
		}
		while(b2 < doc[j+1])
		{
			temp2 += term_weight[b2] * term_weight[b2] * get_Weight(term_id[b2]);
			b2++;
		}
	} else {
		while(b1 < doc[i+1] && b2 < doc[j+1])
		{
			if(term_id[b1] == term_id[b2])
			{
				doc_similarity_ret += term_weight[b1] * term_weight[b2];
				temp1 += term_weight[b1] * term_weight[b1];
				temp2 += term_weight[b2] * term_weight[b2];
				b1++;
				b2++;
			}
			else if(term_id[b1] < term_id[b2])
			{
				temp1 += term_weight[b1] * term_weight[b1];
				b1++;
			}
			else
			{
				temp2 += term_weight[b2] * term_weight[b2];
				b2++;
			}
		}
		while(b1 < doc[i+1])
		{
			temp1 += term_weight[b1] * term_weight[b1];
			b1++;
		}
		while(b2 < doc[j+1])
		{
			temp2 += term_weight[b2] * term_weight[b2];
			b2++;
		}
	}
	return doc_similarity_ret / ( sqrt(temp1) * sqrt(temp2) );
}
void init_similarity()
{
	for(int i = 0; i < doc_num-1; i++)
	{
		for(int j = i+1; j < doc_num; j++)
		{
			int k = doc_num*i - (i+3)*i/2 + j - 1;
			doc_similarity[k] = calc_doc_sim(i, j);
			insert_in_sim(i,j,doc_similarity[k]);
		}
	}
}

void max_similarity(int& node1, int& node2)
{
	similarity_node* largest = mostRight(cluster_similarity);
	node1 = largest->node1;
	node2 = largest->node2;
}
static vector<similarity_node*> gonna_del;
static int node1_g,node2_g;
bool find_gonna_del(similarity_node* node) {
	if(node->node1 == node1_g || 
		node->node2 == node1_g || 
		node->node1 == node2_g || 
		node->node2 == node2_g)
	{
		gonna_del.push_back(node);
	}
	return 1;
}
bool del_sim(int node1, int node2) {
	if(node1 == node2) return 0;
	if(node1 > node2) {
		int temp=node1;node1=node2;node2=temp;
	}
	int key = node1*doc_num*2+node2;
	reference_node *find_ref = rbt_search(similarity_search_root,key);
	if(find_ref != 0) {
		similarity_node* sim = (similarity_node*)find_ref->ref;
		rbt_remove(&similarity_search_root,find_ref);
		rbt_remove(&cluster_similarity,sim);
		reference_node::mem.del(find_ref);
		similarity_node::mem.del(sim);
		similarity_num--;
	} else {
		return 0;
	}
	return 1;
}
bool insert_in_sim(int node1, int node2, float val) {
	if(node1 == node2) return 0;
	if(node1 > node2) {
		int temp=node1;node1=node2;node2=temp;
	}
	int key = node1*doc_num*2+node2;//cluster num can't exceed (2*doc_num - 1)
	reference_node *find_ref = rbt_search(similarity_search_root,key);
	if(find_ref != 0) {
		similarity_node* sim = (similarity_node*)find_ref->ref;
		rbt_remove(&cluster_similarity,sim);
		sim -> key = val;
		rbt_insert(&cluster_similarity,sim);
	}else {
		similarity_node* sim = similarity_node::mem.apply();
		similarity_num ++;
		sim->key = val;sim->node1 = node1;sim->node2 = node2;
		reference_node* ref_node = reference_node::mem.apply();
		ref_node->key = key;ref_node->ref = (void*)sim;
		rbt_insert(&cluster_similarity,sim);
		rbt_insert(&similarity_search_root,ref_node);
	}	
}
void visitSimFromLargeToLow(bool (*fun) (similarity_node*)) {
	visitFromLargeToLow(cluster_similarity,fun);
}
void deleteNodeFromSimilarity(int nodeD) {
	for(int i = 0; i < cluster_queue_size(); i++)
	{
		del_sim(cluster_queue[i],nodeD);
	}
}
void insertNodeInSimilarity(int nodeD) {
	int t1,t2;
	for(int i = 0; i < cluster_queue.size(); i++)
	{
		if(cluster_queue[i] == nodeD) continue;
		float sim_val = 0;
		for(int j = 0; j < cluster_tree[cluster_queue[i]].id.size(); j++)
		{
			t1 = cluster_tree[cluster_queue[i]].id[j];
			for(int k = 0; k < cluster_tree[nodeD].id.size(); k++)
			{
				t2 = cluster_tree[nodeD].id[k];
				if(t1 < t2)
				{
					sim_val += doc_similarity[doc_num*t1-t1*(t1+3)/2+t2-1];
				}
				else
				{
					sim_val +=doc_similarity[doc_num*t2 -t2*(t2+3)/2+t1-1];
				}
			}
		}
		sim_val = sim_val / 
				(cluster_tree[cluster_queue[i]].id.size()*cluster_tree[nodeD].id.size());
		insert_in_sim(cluster_queue[i],nodeD,sim_val);
	}
}
int merge(const int node1, const int node2)
{
	////////add node to tree////////////
	struct cluster_node& cluster_node1 = cluster_tree[cluster_tree_num++];
	int i = 0, j = 0;
	while( i < cluster_tree[node1].id.size() || j < cluster_tree[node2].id.size()) {
		if(i < cluster_tree[node1].id.size() && j < cluster_tree[node2].id.size()){
			if(cluster_tree[node1].id.at(i) == cluster_tree[node2].id.at(j)) {
				cout<<"Error:merge node have same docID"<<endl;
				assert(0);
			}
			if(cluster_tree[node1].id.at(i) < cluster_tree[node2].id.at(j)) {

				cluster_node1.id.push_back(cluster_tree[node1].id[i]);
				doc_in_cluster[cluster_tree[node1].id[i]] = cluster_tree_num - 1;
				i++;
			} else {

				cluster_node1.id.push_back(cluster_tree[node2].id[j]);
				doc_in_cluster[cluster_tree[node2].id[j]] = cluster_tree_num - 1;
				j++;
			}
		} else if(i < cluster_tree[node1].id.size()){

				cluster_node1.id.push_back(cluster_tree[node1].id[i]);
				doc_in_cluster[cluster_tree[node1].id[i]] = cluster_tree_num - 1;
				i++;
		} else {

				cluster_node1.id.push_back(cluster_tree[node2].id[j]);
				doc_in_cluster[cluster_tree[node2].id[j]] = cluster_tree_num - 1;
				j++;
		}
	}
	cluster_node1.lchild = node1;
	cluster_node1.rchild = node2;
	cluster_node1.parent = -1;
	cluster_tree[node1].parent = cluster_tree_num - 1;
	cluster_tree[node2].parent = cluster_tree_num - 1;
	
	//////delete node from sim/////
	deleteNodeFromSimilarity(node1);
	deleteNodeFromSimilarity(node2);
	///////delete node from queue///
	int node_num_in_queue = 0;
	for(vector<int>::iterator iter = cluster_queue.begin(); iter != cluster_queue.end();)
	{
		if(*iter == node1 || *iter == node2){
			iter = cluster_queue.erase(iter);
			node_num_in_queue++;
		}else{
			iter++;
		}
	}
	if(node_num_in_queue!=2) {cout<<"Error:wft in merge!!"<<endl;assert(0);}
	//////add node to queue and sim/
	insertNodeInSimilarity(cluster_tree_num-1);
	cluster_queue.push_back(cluster_tree_num-1);
	return cluster_tree_num - 1;
}

void fprint_cluster_tree()
{
	for(int i = 0; i < cluster_tree_num; i++)
	{
		for(int j = 0; j< cluster_tree[i].id.size(); j++)
		{
			fp_cluster_tree << cluster_tree[i].id[j]<< " ";
			fp_cluster_tree_label << label[cluster_tree[i].id[j]] << " ";
		}
		fp_cluster_tree << endl;
		fp_cluster_tree_label << endl;
	}
	cout<<"print over cluster_tree_num "<<cluster_tree_num<<endl;
}
void fprintf_distance()
{
	cout<<"print distance!"<<endl;
	for(int i = 0; i < label_marked[0].size(); i++) {
		for(int j = 0; j < label_marked[0][i].size();j++) {
			for(int ii = 0; ii <= i; ii++) {
				for(int jj = 0; (ii<i && jj < label_marked[0][ii].size());jj++) {//improve ii != i
					int common_cluster = label_marked[0][i][j];
					bool flag = 1;
					while(!binary_search(cluster_tree[common_cluster].id.begin(),
							cluster_tree[common_cluster].id.end(),label_marked[0][ii][jj])){
						common_cluster = cluster_tree[common_cluster].parent;
						if(common_cluster == -1) {
							cout<<"Error:wtf in print_distance "<<label_marked[0][i][j]<<":"<<label_marked[0][ii][jj]<<endl;
							assert(0);
						}
					}
					fp_distance<<label_marked[0][i][j]<<" "<<label_marked[0][ii][jj]<<" "<<
						cluster_tree[common_cluster].id.size()<<endl;
				}
			}
		}

	}
	cout<<"print end!"<<endl;
}
static int getFirstID(int level,int cluster_index) {
	if(level == 0) {
		return label_marked[level][cluster_index][0];
	}
	return getFirstID(level - 1,label_marked[level][cluster_index][0]);
}
static int getDocNum(int level,int cluster_index) {
	if(level == 0) {
		return label_marked[level][cluster_index].size();
	}
	int size_in_sum = 0;
	for(int i = 0; i < label_marked[level][cluster_index].size(); i++) {
		size_in_sum += getDocNum(level - 1,label_marked[level][cluster_index][i]);
	}
	return size_in_sum;	
}
void fprintf_rt_guide()
{
	cout<<"print guide!"<<endl;
	fp_guide<<"guide level num:"<<label_marked.size()<<endl;
	for(int i = 0; i < label_marked.size(); i++) {
		fp_guide<<"cluster num:"<< label_marked[i].size()<<" detail:";
		for(int j = 0; j < label_marked[i].size(); j++) {
			//get the first labeled doc id and labeled doc num
			fp_guide<<"("<<getFirstID(i,j)<<","<<getDocNum(i,j)<<")";
		}
		fp_guide<<endl;
	}
	cout<<"print end"<<endl;
}