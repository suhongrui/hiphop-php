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

#ifndef __GENERATED_cls_reflectionfunctionabstract_h__
#define __GENERATED_cls_reflectionfunctionabstract_h__


namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/* SRC: classes/reflection.php line 92 */
class c_reflectionfunctionabstract : virtual public ObjectData {
  BEGIN_CLASS_MAP(reflectionfunctionabstract)
  END_CLASS_MAP(reflectionfunctionabstract)
  DECLARE_CLASS(reflectionfunctionabstract, ReflectionFunctionAbstract, ObjectData)
  DECLARE_INVOKES_FROM_EVAL
  void init();
  public: Variant m_info;
  public: Variant t_getname();
  public: Variant t_isinternal();
  public: Variant t_getclosure();
  public: bool t_isuserdefined();
  public: Variant t_getfilename();
  public: Variant t_getstartline();
  public: Variant t_getendline();
  public: Variant t_getdoccomment();
  public: Variant t_getstaticvariables();
  public: Variant t_returnsreference();
  public: Array t_getparameters();
  public: int t_getnumberofparameters();
  public: int64 t_getnumberofrequiredparameters();
};

///////////////////////////////////////////////////////////////////////////////
}

#endif // __GENERATED_cls_reflectionfunctionabstract_h__
