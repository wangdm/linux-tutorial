cmake_minimum_required(VERSION 3.0.0)
project(tutorial VERSION 0.1.0)

include(CTest)
enable_testing()

set(mq_lib rt pthread)

#mq
add_executable(mq_consumer ipc/mq_consumer.c)
target_link_libraries(mq_consumer ${mq_lib})

add_executable(mq_producer ipc/mq_producer.c)
target_link_libraries(mq_producer ${mq_lib})

#sock
add_executable(sock_client ipc/sock_client.c)
target_link_libraries(sock_client ${mq_lib})

add_executable(sock_server ipc/sock_server.c)
target_link_libraries(sock_server ${mq_lib})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)