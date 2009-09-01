
#include <Python.h>

#include <libxklavier/xklavier.h>

static Display*         display;
static XklConfigRec*    config;
static XklEngine*       engine;

static PyObject*
finalize (PyObject* self, PyObject* args)
{
    (void) self;
    (void) args;

    g_object_unref (G_OBJECT (config));
    g_object_unref (G_OBJECT (engine));

    if (display != NULL) {
        XCloseDisplay (display);
        display = NULL;
    }

    Py_RETURN_NONE;
}

static PyObject*
initialize (PyObject* self, PyObject* args)
{
    (void) self;

    const char* display_name;

    if (! PyArg_ParseTuple (args, "z", &display_name)) {
        return NULL;
    }

    display = XOpenDisplay (display_name);
    if (display == NULL) {
        PyErr_SetString (PyExc_EnvironmentError, "Could not open X display");
        return NULL;
    }

    engine = xkl_engine_get_instance (display);
    if (engine == NULL) {
        XCloseDisplay (display);
        PyErr_SetString (PyExc_EnvironmentError, xkl_get_last_error ());
        return NULL;
    }

    config = xkl_config_rec_new ();

    Py_RETURN_NONE;
}

static PyObject*
get_layouts (PyObject* self, PyObject* args)
{
    (void) self;
    (void) args;

    PyObject*   list;
    guint       i;

    if (! xkl_config_rec_get_from_server (config, engine)) {
        PyErr_SetString (PyExc_EnvironmentError, xkl_get_last_error ());
        return NULL;
    }

    list = PyList_New (0);
    for (i = 0; i < g_strv_length (config->layouts); ++i) {
        if (config->variants [i] != NULL && *config->variants [i] != '\0') {
            /*
            gchar*  str;

            str = g_strconcat (config->layouts [i], "(", config->variants [i],
                ")", NULL);
            PyList_Append (list, Py_BuildValue ("s", str));
            g_free (str);
            */
            gchar   buf [strlen (config->layouts [i]) +
                strlen (config->variants [i]) + sizeof ("()")];

            snprintf (buf, sizeof (buf), "%s(%s)", config->layouts [i],
                config->variants [i]);
            PyList_Append (list, Py_BuildValue ("s", buf));
        } else {
            PyList_Append (list, Py_BuildValue ("s", config->layouts [i]));
        }
    }

    return list;
}

static PyObject*
get_max_num_groups (PyObject* self, PyObject* args)
{
    (void) self;
    (void) args;

    return Py_BuildValue ("i", xkl_engine_get_max_num_groups (engine));
}

static PyObject*
get_num_groups (PyObject* self, PyObject* args)
{
    (void) self;
    (void) args;

    return Py_BuildValue ("i", xkl_engine_get_num_groups (engine));
}

static PyObject*
set_group (PyObject* self, PyObject* args)
{
    (void) self;

    gint    group;

    if (! PyArg_ParseTuple (args, "i", &group)) {
        return NULL;
    }

    xkl_engine_lock_group (engine, group);

    Py_RETURN_NONE;
}

static PyObject*
set_layouts (PyObject* self, PyObject* args)
{
    (void) self;

    PyObject*   list;
    Py_ssize_t  list_sz;
    gchar**     old_layouts;
    gchar**     old_variants;
    int         i;

    if (! PyArg_ParseTuple (args, "O!", &PyList_Type, &list)) {
        return NULL;
    }

    list_sz = PyList_Size (list);
    if (list_sz < 1) {
        PyErr_SetString (PyExc_ValueError, "Empty layout list");
        return NULL;
    } else if ((guint) list_sz > xkl_engine_get_max_num_groups (engine)) {
        PyErr_SetString (PyExc_ValueError, "Layout list is too long");
        return NULL;
    }

    old_layouts      = config->layouts;
    old_variants     = config->variants;
    config->layouts  = g_new0 (char*, list_sz + 1);
    config->variants = g_new0 (char*, list_sz + 1);

    for (i = 0; i < list_sz; ++i) {
        config->layouts [i] =
            g_strdup (PyString_AsString (PyList_GetItem (list, i)));
        config->variants [i] =
            g_strdup ("");
    }

    if (! xkl_config_rec_activate (config, engine)) {
        g_strfreev (config->layouts);
        g_strfreev (config->variants);
        config->layouts  = old_layouts;
        config->variants = old_variants;

        PyErr_SetString (PyExc_EnvironmentError, xkl_get_last_error ());
        return NULL;
    } else {
        if (old_layouts != NULL) {
            g_strfreev (old_layouts);
        }
        if (old_variants != NULL) {
            g_strfreev (old_variants);
        }
    }

    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------

static PyMethodDef methods [] = {
    {   "finalize",             finalize,           METH_NOARGS,    ""      },
    {   "initialize",           initialize,         METH_VARARGS,   ""      },
    {   "get_layouts",          get_layouts,        METH_NOARGS,    ""      },
    {   "get_max_num_groups",   get_max_num_groups, METH_NOARGS,    ""      },
    {   "get_num_groups",       get_num_groups,     METH_NOARGS,    ""      },
    {   "set_group",            set_group,          METH_VARARGS,   ""      },
    {   "set_layouts",          set_layouts,        METH_VARARGS,   ""      },
    {   NULL,                   NULL,               0,              NULL    }
};

PyMODINIT_FUNC
initxkbswitch (void)
{
    PyObject*   module;

    module = Py_InitModule ("xkbswitch", methods);
    if (module != NULL) {
        g_type_init ();
    }
}
