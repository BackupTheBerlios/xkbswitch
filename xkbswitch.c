
#include <stdio.h>

#include <dbus/dbus.h>

static DBusConnection* connection;
static DBusError       error = DBUS_ERROR_INIT;
static DBusMessage*    reply;
static DBusMessage*    request;

static void
cleanup (void)
{
    if (reply != NULL) {
        dbus_message_unref (reply);
        reply = NULL;
    }

    if (request != NULL) {
        dbus_message_unref (request);
        request = NULL;
    }

    if (connection != NULL) {
        dbus_connection_unref (connection);
        connection = NULL;
    }

    dbus_error_free (&error);
}

int
main (int argc, char** argv)
{
    (void) argc;

    if (argv [1] == NULL) {
        fprintf (stderr, "Usage: %s layout\n", argv [0]);
        return 1;
    }

    connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
        fprintf (stderr, "Could not connect to session bus: %s\n",
            error.message);
        cleanup ();
        return 1;
    }

    request = dbus_message_new_method_call ("de.berlios.xkbswitch.XkbSwitch",
        "/de/berlios/xkbswitch/XkbSwitch", "de.berlios.xkbswitch.XkbSwitch",
        "activate_layout");
    if (request == NULL) {
        fprintf (stderr, "Could not create request\n");
        cleanup ();
        return 1;
    }

    if (! dbus_message_append_args (request, DBUS_TYPE_STRING, &argv [1],
        DBUS_TYPE_INVALID))
    {
        fprintf (stderr, "Could not set request arguments\n");
        cleanup ();
        return 1;
    }

    reply = dbus_connection_send_with_reply_and_block (connection, request, -1,
        &error);
    if (reply == NULL) {
        fprintf (stderr, "Could not call method: %s\n", error.message);
        cleanup ();
        return 1;
    }

    dbus_bool_t success;

    if (! dbus_message_get_args (reply, &error, DBUS_TYPE_BOOLEAN, &success,
        DBUS_TYPE_INVALID))
    {
        fprintf (stderr, "Could not parse reply arguments: %s\n",
            error.message);
        cleanup ();
        return 1;
    }

    cleanup ();

    if (! success) {
        fprintf (stderr, "Layout switch failed\n");
        return 1;
    }

    return 0;
}
