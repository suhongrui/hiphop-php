/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef __EXTPROFILE_FUNCTION_H__
#define __EXTPROFILE_FUNCTION_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <cpp/ext/ext_function.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

#ifndef PROFILE_BUILTIN
#define x_get_defined_functions f_get_defined_functions
#else
inline Array x_get_defined_functions() {
  FUNCTION_INJECTION(get_defined_functions);
  return f_get_defined_functions();
}
#endif

#ifndef PROFILE_BUILTIN
#define x_function_exists f_function_exists
#else
inline bool x_function_exists(CStrRef function_name) {
  FUNCTION_INJECTION(function_exists);
  return f_function_exists(function_name);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_is_callable f_is_callable
#else
inline bool x_is_callable(CVarRef v, bool syntax = false, Variant name = null) {
  FUNCTION_INJECTION(is_callable);
  return f_is_callable(v, syntax, ref(name));
}
#endif

#ifndef PROFILE_BUILTIN
#define x_call_user_func_array f_call_user_func_array
#else
inline Variant x_call_user_func_array(CVarRef function, CArrRef params) {
  FUNCTION_INJECTION(call_user_func_array);
  return f_call_user_func_array(function, params);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_call_user_func f_call_user_func
#else
inline Variant x_call_user_func(int _argc, CVarRef function, CArrRef _argv = null_array) {
  FUNCTION_INJECTION(call_user_func);
  return f_call_user_func(_argc, function, _argv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_create_function f_create_function
#else
inline String x_create_function(CStrRef args, CStrRef code) {
  FUNCTION_INJECTION(create_function);
  return f_create_function(args, code);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_func_get_arg f_func_get_arg
#else
inline Variant x_func_get_arg(int arg_num) {
  FUNCTION_INJECTION(func_get_arg);
  return f_func_get_arg(arg_num);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_func_get_args f_func_get_args
#else
inline Array x_func_get_args() {
  FUNCTION_INJECTION(func_get_args);
  return f_func_get_args();
}
#endif

#ifndef PROFILE_BUILTIN
#define x_func_num_args f_func_num_args
#else
inline int x_func_num_args() {
  FUNCTION_INJECTION(func_num_args);
  return f_func_num_args();
}
#endif

#ifndef PROFILE_BUILTIN
#define x_register_postsend_function f_register_postsend_function
#else
inline void x_register_postsend_function(int _argc, CVarRef function, CArrRef _argv = null_array) {
  FUNCTION_INJECTION(register_postsend_function);
  f_register_postsend_function(_argc, function, _argv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_register_shutdown_function f_register_shutdown_function
#else
inline void x_register_shutdown_function(int _argc, CVarRef function, CArrRef _argv = null_array) {
  FUNCTION_INJECTION(register_shutdown_function);
  f_register_shutdown_function(_argc, function, _argv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_register_cleanup_function f_register_cleanup_function
#else
inline void x_register_cleanup_function(int _argc, CVarRef function, CArrRef _argv = null_array) {
  FUNCTION_INJECTION(register_cleanup_function);
  f_register_cleanup_function(_argc, function, _argv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_register_tick_function f_register_tick_function
#else
inline bool x_register_tick_function(int _argc, CVarRef function, CArrRef _argv = null_array) {
  FUNCTION_INJECTION(register_tick_function);
  return f_register_tick_function(_argc, function, _argv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_unregister_tick_function f_unregister_tick_function
#else
inline void x_unregister_tick_function(CVarRef function_name) {
  FUNCTION_INJECTION(unregister_tick_function);
  f_unregister_tick_function(function_name);
}
#endif


///////////////////////////////////////////////////////////////////////////////
}

#endif // __EXTPROFILE_FUNCTION_H__