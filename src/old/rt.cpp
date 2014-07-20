#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<time.h>
using namespace std;
ofstream fp_temp("temp");
struct cluster_node
{
	vector<int> id;
	struct cluster_node* parent;
	struct cluster_node* lchild;
	struct cluster_node* rchild;
	int node_loc;
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
void read_num(fstream& fp_data, int& doc_num, int& term_num, int& total_num)
{
	fp_data >> doc_num;
	fp_data >> term_num;
	fp_data >> total_num;
}

void read_data(fstream& fp_data, int doc_num, int term_num, int* doc_frequency, int* doc, int* term_id, float* term_frequency)
{
	int t_id = 0;
	string strin;
	int loc = 0;
	for(int i = 0; i < term_num; i++){
		doc_frequency[i] = 0;
	}
	getline(fp_data, strin);
	for(int i = 0; i < doc_num; i++){
		getline(fp_data,strin);
		stringstream strline(strin);
		doc[i] = loc;
		while(strline >> t_id){
			doc_frequency[t_id-1]++;
			term_id[loc] = t_id;
			strline >> term_frequency[loc];
			loc++;
		}
		
	}
}

void read_label(ifstream& fp_label, const int doc_num, int* label)
{
	for(int i = 0; i < doc_num; i++)
	 {
		 fp_label >> label[i];
	 }
}
void weight(int* doc_frequency, const int doc_num, const int total_num, int* doc, int* term_id, float* term_frequency, float* term_weight)
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

void init_cluster(const int doc_num, struct cluster_node* cluster_tree, vector<int>& cluster_queue, bool* valid1)
{
	for(int i = 0; i < doc_num; i++)
	{
		cluster_tree[i].id.push_back(i);
		cluster_tree[i].parent = NULL;
		cluster_tree[i].lchild = NULL;
		cluster_tree[i].rchild = NULL;
		cluster_tree[i].node_loc = i;
		if(valid1[i] == true){
			cluster_tree[i].merge_size = 1; 
		}
		else{
			cluster_tree[i].merge_size = 0; 
		}
		cluster_queue.push_back(i);
	}
		
}

void init_similarity(const int doc_num, const int total_num, const float cp1, const float cp2, int* doc, int* term_id, float* term_weight, float* doc_similarity, vector<struct similarity_node>& cluster_similarity, struct distance_node* distance1, bool* valid1, bool* valid2, string mode)
{
	int k;
	int k2;
	float temp1;
	float temp2;
	int b1;
	int b2;
	struct similarity_node temp;
	float d3 = 0;
	int n1 = 0;
	int ns;
	int nb;
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
		}
		k = doc_num*i - (i+3)*i/2 + doc_num - 2;  //j=doc_num-1
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
	}
	if(mode == "subtree"){
		for(int i = 0; i < doc_num; i++){
			for(int j = i+1; j < doc_num; j++){
				k = doc_num*i - (i+3)*i/2 + j - 1;
				temp.value = doc_similarity[k];
				temp.node1 = i;
				temp.node2 = j;
				cluster_similarity.push_back(temp);
			}
		}
	}
}

bool can_merge(int& node1, int& node2, int doc_num, struct cluster_node* cluster_tree, struct distance_node* distance2, float* distance3){
	float d1;
	float d2;
	int k;
	if(cluster_tree[node1].merge_size != 0 && cluster_tree[node2].merge_size != 0){
		d1 = cluster_tree[node1].merge_size + cluster_tree[node2].merge_size;// - 1;//bxy change
	}
	else{
		d1 = 0;
	}
	for(int i = 0; i < cluster_tree[node1].id.size(); i++){
		for(int j = 0; j < cluster_tree[node2].id.size(); j++){
			if(cluster_tree[node1].id[i] < cluster_tree[node2].id[j]){
				k = doc_num * cluster_tree[node1].id[i] - (cluster_tree[node1].id[i]+3)*cluster_tree[node1].id[i]/2 + cluster_tree[node2].id[j] - 1;
			}
			else{
				k = doc_num * cluster_tree[node2].id[j] - (cluster_tree[node2].id[j]+3)*cluster_tree[node2].id[j]/2 + cluster_tree[node1].id[i] - 1;
			}
			//d1 = distance3[cluster_tree[node1].id[i]] + distance3[cluster_tree[node2].id[j]] + 1;
			d2 = distance2[k].distance;
			if(d1 < d2){
				return false;
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
void merge(const int doc_num, const int node1, const int node2, const float cp1, const float cp2, struct cluster_node* cluster_tree, vector<int>& cluster_queue, float* doc_similarity, vector<struct similarity_node>& cluster_similarity, struct distance_node* distance1, float* distance3, bool* valid1, bool* valid2, string mode)
{
	int s = 2*doc_num - cluster_queue.size();
	struct similarity_node cluster_similarity1;
	int t1;
	int t2;
	float distance_internal_history;
	float distance_internal_now;
	float distance_external_history;
	float distance_external_now;
	int n_internal;
	int n_external;
	int i1 = 0;
	int i2 = 0;
	int k;
	while(i1<cluster_tree[node1].id.size() && i2<cluster_tree[node2].id.size())
	{
		if(cluster_tree[node1].id[i1] < cluster_tree[node2].id[i2])
		{
			cluster_tree[s].id.push_back(cluster_tree[node1].id[i1]);
			i1++;
		}
		else
		{
			cluster_tree[s].id.push_back(cluster_tree[node2].id[i2]);
			i2++;
		}
	}
	while(i1 < cluster_tree[node1].id.size())
	{
		cluster_tree[s].id.push_back(cluster_tree[node1].id[i1]);
		i1++;
	}
	while(i2 < cluster_tree[node2].id.size())
	{
		cluster_tree[s].id.push_back(cluster_tree[node2].id[i2]);
		i2++;
	}
	cluster_tree[s].lchild = &cluster_tree[node1];
	cluster_tree[s].rchild = &cluster_tree[node2];
	cluster_tree[s].parent = NULL;
	cluster_tree[s].node_loc = s;
	cluster_tree[s].merge_size = cluster_tree[node1].merge_size + cluster_tree[node2].merge_size;
	cluster_tree[node1].parent = &cluster_tree[s];
	cluster_tree[node2].parent = &cluster_tree[s];
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
	cluster_queue.push_back(s);
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

	if(mode == "subtree"){
		for(int i = 0; i < cluster_queue.size()-1; i++){
			cluster_similarity1.value = 0;
			for(int j1 = 0; j1 < cluster_tree[cluster_queue[i]].id.size(); j1++){
				t1 = cluster_tree[cluster_queue[i]].id[j1];
				for(int j2 = 0; j2 < cluster_tree[s].id.size(); j2++){
				    t2 = cluster_tree[s].id[j2];
					if(t1 < t2){
						k = doc_num*t1 - t1*(t1+3)/2 + t2 - 1;
					}
					else{
						k = doc_num*t2 - t2*(t2+3)/2 + t1 -1;
					}
					cluster_similarity1.value += doc_similarity[k];
				}
			}
			cluster_similarity1.value = cluster_similarity1.value / (cluster_tree[cluster_queue[i]].id.size()*cluster_tree[s].id.size());
			cluster_similarity1.node1 = cluster_queue[i];
			cluster_similarity1.node2 = s;
			cluster_similarity.push_back(cluster_similarity1);
		}
	}
}
void fprint_cluster_tree(ofstream& fp_cluster_tree, ofstream& fp_cluster_tree_label, const int doc_num, struct cluster_node* cluster_tree, int* label)
{
	int size = 2*doc_num - 1;
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j< cluster_tree[i].id.size(); j++)
		{
			fp_cluster_tree << cluster_tree[i].id[j] << " ";
			fp_cluster_tree_label << label[cluster_tree[i].id[j]] << " ";
		}
		fp_cluster_tree << endl;
		fp_cluster_tree_label << endl;
	}
}

void fprint_relation(ofstream& fp_relation, const int doc_num, struct cluster_node* cluster_tree)
{
	int size = 2*doc_num - 1;
	for(int i = 0; i < size; i++)
	{
		fp_relation << cluster_tree[i].node_loc << " ";
		if(cluster_tree[i].parent != NULL)
		{
			fp_relation << cluster_tree[i].parent->node_loc << " ";
		}
		else 
		{
			fp_relation << "X ";
		}
		if(cluster_tree[i].lchild != NULL)
		{
			fp_relation << cluster_tree[i].lchild->node_loc << " ";
		}
		else 
		{
			fp_relation << "X ";
		}
		if(cluster_tree[i].rchild != NULL)
		{
			fp_relation << cluster_tree[i].rchild->node_loc << " ";
		}
		else 
		{
			fp_relation << "X ";
		}
		fp_relation << endl;
	}
}

void fprint_relation_link(ofstream& fp_relation_link, const int doc_num, struct cluster_node* cluster_tree)
{
	struct cluster_node* p;
	for(int i = 0; i < doc_num; i++)
	{
		fp_relation_link << cluster_tree[i].node_loc << " ";
		p = cluster_tree[i].parent;
		while(p != NULL)
		{
			fp_relation_link << p->node_loc << " ";
			p = p->parent;
		}
		fp_relation_link << endl;
	}
}
int read_distance(ifstream& fp_distance, const int doc_num, struct distance_node* distance1, struct distance_node* distance2, bool* valid1, bool* valid2){
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
	}
	int tag;
	int tag_merge;
	int doc_id1;
	int doc_id2;
	float distance;
	while(fp_distance >> doc_id1){
		fp_distance >> doc_id2;
		doc_id1--;
		doc_id2--;
		fp_distance >> distance;
		//fp_distance >> tag;
		//fp_distance >> tag_merge;
		valid1[doc_id1] = true;
		valid1[doc_id2] = true;
		k = doc_num * doc_id1 - (doc_id1+3)*doc_id1/2 + doc_id2 - 1;
		//if(tag == 0){
		//	valid2[k] = true;
		//	distance1[k].distance = distance;
		//}
		//if(tag_merge == 0){
		distance2[k].distance = distance;
		//}
	}
	return 0;
}
int main(int argc, char* argv[])
{
	if(argc != 4){
		cout << "usage: arg1 data, arg2 label, arg3 distance" << endl;
		return 1;
	}
	int doc_num;
	int term_num;
	int total_num;
	int node1;
	int node2;
	float cp1;			//change paraneter
	float cp2;
	string mode = "subtree"; 			
	
	fstream fp_data(argv[1]);
	if(fp_data == NULL)
	{
		cerr << "File data can't be opened!" << endl;
		return 1;
	}
	ifstream fp_label(argv[2]);
	if(fp_label == NULL)
	{
		cerr << "File label can't be opened!" << endl;
		return 1;
	}
	ofstream fp_cluster_tree_label("cluster_tree_label");
	if(fp_cluster_tree_label == NULL)
	{
		cerr << "File cluster_tree_label can't created!" << endl;
		return 1;
	}
	
	ofstream fp_cluster_tree("cluster_tree");
	if(fp_cluster_tree == NULL)
	{
		cerr << "File cluster_tree can't be created!" << endl;
		return 1;
	}
	ifstream fp_distance(argv[3]);
	read_num(fp_data, doc_num, term_num, total_num);
	int* doc = new int[doc_num];
	int* term_id = new int[total_num];
	float* term_frequency = new float[total_num];
	int* doc_frequency = new int[term_num];
	float* term_weight = new float[total_num];
	int* label = new int[doc_num];
	struct distance_node* distance1 = new struct distance_node[doc_num*(doc_num-1)/2]; //determinate distance for leaf2leaf, in one class
	struct distance_node* distance2 = new struct distance_node[doc_num*(doc_num-1)/2]; //shortest distance for leaf2leaf, undeterminate distance, between two class
	float* distance3 = new float[doc_num]; //leaf2root distance
	for(int i = 0; i < doc_num; i++)
	{
		distance3[i] = 0; //leaf2root distance
	}
	bool* valid1 = new bool[doc_num]; // If one document is distanced with other documents, the valid1 is true. Otherwise, the valid1 is false.
	bool* valid2 = new bool[doc_num*(doc_num-1/2)]; //If leaf2leaf distance is determinate distance, the valid2 is true. Otherwise, the valid2 is false.
	vector<int> cluster_queue;
  	struct cluster_node* cluster_tree = new cluster_node[2*doc_num-1];
	float* doc_similarity = new float[(doc_num-1)*doc_num/2];
	vector<struct similarity_node> cluster_similarity;
	read_data(fp_data, doc_num, term_num, doc_frequency, doc, term_id, term_frequency);
	read_label(fp_label, doc_num, label);
	read_distance(fp_distance, doc_num, distance1, distance2, valid1, valid2);
    weight(doc_frequency, doc_num, total_num, doc, term_id, term_frequency, term_weight);
	init_cluster(doc_num, cluster_tree, cluster_queue, valid1);
	init_similarity(doc_num, total_num, cp1, cp2, doc, term_id, term_weight, doc_similarity, cluster_similarity, distance1, valid1, valid2, mode);
	vector<struct similarity_node>::iterator iter1;
	vector<struct similarity_node>::iterator iter2;
	//mode : subtree
	if(mode == "subtree" || mode == "combined1" || mode == "combined2" || mode == "combined3" || mode == "combined4"){
		while(cluster_queue.size() != 1){
			iter1 = cluster_similarity.begin();
			iter2 = cluster_similarity.end();
			max_similarity(node1, node2, cluster_similarity.begin(), cluster_similarity.end());
			while(can_merge(node1, node2, doc_num, cluster_tree, distance2, distance3) == false){
   	            if(iter1 < iter2){
      	        	pop_heap(iter1, iter2, cmp);
        	        iter2--;
            	    node1 = iter1->node1;
               	    node2 = iter1->node2;
            	}
            	else{
                	iter2 = cluster_similarity.end();
                	iter2--;
                	node1 = iter2->node1;
                	node2 = iter2->node2;
                	break;
           		}
			}
			merge(doc_num, node1, node2, cp1, cp2, cluster_tree, cluster_queue, doc_similarity, cluster_similarity, distance1, distance3, valid1, valid2, mode);
		}
	}
	
	fprint_cluster_tree(fp_cluster_tree, fp_cluster_tree_label, doc_num, cluster_tree, label);
	//fprint_relation(fp_relation, doc_num, cluster_tree);
	//fprint_relation_link(fp_relation_link, doc_num, cluster_tree);
	return 0;
}