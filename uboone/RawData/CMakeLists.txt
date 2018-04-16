cet_add_compiler_flags(CXX -Wno-dynamic-exception-spec -Wno-return-type-c-linkage -Wno-undefined-var-template )

# Include for Swizzler
set( UBOONEDAQ_DATATYPES_DIR $ENV{UBOONEDAQ_DATATYPES_DIR} )
if( UBOONEDAQ_DATATYPES_DIR )
   add_subdirectory(utils)
   add_subdirectory(supernova)
endif()
