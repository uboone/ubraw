# ======================================================================
#  ubraw main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lardataobj/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(ubraw VERSION 10.00.02 LANGUAGES CXX)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

find_package( ifdh_art REQUIRED EXPORT )
find_package( larevt REQUIRED EXPORT )
find_package( ubcore REQUIRED EXPORT )
find_package( uboonedaq_datatypes REQUIRED EXPORT )
find_package( libwda REQUIRED EXPORT )
find_package( uboonedaq_datatypes REQUIRED EXPORT )

# macros for dictionary and simple_plugin
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(ubraw)

# Unit tests.
add_subdirectory(test)

# ups - table and config files

# packaging utility
cet_cmake_config()
