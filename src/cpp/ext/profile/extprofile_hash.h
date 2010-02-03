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

#ifndef __EXTPROFILE_HASH_H__
#define __EXTPROFILE_HASH_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <cpp/ext/ext_hash.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

#ifndef PROFILE_BUILTIN
#define x_hash f_hash
#else
inline Variant x_hash(CStrRef algo, CStrRef data, bool raw_output = false) {
  FUNCTION_INJECTION(hash);
  return f_hash(algo, data, raw_output);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_algos f_hash_algos
#else
inline Array x_hash_algos() {
  FUNCTION_INJECTION(hash_algos);
  return f_hash_algos();
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_init f_hash_init
#else
inline Variant x_hash_init(CStrRef algo, int options = 0, CStrRef key = null_string) {
  FUNCTION_INJECTION(hash_init);
  return f_hash_init(algo, options, key);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_file f_hash_file
#else
inline Variant x_hash_file(CStrRef algo, CStrRef filename, bool raw_output = false) {
  FUNCTION_INJECTION(hash_file);
  return f_hash_file(algo, filename, raw_output);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_final f_hash_final
#else
inline String x_hash_final(CObjRef context, bool raw_output = false) {
  FUNCTION_INJECTION(hash_final);
  return f_hash_final(context, raw_output);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_hmac_file f_hash_hmac_file
#else
inline Variant x_hash_hmac_file(CStrRef algo, CStrRef filename, CStrRef key, bool raw_output = false) {
  FUNCTION_INJECTION(hash_hmac_file);
  return f_hash_hmac_file(algo, filename, key, raw_output);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_hmac f_hash_hmac
#else
inline Variant x_hash_hmac(CStrRef algo, CStrRef data, CStrRef key, bool raw_output = false) {
  FUNCTION_INJECTION(hash_hmac);
  return f_hash_hmac(algo, data, key, raw_output);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_update_file f_hash_update_file
#else
inline bool x_hash_update_file(CObjRef init_context, CStrRef filename, CObjRef stream_context = null) {
  FUNCTION_INJECTION(hash_update_file);
  return f_hash_update_file(init_context, filename, stream_context);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_update_stream f_hash_update_stream
#else
inline int x_hash_update_stream(CObjRef context, CObjRef handle, int length = -1) {
  FUNCTION_INJECTION(hash_update_stream);
  return f_hash_update_stream(context, handle, length);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_hash_update f_hash_update
#else
inline bool x_hash_update(CObjRef context, CStrRef data) {
  FUNCTION_INJECTION(hash_update);
  return f_hash_update(context, data);
}
#endif


///////////////////////////////////////////////////////////////////////////////
}

#endif // __EXTPROFILE_HASH_H__