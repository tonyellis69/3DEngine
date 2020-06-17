#pragma once

#include <vector>

#include "msg.h"

class CEventObserver;
/** Maintains a list of observers and sends events to them. */
class CEventSubject {
public:
	void subscribe(CEventObserver* observer);
	void notify(CMsg* msg);

private:
	std::vector<CEventObserver*> observers;
};

/** Interface class for being notified about events. */
class CEventObserver {
public:
	virtual void onNotify(CMsg* msg) {};




};