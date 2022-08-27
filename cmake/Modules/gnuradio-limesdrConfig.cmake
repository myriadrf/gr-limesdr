find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_LIMESDR gnuradio-limesdr)

FIND_PATH(
    GR_LIMESDR_INCLUDE_DIRS
    NAMES gnuradio/limesdr/api.h
    HINTS $ENV{LIMESDR_DIR}/include
        ${PC_LIMESDR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_LIMESDR_LIBRARIES
    NAMES gnuradio-limesdr
    HINTS $ENV{LIMESDR_DIR}/lib
        ${PC_LIMESDR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-limesdrTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_LIMESDR DEFAULT_MSG GR_LIMESDR_LIBRARIES GR_LIMESDR_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_LIMESDR_LIBRARIES GR_LIMESDR_INCLUDE_DIRS)
