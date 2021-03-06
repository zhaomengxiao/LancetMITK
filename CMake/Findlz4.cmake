find_path(lz4_INCLUDE_DIR lz4.h
  PATHS ${MITK_EXTERNAL_PROJECT_PREFIX}
  PATH_SUFFIXES include
  NO_DEFAULT_PATH
)
mark_as_advanced(lz4_INCLUDE_DIR)


find_library(lz4_LIBRARY_RELEASE lz4
  PATHS ${MITK_EXTERNAL_PROJECT_PREFIX}
  PATH_SUFFIXES lib lib64
  NO_DEFAULT_PATH)
mark_as_advanced(lz4_LIBRARY_RELEASE)


find_library(lz4_LIBRARY_DEBUG lz4d
  PATHS ${MITK_EXTERNAL_PROJECT_PREFIX}
  PATH_SUFFIXES lib lib64
  NO_DEFAULT_PATH)
mark_as_advanced(lz4_LIBRARY_DEBUG)

set(lz4_LIBRARIES)

if(lz4_LIBRARY_RELEASE)
  list(APPEND lz4_LIBRARIES optimized ${lz4_LIBRARY_RELEASE})
endif()

if(lz4_LIBRARY_DEBUG)
  list(APPEND lz4_LIBRARIES debug ${lz4_LIBRARY_DEBUG})
endif()

find_package_handle_standard_args(lz4
  FOUND_VAR lz4_FOUND
  REQUIRED_VARS lz4_INCLUDE_DIR lz4_LIBRARIES
)
