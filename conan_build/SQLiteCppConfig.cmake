########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(SQLiteCpp_FIND_QUIETLY)
    set(SQLiteCpp_MESSAGE_MODE VERBOSE)
else()
    set(SQLiteCpp_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SQLiteCppTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${sqlitecpp_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(SQLiteCpp_VERSION_STRING "3.3.2")
set(SQLiteCpp_INCLUDE_DIRS ${sqlitecpp_INCLUDE_DIRS_DEBUG} )
set(SQLiteCpp_INCLUDE_DIR ${sqlitecpp_INCLUDE_DIRS_DEBUG} )
set(SQLiteCpp_LIBRARIES ${sqlitecpp_LIBRARIES_DEBUG} )
set(SQLiteCpp_DEFINITIONS ${sqlitecpp_DEFINITIONS_DEBUG} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${sqlitecpp_BUILD_MODULES_PATHS_DEBUG} )
    message(${SQLiteCpp_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


