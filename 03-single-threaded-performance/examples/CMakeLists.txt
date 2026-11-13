find_package (Threads REQUIRED)

add_example (t03_thread_and_block_scope 01_thread_and_block_scope.cpp)
target_link_libraries (t03_thread_and_block_scope PRIVATE Threads::Threads)
add_example (t03_threads_benchmark 02_threads_benchmark.cpp)
target_link_libraries (t03_threads_benchmark PRIVATE Threads::Threads)
add_example (t03_fixed_arena 03_fixed_arena.cpp)
add_example (t03_segments 04_segments.cpp)
