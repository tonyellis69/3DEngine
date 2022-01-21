#pragma once

#include <functional>
#include <typeinfo>       // operator typeid
#include <typeindex>      // std::type_index

#include "event.h"

namespace lis {

	template <typename objT>
	void subscribe(objT* obj) {
		std::function<void(CEvent&)> f = [obj](CEvent& e) {
			obj->onEvent(e);
		};
		subscribe_X(f, std::type_index(typeid(obj)));
	}

	template <typename objT>
	void unsubscribe(objT* obj) {
		unsubscribe_X(std::type_index(typeid(obj)));
	}

	void event(CEvent& e);	
}

void subscribe_X(std::function<void(CEvent&)>& callback, std::type_index idx);
void unsubscribe_X(std::type_index idx);
void unsubscribe_X(int id);
int getId_X(std::type_index idx);

template <typename objT>
int getId(objT* obj) {
	return getId_X(std::type_index(typeid(obj)));
}

class CListener {
public:
	~CListener() {
		unsubscribe();
	}

	template <typename objT>
	void subscribe(objT* obj) {
		lis::subscribe(obj);
		objId = getId_X(std::type_index(typeid(obj)));
	}

	void unsubscribe() {
		unsubscribe_X(objId);
	}

	int objId;
};
