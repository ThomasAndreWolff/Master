# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(sqlitecpp_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(sqlitecpp_FRAMEWORKS_FOUND_DEBUG "${sqlitecpp_FRAMEWORKS_DEBUG}" "${sqlitecpp_FRAMEWORK_DIRS_DEBUG}")

set(sqlitecpp_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET sqlitecpp_DEPS_TARGET)
    add_library(sqlitecpp_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET sqlitecpp_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${sqlitecpp_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${sqlitecpp_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:SQLite::SQLite3>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### sqlitecpp_DEPS_TARGET to all of them
conan_package_library_targets("${sqlitecpp_LIBS_DEBUG}"    # libraries
                              "${sqlitecpp_LIB_DIRS_DEBUG}" # package_libdir
                              "${sqlitecpp_BIN_DIRS_DEBUG}" # package_bindir
                              "${sqlitecpp_LIBRARY_TYPE_DEBUG}"
                              "${sqlitecpp_IS_HOST_WINDOWS_DEBUG}"
                              sqlitecpp_DEPS_TARGET
                              sqlitecpp_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "sqlitecpp"    # package_name
                              "${sqlitecpp_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sqlitecpp_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${sqlitecpp_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${sqlitecpp_LIBRARIES_TARGETS}>
                 )

    if("${sqlitecpp_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET SQLiteCpp
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     sqlitecpp_DEPS_TARGET)
    endif()

    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${sqlitecpp_LINKER_FLAGS_DEBUG}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${sqlitecpp_INCLUDE_DIRS_DEBUG}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${sqlitecpp_LIB_DIRS_DEBUG}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${sqlitecpp_COMPILE_DEFINITIONS_DEBUG}>)
    set_property(TARGET SQLiteCpp
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${sqlitecpp_COMPILE_OPTIONS_DEBUG}>)

########## For the modules (FindXXX)
set(sqlitecpp_LIBRARIES_DEBUG SQLiteCpp)
