
set(lib rt pthread)

add_executable(flock file_lock.c)
target_link_libraries(flock ${lib})