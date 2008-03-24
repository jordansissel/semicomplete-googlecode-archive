#include <boost/xpressive/xpressive.hpp>

#include "../grokregex.hpp"

#include <Python.h>

typedef struct {
  PyObject_HEAD
  GrokRegex<sregex> *gre;
} pyGrokRegex;

static PyObject *
pyGrokRegex_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  pyGrokRegex *self;

  self = (pyGrokRegex *)type->tp_alloc(type, 0);

  if (self != NULL) {
    self->gre = new GrokRegex<sregex>;
  }
  return (PyObject *)self;
}

static PyMethodDef pyGrokRegex_methods[] = {
  {NULL, NULL, 0, NULL},
};

static PyMemberDef pyGrokRegex_members[] = {
  {NULL, NULL, 0, NULL},
};

static PyTypeObject pyGrokRegexType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pygrok.GrokRegex",             /*tp_name*/
    sizeof(pyGrokRegex), /*tp_basicsize*/
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
    "GrokRegex objects",      /* tp_doc */
    0,                   /* tp_traverse */
    0,                   /* tp_clear */
    0,                   /* tp_richcompare */
    0,                   /* tp_weaklistoffset */
    0,                   /* tp_iter */
    0,                   /* tp_iternext */
    pyGrokRegex_methods,             /* tp_methods */
    pyGrokRegex_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,
    //(initproc)pyGrokRegex_init,      /* tp_init */
    0,                         /* tp_alloc */
    pyGrokRegex_new,                 /* tp_new */
};

