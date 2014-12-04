/**
 * @author Tiago de Freitas Pereira <tiago.pereira@idiap.ch>
 * @date Wed 03 Dec 14:38:48 2014
 *
 * @brief Python API for bob::learn::em
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "main.h"

/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static auto GMMStats_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".GMMStats",
  "A container for GMM statistics.\n"
  "With respect to Reynolds, \"Speaker Verification Using Adapted "
  "Gaussian Mixture Models\", DSP, 2000:\n"
  "Eq (8) is n(i)\n"
  "Eq (9) is sumPx(i) / n(i)\n"
  "Eq (10) is sumPxx(i) / n(i)\n"
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "A container for GMM statistics.",
    "",
    true
  )
  .add_prototype("n_gaussians,n_inputs","")
  .add_prototype("other")
  .add_prototype("hdf5")
  .add_prototype("")

  //.add_parameter("mean", "array_like<double, 1D>", "Mean of the Gaussian")
);


static int PyBobLearnMiscGMMStats_init_number(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = GMMStats_doc.kwlist(0);
  size_t n_inputs=1;
  size_t n_gaussians=1;
  //Parsing the input argments
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "II", kwlist, &n_gaussians, &n_inputs))
    return -1;

  self->cxx.reset(new bob::learn::misc::GMMStats(n_gaussians, n_inputs));
  return 0;
}


static int PyBobLearnMiscGMMStats_init_copy(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = GMMStats_doc.kwlist(1);
  PyBobLearnMiscGMMStatsObject* tt;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscGMMStats_Type, &tt)) return -1;

  self->cxx.reset(new bob::learn::misc::GMMStats(*tt->cxx));
  return 0;
}


static int PyBobLearnMiscGMMStats_init_hdf5(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = GMMStats_doc.kwlist(2);

  PyObject* config = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobIoHDF5File_Type, &config)) 
    return -1;
  auto h5f = reinterpret_cast<PyBobIoHDF5FileObject*>(config);

  try {
    self->cxx.reset(new bob::learn::misc::GMMStats(*(h5f->f)));
  }
  catch (std::exception& ex) {
    PyErr_SetString(PyExc_RuntimeError, ex.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot create new object of type `%s' - unknown exception thrown", Py_TYPE(self)->tp_name);
    return -1;
  }

  return 0;
}



static int PyBobLearnMiscGMMStats_init(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // get the number of command line arguments
  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  switch (nargs) {

    case 0: //default initializer ()
      self->cxx.reset(new bob::learn::misc::GMMStats());

    case 1:{
	
      //Reading the input argument
      PyObject* arg = 0;
      if (PyTuple_Size(args))
        arg = PyTuple_GET_ITEM(args, 0);
      else {
        PyObject* tmp = PyDict_Values(kwargs);
        auto tmp_ = make_safe(tmp);
        arg = PyList_GET_ITEM(tmp, 0);
      }
	
      /**If the constructor input is Gaussian object**/	
	  if (PyBobLearnMiscGMMStats_Check(arg))
	    return PyBobLearnMiscGMMStats_init_copy(self, args, kwargs);
	  /**If the constructor input is a HDF5**/
	  else if (PyBobIoHDF5File_Check(arg))
	    return PyBobLearnMiscGMMStats_init_hdf5(self, args, kwargs);	
    }

    case 2:
      return PyBobLearnMiscGMMStats_init_number(self, args, kwargs);
    
    default:

      PyErr_Format(PyExc_RuntimeError, "number of arguments mismatch - %s requires 0, 1 or 2 arguments, but you provided %" PY_FORMAT_SIZE_T "d (see help)", Py_TYPE(self)->tp_name, nargs);
  }

  return -1;
  BOB_CATCH_MEMBER("cannot create GMMStats", 0)

}



static void PyBobLearnMiscGMMStats_delete(PyBobLearnMiscGMMStatsObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PyBobLearnMiscGMMStats_RichCompare(PyBobLearnMiscGMMStatsObject* self, PyObject* other, int op) {
  BOB_TRY

  if (!PyBobLearnMiscGMMStats_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobLearnMiscGMMStatsObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  BOB_CATCH_MEMBER("cannot compare GMMStats objects", 0)
}

int PyBobLearnMiscGMMStats_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnMiscGMMStats_Type));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

/***** n *****/
static auto n = bob::extension::VariableDoc(
  "n",
  "array_like <double, 1D>",
  "For each Gaussian, the accumulated sum of responsibilities, i.e. the sum of P(gaussian_i|x)"
);
PyObject* PyBobLearnMiscGMMStats_getN(PyBobLearnMiscGMMStatsObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->n);
  BOB_CATCH_MEMBER("n could not be read", 0)
}
int PyBobLearnMiscGMMStats_setN(PyBobLearnMiscGMMStatsObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 1D array of floats", Py_TYPE(self)->tp_name, n.name());
    return -1;
  }
  auto b = PyBlitzArrayCxx_AsBlitz<double,1>(o, "n");
  if (!b) return -1;
  self->cxx->n = *b;
  return 0;
  BOB_CATCH_MEMBER("n could not be set", -1)  
}


/***** sum_px *****/
static auto sum_px = bob::extension::VariableDoc(
  "sum_px",
  "array_like <double, 2D>",
  "For each Gaussian, the accumulated sum of responsibility times the sample"
);
PyObject* PyBobLearnMiscGMMStats_getSum_px(PyBobLearnMiscGMMStatsObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->sumPx);
  BOB_CATCH_MEMBER("sum_px could not be read", 0)
}
int PyBobLearnMiscGMMStats_setSum_px(PyBobLearnMiscGMMStatsObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, sum_px.name());
    return -1;
  }
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "sum_px");
  if (!b) return -1;
  self->cxx->sumPx = *b;
  return 0;
  BOB_CATCH_MEMBER("sum_px could not be set", -1)  
}


/***** sum_pxx *****/
static auto sum_pxx = bob::extension::VariableDoc(
  "sum_pxx",
  "array_like <double, 2D>",
  "For each Gaussian, the accumulated sum of responsibility times the sample squared"
);
PyObject* PyBobLearnMiscGMMStats_getSum_pxx(PyBobLearnMiscGMMStatsObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->sumPxx);
  BOB_CATCH_MEMBER("sum_pxx could not be read", 0)
}
int PyBobLearnMiscGMMStats_setSum_pxx(PyBobLearnMiscGMMStatsObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, sum_pxx.name());
    return -1;
  }
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "sum_pxx");
  if (!b) return -1;
  self->cxx->sumPxx = *b;
  return 0;
  BOB_CATCH_MEMBER("sum_pxx could not be set", -1)  
}


/***** t *****/
static auto t = bob::extension::VariableDoc(
  "t",
  "size_t",
  "The accumulated log likelihood of all samples"
);
PyObject* PyBobLearnMiscGMMStats_getT(PyBobLearnMiscGMMStatsObject* self, void*){
  BOB_TRY
  return Py_BuildValue("n", self->cxx->T);
  BOB_CATCH_MEMBER("t could not be read", 0)
}
int PyBobLearnMiscGMMStats_setT(PyBobLearnMiscGMMStatsObject* self, PyObject* value, void*){
  BOB_TRY

  if (!PyInt_Check(value)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects an size_t", Py_TYPE(self)->tp_name, t.name());
    return -1;
  }

  if (PyInt_AsSsize_t(value) < 0){
    PyErr_Format(PyExc_TypeError, "t must be greater than or equal to zero");
	return -1;
  }


  self->cxx->T = PyInt_AsSsize_t(value);
  return 0;
  BOB_CATCH_MEMBER("t could not be set", -1)
}


/***** log_likelihood *****/
static auto log_likelihood = bob::extension::VariableDoc(
  "log_likelihood",
  "double",
  "The accumulated log likelihood of all samples"
);
PyObject* PyBobLearnMiscGMMStats_getLog_likelihood(PyBobLearnMiscGMMStatsObject* self, void*){
  BOB_TRY
  return Py_BuildValue("d","log_likelihood", self->cxx->log_likelihood);
  BOB_CATCH_MEMBER("log_likelihood could not be read", 0)
}
int PyBobLearnMiscGMMStats_setLog_likelihood(PyBobLearnMiscGMMStatsObject* self, PyObject* value, void*){
  BOB_TRY

  if (!PyNumber_Check(value)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects an double", Py_TYPE(self)->tp_name, t.name());
    return -1;
  }

  self->cxx->log_likelihood = PyFloat_AsDouble(value);
  return 0;
  BOB_CATCH_MEMBER("log_likelihood could not be set", -1)
}


static PyGetSetDef PyBobLearnMiscGMMStats_getseters[] = {
  {
    n.name(),
    (getter)PyBobLearnMiscGMMStats_getN,
    (setter)PyBobLearnMiscGMMStats_setN,
    n.doc(),
    0
  },
  {
    sum_px.name(),
    (getter)PyBobLearnMiscGMMStats_getSum_px,
    (setter)PyBobLearnMiscGMMStats_setSum_px,
    sum_px.doc(),
    0
  },
  {
    sum_pxx.name(),
    (getter)PyBobLearnMiscGMMStats_getSum_pxx,
    (setter)PyBobLearnMiscGMMStats_setSum_pxx,
    sum_pxx.doc(),
    0
  },
  {
    t.name(),
    (getter)PyBobLearnMiscGMMStats_getT,
    (setter)PyBobLearnMiscGMMStats_setT,
    t.doc(),
    0
  },
  {
    log_likelihood.name(),
    (getter)PyBobLearnMiscGMMStats_getLog_likelihood,
    (setter)PyBobLearnMiscGMMStats_setLog_likelihood,
    log_likelihood.doc(),
    0
  },

  {0}  // Sentinel
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/


/*** save ***/
static auto save = bob::extension::FunctionDoc(
  "save",
  " Save the configuration of the GMMStats to a given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
;
static PyObject* PyBobLearnMiscGMMStats_Save(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {
  // get list of arguments
  char** kwlist = save.kwlist(0);
  PyBobIoHDF5FileObject* hdf5 = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &hdf5)){
    save.print_usage();
    return NULL;
  }
  auto hdf5_ = make_safe(hdf5);

  try {
    self->cxx->save(*hdf5->f);
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "`%s' cannot write data to file `%s' (at group `%s'): unknown exception caught", Py_TYPE(self)->tp_name,
        hdf5->f->filename().c_str(), hdf5->f->cwd().c_str());
    return 0;
  }

  Py_RETURN_NONE;
}

/*** load ***/
static auto load = bob::extension::FunctionDoc(
  "load",
  " Load the configuration of the GMMStats to a given HDF5 file",
  0,
  true
);
static PyObject* PyBobLearnMiscGMMStats_Load(PyBobLearnMiscGMMStatsObject* self, PyObject* f) {

  if (!PyBobIoHDF5File_Check(f)) {
    PyErr_Format(PyExc_TypeError, "`%s' cannot load itself from `%s', only from an HDF5 file", Py_TYPE(self)->tp_name, Py_TYPE(f)->tp_name);
    return 0;
  }

  auto h5f = reinterpret_cast<PyBobIoHDF5FileObject*>(f);

  try {
    self->cxx->load(*h5f->f);
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot read data from file `%s' (at group `%s'): unknown exception caught", h5f->f->filename().c_str(),
        h5f->f->cwd().c_str());
    return 0;
  }
  Py_RETURN_NONE;
}


/*** is_similar_to ***/
static auto is_similar_to = bob::extension::FunctionDoc(
  "is_similar_to",
  
  "Compares this GMMStats with the ``other`` one to be\n\
  approximately the same.\n\
  \n\
  The optional values ``r_epsilon`` and ``a_epsilon`` refer to the\n\
  relative and absolute precision for the ``weights``, ``biases``\n\
  and any other values internal to this machine.\n\
  \n\
  ",

  0,
  true
)
.add_prototype("other, [r_epsilon], [a_epsilon]","bool")
.add_parameter("other", ":py:class:`bob.learn.misc.GMMStats`", "A gaussian to be compared.")
.add_parameter("[r_epsilon]", "float", "Relative precision.")
.add_parameter("[a_epsilon]", "float", "Absolute precision.")
.add_return("bool","","");
static PyObject* PyBobLearnMiscGMMStats_IsSimilarTo(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"other", "r_epsilon", "a_epsilon", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* other = 0;
  double r_epsilon = 1.e-5;
  double a_epsilon = 1.e-8;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!|dd", kwlist,
        &PyBobLearnMiscGMMStats_Type, &other,
        &r_epsilon, &a_epsilon)) return 0;

  auto other_ = reinterpret_cast<PyBobLearnMiscGMMStatsObject*>(other);

  if (self->cxx->is_similar_to(*other_->cxx, r_epsilon, a_epsilon))
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}


/*** resize ***/
static auto resize = bob::extension::FunctionDoc(
  "resize",
  "(size_t,size_t)"
  " Allocates space for the statistics and resets to zero."
)
.add_prototype("n_gaussians,n_inputs")
.add_parameter("n_gaussians,n_inputs", "(size_t, size_t)", "Tuple with the new shape");
static PyObject* PyBobLearnMiscGMMStats_resize(PyBobLearnMiscGMMStatsObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"n_gaussians", "n_inputs", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  Py_ssize_t n_gaussians = 0;
  Py_ssize_t n_inputs = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "nn", kwlist, &n_gaussians, &n_inputs)) Py_RETURN_NONE;
  self->cxx->resize(n_gaussians, n_inputs);

  BOB_CATCH_MEMBER("cannot perform the resize method", 0)

  Py_RETURN_NONE;
}


/*** init ***/
static auto init = bob::extension::FunctionDoc(
  "init",
  ""
  " Resets statistics to zero."
);
static PyObject* PyBobLearnMiscGMMStats_init_method(PyBobLearnMiscGMMStatsObject* self) {
  BOB_TRY

  self->cxx->init();

  BOB_CATCH_MEMBER("cannot perform the init method", 0)

  Py_RETURN_NONE;
}



static PyMethodDef PyBobLearnMiscGMMStats_methods[] = {
  {
    save.name(),
    (PyCFunction)PyBobLearnMiscGMMStats_Save,
    METH_VARARGS|METH_KEYWORDS,
    save.doc()
  },
  {
    load.name(),
    (PyCFunction)PyBobLearnMiscGMMStats_Load,
    METH_VARARGS|METH_KEYWORDS,
    load.doc()
  },
  {
    is_similar_to.name(),
    (PyCFunction)PyBobLearnMiscGMMStats_IsSimilarTo,
    METH_VARARGS|METH_KEYWORDS,
    is_similar_to.doc()
  },
  {
    resize.name(),
    (PyCFunction)PyBobLearnMiscGMMStats_resize,
    METH_VARARGS|METH_KEYWORDS,
    resize.doc()
  },
  {
    init.name(),
    (PyCFunction)PyBobLearnMiscGMMStats_init_method,
    METH_VARARGS|METH_KEYWORDS,
    init.doc()
  },

  {0} /* Sentinel */
};


/******************************************************************/
/************ Operators *******************************************/
/******************************************************************/

static PyObject* PyBobLearnMiscGMMStats_inplaceadd(PyObject* self, PyObject* other) {
  BOB_TRY

  if (!PyBobLearnMiscGMMStats_Check(other)){
    PyErr_Format(PyExc_TypeError, "expected bob.learn.misc.GMMStats object");
    return 0;
  }

  auto self_ = reinterpret_cast<PyBobLearnMiscGMMStatsObject*>(self);
  auto other_ = reinterpret_cast<PyBobLearnMiscGMMStatsObject*>(other);

  self_->cxx->operator+=(*other_->cxx);

  BOB_CATCH_MEMBER("it was not possible to process the operator +=", 0)

  Py_INCREF(self);
  return self;
}

static PyNumberMethods PyBobLearnMiscGMMStats_operators[] = {{
	     0, // nb_add;
	     0, // nb_subtract;
	     0, // nb_multiply;
	     0, // nb_divide;
	     0, // nb_remainder;
	     0, // nb_divmod;
	     0, // nb_power;
	     0, // nb_negative;
	     0, // nb_positive;
	     0, // nb_absolute;
	     0, // nb_nonzero;       /* Used by PyObject_IsTrue */
	     0, // nb_invert;
	     0, // nb_lshift;
	     0, // nb_rshift;
	     0, // nb_and;
	     0, // nb_xor;
	     0, // nb_or;
	     0, // nb_coerce;       /* Used by the coerce() function */
	     0, // nb_int;
	     0, // nb_long;
	     0, // nb_float;
	     0, // nb_oct;
	     0, // nb_hex;

	     /* Added in release 2.0 */
	     PyBobLearnMiscGMMStats_inplaceadd, // nb_inplace_add;
	     0, // nb_inplace_subtract;
	     0, // nb_inplace_multiply;
	     0, // nb_inplace_divide;
	     0, // nb_inplace_remainder;
	     0, // nb_inplace_power;
	     0, // nb_inplace_lshift;
	     0, // nb_inplace_rshift;
	     0, // nb_inplace_and;
	     0, // nb_inplace_xor;
	     0, // nb_inplace_or;

	     /* Added in release 2.2 */
	     0, // nb_floor_divide;
	     0, // nb_true_divide;
	     0, // nb_inplace_floor_divide;
	     0, // nb_inplace_true_divide;

	     /* Added in release 2.5 */
	     0, // nb_index
}};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gaussian type struct; will be initialized later
PyTypeObject PyBobLearnMiscGMMStats_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobLearnMiscGMMStats(PyObject* module)
{
  // initialize the type struct
  PyBobLearnMiscGMMStats_Type.tp_name = GMMStats_doc.name();
  PyBobLearnMiscGMMStats_Type.tp_basicsize = sizeof(PyBobLearnMiscGMMStatsObject);
  PyBobLearnMiscGMMStats_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobLearnMiscGMMStats_Type.tp_doc = GMMStats_doc.doc();

  // set the functions
  PyBobLearnMiscGMMStats_Type.tp_new = PyType_GenericNew;
  PyBobLearnMiscGMMStats_Type.tp_init = reinterpret_cast<initproc>(PyBobLearnMiscGMMStats_init);
  PyBobLearnMiscGMMStats_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobLearnMiscGMMStats_delete);
  PyBobLearnMiscGMMStats_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobLearnMiscGMMStats_RichCompare);
  PyBobLearnMiscGMMStats_Type.tp_methods = PyBobLearnMiscGMMStats_methods;
  PyBobLearnMiscGMMStats_Type.tp_getset = PyBobLearnMiscGMMStats_getseters;
  //PyBobLearnMiscGMMStats_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobLearnMiscGMMStats_loglikelihood);
  PyBobLearnMiscGMMStats_Type.tp_as_number = PyBobLearnMiscGMMStats_operators;

  // check that everything is fine
  if (PyType_Ready(&PyBobLearnMiscGMMStats_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobLearnMiscGMMStats_Type);
  return PyModule_AddObject(module, "GMMStats", (PyObject*)&PyBobLearnMiscGMMStats_Type) >= 0;
}
