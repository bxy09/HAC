#ifndef RBT_H
#define RBT_H
/*
struct _RBTNode {
  int key;
  bool red;
  struct _RBTNode *left, *right, *p;
};
*/
//typedef struct _RBTNode RBTNode;
//typedef RBTNode *RBTree;
//typedef int RBTNodeKey;
template<class RBTNode>
RBTNode *mostLeft(RBTNode* root);
template<class RBTNode>
RBTNode *mostRight(RBTNode* root);
template<class RBTNode,class FunType>
bool visitFromLargeToLow(RBTNode* root,FunType func);
template<class RBTNode, class RBTNodeKey>
RBTNode *rbt_search(RBTNode* root, RBTNodeKey key);
template<class RBTNode>
void rbt_insert(RBTNode **root, RBTNode *node);
template<class RBTNode>
void rbt_remove(RBTNode **root, RBTNode* node);
template<class RBTNode, class RBTNodeKey>
RBTNode* rbt_delete(RBTNode **root, RBTNodeKey key);
#endif