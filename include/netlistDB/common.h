#pragma once

#ifdef _WIN32
#ifdef NETLISTDB_DLL
	#ifdef NETLISTDB_EXPORTS
		#define NETLISTDB_PUBLIC __declspec(dllexport)
	#else
		#define NETLISTDB_PUBLIC __declspec(dllimport)
	#endif
#else
	#define NETLISTDB_PUBLIC
#endif


#else
	#define NETLISTDB_PUBLIC
#endif
