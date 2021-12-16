# ======================================================================
#  larsoft main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larsoft/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(ubraw VERSION 09.40.00 LANGUAGES CXX)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcorealg )
find_ups_product( larcoreobj )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_boost( )
find_ups_product( canvas )
find_ups_product( ubcore )
find_ups_product( ifdh_art )
find_ups_product( larevt )
find_ups_product( lardata )
find_ups_product( uboonedaq_datatypes )
find_ups_product( art )
find_ups_product(art_root_io)
find_ups_product( larcore )
find_ups_product( nutools )
find_ups_product( nurandom )
find_ups_root()
find_package(PostgreSQL REQUIRED PUBLIC)
cet_find_library( UB_DATA_TYPES NAMES ubdata_types PATHS ENV UBOONEDAQ_DATATYPES_LIB NO_DEFAULT_PATH )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(ubraw)

# Unit tests.
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
