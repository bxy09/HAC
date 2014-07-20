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
using namespace std;

struct cluster_node
{
	vector<int> id;
	int parent;
	int lchild;
	int rchild;
};
struct similarity_node
{
	float value;
	int node1;
	int node2;
};
static int doc_num;
static int term_num;
static int total_num;
static ifstream fp_data;
static ifstream fp_label;
static ofstream fp_cluster_tree;
static ofstream fp_cluster_tree_label;
static ofstream fp_relation;
static ofstream fp_relation_link;
static int* label;
static int* doc_frequency;
static float* term_frequency;
static int* doc;
static int* term_id;
static float* term_weight;
static float* doc_similarity;
static vector<int> cluster_queue;
static struct cluster_node* cluster_tree;
static vector<struct similarity_node> cluster_similarity;
static int cluster_tree_num = 0;
int cluster_queue_size() {
	return cluster_queue.size();
}

bool cmp(struct similarity_node node1, struct similarity_node node2)
{
	return node1.value < node2.value;
}
void read_data();
void read_label();
void weight();
void init_cluster();
void init_similarity();
void fprint_cluster_tree();
void fprint_relation();
void fprint_relation_link();
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
	fp_relation.open("relation");
	if(fp_relation == NULL){
		cout << "File relation cannot creat!";
		return -1;
	}
	fp_relation_link.open("relation_link");
	if(fp_relation_link == NULL){
		cout << "File relation_link cannot creat!";
		return -1;
	}
	//init constrait
	fp_data>>doc_num>>term_num>>total_num;
	doc = new int[doc_num];
	term_id = new int[total_num];
	term_frequency = new float[total_num];
	doc_frequency = new int[term_num];
	term_weight = new float[total_num];
	label = new int[doc_num];
	doc_similarity = new float[(doc_num-1)*doc_num/2];
	cluster_tree = new struct cluster_node[doc_num*2-1];
	read_data();
	read_label();
    weight();
	init_cluster();
	init_similarity();
	make_heap(cluster_similarity.begin(), cluster_similarity.end(), cmp);
	return 2;
}
int end_hc() {
	//end constrait
	fprint_cluster_tree();
	//fprint_relation();
	//fprint_relation_link();
	delete [] doc;
	delete [] term_id;
	delete [] term_frequency;
	delete [] doc_frequency;
	delete [] doc_similarity;
	delete [] term_weight;
	delete [] label;
	delete [] cluster_tree;
}
void read_data()
{
	int t_id = 0;
	string strin;
	int loc = 0;
	for(int i = 0; i < term_num; i++)
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
			doc_frequency[t_id-1]++;
			term_id[loc] = t_id;
			strline >> term_frequency[loc];
			loc++;
		}
		
	}
}

void read_label()
{
	for(int i = 0; i < doc_num; i++)
	 {
		 fp_label >> label[i];
	 }
}
void weight()
{
	for(int i = 0; i < doc_num-1; i++)
	{
		for(int j = doc[i]; j < doc[i+1]; j++)
		{
			term_weight[j] = term_frequency[j] * log( (float)doc_num/(float)doc_frequency[term_id[j]-1] );
		}
	}
	for(int j = doc[doc_num-1]; j < total_num; j++)
	{
		term_weight[j] = term_frequency[j] * log( (float)doc_num/(float)doc_frequency[term_id[j]-1] );
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
		//cluster_tree.push_back(temp);
		cluster_queue.push_back(i);
	}
		
}

void init_similarity()
{
	int k;
	float temp1;
	float temp2;
	int b1;
	int b2;
	struct similarity_node temp;
	for(int i = 0; i < doc_num-1; i++)
	{
		for(int j = i+1; j < doc_num-1; j++)
		{
			k = doc_num*i - (i+3)*i/2 + j - 1;
			doc_similarity[k] = 0;
			temp1 = 0;
			temp2 = 0;
			b1 = doc[i];
			b2 = doc[j];
			while(b1 < doc[i+1] && b2 < doc[j+1])
			{
				if(term_id[b1] == term_id[b2])
				{
					doc_similarity[k] += term_weight[b1] * term_weight[b2];
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
			doc_similarity[k] = doc_similarity[k] / ( sqrt(temp1) * sqrt(temp2) );
			temp.value = doc_similarity[k];
			temp.node1 = i;
			temp.node2 = j;
			cluster_similarity.push_back(temp);
		}
		k = doc_num*i - (i+3)*i/2 + doc_num - 2;
		doc_similarity[k] = 0;
		temp1 = 0;
		temp2 = 0;
		b1 = doc[i];
		b2 = doc[doc_num-1];
		while(b1 < doc[i+1] && b2 < total_num)
		{
			if(term_id[b1] == term_id[b2])
			{
				doc_similarity[k] += term_weight[b1] * term_weight[b2];
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
		while(b2 < total_num)
		{
			temp2 += term_weight[b2] * term_weight[b2];
			b2++;
		}
		doc_similarity[k] = doc_similarity[k] / ( sqrt(temp1) * sqrt(temp2) );
		temp.value = doc_similarity[k];
		temp.node1 = i;
		temp.node2 = doc_num-1;
		cluster_similarity.push_back(temp);
	}
}

 
void max_similarity(int& node1, int& node2)
{
	//make_heap(cluster_similarity.begin(),cluster_similarity.end(),cmp);
	node1 = cluster_similarity.begin()->node1;
	node2 = cluster_similarity.begin()->node2;

}

template <class type, class _Compare>
void 
my_adjust_heap(vector<type> &vec, int holeIndex, _Compare comp)
{
	
	//_Distance holeIndex = _Distance(_holeIndex);
	//_Distance len = _Distance(_len);
	bool flag_up = 0;
	int len = vec.size();
	//siftUp
	type _value = vec[holeIndex];
	
	for (int _Idx = (holeIndex - 1) / 2;
		holeIndex > 0 && comp(vec[_Idx], _value);  
		_Idx = (holeIndex - 1) / 2)  
	{ 
		_value = vec[holeIndex];
		vec[holeIndex] = vec[_Idx];
		vec[_Idx] = _value;
		holeIndex = _Idx;
		flag_up = 1;
	}
	if(flag_up) {
		//vec[holeIndex]= _value;
		return;
	}
	//siftDown
	int firstChild = 2 * holeIndex + 1;
	while (firstChild < len) {
		if (firstChild < len - 1 &&
			comp(vec[firstChild], vec[firstChild + 1])) {
			firstChild++;
		}
		if(!comp(_value,vec[firstChild]))break;

		_value = vec[holeIndex];
		vec[holeIndex] = vec[firstChild];
		vec[firstChild] = _value;

		holeIndex = firstChild;
		firstChild = 2 * firstChild + 1;
	}
	//vec[holeIndex] = _value;
	return;
}

void merge(const int node1, const int node2)
{
	struct cluster_node& cluster_node1 = cluster_tree[cluster_tree_num++];
	struct similarity_node cluster_similarity1;
	int t1;
	int t2;
	static bool flag_573_2811 = 0;
	for(int i = 0; i < cluster_tree[node1].id.size(); i++)
	{
		cluster_node1.id.push_back(cluster_tree[node1].id[i]);
	}
	for(int i = 0; i < cluster_tree[node2].id.size(); i++)
	{
		cluster_node1.id.push_back(cluster_tree[node2].id[i]);
	}
	sort(cluster_node1.id.begin(), cluster_node1.id.end());
	cluster_node1.lchild = node1;
	cluster_node1.rchild = node2;
	cluster_node1.parent = -1;
	cluster_tree[node1].parent = cluster_tree_num;
	cluster_tree[node2].parent = cluster_tree_num; 
	int erase_num = 0;
	for(vector<int>::iterator iter = cluster_queue.begin(); iter != cluster_queue.end();)
	{
		if(*iter == node1 || *iter == node2)
		{
			erase_num += *iter ;
			iter = cluster_queue.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	cluster_queue.push_back(cluster_tree_num-1);

	while(cluster_similarity.size()&&(
				cluster_similarity[cluster_similarity.size()-1].node1 == node1 ||
				cluster_similarity[cluster_similarity.size()-1].node2 == node1 || 
				cluster_similarity[cluster_similarity.size()-1].node1 == node2 || 
				cluster_similarity[cluster_similarity.size()-1].node2 == node2)) {
		cluster_similarity.pop_back();
	}
		for(int i = 0; i < (int)cluster_similarity.size()-1;)
	{
		if(cluster_similarity[i].node1 == node1 || 
			cluster_similarity[i].node2 == node1 || 
			cluster_similarity[i].node1 == node2 || 
			cluster_similarity[i].node2 == node2)
		{
			cluster_similarity[i].value = cluster_similarity[cluster_similarity.size()-1].value;
			cluster_similarity[i].node1 = cluster_similarity[cluster_similarity.size()-1].node1;
			cluster_similarity[i].node2 = cluster_similarity[cluster_similarity.size()-1].node2;
			cluster_similarity.pop_back();
			my_adjust_heap(cluster_similarity, i, cmp);

			while(cluster_similarity.size()&&(
				cluster_similarity[cluster_similarity.size()-1].node1 == node1 ||
					cluster_similarity[cluster_similarity.size()-1].node2 == node1 || 
					cluster_similarity[cluster_similarity.size()-1].node1 == node2 || 
					cluster_similarity[cluster_similarity.size()-1].node2 == node2)) {
				cluster_similarity.pop_back();
			}
		}
		else
		{
			i++;
		}
	}
	for(int i = 0; i < cluster_queue_size()-1; i++)
	{
		cluster_similarity1.value = 0;
		for(int j = 0; j < cluster_tree[cluster_queue[i]].id.size(); j++)
		{
			t1 = cluster_tree[cluster_queue[i]].id[j];
			for(int k = 0; k < cluster_tree[cluster_tree_num-1].id.size(); k++)
			{
				t2 = cluster_tree[cluster_tree_num-1].id[k];
				if(t1 < t2)
				{
					cluster_similarity1.value += doc_similarity[doc_num*t1-t1*(t1+3)/2+t2-1];
				}
				else
				{
					cluster_similarity1.value +=doc_similarity[doc_num*t2 -t2*(t2+3)/2+t1-1];
				}
			}
		}
		cluster_similarity1.value = cluster_similarity1.value / 
				(cluster_tree[cluster_queue[i]].id.size()*cluster_tree[cluster_tree_num-1].id.size());
		cluster_similarity1.node1 = cluster_queue[i];
		cluster_similarity1.node2 = cluster_tree_num-1;
		cluster_similarity.push_back(cluster_similarity1);
		push_heap(cluster_similarity.begin(),cluster_similarity.end(),cmp);
	}
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
