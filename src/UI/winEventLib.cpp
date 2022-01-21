#include "winEventLib.h"

#include <vector>
#include <functional>
#include <typeindex>
#include <typeinfo>


using fnType = std::function<void(CWinEvent& )>;
std::vector <std::function<void(CWinEvent& )>> listeners2;

CguiBase* pRoot;
std::vector<CguiBase*> listeners;

CguiBase* mousedControl; ///<Control the mousepointer is over.

CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos);

namespace winVent {


	void setRoot(CguiBase* p) {
		pRoot = p;
	}

	void subscribe(CguiBase* control) {
		auto result = std::find(listeners.begin(), listeners.end(), control);
		if (result == listeners.end()) {
			listeners.push_back(control);
		}
	}

	void subscribe(std::function<void(CWinEvent& )>& listener) {
		//auto result = std::find(listeners2.begin(), listeners2.end(), listener);
		//if (result == listeners2.end()) {
		//	listeners2.push_back(listener);
		//}


		for (auto& existingListener : listeners2) {
			//auto t = typeid(fnType);
			auto x = listener.target<fnType>();
			//auto x = std::type_index(typeid(listener));
			if ( existingListener.target<void(*)(CWinEvent& )>() == listener.target<void(*)(CWinEvent& )>() ) {


				return;
			}
		}
		listeners2.push_back(listener);
	}

	void unsubscribe(CguiBase* control) {
		auto result = std::find(listeners.begin(), listeners.end(), control);
		if (result != listeners.end()) {
			listeners.erase(result);
		}
	}


	void sendWinEvent(CWinEvent& event) {
		if (event.type == eMouseMove) {
			mousedControl = findControlUnderMouse(pRoot, event.screenPos);
			if (mousedControl) {
				//subscribe(mousedControl);

				std::function<void(CWinEvent&)> f = [](CWinEvent& event) { mousedControl->onWinEvent(event); };
				//subscribe( f); 
				//auto x = CguiBase::onWinEvent;
				subscribeT(mousedControl, &CguiBase::onWinEvent);
			}
		}

		//for (auto& listener : listeners) {
		//	listener->onWinEvent(event);
		//}
		for (auto& listener : listeners2) {
			listener(event);
		}
	}

}


CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos) {
	if (ctrl->posInside(mousePos) == false)
		return nullptr;

	CguiBase* result;
	for (auto* child : ctrl->controls) {
		result = findControlUnderMouse((CguiBase*)child, mousePos);
		if (result)
			return result;
	}

	return ctrl;
}