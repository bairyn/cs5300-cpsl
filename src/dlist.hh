#ifndef CPSL_CC_DLIST_HH
#define CPSL_CC_DLIST_HH

#include <memory>  // std::shared_ptr

template<class T>
class DList {
public:
	DList()
		{}
	DList(std::shared_ptr<DList<T>> prev, std::shared_ptr<DList<T>> next)
		: prev(prev)
		, next(next)
		{}
	std::shared_ptr<DList<T>> prev;
	std::shared_ptr<DList<T>> next;
};

#endif /* #ifndef CPSL_CC_DLIST_HH */
