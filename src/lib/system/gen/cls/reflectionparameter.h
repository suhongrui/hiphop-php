/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
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

#ifndef __GENERATED_CLS_REFLECTIONPARAMETER_H__
#define __GENERATED_CLS_REFLECTIONPARAMETER_H__

#include <cls/reflector.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/* SRC: classes/reflection.php line 17 */
class c_reflectionparameter : virtual public c_reflector {
  BEGIN_CLASS_MAP(reflectionparameter)
    PARENT_CLASS(reflector)
  END_CLASS_MAP(reflectionparameter)
  DECLARE_CLASS(reflectionparameter, ReflectionParameter, ObjectData)
  DECLARE_INVOKES_FROM_EVAL
  void init();
  public: Variant m_info;
  public: void t___construct(Variant v_func, Variant v_param);
  public: ObjectData *create(Variant v_func, Variant v_param);
  public: ObjectData *dynCreate(CArrRef params, bool init = true);
  public: void dynConstruct(CArrRef params);
  public: String t___tostring();
  public: static Variant ti_export(const char* cls, CVarRef v_func, CVarRef v_param, CVarRef v_ret);
  public: Variant t_getname();
  public: Variant t_ispassedbyreference();
  public: Variant t_getdeclaringclass();
  public: Variant t_getclass();
  public: bool t_isarray();
  public: Variant t_allowsnull();
  public: bool t_isoptional();
  public: bool t_isdefaultvalueavailable();
  public: Variant t_getdefaultvalue();
  public: Variant t_getposition();
  public: static Variant t_export(CVarRef v_func, CVarRef v_param, CVarRef v_ret) { return ti_export("reflectionparameter", v_func, v_param, v_ret); }
};

///////////////////////////////////////////////////////////////////////////////
}

#endif // __GENERATED_CLS_REFLECTIONPARAMETER_H__