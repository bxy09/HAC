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
	int level_num; //the number of level
	int doc_num;
	int label_temp;
	int id;
	ifstream fp_label_marked(argv[1]);
	if(fp_label_marked == NULL)
	{
		cout << "File label_marked can't open.\n";
		return 1;
	}
	ofstream fp_constraints("mlb_constraints");
	if(fp_constraints == NULL)
	{
		cout << "File constraints can't creat!\n";
		return 1;
	}

	fp_label_marked >> level_num;
	int* class_num = new int[level_num];//a array for all level
	fp_label_marked >> doc_num;
	vector<int>** label_level =  new vector<int>*[level_num];
	for(int i = 0; i < level_num; i++)
	{
		fp_label_marked >> class_num[i];
		label_level[i] = new vector<int>[class_num[i]];
	}
	
	for(int i = 0; i < doc_num; i++){
		fp_label_marked >> id;
		for(int j = 0; j < level_num; j++){
			fp_label_marked >> label_temp; 
			label_level[j][label_temp-1].push_back(id);
		}
	}
	fp_constraints << level_num << endl;
	for(int level = 0; level < level_num; level++){
		for(int class1 = 0; class1 < class_num[level]; class1++){
			for(int x = 0; x < label_level[level][class1].size(); x++){
				for(int y = x+1; y < label_level[level][class1].size(); y++){
					for(int class2 = 0; class2 < class_num[level]; class2++){
						if(class2 != class1){
							for(int z = 0; z < label_level[level][class2].size(); z++){
								fp_constraints << label_level[level][class1][x] << " " << label_level[level][class1][y] << " " << label_level[level][class2][z] << " " << level_num - level -1;
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
