#pragma once

#include <unordered_map>
#include <typeindex>

#include <functional>
#include <memory>
#include <vector>

#include "msg.h"

/** Wrapper for message handler member function calls. Abstract base enables easy storage in a container. */
class CMessenger;;
struct CFunctorBase {
public:
	virtual void call(CMsg& msg) = 0;
	virtual class CMessenger* getHandlerObj() = 0;
};

template <class handlerT, typename messageT>
struct CFunctor :  public CFunctorBase {
public:
	CFunctor(handlerT* obj, void (handlerT::* m) (messageT&) ) : objPointer(obj), fnPointer(m) {}

	void call(CMsg& param)	{
		 (objPointer->*fnPointer)((messageT&)param);
	}
	handlerT* getHandlerObj() {
		return objPointer;
	}
protected:
	handlerT* objPointer;
	void (handlerT::* fnPointer) (messageT&);
};


/** Class to enable message sending/receiving. */







class CMessenger;
class CMessageBus {
public:

	template <typename messageT, typename handlerObjT, typename memberFuncT >
	void setHandler(handlerObjT* handlerObj, void (memberFuncT::* handlerFn) (messageT&) ) {

		 auto sp = std::make_shared< CFunctor<memberFuncT, messageT> >(handlerObj, handlerFn);	
		 handlers[std::type_index(typeid(messageT))] = sp;
	}


	template <typename messageT>
	void dispatch(messageT& msg) {
		auto msgType = std::type_index(typeid(msg));
		if (handlers.find(msgType) != handlers.end())
			handlers[typeid(msg)]->call(msg);	
	}

	void clearHandlers(CMessenger* handlerObj) {
		if (handlers.empty()) //in case we come here after this global object has been destroyed
			return;

		for (auto& handler = handlers.begin(); handler != handlers.end();) {
			CMessenger* regHandlerObj = handler->second->getHandlerObj();
			if (regHandlerObj == handlerObj) {
				handler = handlers.erase(handler);
			}
			else
				handler++;
		}
	}

	~CMessageBus() {
		handlers.clear();
	}

private:
	std::unordered_map <std::type_index, std::shared_ptr<CFunctorBase> > handlers;

};

extern CMessageBus messageBus;

class CMessenger {
public:

	template <typename messageT>
	void send(messageT& msg) {
		messageBus.dispatch(msg);
	}

	~CMessenger() {
		messageBus.clearHandlers(this);
	};



};