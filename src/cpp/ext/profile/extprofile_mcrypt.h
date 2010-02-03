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

#ifndef __EXTPROFILE_MCRYPT_H__
#define __EXTPROFILE_MCRYPT_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <cpp/ext/ext_mcrypt.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_open f_mcrypt_module_open
#else
inline Variant x_mcrypt_module_open(CStrRef algorithm, CStrRef algorithm_directory, CStrRef mode, CStrRef mode_directory) {
  FUNCTION_INJECTION(mcrypt_module_open);
  return f_mcrypt_module_open(algorithm, algorithm_directory, mode, mode_directory);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_close f_mcrypt_module_close
#else
inline bool x_mcrypt_module_close(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_module_close);
  return f_mcrypt_module_close(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_list_algorithms f_mcrypt_list_algorithms
#else
inline Array x_mcrypt_list_algorithms(CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_list_algorithms);
  return f_mcrypt_list_algorithms(lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_list_modes f_mcrypt_list_modes
#else
inline Array x_mcrypt_list_modes(CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_list_modes);
  return f_mcrypt_list_modes(lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_get_algo_block_size f_mcrypt_module_get_algo_block_size
#else
inline int x_mcrypt_module_get_algo_block_size(CStrRef algorithm, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_get_algo_block_size);
  return f_mcrypt_module_get_algo_block_size(algorithm, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_get_algo_key_size f_mcrypt_module_get_algo_key_size
#else
inline int x_mcrypt_module_get_algo_key_size(CStrRef algorithm, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_get_algo_key_size);
  return f_mcrypt_module_get_algo_key_size(algorithm, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_get_supported_key_sizes f_mcrypt_module_get_supported_key_sizes
#else
inline Array x_mcrypt_module_get_supported_key_sizes(CStrRef algorithm, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_get_supported_key_sizes);
  return f_mcrypt_module_get_supported_key_sizes(algorithm, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_is_block_algorithm_mode f_mcrypt_module_is_block_algorithm_mode
#else
inline bool x_mcrypt_module_is_block_algorithm_mode(CStrRef mode, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_is_block_algorithm_mode);
  return f_mcrypt_module_is_block_algorithm_mode(mode, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_is_block_algorithm f_mcrypt_module_is_block_algorithm
#else
inline bool x_mcrypt_module_is_block_algorithm(CStrRef algorithm, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_is_block_algorithm);
  return f_mcrypt_module_is_block_algorithm(algorithm, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_is_block_mode f_mcrypt_module_is_block_mode
#else
inline bool x_mcrypt_module_is_block_mode(CStrRef mode, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_is_block_mode);
  return f_mcrypt_module_is_block_mode(mode, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_module_self_test f_mcrypt_module_self_test
#else
inline bool x_mcrypt_module_self_test(CStrRef algorithm, CStrRef lib_dir = null_string) {
  FUNCTION_INJECTION(mcrypt_module_self_test);
  return f_mcrypt_module_self_test(algorithm, lib_dir);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_create_iv f_mcrypt_create_iv
#else
inline Variant x_mcrypt_create_iv(int size, int source = 0) {
  FUNCTION_INJECTION(mcrypt_create_iv);
  return f_mcrypt_create_iv(size, source);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_encrypt f_mcrypt_encrypt
#else
inline Variant x_mcrypt_encrypt(CStrRef cipher, CStrRef key, CStrRef data, CStrRef mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_encrypt);
  return f_mcrypt_encrypt(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_decrypt f_mcrypt_decrypt
#else
inline Variant x_mcrypt_decrypt(CStrRef cipher, CStrRef key, CStrRef data, CStrRef mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_decrypt);
  return f_mcrypt_decrypt(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_cbc f_mcrypt_cbc
#else
inline Variant x_mcrypt_cbc(CStrRef cipher, CStrRef key, CStrRef data, int mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_cbc);
  return f_mcrypt_cbc(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_cfb f_mcrypt_cfb
#else
inline Variant x_mcrypt_cfb(CStrRef cipher, CStrRef key, CStrRef data, int mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_cfb);
  return f_mcrypt_cfb(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_ecb f_mcrypt_ecb
#else
inline Variant x_mcrypt_ecb(CStrRef cipher, CStrRef key, CStrRef data, int mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_ecb);
  return f_mcrypt_ecb(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_ofb f_mcrypt_ofb
#else
inline Variant x_mcrypt_ofb(CStrRef cipher, CStrRef key, CStrRef data, int mode, CStrRef iv = null_string) {
  FUNCTION_INJECTION(mcrypt_ofb);
  return f_mcrypt_ofb(cipher, key, data, mode, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_get_block_size f_mcrypt_get_block_size
#else
inline Variant x_mcrypt_get_block_size(CStrRef cipher, CStrRef module = null_string) {
  FUNCTION_INJECTION(mcrypt_get_block_size);
  return f_mcrypt_get_block_size(cipher, module);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_get_cipher_name f_mcrypt_get_cipher_name
#else
inline Variant x_mcrypt_get_cipher_name(CStrRef cipher) {
  FUNCTION_INJECTION(mcrypt_get_cipher_name);
  return f_mcrypt_get_cipher_name(cipher);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_get_iv_size f_mcrypt_get_iv_size
#else
inline Variant x_mcrypt_get_iv_size(CStrRef cipher, CStrRef mode) {
  FUNCTION_INJECTION(mcrypt_get_iv_size);
  return f_mcrypt_get_iv_size(cipher, mode);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_get_key_size f_mcrypt_get_key_size
#else
inline int x_mcrypt_get_key_size(CStrRef cipher, CStrRef module) {
  FUNCTION_INJECTION(mcrypt_get_key_size);
  return f_mcrypt_get_key_size(cipher, module);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_algorithms_name f_mcrypt_enc_get_algorithms_name
#else
inline String x_mcrypt_enc_get_algorithms_name(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_algorithms_name);
  return f_mcrypt_enc_get_algorithms_name(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_block_size f_mcrypt_enc_get_block_size
#else
inline int x_mcrypt_enc_get_block_size(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_block_size);
  return f_mcrypt_enc_get_block_size(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_iv_size f_mcrypt_enc_get_iv_size
#else
inline int x_mcrypt_enc_get_iv_size(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_iv_size);
  return f_mcrypt_enc_get_iv_size(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_key_size f_mcrypt_enc_get_key_size
#else
inline int x_mcrypt_enc_get_key_size(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_key_size);
  return f_mcrypt_enc_get_key_size(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_modes_name f_mcrypt_enc_get_modes_name
#else
inline String x_mcrypt_enc_get_modes_name(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_modes_name);
  return f_mcrypt_enc_get_modes_name(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_get_supported_key_sizes f_mcrypt_enc_get_supported_key_sizes
#else
inline Array x_mcrypt_enc_get_supported_key_sizes(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_get_supported_key_sizes);
  return f_mcrypt_enc_get_supported_key_sizes(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_is_block_algorithm_mode f_mcrypt_enc_is_block_algorithm_mode
#else
inline bool x_mcrypt_enc_is_block_algorithm_mode(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_is_block_algorithm_mode);
  return f_mcrypt_enc_is_block_algorithm_mode(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_is_block_algorithm f_mcrypt_enc_is_block_algorithm
#else
inline bool x_mcrypt_enc_is_block_algorithm(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_is_block_algorithm);
  return f_mcrypt_enc_is_block_algorithm(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_is_block_mode f_mcrypt_enc_is_block_mode
#else
inline bool x_mcrypt_enc_is_block_mode(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_is_block_mode);
  return f_mcrypt_enc_is_block_mode(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_enc_self_test f_mcrypt_enc_self_test
#else
inline int x_mcrypt_enc_self_test(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_enc_self_test);
  return f_mcrypt_enc_self_test(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_generic f_mcrypt_generic
#else
inline Variant x_mcrypt_generic(CObjRef td, CStrRef data) {
  FUNCTION_INJECTION(mcrypt_generic);
  return f_mcrypt_generic(td, data);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_generic_init f_mcrypt_generic_init
#else
inline int x_mcrypt_generic_init(CObjRef td, CStrRef key, CStrRef iv) {
  FUNCTION_INJECTION(mcrypt_generic_init);
  return f_mcrypt_generic_init(td, key, iv);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mdecrypt_generic f_mdecrypt_generic
#else
inline Variant x_mdecrypt_generic(CObjRef td, CStrRef data) {
  FUNCTION_INJECTION(mdecrypt_generic);
  return f_mdecrypt_generic(td, data);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_generic_deinit f_mcrypt_generic_deinit
#else
inline bool x_mcrypt_generic_deinit(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_generic_deinit);
  return f_mcrypt_generic_deinit(td);
}
#endif

#ifndef PROFILE_BUILTIN
#define x_mcrypt_generic_end f_mcrypt_generic_end
#else
inline bool x_mcrypt_generic_end(CObjRef td) {
  FUNCTION_INJECTION(mcrypt_generic_end);
  return f_mcrypt_generic_end(td);
}
#endif


///////////////////////////////////////////////////////////////////////////////
}

#endif // __EXTPROFILE_MCRYPT_H__