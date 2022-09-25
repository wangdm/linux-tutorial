
set(lib rt pthread)

add_executable(file_lock file_lock.c)
target_link_libraries(file_lock ${lib})