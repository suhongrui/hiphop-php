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

#ifndef __EXTPROFILE_ZLIB_H__
#define __EXTPROFILE_ZLIB_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <cpp/ext/ext_zlib.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

#ifndef PROFILE_BUILTIN
#define x_readgzfile f_readgzfile
#else
inline Variant x_readgzfile(CStrRef filename, bool use_include_path = false) {
  FUNCTION_INJECTION(readgzfile);
  return f_readgzfile(filename, use_include_path);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzfile f_gzfile
#else
inline Variant x_gzfile(CStrRef filename, bool use_include_path = false) {
  FUNCTION_INJECTION(gzfile);
  return f_gzfile(filename, use_include_path);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzcompress f_gzcompress
#else
inline Variant x_gzcompress(CStrRef data, int level = -1) {
  FUNCTION_INJECTION(gzcompress);
  return f_gzcompress(data, level);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzuncompress f_gzuncompress
#else
inline Variant x_gzuncompress(CStrRef data, int limit = 0) {
  FUNCTION_INJECTION(gzuncompress);
  return f_gzuncompress(data, limit);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzdeflate f_gzdeflate
#else
inline Variant x_gzdeflate(CStrRef data, int level = -1) {
  FUNCTION_INJECTION(gzdeflate);
  return f_gzdeflate(data, level);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzinflate f_gzinflate
#else
inline Variant x_gzinflate(CStrRef data, int limit = 0) {
  FUNCTION_INJECTION(gzinflate);
  return f_gzinflate(data, limit);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzencode f_gzencode
#else
inline Variant x_gzencode(CStrRef data, int level = -1, int encoding_mode = k_FORCE_GZIP) {
  FUNCTION_INJECTION(gzencode);
  return f_gzencode(data, level, encoding_mode);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzdecode f_gzdecode
#else
inline Variant x_gzdecode(CStrRef data) {
  FUNCTION_INJECTION(gzdecode);
  return f_gzdecode(data);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_zlib_get_coding_type f_zlib_get_coding_type
#else
inline String x_zlib_get_coding_type() {
  FUNCTION_INJECTION(zlib_get_coding_type);
  return f_zlib_get_coding_type();
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzopen f_gzopen
#else
inline Object x_gzopen(CStrRef filename, CStrRef mode, bool use_include_path = false) {
  FUNCTION_INJECTION(gzopen);
  return f_gzopen(filename, mode, use_include_path);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzclose f_gzclose
#else
inline bool x_gzclose(CObjRef zp) {
  FUNCTION_INJECTION(gzclose);
  return f_gzclose(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzrewind f_gzrewind
#else
inline bool x_gzrewind(CObjRef zp) {
  FUNCTION_INJECTION(gzrewind);
  return f_gzrewind(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzeof f_gzeof
#else
inline bool x_gzeof(CObjRef zp) {
  FUNCTION_INJECTION(gzeof);
  return f_gzeof(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzgetc f_gzgetc
#else
inline Variant x_gzgetc(CObjRef zp) {
  FUNCTION_INJECTION(gzgetc);
  return f_gzgetc(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzgets f_gzgets
#else
inline String x_gzgets(CObjRef zp, int64 length = 1024) {
  FUNCTION_INJECTION(gzgets);
  return f_gzgets(zp, length);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzgetss f_gzgetss
#else
inline String x_gzgetss(CObjRef zp, int64 length = 0, CStrRef allowable_tags = null_string) {
  FUNCTION_INJECTION(gzgetss);
  return f_gzgetss(zp, length, allowable_tags);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzread f_gzread
#else
inline Variant x_gzread(CObjRef zp, int64 length = 0) {
  FUNCTION_INJECTION(gzread);
  return f_gzread(zp, length);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzpassthru f_gzpassthru
#else
inline Variant x_gzpassthru(CObjRef zp) {
  FUNCTION_INJECTION(gzpassthru);
  return f_gzpassthru(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzseek f_gzseek
#else
inline Variant x_gzseek(CObjRef zp, int64 offset, int64 whence = SEEK_SET) {
  FUNCTION_INJECTION(gzseek);
  return f_gzseek(zp, offset, whence);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gztell f_gztell
#else
inline Variant x_gztell(CObjRef zp) {
  FUNCTION_INJECTION(gztell);
  return f_gztell(zp);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzwrite f_gzwrite
#else
inline Variant x_gzwrite(CObjRef zp, CStrRef str, int64 length = 0) {
  FUNCTION_INJECTION(gzwrite);
  return f_gzwrite(zp, str, length);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_gzputs f_gzputs
#else
inline Variant x_gzputs(CObjRef zp, CStrRef str, int64 length = 0) {
  FUNCTION_INJECTION(gzputs);
  return f_gzputs(zp, str, length);
}
#endif


///////////////////////////////////////////////////////////////////////////////
}

#endif // __EXTPROFILE_ZLIB_H__