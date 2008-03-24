#include <boost/xpressive/xpressive.hpp>

#include "../grokpatternset.hpp"
#include "../grokregex.hpp"
#include "../grokmatch.hpp"
#include "../grokconfig.hpp"

#include <Python.h>

#include "pygrokregex.hpp"
#include "pygrokmatch.hpp"

static PyMethodDef pygrok_methods[] = {
  {NULL},
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpygrok(void) 
{
    PyObject* m;

    if (PyType_Ready(&pyGrokRegexType) < 0)
        return;
    if (PyType_Ready(&pyGrokMatchType) < 0)
        return;

    m = Py_InitModule3("pygrok", pygrok_methods,
                       "Python C++Grok interface");

    Py_INCREF(&pyGrokRegexType);
    PyModule_AddObject(m, "GrokRegex", (PyObject *)&pyGrokRegexType);

    Py_INCREF(&pyGrokMatchType);
    PyModule_AddObject(m, "GrokMatch", (PyObject *)&pyGrokMatchType);
}
