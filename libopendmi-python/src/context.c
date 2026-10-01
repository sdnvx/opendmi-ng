//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <Python.h>

typedef struct
{
    PyObject_HEAD
} Context;

static void Context_dealloc(Context *self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject *Context_new(PyTypeObject *type, PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwds))
{
    Context *self;

    self = (Context *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static int Context_init(Context *Py_UNUSED(self), PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwds))
{
    return 0;
}

static PyMemberDef Context_members[] =
{
    { nullptr, 0, 0, 0, nullptr }
};

static PyMethodDef Context_methods[] =
{
    { nullptr, nullptr, 0, nullptr }
};

PyTypeObject Context_type = {
    .ob_base           = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name           = "opendmi.Context",
    .tp_doc            = PyDoc_STR("OpenDMI context"),
    .tp_basicsize      = sizeof(Context),
    .tp_itemsize       = 0,
    // The context holds no references to Python objects, so it does not
    // participate in garbage collection
    .tp_flags          = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new            = (newfunc)Context_new,
    .tp_init           = (initproc)Context_init,
    .tp_dealloc        = (destructor)Context_dealloc,
    .tp_members        = Context_members,
    .tp_methods        = Context_methods
};
