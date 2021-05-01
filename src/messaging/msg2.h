#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <tuple>
#include <typeinfo>       // operator typeid
#include <typeindex>      // std::type_index


/** This libary enables objects to register member funcs as callbacks for a message of a
	given id, and other objects to send messages to registered callbacks for that message id.

	A message can have any arguments, but it's up to sender and receiver to ensure they agree.

	Limited-lifetime objects must ensure they unregister their callbacks on destruction.
	
	Usage:
	msg::attach(msgId, this, &thisObject::memberFunc);
	msg::emit(msgId, param1, param2... ); */


class CDispatcherBase {
public:
	int objId;
};

/** Instantiates a dispatcher specialised for callback callers of a given 
	argument signature, eg, <int,int,float>, as these will vary for different message types. */
template <typename callbackCallerT>
class CDispatcher : public CDispatcherBase {
public:
	CDispatcher(callbackCallerT fn) {
		callbackCaller = fn;
	}

	template <typename... Args>
	void callcallbackCaller(Args ...  args) {
		callbackCaller(args...);
	}

	callbackCallerT callbackCaller;
};


extern std::map<int, std::vector< std::shared_ptr<CDispatcherBase> > > slots; //stores a pointer to each callback dispatcher.

extern std::unordered_map < std::type_index, int> objIds; //Stores a unique id for each object that registers a callback.


namespace msg {

	/** Register the given member function as a callback for this message type. */
	template <typename objT, typename memberFuncT, typename... Args>
	void attach(int msgId, objT* obj, void (memberFuncT::* memberFn) (Args...)) {

		using specialisedDispatcher = CDispatcher<std::function<void(Args  ...)>>;

		//make a function object that will call the given function...
		std::function<void(Args  ...)> callbackCaller = [obj, memberFn](Args  ... args) {
				 (obj->*memberFn)(args ...); };

		//...store it in a dispatcher and store a pointer to that with others of the same message type.
		slots[msgId].push_back(std::make_shared<specialisedDispatcher>(callbackCaller));

		static int nextId = 0;
		slots[msgId].back()->objId = nextId;
		objIds[typeid(obj)] = nextId++;

	}

	/** If the given object has registered a callback for msgId, remove it from the register. */
	template <typename objT>
	void remove(int msgId, objT* obj )
	{
		int id = objIds[typeid(obj)];
		for (auto it = slots[msgId].begin(); it != slots[msgId].end(); it++) {
			if ((*it)->objId == id) {
				slots[msgId].erase(it);
				return;
			}
		}
	}


	/** Call all registered callbacks for this message type, passing the given arguments. */
	template <typename... Args>
	void emit(int msgId, Args ... args) {

		using specialisedDispatcher = CDispatcher<std::function<void(Args  ...)>>;

		for (auto& slot : slots[msgId]) {
			std::static_pointer_cast<specialisedDispatcher>(slot)->callbackCaller(args...);
		}
	}
}
	




