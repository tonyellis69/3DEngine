#pragma once

#include <functional>
#include <typeinfo>       // operator typeid
#include <typeindex>      // std::type_index
#include <map>


/** This library enables objects to subscribe to notifications of a given
	event type, and for other code to make notifications of that event type.

	An event can be of any type, but the subscriber must provide a handler function 
	of the format 'onEvent(event-type& e)'
	.
	Limited-lifetime objects must ensure they unsubscribe on destruction.

	Usage:
	lis::subscribe<event-type>(this);
	lis::notify<event-type>(event);
	lis::unsubscribe<event-type>(this);
*/


template <typename eventT>
struct TEventHandler {
	std::function<void(eventT&)> func;
	std::type_index idx;
};

template <typename eventT>
std::unordered_map<std::type_index, std::vector< TEventHandler<eventT> > > eventHandlers;


namespace lis {

	template <typename eventT, typename objT>
	void subscribe(objT* obj) {
		std::function<void(eventT&)> f = [obj](eventT& e) {
			obj->onEvent(e);
		};

		TEventHandler<eventT> eventHandler = {f, std::type_index(typeid(obj))};
		eventHandlers<eventT>[std::type_index(typeid(eventT))].push_back(eventHandler);
		//creates a new list of event subscribers for each new event type
	}


	template <typename eventT, typename objT>
	void unsubscribe(objT* obj) {
		auto& handlers = eventHandlers<eventT>[std::type_index(typeid(eventT))];
		for (auto& it = handlers.begin(); it < handlers.end(); it++) {
			if (it->idx == std::type_index(typeid(obj))) {
				handlers.erase(it);
				return;
			}
		}
	}


	template <typename eventT>
	void event(eventT& e) {
		for (auto& listener : eventHandlers<eventT>[std::type_index(typeid(e))]) {
				listener.func(e);
		}
	}

}



