cmake_minimum_required ( VERSION 2.8.5 )
project ( zlib )

set ( CMAKE_CURRENT_SOURCE_DIR "${MOAI_ROOT}/3rdparty/zlib-1.2.11/" )
set ( ZLIB_INCLUDES "${MOAI_ROOT}/3rdparty/zlib-1.2.11/" )

file ( GLOB ZLIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" )
file ( GLOB ZLIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

add_library ( zlib STATIC ${ZLIB_SOURCES} ${ZLIB_HEADERS} )
target_include_directories ( zlib PUBLIC "$<BUILD_INTERFACE:${ZLIB_INCLUDES}>")
set_target_properties ( zlib PROPERTIES FOLDER Third-Party )
install(TARGETS zlib EXPORT libmoai ARCHIVE DESTINATION lib)
