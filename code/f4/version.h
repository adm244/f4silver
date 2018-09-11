/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

#ifndef _F4_VERSION_H_
#define _F4_VERSION_H_

//#define MAKE_EXE_VERSION_FULL(major, minor, build, sub) (MAJOR(major) | MINOR(minor) | BUILD(build) | SUB(sub))
//#define MAKE_EXE_VERSION_FULL(major, minor, build, sub)	((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((build) & 0xFFF) << 4) | ((sub) & 0xF))
//#define MAKE_EXE_VERSION(major, minor, build) MAKE_EXE_VERSION_FULL(major, minor, build, 0)

//NOTE(adm244): fallout 4 runtime versions
//#define F4_VERSION_1_10_26 MAKE_EXE_VERSION(1, 10, 26)
//#define F4_VERSION_1_10_40 MAKE_EXE_VERSION(1, 10, 40)

//#define F4_VERSION F4_VERSION_1_10_40

#define F4_VERSION_1_10_40

#endif
