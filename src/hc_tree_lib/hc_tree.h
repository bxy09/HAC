#ifndef HC_TREE_H
#define HC_TREE_H
#include "memory_pool.h"
#include "rbt.h"
#include <vector>
struct cluster_node
{
	std::vector<int> id;
	int parent;
	int lchild;
	int rchild;
};
struct similarity_node
{
	static memory_pool<similarity_node> mem;
	bool red;
	struct similarity_node *left, *right, *p;

	float key;
	int node1;
	int node2;
};
struct reference_node
{
	static memory_pool<reference_node> mem;
	bool red;
	struct reference_node *left, *right, *p;
	int key;
	void *ref;
};
int init_gen_distance(int argc, char* argv[]);
int end_gen_distance();
void max_similarity(int& node1, int &node2);
int merge(const int node1, const int node2);
int init_hc(int argc, char* argv[]);
int end_hc();
float calc_doc_sim(int i, int j);
void insertNodeInSimilarity(int nodeD);
void visitSimFromLargeToLow(bool (*fun) (similarity_node*));
typedef std::vector<int> vecint1;
typedef std::vector<vecint1> vecint2;
typedef std::vector<vecint2> vecint3;
extern vecint3 label_marked;
extern std::vector<int> cluster_queue;
extern int similarity_num;
extern int doc_num;
extern int term_num;
extern float* doc_similarity;
extern struct cluster_node* cluster_tree;
#endif