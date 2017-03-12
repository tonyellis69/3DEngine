#pragma once


class CTimedEvent {
public:
	CTimedEvent() {};
	virtual void Action(const Double& dT); ///<Action to perform when this event is triggered.


	double StartTime; ///<Game time in milliseconds at which to activate this event.
	double Duration; ///Time period after activation before this event is deleted.
};