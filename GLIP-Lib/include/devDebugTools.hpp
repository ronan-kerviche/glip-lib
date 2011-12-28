#ifndef __DEV_DEBUG_TOOLS__
#define __DEV_DEBUG_TOOLS__

	// Development flags & Tools
		//#define __VERBOSE__
		//#define __DEVELOPMENT_VERBOSE__
		#define STR_EXPAND(tok) 	#tok
		#define STR(tok) 		STR_EXPAND(tok)
		//#define __HERE__		'[' << __FUNCTION__ << " at " << __FILE__ << ':' << __LINE__ << "] "
		#define __HERE__		"["STR(__FUNCTION__)" at "STR(__FILE__)" : "STR(__LINE__)"] "

#endif
