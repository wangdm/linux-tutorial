//
// Created by wangdm on 2020/1/1.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include "dbus/dbus.h"

#define DBUS_NAME "cn.wangdm.receiver"
#define DBUS_PATH "/cn/wangdm/test"
#define DBUS_IFACE "cn.wangdm.test"

static dbus_bool_t dbus_add_handle(DBusWatch *watch, void *data) {
    printf("dbus_add_handle\n");
    return TRUE;
}

static void dbus_remove_handle(DBusWatch *watch, void *data) {
    printf("dbus_remove_handle\n");
}

static void dbus_toggled_handle(DBusWatch *watch, void *data) {
    printf("dbus_toggled_handle\n");
}

static DBusHandlerResult dbus_message_handle1(DBusConnection *connection, DBusMessage *message, void *user_data) {
    printf("receive message int handler1\n");
    int type = dbus_message_get_type(message);
    const char *path = dbus_message_get_path(message);
    const char *interface = dbus_message_get_interface(message);
    const char *member = dbus_message_get_member(message);
    const char *sender = dbus_message_get_sender(message);
    const char *dest = dbus_message_get_destination(message);
    printf("type: %s\n", dbus_message_type_to_string(type));
    printf("path: %s\n", path);
    printf("interface: %s\n", interface);
    printf("member: %s\n", member);
    printf("sender: %s\n", sender);
    printf("dest: %s\n", dest);
    printf("\n");
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusHandlerResult dbus_message_handle2(DBusConnection *connection, DBusMessage *message, void *user_data) {
    printf("receive message int handler2\n");
    int type = dbus_message_get_type(message);
    const char *path = dbus_message_get_path(message);
    const char *interface = dbus_message_get_interface(message);
    const char *member = dbus_message_get_member(message);
    const char *sender = dbus_message_get_sender(message);
    const char *dest = dbus_message_get_destination(message);
    printf("type: %s\n", dbus_message_type_to_string(type));
    printf("path: %s\n", path);
    printf("interface: %s\n", interface);
    printf("member: %s\n", member);
    printf("sender: %s\n", sender);
    printf("dest: %s\n", dest);
    printf("\n");
    return DBUS_HANDLER_RESULT_HANDLED;
}

int main(int argc, char **argv) {
    int ret;
    DBusError error;
    dbus_error_init(&error);
    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (!connection) {
        printf("Get dbus conenct failed, name:%s, message:%s\n", error.name, error.message);
        dbus_error_free(&error);
        return 0;
    }

    const char *name = dbus_bus_get_unique_name(connection);
    printf("DBus connect success, name: %s\n", name);

    ret = dbus_bus_request_name(connection, DBUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (TRUE != ret) {
        printf("dbus_bus_request_name failed, name:%s, message:%s\n", error.name, error.message);
    }

    //dbus_bus_add_match(connection, "type='signal'", &error);
    dbus_bus_add_match(connection, "type='signal',interface='"DBUS_IFACE"'", &error);

    dbus_connection_add_filter(connection, dbus_message_handle2, NULL, NULL);
    dbus_connection_add_filter(connection, dbus_message_handle1, NULL, NULL);

    ret = dbus_connection_set_watch_functions(connection, dbus_add_handle, dbus_remove_handle, dbus_toggled_handle,
                                              NULL, NULL);
    if (TRUE != ret) {
        printf("dbus_connection_set_watch_functions failed\n");
    }

    DBusDispatchStatus status = dbus_connection_read_write_dispatch(connection, 0);

    do {
        ret = dbus_connection_read_write(connection, 3000);
        if (FALSE == ret) {
            printf("dbus_connection_read_write failed\n");
            break;
        }
        DBusMessage *message = dbus_connection_pop_message(connection);
        if (message) {
            printf("receive message in loop\n");
            int type = dbus_message_get_type(message);
            const char *path = dbus_message_get_path(message);
            const char *interface = dbus_message_get_interface(message);
            const char *member = dbus_message_get_member(message);
            const char *sender = dbus_message_get_sender(message);
            const char *dest = dbus_message_get_destination(message);
            printf("type: %s\n", dbus_message_type_to_string(type));
            printf("path: %s\n", path);
            printf("interface: %s\n", interface);
            printf("member: %s\n", member);
            printf("sender: %s\n", sender);
            printf("dest: %s\n", dest);
            printf("\n");
        }

        sleep(1);
    } while (1);


    dbus_connection_unref(connection);
    return 0;
}