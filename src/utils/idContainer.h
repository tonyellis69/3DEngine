#pragma once

#include <map>

/** Simple map class for storing items with a unique id. */
template <class T>
class CIdMap {
public:
	unsigned int addItem(T item) {
		if (id == 0) //skip zero so we can use it for NULL etc
			id = 1;
		container[id] = item;
		id++;
		return id - 1;
	};
	void removeItem(unsigned int id) {
		container.erase(id);
	};
	T getItem(unsigned int id) {
		return container[id];
	}

	std::map<unsigned int, T> container;
	static unsigned int id;
};
template <class T>
unsigned int CIdMap<T>::id = 1;


