#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<assert.h>
using namespace std;
#define FIRST_TERM_ID 1
#define FIRST_DOC_ID 0
#define DIFF 0.05f

struct constraint_node
{
	int x;
	int y;
	int z;
};
void read_num(fstream& fp_data, int& doc_num, 
				int& term_num, int& total_num);
void read_data(fstream& fp_data, int doc_num, 
				int term_num, int* doc_frequency, 
				int* doc, int* term_id, float* term_frequency);
void read_constraint(fstream & fp_constraint, 
					vector<struct constraint_node>** constraint_queue, int &constraint_level_num);
void get_weight(ofstream & fp_weight, float* term_final_weight, int term_num, int* doc, 
				int* term_id, float* term_base_vector,vector<struct constraint_node> *constraint_queue, 
				int constraint_level_num, int iteration_num);
void get_level_weight(float* term_weight, int term_num, int* doc, int* term_id, float* term_base_vector,
				vector<struct constraint_node>& constraint_queue, int iteration_num);
void base_vector(int* doc_frequency, const int doc_num, int* doc, int* term_id, 
				float* term_frequency, float* term_base_vector);
void fprint_weight(ofstream & fp_weight, float *term_weight, int term_num);
float similarity(int o1, int o2, int* doc, int* term_id, 
				float* term_base_vector, float* term_weight, 
				float &o1_length2, float &o2_length2);
int main(int argc, char* argv[])
{
	if(argc != 4){
		cout << "usage: arg1:data, arg2:constraint, arg3:iteration_num" << endl;
	}
	int doc_num;
	int term_num;
	int total_num;
	
	fstream fp_data(argv[1]);
	if(fp_data == NULL)
	{
		cout << "File data cannot open!";
		return 1;
	}
	fstream fp_constraint(argv[2]);
	if(fp_constraint == NULL)
	{
		cout << "File constraint cannot open!";
		return 1;
	}
	int iteration_num;
	sscanf(argv[3], "%d", &iteration_num);
	ofstream fp_weight("weight");
	if(fp_weight == NULL){
		cout << "File weight cannot creat!";
		return 1;
	}
	read_num(fp_data, doc_num, term_num, total_num);
	int* doc = new int[doc_num + 1];
	int* term_id = new int[total_num];
	float* term_frequency = new float[total_num];
	float* term_final_weight = new float[term_num + FIRST_TERM_ID];
	int* doc_frequency = new int[term_num + FIRST_TERM_ID];
	float* term_base_vector = new float[total_num];
	vector<struct constraint_node> *constraint_queue;

	read_data(fp_data, doc_num, term_num, doc_frequency, doc, term_id, term_frequency);
	doc[doc_num] = total_num;
	int constraint_level_num;
	read_constraint(fp_constraint, &constraint_queue, constraint_level_num);
    base_vector(doc_frequency, doc_num, doc, term_id, term_frequency, term_base_vector);
	get_weight(fp_weight, term_final_weight, term_num, doc, term_id, term_base_vector,
			   constraint_queue, constraint_level_num, iteration_num);
	delete [] constraint_queue;
	delete [] doc;
	delete [] term_id;
	delete [] term_frequency;
	delete [] term_final_weight;
	delete [] term_base_vector;
	return 0;
}

void read_num(fstream& fp_data, int& doc_num, int& term_num, int& total_num)
{
	fp_data >> doc_num;
	fp_data >> term_num;
	fp_data >> total_num;
}

void read_data(fstream& fp_data, int doc_num, int term_num, 
				int* doc_frequency, int* doc, int* term_id, 
				float* term_frequency)
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
		int termsInDoc = 0;
		while(strline >> t_id)
		{
			doc_frequency[t_id]++;
			term_id[loc] = t_id;
			strline >> term_frequency[loc];
			termsInDoc += term_frequency[loc];
			loc++;
		}
		for(int j = doc[i]; j < loc; j++) {
			term_frequency[j] /= termsInDoc;
		}
		
	}
}
void read_constraint(fstream & fp_constraint, 
					vector<struct constraint_node> **constraint_queue, int & constraint_level_num)
{
	string strin;
	getline(fp_constraint, strin);
	stringstream strline(strin);
	strline>>constraint_level_num;
	(*constraint_queue) = new std::vector<struct  constraint_node>[constraint_level_num];
	while(getline(fp_constraint, strin))
	{
		stringstream strline(strin);
		struct constraint_node cn;
		strline>>cn.x;
		strline>>cn.y;
		strline>>cn.z;
		int level;
		strline>>level;
		cn.x -= FIRST_DOC_ID;
		cn.y -= FIRST_DOC_ID;
		cn.z -= FIRST_DOC_ID;
		(*constraint_queue)[level].push_back(cn);
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
float similarity(int o1, int o2, int* doc, int* term_id, 
				float* term_base_vector, float* term_weight, 
				float &o1_length2, float &o2_length2)
{
	float doc_similarity = 0;
	o1_length2 = 0;
	o2_length2 = 0;
	int b1 = doc[o1];
	int b2 = doc[o2];
	int b1max = doc[o1+1];
	int b2max = doc[o2+1];
	while(b1 < b1max && b2 < b2max)
	{
		if(term_id[b1] == term_id[b2])
		{
			doc_similarity += term_base_vector[b1] * term_base_vector[b2] * term_weight[term_id[b1]];
			o1_length2 += term_base_vector[b1] * term_base_vector[b1] * term_weight[term_id[b1]];
			o2_length2 += term_base_vector[b2] * term_base_vector[b2] * term_weight[term_id[b1]];
			b1++;
			b2++;
		}
		else if(term_id[b1] < term_id[b2])
		{
			o1_length2 += term_base_vector[b1] * term_base_vector[b1] * term_weight[term_id[b1]];
			b1++;
		}
		else
		{
			o2_length2 += term_base_vector[b2] * term_base_vector[b2] * term_weight[term_id[b2]];
			b2++;
		}
	}
	while(b1 < b1max)
	{
		o1_length2 += term_base_vector[b1] * term_base_vector[b1] * term_weight[term_id[b1]];
		b1++;
	}
	while(b2 < b2max)
	{
		o2_length2 += term_base_vector[b2] * term_base_vector[b2] * term_weight[term_id[b2]];
		b2++;
	}
	//assert(o1_length2!=0 && o2_length2!=0);
	doc_similarity = doc_similarity / ( sqrt(o1_length2) * sqrt(o2_length2) );
	return doc_similarity;
}
void get_weight(ofstream & fp_weight, float* term_final_weight, int term_num, int* doc, 
				int* term_id, float* term_base_vector,
				vector<struct constraint_node> *constraint_queue, int constraint_level_num, int iteration_num)
{
	fp_weight<<term_num<<" "<<constraint_level_num<<endl;
	for(int i = FIRST_TERM_ID; i < term_num + FIRST_TERM_ID; i++)
	{
		term_final_weight[i] = 0;
	}
	float *term_level_weight = new float[term_num + FIRST_TERM_ID];
	for(int i = 0; i < constraint_level_num; i++)
	{
		cout<<"Level "<<i<<": constraint_num:"<<constraint_queue[i].size()<<endl;
		get_level_weight(term_level_weight, term_num, doc, term_id, term_base_vector, constraint_queue[i], iteration_num);
		for(int j = FIRST_TERM_ID; j < term_num + FIRST_TERM_ID; j++)
		{
			term_final_weight[j] += term_level_weight[j] * (2 * i + 1);
			fp_weight<<term_level_weight[j]<<" ";
		}
		fp_weight<<endl;
	}
	float sumw = 0;
	/*其实可以推断sumw = term_num*sigma(2*i+1)*/
	for(int i = FIRST_TERM_ID; i < term_num + FIRST_TERM_ID; i++)
	{
		sumw += term_final_weight[i];
	}
	//cout<<sumw<<endl;
	float factor = term_num/sumw;
	for(int i = FIRST_TERM_ID; i < term_num + FIRST_TERM_ID; i++)
	{
		term_final_weight[i] *= factor;
		fp_weight<<term_final_weight[i]<<" ";
	}
	fp_weight<<endl;
	delete [] term_level_weight;
}
void get_level_weight(float* term_weight, int term_num, int* doc, int* term_id, float* term_base_vector,
				vector<struct constraint_node>& constraint_queue, int iteration_num)
{
	float lowest_sim_diff = 0.2;
	for(int i = FIRST_TERM_ID; i < term_num+FIRST_TERM_ID; i++)
	{
		term_weight[i] = 1;
	}
	bool end = false;
	int k = 1;
	while(!end)
	{
		int change_count = 0;
		float yipsi = 20.0f/(k*k);/*设置步长*/
		for(int i = 0; i < constraint_queue.size(); i++)
		{
			struct constraint_node cn = constraint_queue[i];
			float sim_xy, sim_tz;
			int b1, b2, bz, b1max, b2max, bzmax;
			float near1_length_square, near2_length_square, z_length_square;
			for(int direction = 0; direction < 2; direction++)
			{
				if(direction == 0 )
				{
					/*sim_xy - sim_xz*/
					sim_xy = similarity(cn.x, cn.y, doc, term_id, 
						term_base_vector, term_weight, 
						near1_length_square, near2_length_square);
					sim_tz = similarity(cn.x, cn.z, doc, term_id, 
						term_base_vector, term_weight, 
						near1_length_square, z_length_square);
					b1 = doc[cn.x];
					b2 = doc[cn.y];
					bz = doc[cn.z];
					b1max = doc[cn.x+1];
					b2max = doc[cn.y+1];
					bzmax = doc[cn.z+1];

				} else{
					/*sim_yx - sim_yz*/
					sim_xy = similarity(cn.y, cn.x, doc, term_id, 
						term_base_vector, term_weight, 
						near1_length_square, near2_length_square);
					sim_tz = similarity(cn.y, cn.z, doc, term_id, 
						term_base_vector, term_weight, 
						near1_length_square, z_length_square);
					b1 = doc[cn.y];
					b2 = doc[cn.x];
					bz = doc[cn.z];
					b1max = doc[cn.y+1];
					b2max = doc[cn.x+1];
					bzmax = doc[cn.z+1];
				}
				
				if(sim_xy - sim_tz <= DIFF)
				{
					change_count++;
					float sumw = term_num;
					
					
					while(b1!=b1max || b2!=b2max || bz!=bzmax)
					{
						int j = term_num +1;
						if(b1 != b1max) {
							j = term_id[b1];
						}
						if(b2 != b2max) {
							if(term_id[b2]<j)
								j = term_id[b2];
						}
						if(bz != bzmax) {
							if(term_id[bz]<j)
								j = term_id[bz];
						}
						float detaw = 0;
						if(b1 != b1max && term_id[b1] == j)
						{
							detaw += 0.5f*(sim_tz - sim_xy) * term_base_vector[b1]*term_base_vector[b1]/near1_length_square;
							if(b2 != b2max && term_id[b2] == j)
							{
								detaw += term_base_vector[b1]*term_base_vector[b2]/(sqrt(near1_length_square*near2_length_square));
								detaw -= 0.5f*sim_xy * term_base_vector[b2]*term_base_vector[b2]/near2_length_square;
								b2 ++;
							}
							if(bz != bzmax && term_id[bz] == j)
							{
								detaw -= term_base_vector[b1]*term_base_vector[bz]/(sqrt(near1_length_square*z_length_square));
								detaw += 0.5f*sim_tz * term_base_vector[bz]*term_base_vector[bz]/z_length_square;
								bz ++;
							}
							b1 ++;
						} else {
							if(b2 != b2max && term_id[b2] == j)
							{
								detaw -= 0.5f*sim_xy * term_base_vector[b2]*term_base_vector[b2]/near2_length_square;
								b2 ++;
							}
							if(bz != bzmax && term_id[bz] == j)
							{
								detaw += 0.5f*sim_tz * term_base_vector[bz]*term_base_vector[bz]/z_length_square;
								bz ++;
							}
						}
						sumw -= term_weight[j];
						term_weight[j] += yipsi*detaw;
						if(term_weight[j] < 0)
							term_weight[j] = 0;
						sumw += term_weight[j];
					}
					float factor = term_num/sumw;
					for(int j = FIRST_TERM_ID; j < term_num + FIRST_TERM_ID; j++)
					{
						term_weight[j] *= factor;
					}
				}//end if diff
			}//end for direction
		}//end for constraint
		cout<<"\t"<<k<<"  cc:"<<change_count<<endl;
		k++;
		if(k == iteration_num + 1) end = true;/*结束条件设置*/
	}
}

void fprint_weight(ofstream & fp_weight, float *term_weight, int term_num)
{
	fp_weight<<term_num<<endl;
	for(int i = FIRST_TERM_ID; i < term_num + FIRST_TERM_ID; i++)
	{
		fp_weight<<term_weight[i]<<endl;
	}
	
}
