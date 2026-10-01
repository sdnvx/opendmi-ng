//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <Python.h>

#include <opendmi/python/context.h>

/**
 * @internal
 * @brief Initialize the module, registering the types it exports.
 *
 * @param[in] module Module being initialized.
 *
 * @return Zero on success, -1 with a Python exception set on failure.
 */
static int dmi_mod_exec(PyObject *module);

static PyMethodDef dmi_methods[] = {
     { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef_Slot dmi_slots[] = {
    { Py_mod_exec,                  (void *)dmi_mod_exec                       },
    { Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED },
    { 0, NULL}
};

static struct PyModuleDef dmi_module = {
    .m_base    = PyModuleDef_HEAD_INIT,
    .m_name    = "opendmi",
    .m_doc     = "OpenDMI module",
    .m_size    = 0,
    .m_methods = dmi_methods,
    .m_slots   = dmi_slots
};

PyMODINIT_FUNC PyInit_opendmi(void)
{
    return PyModuleDef_Init(&dmi_module);
}

static int dmi_mod_exec(PyObject *module)
{
    if (PyType_Ready(&Context_type) < 0)
        return -1;
    if (PyModule_AddObjectRef(module, "Context", (PyObject *)&Context_type) < 0)
        return -1;

    return 0;
}
