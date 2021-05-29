project(Damerau-Levenshtein LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 17)

include( "build-aux/create_library.cmake" )

# required devinition
add_compile_definitions(PACKAGE_VERSION="1.0.0")
add_compile_definitions(PACKAGE_URL="https://github.com/sp1ff/damerau-levenshtein")

# create libraries
create_library( berghel_roach br )
create_library( ukkonen uk )
create_library( lowrance_wagner lw )

# add executable
option(CREATE_DL_EXECUTABLE "determines to generate docs or not" ON)
if(CREATE_DL_EXECUTABLE)
	add_executable(dl src/dl.cc)
	target_link_libraries(dl PRIVATE berghel_roach ukkonen lowrance_wagner)
endif()