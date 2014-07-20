#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "rbt.h"
template<class RBTNode>
static RBTNode **rbt_nodeptr(RBTNode **root, RBTNode *node) {
  if (*root == node) return root;
  if (node->p->left == node)
    return &(node->p->left);
  else
    return &(node->p->right);
}

template<class RBTNode>
static void rbt_rotate_left(RBTNode **root)
{
  RBTNode *node = (*root)->right;
  (*root)->right = node->left;
  if ((*root)->right != NULL)
    (*root)->right->p = (*root);
  node->left = (*root);
  node->p = (*root)->p;
  (*root)->p = node;
  (*root) = node;  
}

template<class RBTNode>
static void rbt_rotate_right(RBTNode **root)
{
  RBTNode *node = (*root)->left;
  (*root)->left = node->right;
  if ((*root)->left != NULL)
    (*root)->left->p = (*root);
  node->right = (*root);
  node->p = (*root)->p;
  (*root)->p = node;
  (*root) = node;
}

template<class RBTNode>
static RBTNode *_mostLeft(RBTNode *root){
    if(root->left  == NULL) return root;
    return _mostLeft(root->left);
}

template<class RBTNode>
RBTNode *mostLeft(RBTNode *root) {
    if(root ==NULL || root->left == NULL) return root;
    return _mostLeft(root->left);
}

template<class RBTNode>
static RBTNode *_mostRight(RBTNode *root){
    if(root->right  == NULL) return root;
    return _mostRight(root->right );
}

template<class RBTNode>
RBTNode *mostRight(RBTNode *root) {
    if(root ==NULL || root->right == NULL) return root;
    return _mostRight(root->right);
}
template<class RBTNode,class FunType>
bool visitFromLargeToLow(RBTNode *root,FunType func) {
    if(root==NULL) return 1;
    if(!visitFromLargeToLow(root->right,func)){
        return 0;
    }
    if(!func(root)) return 0;
    return visitFromLargeToLow(root->left,func);
}


template<class RBTNode, class RBTNodeKey>
RBTNode *rbt_search(RBTNode *root, RBTNodeKey key){
  if (root == NULL || root->key == key) return root;
  else if (root->key > key) return rbt_search(root->left, key);
  else return rbt_search(root->right, key);
  /*RBTNode *node = root;
  while (node != NULL && node->key != key) {
    if (node->key > key) node = node->left;
    else node = node->right;
  }
  return node;*/
}

template<class RBTNode>
static void rbt_insert_help(RBTNode **root, RBTNode* node) {
  if (*root == NULL) {
    node->left = NULL;
    node->right = NULL;
    node->red = true;
    *root = node;
    return;
  }
  node->p = *root;
  if (node->key > (*root)->key) {
    rbt_insert_help(&((*root)->right), node);
  } else {
    rbt_insert_help(&((*root)->left), node);
  }
}

template<class RBTNode>
static void rbt_insert_fixup(RBTNode **root, RBTNode* node) {
  if ((*root) == node) { // insert case 1
    (*root)->red = false;
    return;
  }
  RBTNode *parent = node->p;
  if (!parent->red) // insert case 2
    return;
    
  RBTNode *grandparent = parent->p;
  RBTNode *uncle;
  if (parent == grandparent->left) {
  	  uncle = grandparent->right;
  	if (uncle != NULL && uncle->red) { // insert case 3
    	uncle->red = false;
    	parent->red = false;
    	grandparent->red = true;
    	rbt_insert_fixup(root, grandparent);
	    return;
  	}
  	if (node == parent->right) { // insert case 4
  		rbt_rotate_left(&(grandparent->left));
  		RBTNode *tmp = parent;
  		parent = node;
  		node = tmp;
  	}
  	parent->red = false;
  	grandparent->red = true;
  	rbt_rotate_right(rbt_nodeptr(root, grandparent));
  	rbt_insert_fixup(root, node);
  } else {
    uncle = grandparent->left;
  	if (uncle != NULL && uncle->red) { // insert case 3
    	uncle->red = false;
    	parent->red = false;
    	grandparent->red = true;
    	rbt_insert_fixup(root, grandparent);
	    return;
  	}
  	if (node == parent->left) { // insert case 4
  		rbt_rotate_right(&(grandparent->right));
  		RBTNode *tmp = parent;
  		parent = node;
  		node = tmp;
  	}
  	parent->red = false;
  	grandparent->red = true;
  	rbt_rotate_left(rbt_nodeptr(root, grandparent));
  	rbt_insert_fixup(root, node);
  }
}

template<class RBTNode>
void rbt_insert(RBTNode **root, RBTNode *node) {
  rbt_insert_help(root, node);
  rbt_insert_fixup(root, node);
}

template<class RBTNode>
static void rbt_remove_fixup(RBTNode **root, RBTNode *node, RBTNode *parent) {
  RBTNode *other;
  while ((!node || !node->red) && node != *root) {
    if (node == parent->left) {
      other = parent->right;
      if (other->red) {
	other->red = false;
	parent->red = true;
	rbt_rotate_left(rbt_nodeptr(root, parent));
	other = parent->right;
      }
      if ((other->left == NULL || !other->left->red) && (other->right == NULL || !other->right->red)) {
	other->red = true;
	node = parent;
	parent = node->p;
      } else {
	if (other->right == NULL || !other->right->red) {
	  other->left->red = false;
	  other->red = true;
	  rbt_rotate_right(&parent->right/*rbt_nodeptr(root, other)*/);
	  other = parent->right;
	}
	other->red = parent->red;
	parent->red = false;
	other->right->red = false;
	rbt_rotate_left(rbt_nodeptr(root, parent));
	node = *root;
	break;
      }
    } else {
      other = parent->left;
      if (other->red) {
	other->red = false;
	parent->red = true;
	rbt_rotate_right(rbt_nodeptr(root, parent));
	other = parent->left;
      }
      if ((other->left == NULL || !other->left->red) && (other->right == NULL || !other->right->red)) {
	other->red = true;
	node = parent;
	parent = node->p;
      } else {
	if (other->left == NULL || !other->left->red) {
	  other->right->red = false;
	  other->red = true;
	  rbt_rotate_left(&parent->left/*rbt_nodeptr(root, other)*/);
	  other = parent->left;
	}
	other->red = parent->red;
	parent->red = false;
	other->left->red = false;
	rbt_rotate_right(rbt_nodeptr(root, parent));
	node = *root;
	break;
      }
    }
  }
  if (node) node->red = false;
}

template<class RBTNode>
void rbt_remove(RBTNode **root, RBTNode* node) {
  RBTNode *parent, *child;
  bool red;
  if (node->left == NULL) {
    child = node->right;
  } else if (node->right == NULL) {
    child = node->left;
  } else {
    RBTNode *x = node->left;
    if (x->right != NULL) {
      do {
	x = x->right;
      } while (x->right != NULL);
      parent = x->p;
      child = x->left;
      x->p->right = x->left;
      if (x->left) x->left->p = x->p;
      x->left = node->left;
      x->left->p = x;
    } else {
      parent = x;
      child = x->left;
    }
    x->right = node->right;
    x->right->p = x;
    x->p = node->p;
    *rbt_nodeptr(root, node) = x;
    red = x->red;
    x->red = node->red;
    goto FIXUP;
  }
  parent = node->p;
  red = node->red;
  if (child)
    child->p = parent;
  *rbt_nodeptr(root, node) = child;
FIXUP:
  if (!red) {
    rbt_remove_fixup(root, child, parent);
    /*if (child == *root || (child && child->red)) {
      if (child)
	child->red = false;
    } else {
      rbt_remove_fixup(root, child, parent);
      }*/
  }
}

template<class RBTNode, class RBTNodeKey>
RBTNode* rbt_delete(RBTNode **root, RBTNodeKey key) {
  RBTNode *node = rbt_search(*root, key);
  rbt_remove(root, node);
  return node;
}
/*
int main(int argc, char *argv[])
{
  if (argc <= 1) {
    printf("Usage: %s n, n is the number of tree nodes.", argv[0]);
    return 1;
  }
    int N = atoi(argv[1]);
    RBTNode *tree = NULL;
    RBTNode *nodes = NULL;
    nodes = (RBTNode *)malloc(N * sizeof(RBTNode));
    RBTNode **node_ptrs = NULL;
    node_ptrs = (RBTNode **)malloc(N * sizeof(RBTNode *));
    srand(static_cast<unsigned long>(time(NULL)));
    // srand((unsigned)clock());
    for (int i = 0; i < N; i++) {
      nodes[i].key = rand();
    }

    long begin, start, stop;
    begin = clock();
    start = begin;
    for (int i = 0; i < N; i++) {
      rbt_insert(&tree, &(nodes[i]));
    }
    stop = clock();
    printf("Insert %d : %ld\n", N, stop - start);

    printf("\nTotal clocks till now : %ld\n\n", stop - begin);

    start = stop;
    for (int i = 0; i < N; i++) {
      assert(rbt_search(tree, nodes[i].key) != NULL);
    }
    stop = clock();
    printf("Search key of all : %ld\n",  stop - start);

    printf("\nTotal clocks till now : %ld\n\n", stop - begin);

    {
      int z = 1;
      while (z <= N) {
	start = stop;
	for (int i = 0; i < z; i++) {
	  rbt_remove(&tree, &(nodes[i]));
	}
	stop = clock();
	printf("Remove %d by node, search rest, insert back : %ld", z, stop - start);
	start = stop;
	for (int i = z; i < N; i++) {
	  assert(rbt_search(tree, nodes[i].key) != NULL);
	}
	stop = clock();
	printf(", %ld", stop - start);
	start = stop;
	for (int i = 0; i < z; i++) {
	  rbt_insert(&tree, &(nodes[i]));
	}
	stop = clock();
	printf(", %ld\n", stop - start);
	z <<= 1;
      }
    }

    printf("\nTotal clocks till now : %ld\n\n", stop - begin);

    {
      int z = 1;
      while (z <= N) {
	start = stop;
	for (int i = 0; i < z; i++) {
	  node_ptrs[i] = rbt_delete(&tree, nodes[i].key);
	}
	stop = clock();
	printf("Remove %d by key, insert back : %ld", z, stop - start);
	start = stop;
	for (int i = 0; i < z; i++) {
	  rbt_insert(&tree, node_ptrs[i]);
	}
	stop = clock();
	printf(", %ld\n", stop - start);
	start = stop;
	z <<= 1;
      }
    }

    printf("\nTotal clocks till now : %ld\n\n", stop - begin);

    start = stop;
    for (int i = 0; i < N; i++) {
      rbt_remove(&tree, &(nodes[i]));
    }
    stop = clock();
    printf("Remove all by nodes : %ld\n", stop - start);

    printf("\nTotal clocks till now : %ld\n\n", stop - begin);

    free(nodes);
    return 0;
}
*/