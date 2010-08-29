
#include <stdio.h>
#include <stdlib.h>

#include <dbus/dbus.h>

static DBusConnection* connection;
static DBusError error = DBUS_ERROR_INIT;
static DBusMessage* reply;
static DBusMessage* request;

static void
cleanup(void)
{
    if (reply != NULL) {
        dbus_message_unref(reply);
        reply = NULL;
    }

    if (request != NULL) {
        dbus_message_unref(request);
        request = NULL;
    }

    if (connection != NULL) {
        dbus_connection_unref(connection);
        connection = NULL;
    }

    dbus_error_free(&error);
}

int
main(int argc, char** argv)
{
    dbus_bool_t ok;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s layout_name\n", argv[0]);
        return 1;
    }

    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
        fprintf(stderr,
                "Couldn't connect to session bus (error \"%s\")\n",
                error.message);
        return 1;
    }

    atexit(cleanup);

    request = dbus_message_new_method_call("de.berlios.xkbswitch.XkbSwitch",
                                           "/XkbSwitch",
                                           "de.berlios.xkbswitch.XkbSwitch",
                                           "activate_layout");
    if (request == NULL ||
        dbus_message_append_args(request,
                                 DBUS_TYPE_STRING,
                                 &argv[1],
                                 DBUS_TYPE_INVALID) != TRUE)
    {
        fprintf(stderr, "Couldn't construct DBus request\n");
        return 1;
    }

    reply = dbus_connection_send_with_reply_and_block(connection,
                                                      request,
                                                      -1,
                                                      &error);
    if (reply == NULL) {
        fprintf(stderr,
                "Couldn't call method (error \"%s\")\n",
                error.message);
        return 1;
    }

    if (!dbus_message_get_args(reply,
                               &error,
                               DBUS_TYPE_BOOLEAN,
                               &ok,
                               DBUS_TYPE_INVALID))
    {
        fprintf(stderr,
                "Couldn't parse reply (error \"%s\")\n",
                error.message);
        return 1;
    }

    if (!ok) {
        fprintf(stderr, "Couldn't activate layout\n");
        return 1;
    }

    return 0;
}
