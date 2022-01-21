#pragma once

#include<functional>

#include "guiBase2.h"

namespace winVent {

	void setRoot(CguiBase* p);
	void subscribe(CguiBase* control);
	void subscribe(std::function<void(CWinEvent& event)>& listener);

	template <typename objT, typename memberFuncT>
	void subscribeT(objT* obj, void (memberFuncT::* handlerFn) (CWinEvent& )) {

	//std::function<void(CWinEvent&)> f = [=](CWinEvent& event) { (obj->*handlerFn) (event); };
	std::function<void(CWinEvent&)> f =  (ob      objT*::handlerFn;
		subscribe(f);
	}

	void unsubscribe(CguiBase* control);

	void sendWinEvent(CWinEvent& event);


}