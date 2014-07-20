#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H
#include <vector>
template<class object>
class memory_pool {
	std::vector<object*> freeVec;
	std::vector<object*> spaces;
	object* current_space;
	int freeSize;
	int lastApply;
public:
	memory_pool();
	~memory_pool();
	void del(object*);
	object* apply(void);
	void clear_pool();
};
#endif