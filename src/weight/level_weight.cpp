#include <iostream>
#include <fstream>
#include <sstream>
#define FIRST_TERM_ID 1
using namespace std;
int use_weight = 1;
static ifstream fp_weight;
static int term_count;
int weight_level;
static int current_level;
float** bonus_weight;
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
	string str("");
	getline(fp_weight,str);
	bonus_weight = new float*[weight_level];

	for(int i = weight_level - 1; i>= 0; i--) {
		bonus_weight[i] = new float[term_count+FIRST_TERM_ID];
		getline(fp_weight,str);
		int loc;
		stringstream strstr(str);
		loc = FIRST_TERM_ID;
		while(strstr >> bonus_weight[i][loc]){
			loc++;
		}
		if(loc!=term_count+FIRST_TERM_ID) {
			cout<<"Error:wtf in read_weight from average_weight"<<endl;
			return -1;
		}
	}
	fp_weight.close();
	current_level = 0;
	return 1;
}
float get_Weight(int term_id) {
	if(current_level >= weight_level) return 1;
	return bonus_weight[current_level][term_id];
}
void next_level() {
	current_level++;
}
int end_weight() {
	for(int i = 0; i < weight_level; i++) {
		delete [] bonus_weight[i];
	}
	delete [] bonus_weight;
}