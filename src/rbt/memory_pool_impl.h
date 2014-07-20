#include "memory_pool.h"
template<class object>
memory_pool<object>::memory_pool() {
	freeVec.clear();
	lastApply = 1000;
	current_space = new object[lastApply];
	freeSize = lastApply;
	spaces.push_back(current_space);
}
template<class object>
void memory_pool<object>::del(object* tar) {
	freeVec.push_back(tar);
}
template<class object>
object* memory_pool<object>::apply(void) {
	if(freeVec.size()!=0) {
		object* ret =  freeVec.back();
		freeVec.pop_back();
		return ret;
	}
	if(freeSize != 0) {
		freeSize--;
		return current_space ++;
	}
	lastApply*=2;
	current_space = new object[lastApply];
	freeSize = lastApply - 1;
	spaces.push_back(current_space);
	return current_space++;
}
template<class object>
void memory_pool<object>::clear_pool() {
	for(int i = 0; i < spaces.size(); i++) {
		delete [] spaces[i];
	}
	spaces.clear();
	freeVec.clear();
	lastApply = 1000;
	current_space = new object[lastApply];
	freeSize = lastApply;
	spaces.push_back(current_space);
}
template<class object>
memory_pool<object>::~memory_pool() {
	for(int i = 0; i < spaces.size(); i++) {
		delete [] spaces[i];
	}
}