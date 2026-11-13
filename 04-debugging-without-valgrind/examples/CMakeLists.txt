add_example (t04_padding_check 01_padding_check.cpp)
add_example (t04_double_free 02_double_free.cpp)
add_example (t04_leak_report 03_leak_report.cpp)
add_example (t04_allocation_numbers 04_allocation_numbers.cpp)
add_example (t04_statistics 05_statistics.cpp)
add_example (t04_debug_config 06_debug_config.cpp)
# The same program again, as a debug run.
add_test (NAME t04_debug_config_debug_run COMMAND t04_debug_config --debug-memory)
add_example (t04_padding_cost 07_padding_cost.cpp)
