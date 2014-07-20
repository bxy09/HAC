#include "rbt_impl.h"
#include "memory_pool_impl.h"
#include <iostream>
#include <stdlib.h>
using namespace std;
class IntNode{
public:
	static memory_pool<IntNode> mem;
	int key;
	bool red;
	struct IntNode *left, *right, *p;
};
memory_pool<IntNode> IntNode::mem = memory_pool<IntNode>();
bool print(IntNode* node) {
	cout<<node->key<<" ";
	return 1;
}
int main() {
	IntNode * root = 0;
	for(int  i = 0; i < 20 ; i++) {
		IntNode * temp = IntNode::mem.apply();
		temp->key = 10+i;
		rbt_insert(&root,temp);
	}
	visitAll(root,print);
	cout<<endl;
	for (int i = 0; i < 20 ; i++) {
		rbt_remove(&root,root);
		visitAll(root,print);
		cout<<endl;
	}
	
	return 0;
}