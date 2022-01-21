#include "listen.h"

#include <vector>
#include <functional>


struct TEventListener {
	std::function<void(CEvent&)> func;
	std::type_index idx;
	int idNo;
};

std::vector<TEventListener> listeners;

int eventListenerId = 0;

void lis::event(CEvent& e) {
	for (auto& listener : listeners) {
		listener.func(e);
	}
}


void subscribe_X(std::function<void(CEvent&)>& callback, std::type_index idx) {
	listeners.push_back({ callback, idx, eventListenerId++ });

}

void unsubscribe_X(std::type_index idx) {
	for (auto& it = listeners.begin(); it < listeners.end(); it++) {
		if (it->idx == idx) {
			listeners.erase(it);
			return;
		}
	}
}

void unsubscribe_X(int id) {
	for (auto& it = listeners.begin(); it < listeners.end(); it++) {
		if (it->idNo == id) {
			listeners.erase(it);
			return;
		}
	}
}

int getId_X(std::type_index idx) {
	for (auto& it = listeners.begin(); it < listeners.end(); it++) {
		if (it->idx == idx) {
			return it->idNo;
		}
	}
	return -1;
}

