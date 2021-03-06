add_library(cloudphxx SHARED lib.cpp)
add_dependencies(cloudphxx git_revision.h)
set_target_properties(cloudphxx PROPERTIES SUFFIX ".so") # e.g. Mac defaults to .dylib which is not looked for by Python

# informing the Python bindings where to find Python
find_package(PythonLibs)
if (NOT PYTHON_LIBRARIES)
  message(FATAL_ERROR "
    Python libraries not found. 
    Please install them (e.g. sudo apt-get install python-dev).
  ")
endif()
target_include_directories(cloudphxx PUBLIC ${PYTHON_INCLUDE_DIRS})
target_link_libraries(cloudphxx ${PYTHON_LIBRARIES})

# informing the Python bindings where to find Boost.Python 
find_package(Boost COMPONENTS python)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "
    Boost.Python not found. 
    Please install it (e.g. sudo apt-get install libboost-python-dev).
  ")
endif()
target_link_libraries(cloudphxx ${Boost_LIBRARIES})

# boost python 1.65.0 replaced the booost::python::numeric API with boost::python::numpy
if(${Boost_MINOR_VERSION} LESS 65)
  target_compile_options(cloudphxx PRIVATE -DBPNUMERIC)
else()
  find_package(Boost COMPONENTS numpy REQUIRED)
  target_link_libraries(cloudphxx ${Boost_LIBRARIES})
  target_compile_options(cloudphxx PRIVATE  -DBPNUMPY)
endif()

# requireing Blitz++
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
    #include <blitz/array.h>
    int main() {}
  " BLITZ_FOUND)
if (NOT BLITZ_FOUND)
  message(FATAL_ERROR "
    Blitz++ library not found. 
    Please install it (e.g. sudo apt-get install libblitz0-dev).
  ")
else()
  target_link_libraries(cloudphxx blitz)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cloudphxx PRIVATE -DBZDEBUG)
  endif()
endif()

target_link_libraries(cloudphxx cloudphxx_lgrngn)

#to retain rpath to libcloudphxx_lgrngn.so linked by libcloudphxx.so after installation
set_property(TARGET cloudphxx PROPERTY INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set_property(TARGET cloudphxx PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

# where to install python modules (see http://stackoverflow.com/questions/1242904/finding-python-site-packages-directory-with-cmake)
execute_process(
  COMMAND "${PYTHON_EXECUTABLE}" -c "if True:
    import sysconfig as sc
    print(sc.get_path('platlib'))"
  OUTPUT_VARIABLE PYTHON_SITE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)


install ( TARGETS cloudphxx
   LIBRARY
     DESTINATION ${PYTHON_SITE_DIR}
     COMPONENT library
)
