#include <iostream>
#include <stdlib.h>
#include "hc_tree.h"
using std::cout;
using std::endl;
extern int read_merge_condition(int argc, char* argv[]);
extern bool can_merge(int node1, int node2);
extern void when_merge(int node1, int node2, int endNode);
bool visitIfCanMerge(similarity_node* node) {
	int node1 = node->node1,node2 = node->node2;
	if(can_merge(node1,node2)) {
		when_merge(node1,node2,merge(node1,node2));
		return 0;
	}
	return 1;
}
int main(int argc, char* argv[])
{
	int retVal = init_hc(--argc, ++argv);
	if(retVal<0)return -1;
	argc-=retVal;argv+=retVal;
	retVal = read_merge_condition(argc,argv);
	if(retVal<0)return -1;

	cout<<"after init"<<endl;
	while(cluster_queue.size() != 1)
	{
		visitSimFromLargeToLow(visitIfCanMerge);
		//cout<<rand();
	}
	end_hc();
	return 0;
}
