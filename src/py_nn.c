//Compile with location /usr/include/python3.5m/Python.h
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "chess.h"

static int **weights_layers_conv;
static int **weights_layers_fc;
static int **biases_layers_conv;
static int **biases_layers_fc;
//5 dimensions
static double *weights_conv;
//3 dimensions
static double *weights_fc;
//2 dimensions
static double *biases_conv;
//2 dimensions
static double *biases_fc;

static PyObject *setup_py_objects(PyObject *self, PyObject *args) {
    int *layer, i, length, product, counter;
    Py_ssize_t i0, i1, i2, i3, i4, n0, n1, n2, n3, n4;
    PyObject *item;

    PyObject *nested_0, *nested_1, *nested_2, *nested_3;
    PyObject *py_weights_layers_conv;
    PyObject *py_weights_layers_fc;
    PyObject *py_biases_layers_conv;
    PyObject *py_biases_layers_fc;
    PyObject *py_weights_conv;
    PyObject *py_weights_fc;
    PyObject *py_biases_conv;
    PyObject *py_biases_fc;

    if (!PyArg_ParseTuple(args, "OOOOOOOO", &py_weights_layers_conv, &py_weights_layers_fc, &py_biases_layers_conv, &py_biases_layers_fc, &py_weights_conv, &py_weights_fc, &py_biases_conv, &py_biases_fc)) {
        return NULL;
    }

    //Populate the weights_layers_conv
    n0 = PyList_Size(py_weights_layers_conv);
    if (n0 < 0)
        return -1;
    weights_layers_conv = (int **)malloc(sizeof(int *) * (n0 + 1));
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_weights_layers_conv, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        weights_layers_conv[i0] = (int *)malloc(sizeof(int) * n1);
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            weights_layers_conv[i0][i1] = PyLong_AsLong(item);
            if (weights_layers_conv[i0][i1] = -1 && PyErr_Occurred())
                return -1;
        }
    }
    weights_layers_conv[n0] = NULL;

    //Populate the weights_layers_fc
    n0 = PyList_Size(py_weights_layers_fc);
    if (n0 < 0)
        return -1;
    weights_layers_fc = (int **)malloc(sizeof(int *) * (n0 + 1));
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_weights_layers_fc, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        weights_layers_fc[i0] = (int *)malloc(sizeof(int) * n1);
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            weights_layers_fc[i0][i1] = PyLong_AsLong(item);
            if (weights_layers_fc[i0][i1] = -1 && PyErr_Occurred())
                return -1;
        }
    }
    weights_layers_fc[n0] = NULL;

    //Populate the biases_layers_conv
    n0 = PyList_Size(py_biases_layers_conv);
    if (n0 < 0)
        return -1;
    biases_layers_conv = (int **)malloc(sizeof(int *) * (n0 + 1));
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_biases_layers_conv, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        biases_layers_conv[i0] = (int *)malloc(sizeof(int) * n1);
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            biases_layers_conv[i0][i1] = PyLong_AsLong(item);
            if (biases_layers_conv[i0][i1] = -1 && PyErr_Occurred())
                return -1;
        }
    }
    biases_layers_conv[n0] = NULL;

    //Populate the biases_layers_fc
    n0 = PyList_Size(py_biases_layers_fc);
    if (n0 < 0)
        return -1;
    biases_layers_fc = (int **)malloc(sizeof(int *) * (n0 + 1));
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_biases_layers_fc, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        biases_layers_fc[i0] = (int *)malloc(sizeof(int) * n1);
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            biases_layers_fc[i0][i1] = PyLong_AsLong(item);
            if (biases_layers_fc[i0][i1] = -1 && PyErr_Occurred())
                return -1;
        }
    }
    biases_layers_fc[n0] = NULL;

    //Populate the weights_conv
    n0 = PyList_Size(py_weights_conv);
    if (n0 < 0)
        return -1;
    //Find the length of the weights_conv array
    length = 0;
    for (layer = *weights_layers_conv; *layer; layer++) {
        product = 1;
        for (i = 0; i < 4; i++) {
            product *= layer[i];
        }
        length += product;
    }
    weights_conv = (double *)malloc(sizeof(double) * length);
    counter = 0;
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_weights_conv, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        for (i1 = 0; i1 < n1; i1++) {
            nested_1 = PyList_GetItem(nested_0, i1);
            n2 = PyList_Size(nested_1);
            if (n2 < 0)
                return -1;
            for (i2 = 0; i2 < n2; i2++) {
                nested_2 = PyList_GetItem(nested_1, i2);
                n3 = PyList_Size(nested_2);
                if (n3 < 0)
                    return -1;
                for (i3 = 0; i3 < n3; i3++) {
                    nested_3 = PyList_GetItem(nested_2, i3);
                    n4 = PyList_Size(nested_3);
                    if (n4 < 0)
                        return -1;
                    for (i4 = 0; i4 < n4; i4++) {
                        item = PyList_GetItem(nested_3, i4);
                        if (item == NULL)
                            return -1;
                        weights_conv[counter] = PyFloat_AsDouble(item);
                        if (weights_conv[counter] == -1.0 && PyErr_Occurred())
                            return -1;
                        counter++;
                    }
                }
            }
        }
    }

    //Populate the weights_fc
    n0 = PyList_Size(py_weights_fc);
    if (n0 < 0)
        return -1;
    //Find the length of the weights_fc array
    length = 0;
    for (layer = *weights_layers_fc; *layer; layer++) {
        product = 1;
        for (i = 0; i < 2; i++) {
            product *= layer[i];
        }
        length += product;
    }
    weights_fc = (double *)malloc(sizeof(double) * length);
    counter = 0;
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_weights_fc, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        for (i1 = 0; i1 < n1; i1++) {
            nested_1 = PyList_GetItem(nested_0, i1);
            n2 = PyList_Size(nested_1);
            if (n2 < 0)
                return -1;
            for (i2 = 0; i2 < n2; i2++) {
                item = PyList_GetItem(nested_1, i2);
                if (item == NULL)
                    return -1;
                weights_fc[counter] = PyFloat_AsDouble(item);
                if (weights_fc[counter] == -1.0 && PyErr_Occurred())
                    return -1;
                counter++;
            }
        }
    }

    //Populate the biases_conv
    n0 = PyList_Size(py_biases_conv);
    if (n0 < 0)
        return -1;
    //Find the length of the array
    length = 0;
    for (layer = *biases_layers_conv; *layer; layer++) {
        product = 1;
        for (i = 0; i < 1; i++) {
            product *= layer[i];
        }
        length += product;
    }
    biases_conv = (double *)malloc(sizeof(double) * length);
    counter = 0;
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_biases_conv, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            biases_conv[counter] = PyFloat_AsDouble(item);
            if (biases_conv[counter] == -1.0 && PyErr_Occurred())
                return -1;
            counter++;
        }
    }

    //Populate the biases_fc
    n0 = PyList_Size(py_biases_fc);
    if (n0 < 0)
        return -1;
    //Find the length of the array
    length = 0;
    for (layer = *biases_layers_fc; *layer; layer++) {
        product = 1;
        for (i = 0; i < 1; i++) {
            product *= layer[i];
        }
        length += product;
    }
    biases_fc = (double *)malloc(sizeof(double) * length);
    counter = 0;
    for (i0 = 0; i0 < n0; i0++) {
        nested_0 = PyList_GetItem(py_biases_fc, i0);
        n1 = PyList_Size(nested_0);
        if (n1 < 0)
            return -1;
        for (i1 = 0; i1 < n1; i1++) {
            item = PyList_GetItem(nested_0, i1);
            if (item == NULL)
                return -1;
            biases_fc[counter] = PyFloat_AsDouble(item);
            if (biases_fc[counter] == -1.0 && PyErr_Occurred())
                return -1;
            counter++;
        }
    }

    Py_INCREF(Py_None);
    return Py_None;

}

static PyObject *close_py_objects(PyObject *self, PyObject *args) {
    int *layer;

    //Now free the layers metadata
    for (layer = *weights_layers_conv; *layer; layer++) {
        free(layer);
    }
    free(weights_layers_conv);

    for (layer = *weights_layers_fc; *layer; layer++) {
        free(layer);
    }
    free(weights_layers_fc);

    for (layer = *biases_layers_conv; *layer; layer++) {
        free(layer);
    }
    free(biases_layers_conv);

    for (layer = *biases_layers_fc; *layer; layer++) {
        free(layer);
    }
    free(biases_layers_fc);

    free(weights_conv);
    free(weights_fc);
    free(biases_conv);
    free(biases_fc);

    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *py_fire(PyObject *self, PyObject *args) {
    char *fen;
    double eval;
    
    if (!PyArg_ParseTuple(args, "s", &fen)) {
        return NULL;
    }

    eval = fen_fire(fen, weights_layers_conv, weights_layers_fc, biases_layers_conv, biases_layers_fc, weights_conv, weights_fc, biases_conv, biases_fc);

    return Py_BuildValue("d", eval);

}

PyMethodDef PyNNMethods[] = {
    {"setup_py_objects", setup_py_objects, METH_VARARGS, "Initialize NN"},
    {"close_py_objects", close_py_objects, METH_VARARGS, "Free NN Memory"},
    {"py_fire", py_fire, METH_VARARGS, "Fire / Trigger NN"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef py_nnmodule = {
    PyModuleDef_HEAD_INIT,
    "py_nn",
    NULL,
    -1,
    PyNNMethods
};

PyMODINIT_FUNC PyInit_py_nn(void) {
    return PyModule_Create(&py_nnmodule);
};
