// License:
// 
// Syldeyn
// Compiler configuration based on platform
// 
// 
// 
// 	(c) Jeroen P. Broks, 2024
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 24.10.09
// End License


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
