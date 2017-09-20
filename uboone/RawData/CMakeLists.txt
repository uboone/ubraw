# Include for Swizzler
set( UBOONEDAQ_DATATYPES_DIR $ENV{UBOONEDAQ_DATATYPES_DIR} )
if( UBOONEDAQ_DATATYPES_DIR )
   add_subdirectory(utils)
#   add_subdirectory(supernova)
#   add_subdirectory(uboone_datatypes)
endif()
