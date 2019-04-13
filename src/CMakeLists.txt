cmake_minimum_required(VERSION 3.7)

set(CMAKE_BUILD_TYPE Release)

if(NOT UNIX) {
    message(FATAL_ERROR "not UNIX!")
}

execute_process(
    COMMAND pg_config --includedir-server 
    OUTPUT_VARIABLE POSTGRES_INCLUDE
)

include_directories(
    "${POSTGRES_INCLUDE}"
)

# pg_dummy
add_library(pg_dummy SHARED
    pg_dummy.cpp
)

# ライブラリのインストール先 → ${POSTGRES_PKGLIBDIR}
execute_process(
    COMMAND pg_config --pkglibdir
    OUTPUT_VARIABLE POSTGRES_PKGLIBDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Erase prefix like "lib": libpg_dummy.so → pg_dummy.so
# See: https://cmake.org/cmake/help/v3.0/command/set_target_properties.html
SET_TARGET_PROPERTIES(
    pg_dummy
    PROPERTIES PREFIX ""
)

install(
    TARGETS pg_dummy
    LIBRARY DESTINATION "${POSTGRES_PKGLIBDIR}"
)

# See: https://cmake.org/cmake/help/v3.7/module/CPackDeb.html#variable:CPACK_DEBIAN_FILE_NAME
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

# See: https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/PackageGenerators#deb-unix-only
# See: https://stackoverflow.com/q/13144181
# See: https://cmake.org/cmake/help/v3.7/module/CPackDeb.html
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME "postgresql-10-pg_dummy")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "postgresql-10")
set(CPACK_PACKAGE_CONTACT "kenjiuno <ku@digitaldolphins.jp>")
set(CPACK_DEBIAN_PACKAGE_SECTION "database")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "pg_dummy for PostgreSQL C libraries")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
include(CPack)
