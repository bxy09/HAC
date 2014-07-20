#include <iostream>
#include "hc_tree.h"
#include <assert.h>
using std::cout;
using std::endl;
extern vecint2 nodeID_distance;
int main(int argc, char* argv[])
{
	cout<<"going to gen_dis"<<endl;
	if(init_gen_distance(argc -1, argv + 1)<0)return -1;
	int node1,node2;
	cout<<"after init"<<endl;

	for(int i = 0; i < label_marked[0].size(); i++) {
		for(int j = 0; j < label_marked[0][i].size();j++) {
			for(int ii = 0; ii <= i; ii++) {
				for(int jj = 0; (ii<i && jj < label_marked[0][ii].size()) || (ii==i && jj < j);jj++) {
					int temp1 = label_marked[0][i][j];
					int temp2 = label_marked[0][ii][jj];
					int k = doc_num*temp1 - (temp1+3)*temp1/2 + temp2 - 1;
					doc_similarity[k] = calc_doc_sim(temp1, temp2);
				}
			}
		}
	}

	nodeID_distance = vecint2(label_marked.size()+1);
	for(int i = 0; i < doc_num; i++) {
		nodeID_distance[0].push_back(i);
	}

	for(int i = 0; i < label_marked.size(); i++) {
		nodeID_distance[i+1].clear();
		for(int j = 0; j < label_marked[i].size(); j++) {
			cluster_queue.clear();
			if(similarity_num != 0) {
				cout<<"sim_num_error!!"<<endl;
				assert(0);
			}
			//insert node in queue and similarity
			for(int k = 0; k < label_marked[i][j].size(); k++) {
				insertNodeInSimilarity(nodeID_distance[i].at(label_marked[i][j][k]));
				//insert node in queue
				cluster_queue.push_back(nodeID_distance[i].at(label_marked[i][j][k]));
			}
			//start merge!!
			while(cluster_queue.size() != 1)
			{
				max_similarity(node1, node2);
				merge(node1, node2);
			}
			//when merge end pop the only node from queue and push in nodeID[i+1]
			nodeID_distance[i+1].push_back(cluster_queue.back());

		}
		//todo :if have different weight , learn it!!
	}
	end_gen_distance();//out put distance!!!
	return 0;
}
