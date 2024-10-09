

#pragma once


/* 
* 
* This is just some specific configuration to several platforms, as I don't wish having to change the entire source code all the time.
* This configuration is compiler configuration! If can of course affect how Syldeyn operates runtime.
* 
*/


#include <Slyvina.hpp>

#ifdef SlyvWindows
#define SylJCRUse_zlib
#else
#undef SylJCRUse_zlib
#endif


#ifdef SylJCRUse_zlib
#include <JCR6_zlib.hpp>
#define SylInit_zlib() Slyvina::JCR6::init_zlib()
#define SylJCRStorage "zlib"
#else
#define SylJCRStorage "Store"
#define SylInit_zlib()
#endif