#if !defined(MACROS_H)
#define MACROS_H

#include <iostream>
using namespace std;

#define ASSERT_ALWAYS(exp, str) 	if (!(exp))	{ cerr << "\nAssertion failed: " << #exp << ", " << #str << "\n";	exit(0); }


#if defined(_DEBUG)
	#define VM_ERROR(exp, str) if (exp)	{ cerr << "\n" << #str << "\n" ;	exit(0); }
#else
	#define VM_ERROR(exp, str)
#endif



#endif
