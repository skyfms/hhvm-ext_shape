include(Findlibshp)
if (LIBSHP_FOUND)
else (LIBSHP_FOUND)
  message(FATAL_ERROR "Could not find libshp")
endif (LIBSHP_FOUND)

include_directories(${LIBSHP_INCLUDE_DIR})

HHVM_EXTENSION(shp ext_shape.cpp)
HHVM_SYSTEMLIB(shp ext_shp.php)

target_link_libraries(shp ${LIBSHP_LIBRARIES})
