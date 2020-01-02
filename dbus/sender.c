//
// Created by wangdm on 2020/1/1.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "dbus/dbus.h"

int main(int argc, char *argv[]) {
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

    ret = dbus_bus_request_name(connection, "dbus.sender", DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (TRUE != ret) {
        printf("dbus_bus_request_name failed, name:%s, message:%s\n", error.name, error.message);
    }

    DBusMessage *message = dbus_message_new_signal("/test", "cn.wangdm.dbus", "test");
    ret = dbus_connection_send(connection, message, NULL);
    if (TRUE != ret) {
        printf("DBus send failed\n");
    }

    dbus_connection_flush(connection);

    dbus_message_unref(message);
    dbus_connection_unref(connection);
    return 0;
}