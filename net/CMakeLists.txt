
set(lib rt pthread)

add_executable(tcp_server tcp_server.c)
target_link_libraries(tcp_server ${lib})

add_executable(tcp_client tcp_client.c)
target_link_libraries(tcp_client ${lib})

add_executable(tls_server tls_server.c)
target_link_libraries(tls_server ${lib} ssl crypto)

add_executable(tls_client tls_client.c)
target_link_libraries(tls_client ${lib} ssl crypto)