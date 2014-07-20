#include<iostream>
#include<fstream>
#include<vector>
using namespace std;
int main(int argc, char* argv[])
{
	if(argc != 2){
		cout << "usage: arg1 label_marked" << endl;
		return 1;
	}
	int num_level; //the number of level
	int num_doc;
	int label_temp;
	int id;
	ifstream fp_label_marked(argv[1]);
	if(fp_label_marked == NULL)
	{
		cout << "File label_marked can't open.\n";
		return 1;
	}
	ofstream fp_constraints("constraints");
	if(fp_constraints == NULL)
	{
		cout << "File constraints can't creat!\n";
		return 1;
	}

	fp_label_marked >> num_level;
	int* class_num = new int[num_level];//a array for all level
	fp_label_marked >> num_doc;
	vector<int>** label_level =  new vector<int>*[num_level];
	for(int i = 0; i < num_level; i++)
	{
		fp_label_marked >> class_num[i];
		label_level[i] = new vector<int>[class_num[i]];
	}
	
	for(int i = 0; i < num_doc; i++){
		fp_label_marked >> id;
		for(int j = 0; j < num_level; j++){
			fp_label_marked >> label_temp; 
			label_level[j][label_temp-1].push_back(id);
		}
	}
	for(int i1 = 0; i1 < num_level; i1++){
		for(int i2 = 0; i2 < class_num[i1]; i2++){
			for(int j1 = 0; j1 < label_level[i1][i2].size(); j1++){
				for(int j2 = j1+1; j2 < label_level[i1][i2].size(); j2++){
					for(int k1 = 0; k1 < class_num[i1]; k1++){
						if(k1 != i2){
							for(int k2 = 0; k2 < label_level[i1][k1].size(); k2++){
								fp_constraints << label_level[i1][i2][j1] << " " << label_level[i1][i2][j2] << " " << label_level[i1][k1][k2];
								fp_constraints << endl;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
