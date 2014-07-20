#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<assert.h>
#define FIRST_TERM_ID 1
#define FIRST_DOC_ID 0
using namespace std;
struct cluster_node
{
	vector<int> id;
	int merge_size;
};
struct similarity_node
{
	float value;
	int node1;
	int node2;
};
struct distance_node{
	int doc_id1;
	int doc_id2;
	float distance;
};
void read_num(ifstream& fp_data, int& doc_num, int& term_num, int& total_num)
{
	fp_data >> doc_num;
	fp_data >> term_num;
	fp_data >> total_num;
}
void read_data(ifstream& fp_data, int doc_num, int term_num, int* doc_frequency, int* doc, int* term_id, float* term_frequency)
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
}

void base_vector(int* doc_frequency, const int doc_num, int* doc, int* term_id, 
				float* term_frequency, float* term_base_vector)
{
	for(int i = 0; i < doc_num; i++)
	{
		for(int j = doc[i]; j < doc[i+1]; j++)
		{
			term_base_vector[j] = term_frequency[j] * log( (float)doc_num/(float)doc_frequency[term_id[j]] );
		}
	}
}
void init_cluster(const int doc_num, vector<struct cluster_node>& cluster_tree, vector<int>& cluster_queue, bool* valid1)
{
	for(int i = 0; i < doc_num; i++)
	{
		struct cluster_node c_node;
		c_node.id.push_back(i);
		if(valid1[i] == true){
			c_node.merge_size = 1; 
		}
		else{
			c_node.merge_size = 0; 
		}
		cluster_tree.push_back(c_node);
		cluster_queue.push_back(i);
	}
		
}
void init_similarity(const int doc_num, int* doc, int* term_id, float* term_base_vector, float* bottom_weight, float* doc_similarity, vector<struct similarity_node>& cluster_similarity)
{
	int k;
	float length1;
	float length2;
	int b1;
	int b2;
	struct similarity_node sim_node;
	for(int i = 0; i < doc_num; i++)
	{
		for(int j = i+1; j < doc_num; j++)
		{
			k = doc_num*i - (i+3)*i/2 + j - 1;
			doc_similarity[k] = 0;
			length1 = 0;
			length2 = 0;
			b1 = doc[i];
			b2 = doc[j];
			while(b1 < doc[i+1] && b2 < doc[j+1])
			{
				if(term_id[b1] == term_id[b2])
				{
					doc_similarity[k] += term_base_vector[b1] * term_base_vector[b2];
					length1 += term_base_vector[b1] * term_base_vector[b1];
					length2 += term_base_vector[b2] * term_base_vector[b2];
					b1++;
					b2++;
				}
				else if(term_id[b1] < term_id[b2])
				{
					length1 += term_base_vector[b1] * term_base_vector[b1];
					b1++;
				}
				else
				{
					length2 += term_base_vector[b2] * term_base_vector[b2];
					b2++;
				}
			}
			while(b1 < doc[i+1])
			{
				length1 += term_base_vector[b1] * term_base_vector[b1];
				b1++;
			}
			while(b2 < doc[j+1])
			{
				length2 += term_base_vector[b2] * term_base_vector[b2];
				b2++;
			}
			assert(length1 != 0 && length2 != 0);
			doc_similarity[k] = doc_similarity[k] / ( sqrt(length1) * sqrt(length2) );
			sim_node.value = doc_similarity[k];
			sim_node.node1 = i;
			sim_node.node2 = j;
			cluster_similarity.push_back(sim_node);
		}
	}
}
bool can_merge(int& node1, int& node2, int doc_num, vector<struct cluster_node>& cluster_tree, int can_merge_level, bool* bottom_tag, bool* middle_tag, bool* top_tag){
	int k;
	for(int i = 0; i < cluster_tree[node1].id.size(); i++){
		for(int j = 0; j < cluster_tree[node2].id.size(); j++){
			if(cluster_tree[node1].id[i] < cluster_tree[node2].id[j]){
				k = doc_num * cluster_tree[node1].id[i] - (cluster_tree[node1].id[i]+3)*cluster_tree[node1].id[i]/2 + cluster_tree[node2].id[j] - 1;
			}
			else{
				k = doc_num * cluster_tree[node2].id[j] - (cluster_tree[node2].id[j]+3)*cluster_tree[node2].id[j]/2 + cluster_tree[node1].id[i] - 1;
			}
			if(can_merge_level == 0){
				if(bottom_tag[k] == false){
					return false;
				}
			}
			else if(can_merge_level == 1){				
				if(middle_tag[k] == false){
					return false;
				}
			}
			else if(can_merge_level == 2){
				if(top_tag[k] == false){
					return false;
				}
			}
			else{
				return true;
			}
		}
	}
	return true;
}
bool cmp(struct similarity_node node1, struct similarity_node node2)
{
	return node1.value < node2.value;
}
void max_similarity(int& node1, int& node2, vector<struct similarity_node>::iterator iter1, vector<struct similarity_node>::iterator iter2)
{
	make_heap(iter1, iter2, cmp);
	node1 = iter1->node1;
	node2 = iter1->node2;

}
void merge(const int doc_num, const int node1, const int node2, vector<struct cluster_node>& cluster_tree, vector<int>& cluster_queue, float* doc_similarity, vector<struct similarity_node>& cluster_similarity){
	struct cluster_node c_node;
	struct similarity_node cluster_similarity1;
	int t1;
	int t2;
	int i1 = 0;
	int i2 = 0;
	int k;
	while(i1<cluster_tree[node1].id.size() && i2<cluster_tree[node2].id.size())
	{
		if(cluster_tree[node1].id[i1] < cluster_tree[node2].id[i2])
		{
			c_node.id.push_back(cluster_tree[node1].id[i1]);
			i1++;
		}
		else
		{
			c_node.id.push_back(cluster_tree[node2].id[i2]);
			i2++;
		}
	}
	while(i1 < cluster_tree[node1].id.size())
	{
		c_node.id.push_back(cluster_tree[node1].id[i1]);
		i1++;
	}
	while(i2 < cluster_tree[node2].id.size())
	{
		c_node.id.push_back(cluster_tree[node2].id[i2]);
		i2++;
	}
	sort(c_node.id.begin(), c_node.id.end());
	c_node.merge_size = cluster_tree[node1].merge_size + cluster_tree[node2].merge_size;
	cluster_tree.push_back(c_node);
	for(vector<int>::iterator iter = cluster_queue.begin(); iter != cluster_queue.end();)
	{
		if(*iter == node1 || *iter == node2)
		{
			iter = cluster_queue.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	cluster_queue.push_back(cluster_tree.size()-1);
	for(int i = 0; i < cluster_similarity.size()-1;)
	{
		if(cluster_similarity[i].node1 == node1 || cluster_similarity[i].node2 == node1 || cluster_similarity[i].node1 == node2 || cluster_similarity[i].node2 == node2)
		{
			cluster_similarity[i].value = cluster_similarity[cluster_similarity.size()-1].value;
			cluster_similarity[i].node1 = cluster_similarity[cluster_similarity.size()-1].node1;
			cluster_similarity[i].node2 = cluster_similarity[cluster_similarity.size()-1].node2;
			cluster_similarity.pop_back();
		}
		else
		{
			i++;
		}
	}
	if(cluster_similarity[cluster_similarity.size()-1].node1 == node1 || cluster_similarity[cluster_similarity.size()-1].node2 == node1 || cluster_similarity[cluster_similarity.size()-1].node1 == node2 || cluster_similarity[cluster_similarity.size()-1].node2 == node2)
	{
		cluster_similarity.pop_back();
	}
	for(int i = 0; i < cluster_queue.size()-1; i++){
		cluster_similarity1.value = 0;
		for(int j1 = 0; j1 < cluster_tree[cluster_queue[i]].id.size(); j1++){
			t1 = cluster_tree[cluster_queue[i]].id[j1];
			for(int j2 = 0; j2 < cluster_tree[cluster_tree.size()-1].id.size(); j2++){
				t2 = cluster_tree[cluster_tree.size()-1].id[j2];
				if(t1 < t2){
					k = doc_num*t1 - t1*(t1+3)/2 + t2 - 1;
				}
				else{
					k = doc_num*t2 - t2*(t2+3)/2 + t1 -1;
				}
				cluster_similarity1.value += doc_similarity[k];
			}
		}
		cluster_similarity1.value = cluster_similarity1.value / (cluster_tree[cluster_queue[i]].id.size()*cluster_tree[cluster_tree.size()-1].id.size());
		cluster_similarity1.node1 = cluster_queue[i];
		cluster_similarity1.node2 = cluster_tree.size() - 1;
		cluster_similarity.push_back(cluster_similarity1);
	}
}
void fprint_cluster_tree(ofstream& fp_cluster_tree, const vector<struct cluster_node>& cluster_tree)
{
	for(int i = 0; i < cluster_tree.size(); i++)
	{
		for(int j = 0; j< cluster_tree[i].id.size(); j++)
		{
			fp_cluster_tree << cluster_tree[i].id[j] << " ";
		}
		fp_cluster_tree << endl;
	}
}
int read_distance(ifstream& fp_distance, const int doc_num, struct distance_node* distance1, struct distance_node* distance2, bool* valid1, bool* valid2, bool* bottom_tag, bool* middle_tag, bool* top_tag){
	int k;
	for(int i = 0; i < doc_num; i++){
		valid1[i] = false;
		for(int j = i+1; j < doc_num; j++){
			k = doc_num*i - (i+3)*i/2 + j - 1;
			distance1[k].distance = 0;
			distance2[k].distance = 0;
			distance1[k].doc_id1 = i;
			distance2[k].doc_id1 = i;
			distance1[k].doc_id2 = j;
			distance2[k].doc_id2 = j;
		}
	}
	for(int i = 0; i < doc_num*(doc_num-1)/2; i++){
		valid2[i] = false;
		bottom_tag[i] = true;
		middle_tag[i] = true;
		top_tag[i] = true;
	}
	int b_tag;
	int m_tag;
	int t_tag;
	int doc_id1;
	int doc_id2;
	float distance;
	while(fp_distance >> doc_id1){
		fp_distance >> doc_id2;
		doc_id1--;
		doc_id2--;
		fp_distance >> distance;
		fp_distance >> b_tag;
		fp_distance >> m_tag;
		fp_distance >> t_tag;
		valid1[doc_id1] = true;
		valid1[doc_id2] = true;
		k = doc_num * doc_id1 - (doc_id1+3)*doc_id1/2 + doc_id2 - 1;
		if(b_tag == 1){
			bottom_tag[k] = false;
		}
		if(m_tag == 1){
			middle_tag[k] = false; 
		}	
		if(t_tag == 0){
			valid2[k] = true;
			distance1[k].distance = distance;
		}
		else{
			top_tag[k] = false;
			distance2[k].distance = distance;
		}
	}
	return 0;
}
int main(int argc, char* argv[])
{
	if(argc != 4){
		cout << "usage: arg1 data, arg2 weight, arg3 distance" << endl;
		return 1;
	}
	int doc_num;
	int term_num;
	int total_num;
	int node1;
	int node2;
	ifstream fp_data(argv[1]);
	assert(fp_data != NULL);
	ifstream fp_weight(argv[2]);
	assert(fp_weight != NULL);
	ifstream fp_distance(argv[3]);
	assert(fp_distance != NULL);
	ofstream fp_cluster_tree("cluster_tree");
	assert(fp_cluster_tree != NULL);

	read_num(fp_data, doc_num, term_num, total_num);
	int* doc = new int[doc_num + 1];
	int* term_id = new int[total_num];
	float* term_frequency = new float[total_num];
	int* doc_frequency = new int[term_num + FIRST_TERM_ID];
	float* term_base_vector = new float[total_num];
    float* bottom_weight = new float[term_num + FIRST_TERM_ID];
    float* middle_weight = new float[term_num + FIRST_TERM_ID];
    float* top_weight = new float[term_num + FIRST_TERM_ID];
	struct distance_node* distance1 = new struct distance_node[doc_num*(doc_num-1)/2]; //determinate distance for leaf2leaf, in one class
	struct distance_node* distance2 = new struct distance_node[doc_num*(doc_num-1)/2]; //shortest distance for leaf2leaf, undeterminate distance, between two class
	float* distance3 = new float[doc_num]; //leaf2root distance
	for(int i = 0; i < doc_num; i++){
		distance3[i] = 0; //leaf2root distance
	}
	bool* valid1 = new bool[doc_num]; // If one document is distanced with other documents, the valid1 is true. Otherwise, the valid1 is false.
	bool* valid2 = new bool[doc_num*(doc_num-1/2)]; //If leaf2leaf distance is determinate distance, the valid2 is true. Otherwise, the valid2 is false.
	bool* bottom_tag = new bool[doc_num*(doc_num-1/2)]; //true can , false can't
	bool* middle_tag = new bool[doc_num*(doc_num-1/2)]; //true can , false can't
	bool* top_tag = new bool[doc_num*(doc_num-1/2)]; //true can , false can't
	vector<int> cluster_queue;
	vector<struct cluster_node> cluster_tree;
	float* doc_similarity = new float[(doc_num-1)*doc_num/2];
	vector<struct similarity_node> cluster_similarity;
	read_data(fp_data, doc_num, term_num, doc_frequency, doc, term_id, term_frequency);
	doc[doc_num] = total_num;
	read_distance(fp_distance, doc_num, distance1, distance2, valid1, valid2, bottom_tag, middle_tag, top_tag);
    read_weight(fp_weight, bottom_weight, middle_weight, top_weight);
    base_vector(doc_frequency, doc_num, doc, term_id, term_frequency, term_base_vector);
	init_cluster(doc_num, cluster_tree, cluster_queue, valid1);
	init_similarity(doc_num, doc, term_id, term_base_vector, bottom_weight, doc_similarity, cluster_similarity);
	//cout << doc_similarity[2] << endl;
	vector<struct similarity_node>::iterator iter1;
	vector<struct similarity_node>::iterator iter2;
	int can_merge_level = 0;
	while(cluster_queue.size() != 1){
		//cout << cluster_queue.size() << endl;
		iter1 = cluster_similarity.begin();
		iter2 = cluster_similarity.end();
		max_similarity(node1, node2, cluster_similarity.begin(), cluster_similarity.end());
		while(can_merge(node1, node2, doc_num, cluster_tree, can_merge_level, bottom_tag, middle_tag, top_tag) == false){
   	    	if(iter1 < iter2){
      			pop_heap(iter1, iter2, cmp);
        		iter2--;
        		node1 = iter1->node1;
        		node2 = iter1->node2;
            }
  			else if(iter1 == iter2 && can_merge_level == 0){
				cout << "l_1" << endl;
				cout << cluster_queue.size() << endl;
				can_merge_level = 1;
				update_similarity(doc_num, doc, term_id, term_base_vector, middle_weight, doc_similarity, cluster_similarity, cluster_tree);
				max_similarity(node1, node2, cluster_similarity.begin(), cluster_similarity.end());
           	}
  			else if(iter1 == iter2 && can_merge_level == 1){
				cout << "l_2" << endl;
				cout << cluster_queue.size() << endl;
				can_merge_level = 2;
				update_similarity(doc_num, doc, term_id, term_base_vector, top_weight, doc_similarity, cluster_similarity, cluster_tree);
				max_similarity(node1, node2, cluster_similarity.begin(), cluster_similarity.end());
           	}
  			else if(iter1 == iter2 && can_merge_level == 2){
				cout << "l_3" << endl;
				cout << cluster_queue.size() << endl;
				can_merge_level = 3;
				iter2 = cluster_similarity.end();
		        iter2--;
		        node1 = iter2->node1;
		        node2 = iter2->node2;
           	}
			else{		
				cout << "error" << endl;
				iter2 = cluster_similarity.end();
                iter2--;
                node1 = iter2->node1;
                node2 = iter2->node2;
                break;
			}
		}
		merge(doc_num, node1, node2, cluster_tree, cluster_queue, doc_similarity, cluster_similarity);
	}
	fprint_cluster_tree(fp_cluster_tree, cluster_tree);
	return 0;
}
