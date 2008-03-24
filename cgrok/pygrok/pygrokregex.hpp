#include <boost/xpressive/xpressive.hpp>
#include "../grokregex.hpp"

#include <Python.h>
#include <structmember.h>

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

static PyObject* pyGrokRegex_search(pyGrokRegex *self, PyObject *args) {
  PyObject* search_pystr = NULL;
  char *search_pchar = NULL;

  if (!PyArg_UnpackTuple(args, "search", 1, 1, &search_pystr))
    return NULL;

  search_pchar = PyString_AsString(search_pystr);
  self->gre->SetRegex((const char *)search_pchar);

  Py_RETURN_NONE;
}

static PyObject* pyGrokRegex_set_regex(pyGrokRegex *self, PyObject *args) {
  PyObject* regex_pystr = NULL;
  char *regex_pchar = NULL;

  if (!PyArg_UnpackTuple(args, "set_regex", 1, 1, &regex_pystr))
    return NULL;

  regex_pchar = PyString_AsString(regex_pystr);
  self->gre->SetRegex((const char *)regex_pchar);

  Py_RETURN_NONE;
}

static PyMethodDef pyGrokRegex_methods[] = {
  {"search", (PyCFunction)pyGrokRegex_search, METH_VARARGS},
  {"set_regex", (PyCFunction)pyGrokRegex_set_regex, METH_VARARGS},
  {NULL, NULL, 0, NULL},
};

static PyMemberDef pyGrokRegex_members[] = {
  {NULL},
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

