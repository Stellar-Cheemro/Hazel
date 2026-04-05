#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Hazel::Hazel" for configuration "MinSizeRel"
set_property(TARGET Hazel::Hazel APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(Hazel::Hazel PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/Hazel.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/Hazel.dll"
  )

list(APPEND _cmake_import_check_targets Hazel::Hazel )
list(APPEND _cmake_import_check_files_for_Hazel::Hazel "${_IMPORT_PREFIX}/lib/Hazel.lib" "${_IMPORT_PREFIX}/bin/Hazel.dll" )

# Import target "Hazel::Sandbox" for configuration "MinSizeRel"
set_property(TARGET Hazel::Sandbox APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(Hazel::Sandbox PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/Sandbox.exe"
  )

list(APPEND _cmake_import_check_targets Hazel::Sandbox )
list(APPEND _cmake_import_check_files_for_Hazel::Sandbox "${_IMPORT_PREFIX}/bin/Sandbox.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
