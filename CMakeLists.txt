cmake_minimum_required(VERSION 3.0.0)
project(tutorial VERSION 0.1.0)

include(CTest)
enable_testing()

add_compile_definitions(PROJECT_SOURCE_ROOT="${PROJECT_SOURCE_DIR}")
string(LENGTH ${PROJECT_SOURCE_DIR}/ SOURCE_ROOT_LENGTH)
add_compile_definitions(WDM_SOURCE_ROOT_LENGTH=${SOURCE_ROOT_LENGTH})

include_directories(${PROJECT_SOURCE_DIR}/include/)
link_directories(${PROJECT_SOURCE_DIR}/library/)

set(mq_lib rt pthread)

#semaphore
add_executable(semaphore ipc/semaphore.c)
target_link_libraries(semaphore ${mq_lib})

#message queue
add_executable(mq_consumer ipc/mq_consumer.c)
target_link_libraries(mq_consumer ${mq_lib})

add_executable(mq_producer ipc/mq_producer.c)
target_link_libraries(mq_producer ${mq_lib})

#local socket
add_executable(sock_client ipc/sock_client.c)
target_link_libraries(sock_client ${mq_lib})

add_executable(sock_server ipc/sock_server.c)
target_link_libraries(sock_server ${mq_lib})

#dbus
add_executable(dbus_sender dbus/sender.c)
target_link_libraries(dbus_sender dbus pthread)

add_executable(dbus_receiver dbus/receiver.c)
target_link_libraries(dbus_receiver dbus pthread)

#uvc
add_executable(uvc uvc/main.c)
target_link_libraries(uvc jpeg)

add_subdirectory(sync)
add_subdirectory(net)
add_subdirectory(libuv)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
