#include "events.h"

void CEventSubject::subscribe(CEventObserver* observer) {
	observers.push_back(observer);
}


void CEventSubject::notify(CMsg* msg) {
	for (auto observer : observers) {
		observer->onNotify(msg);
	}
}
