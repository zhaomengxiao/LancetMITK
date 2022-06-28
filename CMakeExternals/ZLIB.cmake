set(proj ZLIB)
set(proj_DEPENDENCIES "")

if(MITK_USE_${proj})
  set(${proj}_DEPENDS ${proj})

  if(DEFINED ${proj}_DIR AND NOT EXISTS ${${proj}_DIR})
    message(FATAL_ERROR "${proj}_DIR variable is defined but corresponds to non-existing directory!")
  endif()

  find_package(ZLIB QUIET)

  if(NOT DEFINED ${proj}_DIR AND NOT ZLIB_FOUND)
    ExternalProject_Add(${proj}
      URL ${MITK_THIRDPARTY_DOWNLOAD_PREFIX_URL}/zlib-1.2.11.zip
      URL_MD5 9d6a627693163bbbf3f26403a3a0b0b1
      CMAKE_GENERATOR ${gen}
      CMAKE_GENERATOR_PLATFORM ${gen_platform}
      CMAKE_ARGS ${ep_common_args}
      CMAKE_CACHE_ARGS ${ep_common_cache_args}
      CMAKE_CACHE_DEFAULT_ARGS ${ep_common_cache_default_args}
      DEPENDS ${proj_DEPENDENCIES}
    )

    set(${proj}_DIR ${ep_prefix})
  else()
    mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  endif()
endif()
