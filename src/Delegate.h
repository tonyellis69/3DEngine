#pragma once


/** A universal delegates class, for communication between de-coupled classes */

template <typename returnType, typename paramType>
class Callback1p {
public:
	virtual returnType invoke(paramType param1) = 0;
};


template <typename returnType, typename paramType, typename T, typename methodType>
class MethodCallback1p: public Callback1p<returnType,paramType> {
private: 
	void* mObject;
	methodType mMethod;

public:
	MethodCallback1p(void* object, methodType method) 
		: mObject(object), mMethod(method) {}

	virtual returnType invoke(paramType param1) {
		T* obj = static_cast<T*>(mObject);
		return (obj->*mMethod)(param1);
	}
};



template <typename returnType, typename paramType>
class DelegateP {
private:
	Callback1p <returnType, paramType>* callback;

public:
	DelegateP() { callback = NULL;};
	template <typename T, typename methodType>
	void Set(T *object, methodType method) {
		 callback = new MethodCallback1p <returnType,paramType,T,methodType> (object, method);
	}

	~DelegateP(void) {
		if (callback != NULL)
			delete callback;
	}

	returnType operator()(paramType param1) {
		return callback->invoke(param1);
	}
};



template <typename returnType, typename paramType, typename paramType2>
class Callback2p {
public:
	virtual returnType invoke(paramType param1,paramType2 param2) = 0;
};

template <typename returnType, typename paramType,typename paramType2, typename T, typename methodType>
class MethodCallback2p: public Callback2p<returnType,paramType,paramType2> {
private: 
	void* mObject;
	methodType mMethod;

public:
	MethodCallback2p(void* object, methodType method) 
		: mObject(object), mMethod(method) {}

	virtual returnType invoke(paramType param1,paramType2 param2 ) {
		T* obj = static_cast<T*>(mObject);
		return (obj->*mMethod)(param1,param2);
	}
};

template <typename returnType, typename paramType, typename paramType2>
class DelegatePP {
private:
	Callback2p <returnType, paramType, paramType2>* callback;

public:
	DelegatePP() { callback = NULL;};
	template <typename T, typename methodType>
	void Set(T *object, methodType method) {
		 callback = new MethodCallback2p <returnType,paramType,paramType2,T,methodType> (object, method);
	}

	~DelegatePP(void) {
		if (callback != NULL)
			delete callback;
	}

	returnType operator()(paramType param1, paramType2 param2) {
		return callback->invoke(param1, param2);
	}
};



template <typename returnType, typename paramType, typename paramType2,typename paramType3>
class Callback3p {
public:
	virtual returnType invoke(paramType param1,paramType2 param2,paramType3 param3) = 0;
};

template <typename returnType, typename paramType,typename paramType2,typename paramType3, typename T, typename methodType>
class MethodCallback3p: public Callback3p<returnType,paramType,paramType2,paramType3> {
private: 
	void* mObject;
	methodType mMethod;

public:
	MethodCallback3p(void* object, methodType method) 
		: mObject(object), mMethod(method) {}

	virtual returnType invoke(paramType param1,paramType2 param2, paramType3 param3 ) {
		T* obj = static_cast<T*>(mObject);
		return (obj->*mMethod)(param1,param2,param3);
	}
};

template <typename returnType, typename paramType, typename paramType2, typename paramType3>
class DelegatePPP {
private:
	Callback3p <returnType, paramType, paramType2, paramType3>* callback;

public:
	DelegatePPP() { callback = NULL;};
	template <typename T, typename methodType>
	void Set(T *object, methodType method) {
		 callback = new MethodCallback3p <returnType,paramType,paramType2,paramType3,T,methodType> (object, method);
	}

	~DelegatePPP(void) {
		if (callback != NULL)
			delete callback;
	}

	returnType operator()(paramType param1, paramType2 param2, paramType3 param3) {
		return callback->invoke(param1, param2, param3);
	}
};


template <typename returnType, typename paramType, typename paramType2,typename paramType3, typename paramType4>
class Callback4p {
public:
	virtual returnType invoke(paramType param1,paramType2 param2,paramType3 param3, paramType4 param4) = 0;
};

template <typename returnType, typename paramType,typename paramType2,typename paramType3, typename paramType4, typename T, typename methodType>
class MethodCallback4p: public Callback4p<returnType,paramType,paramType2,paramType3,paramType4> {
private: 
	void* mObject;
	methodType mMethod;

public:
	MethodCallback4p(void* object, methodType method) 
		: mObject(object), mMethod(method) {}

	virtual returnType invoke(paramType param1,paramType2 param2, paramType3 param3, paramType4 param4 ) {
		T* obj = static_cast<T*>(mObject);
		return (obj->*mMethod)(param1,param2,param3,param4);
	}
};

template <typename returnType, typename paramType, typename paramType2, typename paramType3, typename paramType4>
class DelegatePPPP {
private:
	Callback4p <returnType, paramType, paramType2, paramType3, paramType4>* callback;

public:
	template <typename T, typename methodType>
	void Set(T *object, methodType method) {
		 callback = new MethodCallback4p <returnType,paramType,paramType2,paramType3,paramType4,T,methodType> (object, method);
	}

	~DelegatePPPP(void) {
		delete callback;
	}

	returnType operator()(paramType param1, paramType2 param2, paramType3 param3, paramType4 param4) {
		return callback->invoke(param1, param2, param3, param4);
	}
};