#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cmath>
using namespace std;

struct class_node
{
	int label;
	float num;
	float fscore;
};

struct cluster_tree_node
{
	vector<int> label;
	float num;
};

void init_class(ifstream&, vector<struct class_node>&, vector<int>&);
void init_cluster(ifstream&, vector<struct cluster_tree_node>&, vector<int>&);
void init_nir(vector<struct class_node>&, vector<struct cluster_tree_node>&, float**);
void init_recall(vector<struct class_node>&, vector<struct cluster_tree_node>&, float**, float**);
void init_precision(vector<struct class_node>&, vector<struct cluster_tree_node>&, float**, float**);
void init_fsc(int, int, float**, float**, float**);
void class_fscore(vector<struct class_node>& label_class, vector<struct cluster_tree_node>& cluster, float** fsc, ofstream& fp_fscore);
float Fscore(vector<struct class_node>&);

int main(int argc, char* argv[])
{
	if(argc != 3){
		cout  << "usage: arg1 label, arg2 cluster_tree" << endl;
	}
	ifstream fp1(argv[1]);
	if(!fp1)
	{
		cout << "Cannot open file label!";
		return 1;
	}

	ifstream fp2(argv[2]);
	if(!fp2)
	{
		cout << "Cannot open file class_tree_label!";
		return 1;
	}
	ofstream fp_fscore("fscore");
	vector<struct class_node> label_class;
	vector<int> doc_label;
	init_class(fp1, label_class, doc_label);
	vector<struct cluster_tree_node> cluster;
	init_cluster(fp2,cluster, doc_label);
	float ** nir = new float*[cluster.size()]; 
	init_nir(label_class,cluster,nir);

	float **recall = new float*[cluster.size()];
	float **precision = new float*[cluster.size()];
	init_recall(label_class,cluster,nir,recall);
	init_precision(label_class,cluster,nir,precision);

	float **fsc =new float*[cluster.size()];
	init_fsc(cluster.size(),label_class.size(),fsc,recall,precision);

	class_fscore(label_class,cluster,fsc, fp_fscore);

	float fscore;
	fscore = Fscore(label_class);
	fp_fscore << fscore << endl;

	return 0;

}

void init_class(ifstream& fp1, vector<struct class_node>& label_class, vector<int>& doc_label)
{
	struct class_node temp;
	int label;

	while(fp1 >> label)
	{
		doc_label.push_back(label);
		vector<struct class_node>::iterator iter;
		for(iter = label_class.begin(); iter != label_class.end(); iter++)
		{
			if(iter->label == label)
			{
				iter->num ++;
				break;
			}
		}
		if(iter == label_class.end())
		{
			temp.label = label;
			temp.fscore = 0;
			temp.num = 1;
			label_class.push_back(temp);
		}
	}
}

void init_cluster(ifstream& fp2, vector<struct cluster_tree_node>& cluster, vector<int>& doc_label)
{
	string strin;
	int temp;
	while(getline(fp2,strin))
	{
		struct cluster_tree_node a;
		a.num = 0;
		stringstream strs(strin);
		while(strs >> temp)
		{
			a.label.push_back(doc_label[temp]);
			//a.label.push_back(temp);
			a.num++;
		}
		cluster.push_back(a);
	}
}
void init_nir(vector<struct class_node>& label_class,vector<struct cluster_tree_node>& cluster,float** nir) 
{
	for(int i = 0; i < (int)cluster.size(); i++)
	{   
		nir[i] = new float[label_class.size()];
		for(int j = 0; j< (int)label_class.size(); j++)
		{
			nir[i][j] = 0;
			for(int k = 0; k < cluster[i].label.size(); k++)
			{   
				if(cluster[i].label[k] == label_class[j].label)
 				{
					nir[i][j] ++;
				}
			}
		}
	}
}

void init_recall(vector<struct class_node>& label_class, vector<struct cluster_tree_node>& cluster, float** nir, float** recall)
{
	for(int i = 0; i < cluster.size(); i++)
	{
		recall[i] = new float[label_class.size()];
		for(int j = 0; j < label_class.size(); j++)
		{
			recall[i][j] = nir[i][j] / label_class[j].num;
		}
	}
}

void init_precision(vector<struct class_node>& label_class, vector<struct cluster_tree_node>& cluster, float** nir, float** precision)
{
	for(int i = 0; i < cluster.size(); i++)
	{
		precision[i] =new float[label_class.size()];
		for(int j = 0; j < label_class.size(); j++)
		{
			precision[i][j] = nir[i][j] / cluster[i].num;
		}
	}
}

void init_fsc(int num1, int num2, float** fsc, float** recall, float** precision)
{
	float EPSINON = (float)0.00001;
    for(int i = 0; i < num1; i++)
	{
		fsc[i] = new float[num2];
		for(int j = 0; j < num2; j++)
		{
			if(fabs(recall[i][j]) < EPSINON && fabs(precision[i][j]) < EPSINON)
			{
				fsc[i][j] = 0;
			}
			else
			{
				fsc[i][j] = 2*recall[i][j]*precision[i][j]/(recall[i][j] + precision[i][j]);
			}
		}
	}
}

void class_fscore(vector<struct class_node>& label_class, vector<struct cluster_tree_node>& cluster, float** fsc, ofstream& fp_fscore)
{
	float temp;
	int temp_i;
	for(int j = 0; j < label_class.size(); j++)
	{
		temp = 0;
		for(int i = 0; i < cluster.size(); i++)
		{
			if(fsc[i][j] > temp)
			{
				temp = fsc[i][j];
			//	temp_i = i;  //print which node is selected for fscore
			}
		}
		label_class[j].fscore = temp;
		//fp_fscore << label_class[j].label << " " << label_class[j].num << " " << label_class[j].fscore << endl;
		//fp_fscore << temp_i+1 << ": ";
		//for(int k = 0; k < cluster[temp_i].label.size(); k++){ //print which node is selected for fscore
		//	fp_fscore << cluster[temp_i].label[k] << " ";
		//}
		//fp_fscore << endl;
	}
}

float Fscore(vector<struct class_node>& label_class)
{
	float n = 0;
	float fscore = 0;
	for(int i = 0; i < label_class.size(); i++)
	{
		n += label_class[i].num;
		fscore += label_class[i].fscore * label_class[i].num;
	}
	fscore = fscore / n;
	return fscore;
}
