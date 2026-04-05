#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Hazel::Hazel" for configuration "RelWithDebInfo"
set_property(TARGET Hazel::Hazel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Hazel::Hazel PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/Hazel.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/Hazel.dll"
  )

list(APPEND _cmake_import_check_targets Hazel::Hazel )
list(APPEND _cmake_import_check_files_for_Hazel::Hazel "${_IMPORT_PREFIX}/lib/Hazel.lib" "${_IMPORT_PREFIX}/bin/Hazel.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
