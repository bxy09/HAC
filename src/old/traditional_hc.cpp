#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cmath>
#include<algorithm>
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

void read_num(fstream& fp_data, int& doc_num, int& term_num, int& total_num);
void read_data(fstream& fp_data, int doc_num, int term_num, int* doc_frequency, int* doc, int* term_id, float* term_frequency);
void read_label(ifstream& fp_label, const int doc_num, int* label);
void weight(int* doc_frequency, const int doc_num, const int total_num, int* doc, int* term_id, float* term_frequency, float* term_weight);
void init_cluster(const int doc_num, vector<struct cluster_node>& cluster_tree, vector<int>& clsuter_queue);
void init_similarity(const int doc_num, const int total_num, int* doc, int* term_id, float* term_weight, float* doc_similarity, vector<struct similarity_node>& cluster_similarity);
bool cmp(struct similarity_node node1, struct similarity_node node2);
void max_similarity(int& node1, int& node2, vector<struct similarity_node>& cluster_similarity);
void merge(const int doc_num, const int node1, const int node2, vector<struct cluster_node>& cluster_tree, vector<int>& cluster_queue, float* doc_similarity, vector<struct similarity_node>& similarity);
void fprint_cluster_tree(ofstream& fp_cluster_tree, ofstream& fp_cluster_tree_label, const vector<struct cluster_node>& cluster_tree, int* label);
void fprint_relation(ofstream& fp_relation, const vector<struct cluster_node>& cluster_tree);
void fprint_relation_link(ofstream& fp_relation_link, const int doc_num, const vector<struct cluster_node>& cluster_tree);

int main(int argc, char* argv[])
{
	if(argc != 3){
		cout << "usage: arg1:data, arg2:label" << endl;
	}
	int doc_num;
	int term_num;
	int total_num;
	int node1;
	int node2;
	
	fstream fp_data(argv[1]);
	if(fp_data == NULL)
	{
		cout << "File data cannot open!";
		return 1;
	}
	ifstream fp_label(argv[2]);
	if(fp_label == NULL)
	{
		cout << "File label cannot open!";
		return 1;
	}
	ofstream fp_cluster_tree("cluster_tree");
	if(fp_cluster_tree == NULL)
	{
		cout << "File cluster_tree cannot open!";
		return 1;
	}
	ofstream fp_cluster_tree_label("cluster_tree_label");
	if(fp_cluster_tree_label == NULL)
	{
		cout << "File cluster_tree_label cannot open!";
		return 1;
	}
	ofstream fp_relation("relation");
	if(fp_relation == NULL){
		cout << "File relation cannot creat!";
		return 1;
	}
	ofstream fp_relation_link("relation_link");
	if(fp_relation_link == NULL){
		cout << "File relation_link cannot creat!";
		return 1;
	}
	read_num(fp_data, doc_num, term_num, total_num);
	int* doc = new int[doc_num];
	int* term_id = new int[total_num];
	float* term_frequency = new float[total_num];
	int* doc_frequency = new int[term_num];
	float* term_weight = new float[total_num];
	int* label = new int[doc_num];
	vector<int> cluster_queue;
	vector<struct cluster_node> cluster_tree;
	float* doc_similarity = new float[(doc_num-1)*doc_num/2];
	vector<struct similarity_node> cluster_similarity;
	read_data(fp_data, doc_num, term_num, doc_frequency, doc, term_id, term_frequency);
	read_label(fp_label, doc_num, label);
    weight(doc_frequency, doc_num, total_num, doc, term_id, term_frequency, term_weight);
	init_cluster(doc_num, cluster_tree, cluster_queue);
	init_similarity(doc_num, total_num, doc, term_id, term_weight, doc_similarity, cluster_similarity);
	while(cluster_queue.size() != 1)
	{
		//cout << cluster_queue.size() << endl;
		max_similarity(node1, node2, cluster_similarity);
		merge(doc_num, node1, node2, cluster_tree, cluster_queue, doc_similarity, cluster_similarity);
	}
	fprint_cluster_tree(fp_cluster_tree, fp_cluster_tree_label, cluster_tree, label);
	fprint_relation(fp_relation, cluster_tree);
	fprint_relation_link(fp_relation_link, doc_num, cluster_tree);		
	return 0;
}

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
void init_cluster(const int doc_num, vector<struct cluster_node>& cluster_tree, vector<int>& cluster_queue)
{
	for(int i = 0; i < doc_num; i++)
	{
		struct cluster_node temp;
		temp.id.push_back(i);
		temp.parent = -1;
		temp.lchild = -1;
		temp.rchild = -1;
		cluster_tree.push_back(temp);
		cluster_queue.push_back(i);
	}
		
}

void init_similarity(const int doc_num, const int total_num, int* doc, int* term_id, float* term_weight, float* doc_similarity, vector<struct similarity_node>& cluster_similarity)
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

bool cmp(struct similarity_node node1, struct similarity_node node2)
{
	return node1.value < node2.value;
}
 
void max_similarity(int& node1, int& node2, vector<struct similarity_node>& cluster_similarity)
{
	make_heap(cluster_similarity.begin(), cluster_similarity.end(), cmp);
	node1 = cluster_similarity.begin()->node1;
	node2 = cluster_similarity.begin()->node2;

}

void merge(const int doc_num, const int node1, const int node2, vector<struct cluster_node>& cluster_tree, vector<int>& cluster_queue, float* doc_similarity, vector<struct similarity_node>& cluster_similarity)
{
	struct cluster_node cluster_node1;
	struct similarity_node cluster_similarity1;
	int t1;
	int t2;
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
	cluster_tree[node1].parent = cluster_tree.size();
	cluster_tree[node2].parent = cluster_tree.size(); 
	cluster_tree.push_back(cluster_node1);
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
	for(int i = 0; i < cluster_queue.size()-1; i++)
	{
		cluster_similarity1.value = 0;
		for(int j = 0; j < cluster_tree[cluster_queue[i]].id.size(); j++)
		{
			t1 = cluster_tree[cluster_queue[i]].id[j];
			for(int k = 0; k < cluster_tree[cluster_tree.size()-1].id.size(); k++)
			{
				t2 = cluster_tree[cluster_tree.size()-1].id[k];
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
		cluster_similarity1.value = cluster_similarity1.value / (cluster_tree[cluster_queue[i]].id.size()*cluster_tree[cluster_tree.size()-1].id.size());
		cluster_similarity1.node1 = cluster_queue[i];
		cluster_similarity1.node2 = cluster_tree.size()-1;
		cluster_similarity.push_back(cluster_similarity1);
	}
}

void fprint_cluster_tree(ofstream& fp_cluster_tree, ofstream& fp_cluster_tree_label, const vector<struct cluster_node>& cluster_tree, int* label)
{
	for(int i = 0; i < cluster_tree.size(); i++)
	{
		for(int j = 0; j< cluster_tree[i].id.size(); j++)
		{
			fp_cluster_tree << cluster_tree[i].id[j]<< " ";
			fp_cluster_tree_label << label[cluster_tree[i].id[j]] << " ";
		}
		fp_cluster_tree << endl;
		fp_cluster_tree_label << endl;
	}
}
void fprint_relation(ofstream& fp_relation, const vector<struct cluster_node>& cluster_tree){
	for(int i = 0; i < cluster_tree.size(); i++){
		fp_relation << cluster_tree[i].parent+1 << " " << cluster_tree[i].lchild+1 << " " << cluster_tree[i].rchild+1 << endl;
	}
}
void fprint_relation_link(ofstream& fp_relation_link, const int doc_num, const vector<struct cluster_node>& cluster_tree){
	int parent;
	for(int i = 0; i < doc_num; i++){
		fp_relation_link << i+1 << " ";
		parent = cluster_tree[i].parent;
		while(parent != -1){
			fp_relation_link << parent+1 << " ";
			parent = cluster_tree[parent].parent;
		}
		fp_relation_link << endl;
	}
}
