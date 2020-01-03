//
// Created by wangdm on 2020/1/1.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "dbus/dbus.h"

#define DBUS_NAME "cn.wangdm.sender"
#define DBUS_PATH "/cn/wangdm/test"
#define DBUS_IFACE "cn.wangdm.test"

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

    ret = dbus_bus_request_name(connection, DBUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (TRUE != ret) {
        printf("dbus_bus_request_name failed, name:%s, message:%s\n", error.name, error.message);
    }

    DBusMessage *signal_msg = dbus_message_new_signal(DBUS_PATH, DBUS_IFACE, "test");
    if (signal_msg){
        DBusMessageIter iter;
        dbus_message_iter_init_append(signal_msg, &iter);
        int value = 10;
        if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &value)) {
            printf("dbus_message_iter_append_basic failed\n");
        }
        ret = dbus_connection_send(connection, signal_msg, NULL);
        if (TRUE != ret) {
            printf("DBus send failed\n");
        }
        dbus_message_unref(signal_msg);
    }

    DBusMessage *method_msg = dbus_message_new_method_call(DBUS_IFACE, DBUS_PATH, DBUS_IFACE, "test");
    if (method_msg){
        DBusMessageIter iter;
        dbus_message_iter_init_append(method_msg, &iter);
        int int_value = 1989;
        if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &int_value)) {
            printf("dbus_message_iter_append_basic failed\n");
        }
        char* str_value = "Hello";
        if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str_value)) {
            printf("dbus_message_iter_append_basic failed\n");
        }
        ret = dbus_connection_send(connection, method_msg, NULL);
        if (TRUE != ret) {
            printf("DBus send failed\n");
        }
    }

    dbus_connection_flush(connection);

    dbus_connection_unref(connection);
    return 0;
}