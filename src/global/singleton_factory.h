//
// Summary: squirrel source code.
//
// Author: Tony.
// Email: tonyjobmails@gmail.com.
// Last modify: 2012-09-03 13:47:40.
// File name: singleton_factory.h
//
// Description: 
// Define class SingletonFactory.
//

#ifndef _squirrel_global__SINGLETON__FACTORY__H
#define _squirrel_global__SINGLETON__FACTORY__H

#include <stddef.h>

template <typename _T>
class SingletonFactory {
 public:
	SingletonFactory() {};
	~SingletonFactory() {};

	static _T* GetInstance() {
		if(!g_Instance) {
			g_Instance = new _T;
		}
		return g_Instance;
	}

	static void ReleaseInstance() {
		if(g_Instance) {
			delete g_Instance;
			g_Instance = NULL;
		}
	}

private:
	static _T* g_Instance;		/// Î¨Ò»ÊµÀý
};

template <typename _T>
_T* SingletonFactory<_T>::g_Instance;

#endif	// _squirrel_global__SINGLETON__FACTORY__H

