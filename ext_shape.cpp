 /*
   ESRI Shapefile extension for HHVM.
   Copyright (c) 2013 SIA Intelligent Systems.
   
   Based on PECL shape extension with the following copyright:

   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrey Hristov                                               |
   +----------------------------------------------------------------------+

   Extended and ported to HHVM by Kristaps Kaupe.
*/

#include "hphp/runtime/base/base-includes.h"
#include <cinttypes>
#include <libshp/shapefil.h>

namespace HPHP {

const StaticString s_SHPT_NULL("SHPT_NULL");
const int64_t k_SHPT_NULL = SHPT_NULL;
const StaticString s_SHPT_POINT("SHPT_POINT");
const int64_t k_SHPT_POINT = SHPT_POINT;
const StaticString s_SHPT_ARC("SHPT_ARC");
const int64_t k_SHPT_ARC = SHPT_ARC;
const StaticString s_SHPT_POLYGON("SHPT_POLYGON");
const int64_t k_SHPT_POLYGON = SHPT_POLYGON;
const StaticString s_SHPT_MULTIPOINT("SHPT_MULTIPOINT");
const int64_t k_SHPT_MULTIPOINT = SHPT_MULTIPOINT;
const StaticString s_SHPT_POINTZ("SHPT_POINTZ");
const int64_t k_SHPT_POINTZ = SHPT_POINTZ;
const StaticString s_SHPT_ARCZ("SHPT_ARCZ");
const int64_t k_SHPT_ARCZ = SHPT_ARCZ;
const StaticString s_SHPT_POLYGONZ("SHPT_POLYGONZ");
const int64_t k_SHPT_POLYGONZ = SHPT_POLYGONZ;
const StaticString s_SHPT_MULTIPOINTZ("SHPT_MULTIPOINTZ");
const int64_t k_SHPT_MULTIPOINTZ = SHPT_MULTIPOINTZ;
const StaticString s_SHPT_POINTM("SHPT_POINTM");
const int64_t k_SHPT_POINTM = SHPT_POINTM;
const StaticString s_SHPT_ARCM("SHPT_ARCM");
const int64_t k_SHPT_ARCM = SHPT_ARCM;
const StaticString s_SHPT_POLYGONM("SHPT_POLYGONM");
const int64_t k_SHPT_POLYGONM = SHPT_POLYGONM;
const StaticString s_SHPT_MULTIPOINTM("SHPT_MULTIPOINTM");
const int64_t k_SHPT_MULTIPOINTM = SHPT_MULTIPOINTM;
const StaticString s_SHPT_MULTIPATCH("SHPT_MULTIPATCH");
const int64_t k_SHPT_MULTIPATCH = SHPT_MULTIPATCH;
const StaticString s_SHPP_TRISTRIP("SHPP_TRISTRIP");
const int64_t k_SHPP_TRISTRIP = SHPP_TRISTRIP;
const StaticString s_SHPP_TRIFAN("SHPP_TRIFAN");
const int64_t k_SHPP_TRIFAN = SHPP_TRIFAN;
const StaticString s_SHPP_OUTERRING("SHPP_OUTERRING");
const int64_t k_SHPP_OUTERRING = SHPP_OUTERRING;
const StaticString s_SHPP_INNERRING("SHPP_INNERRING");
const int64_t k_SHPP_INNERRING = SHPP_INNERRING;
const StaticString s_SHPP_FIRSTRING("SHPP_FIRSTRING");
const int64_t k_SHPP_FIRSTRING = SHPP_FIRSTRING;
const StaticString s_SHPP_RING("SHPP_RING");
const int64_t k_SHPP_RING = SHPP_RING;

class SHPHandleRes : public SweepableResourceData {
public:
  explicit SHPHandleRes(SHPHandle handle) : shp_handle(handle) { };
  SHPHandle operator* () const { return shp_handle; };

  static StaticString s_class_name;
  virtual const String& o_getClassNameHook() const { return s_class_name; }

  SHPHandle shp_handle;
};

StaticString SHPHandleRes::s_class_name("SHPHandle");

class SHPObjectRes : public SweepableResourceData {
public:
  explicit SHPObjectRes(SHPObject* object) : shp_object(object) { };
  SHPObject* operator* () const { return shp_object; };

  static StaticString s_class_name;
  virtual const String& o_getClassNameHook() const { return s_class_name; }

  SHPObject* shp_object;
};

StaticString SHPObjectRes::s_class_name("SHPObject");

#define CHECK_HANDLE(handle, type, shp, ret) \
  type ## Res * r_ ## shp = handle.getTyped<type ## Res>(true, true); \
  if (r_ ## shp == nullptr) { \
    raise_warning("Got NULL for resource"); \
    return (ret); \
  } else { \
    shp = **(r_ ## shp); \
  }

// Handle differences between pre-3.5.0 and later HHVM versions.
#ifdef NEWOBJ
    #define NEWRES(x) NEWOBJ(x)
#else
    #define NEWRES(x) newres<x>
#endif

static Variant HHVM_FUNCTION(shp_open, const String& filename, const String& access) {
  SHPHandle shph = SHPOpen(filename.c_str(), access.c_str());

  if (!shph) {
    return null_variant;
  }

  SHPHandleRes* shph_res = NEWRES(SHPHandleRes)(shph);
  return Resource(shph_res);
}

static Variant HHVM_FUNCTION(shp_create, const String& filename, int64_t shape_type) {
  switch (shape_type) {
  case SHPT_POINT:
  case SHPT_ARC:
  case SHPT_POLYGON:
  case SHPT_MULTIPOINT:
    break;
  default:
    raise_warning("Unsupported type %d", (int)shape_type);
    break;
  }
  SHPHandle shph = SHPCreate(filename.c_str(), shape_type);

  if (!shph) {
    return null_variant;
  }

  SHPHandleRes* shph_res = NEWRES(SHPHandleRes)(shph);
  return Resource(shph_res);
}

static bool HHVM_FUNCTION(shp_close, const Resource& shp) {
  SHPHandle shph;
  CHECK_HANDLE(shp, SHPHandle, shph, false);
  // the following is not done by PECL shape ext, but we will be incompatible here,
  // because we don't want memory leaks.
  SHPClose(shph);
  return true;
}

static Variant HHVM_FUNCTION(shp_read_object, const Resource& shp, int64_t ord) {
  SHPHandle shph;
  CHECK_HANDLE(shp, SHPHandle, shph, false);

  SHPObject* shp_obj = SHPReadObject(shph, ord);
  if (!shp_obj) {
    raise_warning("Reading of object %d failed", (int)ord);
    return null_variant;
  }

  SHPObjectRes* shp_obj_res = NEWRES(SHPObjectRes)(shp_obj);
  return Resource(shp_obj_res);
}

static bool HHVM_FUNCTION(shp_destroy_object, const Resource& shp_object) {
  SHPObject* shp_obj;
  CHECK_HANDLE(shp_object, SHPObject, shp_obj, false);
  // the following is not done by PECL shape ext, but we will be incompatible here,
  // because we don't want memory leaks.
  SHPDestroyObject(shp_obj);
  return true;
}

static bool HHVM_FUNCTION(shp_rewind_object, const Resource& shp_handle, const Resource& shp_object) {
  SHPHandle shph;
  CHECK_HANDLE(shp_handle, SHPHandle, shph, false);
  SHPObject* shp_obj;
  CHECK_HANDLE(shp_object, SHPObject, shp_obj, false);
  return SHPRewindObject(shph, shp_obj);
}

static Variant HHVM_FUNCTION(shp_write_object, const Resource& shp_handle, int64_t entity_num, const Resource& shp_object) {
  SHPHandle shph;
  CHECK_HANDLE(shp_handle, SHPHandle, shph, false);
  SHPObject* shp_obj;
  CHECK_HANDLE(shp_object, SHPObject, shp_obj, false);
  return Variant(SHPWriteObject(shph, entity_num, shp_obj));
}

static Variant HHVM_FUNCTION(shp_get_info, const Resource& shp_handle) {
  SHPHandle shph;
  CHECK_HANDLE(shp_handle, SHPHandle, shph, false);

  int ent_n, shp_type;
  double min_b[4], max_b[4];
  SHPGetInfo(shph, &ent_n, &shp_type, min_b, max_b);
  Array min_bound = Array::Create();
  Array max_bound = Array::Create();
  for (int i = 0; i < 4; i++) {
    min_bound.add(i, Variant(min_b[i]));
    max_bound.add(i, Variant(max_b[i]));
  }
  Array return_value = Array::Create();
  return_value.add(String("pnEntities"), Variant(ent_n));
  return_value.add(String("pnShapetype"), Variant(shp_type));
  return_value.add(String("padfMinBound"), Variant(min_bound));
  return_value.add(String("padfMaxBound"), Variant(max_bound));
  return Variant(return_value);
}

static bool HHVM_FUNCTION(shp_compute_extents, const Resource& shp_object) {
  SHPObject* shp_obj;
  CHECK_HANDLE(shp_object, SHPObject, shp_obj, false);

  SHPComputeExtents(shp_obj);
  return true;
}

static Variant HHVM_FUNCTION(shp_create_object_impl, const Array& args) {
  if (args.size() < 10) {
    return Variant(false);
  }
  int shp_type = args[0].toInt32(10);
  int shp_id = args[1].toInt32(10);
  int parts_num = args[2].toInt32(10);
  Array panPartStart = args[3].toArray();
  Array panPartType = args[4].toArray();
  int vertices_num = args[5].toInt32(10);
  Array padfX = args[6].toArray();
  Array padfY = args[7].toArray();
  Array padfZ = args[8].toArray();
  Array padfM = args[9].toArray();

  if (padfX.size() != padfY.size()) {
    raise_warning("padfX and padfY have different number of elements");
    return Variant(false);
  }
  if (padfX.size() < vertices_num) {
    raise_warning("less elements in padfX than specified in vertices_num");
    return Variant(false);
  }

  int c_panPartStart[panPartStart.size()];
  {
    size_t i = 0;
    for (ArrayIter arr_it(panPartStart); arr_it; ++arr_it, i++) {
      c_panPartStart[i] = arr_it.second().toInt32(10);
    }
  }

  int c_panPartType[panPartType.size()];
  {
    size_t i = 0;
    for (ArrayIter arr_it(panPartType); arr_it; ++arr_it, i++) {
      c_panPartType[i] = arr_it.second().toInt32(10);
    }
  }

  assert(padfX.size() == padfY.size());
  ssize_t padfZ_size = padfZ.size();
  if (padfZ_size == 0) { padfZ_size = SSIZE_MAX; }
  int real_vert_num = std::min(padfX.size(), padfZ_size);
  if (real_vert_num != vertices_num) {
    raise_notice("The real number of used vertices %d, passed %d", real_vert_num, vertices_num);
  }

  double c_padfX[padfX.size()];
  double c_padfY[padfY.size()];
  double c_padfZ[padfZ.size()];
  double c_padfM[padfM.size()];
  if (real_vert_num > 0) {
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfX); arr_it; ++arr_it, i++) {
        c_padfX[i] = arr_it.second().toDouble();
      }
    }
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfY); arr_it; ++arr_it, i++) {
        c_padfY[i] = arr_it.second().toDouble();
      }
    }
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfZ); arr_it; ++arr_it, i++) {
        c_padfZ[i] = arr_it.second().toDouble();
      }
    }
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfM); arr_it; ++arr_it, i++) {
        c_padfM[i] = arr_it.second().toDouble();
      }
    }
  }

  SHPObject* shp_obj = SHPCreateObject(shp_type, shp_id, parts_num, c_panPartStart, c_panPartType,
    real_vert_num, c_padfX, c_padfY, c_padfZ, c_padfM);
  if (shp_obj == nullptr) {
    raise_warning("SHPCreateObject returned NULL");
    return Variant(false);
  }

  SHPObjectRes* shp_obj_res = NEWRES(SHPObjectRes)(shp_obj);
  return Resource(shp_obj_res);
}

static Variant HHVM_FUNCTION(shp_create_simple_object,
  int64_t shp_type, int64_t vertices_num,
  const Array& padfX, const Array& padfY, const Array& padfZ
) {
  if (padfX.size() != padfY.size()) {
    raise_warning("padfX and padfY have different number of elements");
    return Variant(false);
  }
  if (padfX.size() < vertices_num) {
    raise_warning("less elements in padfX than specified in vertices_num");
    return Variant(false);
  }

  assert(padfX.size() == padfY.size());
  ssize_t padfZ_size = padfZ.size();
  if (padfZ_size == 0) { padfZ_size = SSIZE_MAX; }
  int real_vert_num = std::min(padfX.size(), padfZ_size);
  if (real_vert_num != vertices_num) {
    raise_notice("The real number of used vertices %d, passed %d", real_vert_num, (int)vertices_num);
  }

  double c_padfX[padfX.size()];
  double c_padfY[padfY.size()];
  double c_padfZ[padfZ.size()];
  if (real_vert_num > 0) {
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfX); arr_it; ++arr_it, i++) {
        c_padfX[i] = arr_it.second().toDouble();
      }
    }
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfY); arr_it; ++arr_it, i++) {
        c_padfY[i] = arr_it.second().toDouble();
      }
    }
    {
      size_t i = 0;
      for (ArrayIter arr_it(padfZ); arr_it; ++arr_it, i++) {
        c_padfZ[i] = arr_it.second().toDouble();
      }
    }
  }

  SHPObject* shp_obj = SHPCreateSimpleObject(shp_type, real_vert_num, c_padfX, c_padfY, c_padfZ);
  if (shp_obj == nullptr) {
    raise_warning("SHPCreateSimpleObject returned NULL");
    return Variant(false);
  }

  SHPObjectRes* shp_obj_res = NEWRES(SHPObjectRes)(shp_obj);
  return Resource(shp_obj_res);
}

static Variant HHVM_FUNCTION(shp_get_array_from_object, const Resource& shp_object) {
  SHPObject* shp_obj;
  CHECK_HANDLE(shp_object, SHPObject, shp_obj, false);

  Array return_value = Array::Create();
  return_value.add(String("nVertices"), Variant(shp_obj->nVertices));
  Array pointX_arr = Array::Create();
  Array pointY_arr = Array::Create();
  for (int i = 0; i < shp_obj->nVertices; i++) {
    pointX_arr.add(i, Variant(shp_obj->padfX[i]));
    pointY_arr.add(i, Variant(shp_obj->padfY[i]));
  }
  return_value.add(String("padfX"), Variant(pointX_arr));
  return_value.add(String("padfY"), Variant(pointY_arr));
  return Variant(return_value);
}

//////////////////////////////////////////////////////////////////////////////

class shpExtension: public Extension {
public:
  // 0.9.2-dev is PECL extension version we ported.
  shpExtension(): Extension("shp", "0.9.2-dev") { /* nothing */ }
  virtual void moduleInit() override {
    Native::registerConstant<KindOfInt64>(s_SHPT_NULL.get(), k_SHPT_NULL);
    Native::registerConstant<KindOfInt64>(s_SHPT_POINT.get(), k_SHPT_POINT);
    Native::registerConstant<KindOfInt64>(s_SHPT_ARC.get(), k_SHPT_ARC);
    Native::registerConstant<KindOfInt64>(s_SHPT_POLYGON.get(), k_SHPT_POLYGON);
    Native::registerConstant<KindOfInt64>(s_SHPT_MULTIPOINT.get(), k_SHPT_MULTIPOINT);
    Native::registerConstant<KindOfInt64>(s_SHPT_POINTZ.get(), k_SHPT_POINTZ);
    Native::registerConstant<KindOfInt64>(s_SHPT_ARCZ.get(), k_SHPT_ARCZ);
    Native::registerConstant<KindOfInt64>(s_SHPT_POLYGONZ.get(), k_SHPT_POLYGONZ);
    Native::registerConstant<KindOfInt64>(s_SHPT_MULTIPOINTZ.get(), k_SHPT_MULTIPOINTZ);
    Native::registerConstant<KindOfInt64>(s_SHPT_POINTM.get(), k_SHPT_POINTM);
    Native::registerConstant<KindOfInt64>(s_SHPT_ARCM.get(), k_SHPT_ARCM);
    Native::registerConstant<KindOfInt64>(s_SHPT_POLYGONM.get(), k_SHPT_POLYGONM);
    Native::registerConstant<KindOfInt64>(s_SHPT_MULTIPOINTM.get(), k_SHPT_MULTIPOINTM);
    Native::registerConstant<KindOfInt64>(s_SHPT_MULTIPATCH.get(), k_SHPT_MULTIPATCH);
    Native::registerConstant<KindOfInt64>(s_SHPP_TRISTRIP.get(), k_SHPP_TRISTRIP);
    Native::registerConstant<KindOfInt64>(s_SHPP_TRIFAN.get(), k_SHPP_TRIFAN);
    Native::registerConstant<KindOfInt64>(s_SHPP_OUTERRING.get(), k_SHPP_OUTERRING);
    Native::registerConstant<KindOfInt64>(s_SHPP_INNERRING.get(), k_SHPP_INNERRING);
    Native::registerConstant<KindOfInt64>(s_SHPP_FIRSTRING.get(), k_SHPP_FIRSTRING);
    Native::registerConstant<KindOfInt64>(s_SHPP_RING.get(), k_SHPP_RING);
    HHVM_FE(shp_open);
    HHVM_FE(shp_create);
    HHVM_FE(shp_close);
    HHVM_FE(shp_read_object);
    HHVM_FE(shp_destroy_object);
    HHVM_FE(shp_rewind_object);
    HHVM_FE(shp_write_object);
    HHVM_FE(shp_get_info);
    HHVM_FE(shp_compute_extents);
    HHVM_FE(shp_create_object_impl);
    HHVM_FE(shp_create_simple_object);
    HHVM_FE(shp_get_array_from_object);
    loadSystemlib();
  }
} s_shp_extension;

HHVM_GET_MODULE(shp);

}

