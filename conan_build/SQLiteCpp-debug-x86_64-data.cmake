########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(sqlitecpp_COMPONENT_NAMES "")
if(DEFINED sqlitecpp_FIND_DEPENDENCY_NAMES)
  list(APPEND sqlitecpp_FIND_DEPENDENCY_NAMES SQLite3)
  list(REMOVE_DUPLICATES sqlitecpp_FIND_DEPENDENCY_NAMES)
else()
  set(sqlitecpp_FIND_DEPENDENCY_NAMES SQLite3)
endif()
set(SQLite3_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(sqlitecpp_PACKAGE_FOLDER_DEBUG "/home/vscode/.conan2/p/b/sqlit778b1772526f2/p")
set(sqlitecpp_BUILD_MODULES_PATHS_DEBUG )


set(sqlitecpp_INCLUDE_DIRS_DEBUG "${sqlitecpp_PACKAGE_FOLDER_DEBUG}/include")
set(sqlitecpp_RES_DIRS_DEBUG )
set(sqlitecpp_DEFINITIONS_DEBUG )
set(sqlitecpp_SHARED_LINK_FLAGS_DEBUG )
set(sqlitecpp_EXE_LINK_FLAGS_DEBUG )
set(sqlitecpp_OBJECTS_DEBUG )
set(sqlitecpp_COMPILE_DEFINITIONS_DEBUG )
set(sqlitecpp_COMPILE_OPTIONS_C_DEBUG )
set(sqlitecpp_COMPILE_OPTIONS_CXX_DEBUG )
set(sqlitecpp_LIB_DIRS_DEBUG "${sqlitecpp_PACKAGE_FOLDER_DEBUG}/lib")
set(sqlitecpp_BIN_DIRS_DEBUG )
set(sqlitecpp_LIBRARY_TYPE_DEBUG STATIC)
set(sqlitecpp_IS_HOST_WINDOWS_DEBUG 0)
set(sqlitecpp_LIBS_DEBUG SQLiteCpp)
set(sqlitecpp_SYSTEM_LIBS_DEBUG pthread dl m)
set(sqlitecpp_FRAMEWORK_DIRS_DEBUG )
set(sqlitecpp_FRAMEWORKS_DEBUG )
set(sqlitecpp_BUILD_DIRS_DEBUG )
set(sqlitecpp_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(sqlitecpp_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${sqlitecpp_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${sqlitecpp_COMPILE_OPTIONS_C_DEBUG}>")
set(sqlitecpp_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlitecpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlitecpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlitecpp_EXE_LINK_FLAGS_DEBUG}>")


set(sqlitecpp_COMPONENTS_DEBUG )