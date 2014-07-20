#include <iostream>
#include <fstream>
#include <sstream>
#define FIRST_TERM_ID 1
int use_weight = 1;
using namespace std;
static ifstream fp_weight;
static int term_count;
static int weight_level;
float* bonus_weight;
int read_weight(int argc, char* argv[]){
	if(argc < 1) {
		cout<<"parameter error!"<<endl;
		return -1;
	}
	fp_weight.open(argv[0]);
	if(fp_weight == NULL)
	{
		cout << "File weight("<<argv[0]<<") cannot open!";
		return -1;
	}
	fp_weight>>term_count;
	fp_weight>>weight_level;
	bonus_weight = new float[term_count+FIRST_TERM_ID];
	string str("");
	int loc;
	int term_num;

	getline(fp_weight,str);
	for(int i = 0; i < weight_level; i++){
		getline(fp_weight,str);
	}

	stringstream strstr(str);
	loc = FIRST_TERM_ID;
	while(strstr >> bonus_weight[loc]){
		loc++;
	}
	if(loc!=term_count+FIRST_TERM_ID) {
		cout<<"Error:wtf in read_weight from average_weight"<<endl;
		return -1;
	}
	fp_weight.close();
	return 1;
}
float get_Weight(int term_id) {
	return bonus_weight[term_id];
}
int end_weight() {
	delete [] bonus_weight;
}