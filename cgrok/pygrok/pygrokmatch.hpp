#include <boost/xpressive/xpressive.hpp>

#include "../grokmatch.hpp"

#include <Python.h>

typedef struct {
    PyObject_HEAD
} pyGrokMatch;

static PyTypeObject pyGrokMatchType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pygrok.GrokMatch",             /*tp_name*/
    sizeof(pyGrokMatch), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "GrokMatch objects",            /* tp_doc */
};

