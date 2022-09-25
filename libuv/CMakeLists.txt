
set(lib rt pthread)

#libuv
add_executable(uv_tcp_server tcp_server.c)
target_link_libraries(uv_tcp_server uv pthread)

add_executable(uv_tcp_client tcp_client.c)
target_link_libraries(uv_tcp_client uv pthread)