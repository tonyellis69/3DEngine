#pragma once

#include <vector>

#include "msg.h"
#include "..\3DTest\src\hexMsg.h"

class CEventObserver;
/** Maintains a list of observers and sends events to them. */
class CEventSubject {
public:
	void subscribe(CEventObserver* observer) {
		observers.push_back(observer);
	}

	template <typename T>
	void notify(T& msg) {
		for (auto observer : observers) {
			observer->onNotify(msg);
		}
	}

private:
	std::vector<CEventObserver*> observers;
};

/** Interface class for being notified about events. */
class CEventObserver {
public:
	virtual void onNotify(CMsg& msg) {};
	



};