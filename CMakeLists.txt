# Builds and tests the examples of every tutorial against a CookMem release.
cmake_minimum_required (VERSION 3.14)

project (CookMemTutorials CXX)

# The CookMem release the tutorials are written for.  Update it with each
# CookMem release, and fix any example that no longer builds or runs.
set (COOKMEM_TAG "v1.5.0" CACHE STRING "CookMem tag to build the examples with")

include (FetchContent)
FetchContent_Declare (cookmem
    GIT_REPOSITORY https://github.com/coconut2015/cookmem.git
    GIT_TAG        ${COOKMEM_TAG}
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable (cookmem)

enable_testing ()

# add_example (<name> <source> [CXX_STANDARD <n>])
#
# Builds an example and runs it as a test.  The examples use C++11, like
# CookMem, unless they need a newer standard.
function (add_example name source)
    cmake_parse_arguments (ARG "" "CXX_STANDARD" "" ${ARGN})
    if (NOT ARG_CXX_STANDARD)
        set (ARG_CXX_STANDARD 11)
    endif ()
    add_executable (${name} ${source})
    target_link_libraries (${name} PRIVATE cookmem::cookmem)
    set_target_properties (${name} PROPERTIES
        CXX_STANDARD ${ARG_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON)
    if (MSVC)
        # The examples use getenv and similar standard functions.
        target_compile_definitions (${name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else ()
        target_compile_options (${name} PRIVATE -Wall -Wextra)
    endif ()
    add_test (NAME ${name} COMMAND ${name})
endfunction ()

add_subdirectory (01-getting-started/examples)
add_subdirectory (02-complex-pointer-graphs/examples)
add_subdirectory (03-single-threaded-performance/examples)
add_subdirectory (04-debugging-without-valgrind/examples)
