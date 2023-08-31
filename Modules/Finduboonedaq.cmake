#[================================================================[.rst:

Name:    Finduboonedaq.cmake

Purpose: find_package module for ups product uboonedaq_datatypes.

Created: 31-Aug-2023  H. Greenlee

------------------------------------------------------------------

The uboonedaq_datatypes ups product defines the following environment variables,
which are used by this module.

UBOONEDAQ_DATATYPES_INC - Include path
UBOONEDAQ_DATATYPES_LIB - Library path

This module creates the following target, corresponding to the library
in the library directory.

uboonedaq::data_types - libubdata_types.so

#]================================================================]

# Don't do anything of this package has already been found.

if(NOT uboonedaq_FOUND)

  # First hunt for the uboonedaq include directory.

  message("Finding package uboonedaq")
  find_file(_uboonedaq_h NAMES datatypes HINTS ENV UBOONEDAQ_DATATYPES_INC NO_CACHE)
  if(_uboonedaq_h)
    get_filename_component(_uboonedaq_include_dir ${_uboonedaq_h} DIRECTORY)
    message("Found uboonedaq include directory ${_uboonedaq_include_dir}")
    set(uboonedaq_FOUND TRUE)
  else()
    message("Could not find uboonedaq include directory")
  endif()

  # Next hunt for the uboonedaq libraries.

  if(uboonedaq_FOUND)

    # Loop over libraries.

    if(NOT TARGET uboonedaq::data_types)

      # Hunt for this library.

      find_library(_uboonedaq_lib_path LIBRARY NAMES ubdata_types HINTS ENV UBOONEDAQ_DATATYPES_LIB REQUIRED NO_CACHE)
      message("Found uboonedaq library ${_uboonedaq_lib_path}")

      # Make taret.

      message("Making target uboonedaq::data_types")
      add_library(uboonedaq::data_types SHARED IMPORTED)
      set_target_properties(uboonedaq::data_types PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_uboonedaq_include_dir}"
        IMPORTED_LOCATION "${_uboonedaq_lib_path}"
      )
    endif()
  endif()
endif()
