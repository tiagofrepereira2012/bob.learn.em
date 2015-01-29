/**
 * @date Wed Jan 28 17:46:15 2015 +0200
 * @author Tiago de Freitas Pereira <tiago.pereira@idiap.ch>
 *
 * @brief Python API for bob::learn::em
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "main.h"

/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static auto IVectorMachine_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".IVectorMachine",
  "An IVectorMachine consists of a Total Variability subspace \f$T\f$ and allows the extraction of IVector"
  "References: [Dehak2010]",
  ""
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Constructor. Builds a new IVectorMachine",
    "",
    true
  )
  .add_prototype("ubm, rt, variance_threshold","")
  .add_prototype("other","")
  .add_prototype("hdf5","")

  .add_parameter("ubm", ":py:class:`bob.learn.misc.GMMMachine`", "The Universal Background Model.")
  .add_parameter("rt", "int", "Size of the Total Variability matrix (CD x rt).")
  .add_parameter("variance_threshold", "double", "Variance flooring threshold for the Sigma (diagonal) matrix")

  .add_parameter("other", ":py:class:`bob.learn.misc.IVectorMachine`", "A IVectorMachine object to be copied.")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading")

);


static int PyBobLearnMiscIVectorMachine_init_copy(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = IVectorMachine_doc.kwlist(1);
  PyBobLearnMiscIVectorMachineObject* o;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscIVectorMachine_Type, &o)){
    IVectorMachine_doc.print_usage();
    return -1;
  }

  self->cxx.reset(new bob::learn::misc::IVectorMachine(*o->cxx));
  return 0;
}


static int PyBobLearnMiscIVectorMachine_init_hdf5(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = IVectorMachine_doc.kwlist(2);

  PyBobIoHDF5FileObject* config = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, &PyBobIoHDF5File_Converter, &config)){
    IVectorMachine_doc.print_usage();
    return -1;
  }

  self->cxx.reset(new bob::learn::misc::IVectorMachine(*(config->f)));

  return 0;
}


static int PyBobLearnMiscIVectorMachine_init_ubm(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = IVectorMachine_doc.kwlist(0);
  
  PyBobLearnMiscGMMMachineObject* gmm_machine;
  int rt = 1;
  double variance_threshold = 1e-10;

  //Here we have to select which keyword argument to read  
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!i|d", kwlist, &PyBobLearnMiscGMMMachine_Type, &gmm_machine, &rt, &variance_threshold)){
    IVectorMachine_doc.print_usage();
    return -1;
  }
    
  if(rt < 1){
    PyErr_Format(PyExc_TypeError, "rt argument must be greater than or equal to one");
    return -1;
  }
  
  if(variance_threshold <= 0){
    PyErr_Format(PyExc_TypeError, "variance_threshold argument must be greater than zero");
    return -1;
  }

  self->cxx.reset(new bob::learn::misc::IVectorMachine(gmm_machine->cxx, rt, variance_threshold));
  return 0;
}


static int PyBobLearnMiscIVectorMachine_init(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // get the number of command line arguments
  int nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  if(nargs == 1){
    //Reading the input argument
    PyObject* arg = 0;
    if (PyTuple_Size(args))
      arg = PyTuple_GET_ITEM(args, 0);
    else {
      PyObject* tmp = PyDict_Values(kwargs);
      auto tmp_ = make_safe(tmp);
      arg = PyList_GET_ITEM(tmp, 0);
    }

    // If the constructor input is Gaussian object
    if (PyBobLearnMiscIVectorMachine_Check(arg))
      return PyBobLearnMiscIVectorMachine_init_copy(self, args, kwargs);
    // If the constructor input is a HDF5
    else
      return PyBobLearnMiscIVectorMachine_init_hdf5(self, args, kwargs);
  }
  else if ((nargs == 2) || (nargs == 3))
    PyBobLearnMiscIVectorMachine_init_ubm(self, args, kwargs);
  else{
    PyErr_Format(PyExc_RuntimeError, "number of arguments mismatch - %s requires 1,2 or 3 argument, but you provided %d (see help)", Py_TYPE(self)->tp_name, nargs);
    IVectorMachine_doc.print_usage();
    return -1;
  }
  
  BOB_CATCH_MEMBER("cannot create IVectorMachine", 0)
  return 0;
}

static void PyBobLearnMiscIVectorMachine_delete(PyBobLearnMiscIVectorMachineObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PyBobLearnMiscIVectorMachine_RichCompare(PyBobLearnMiscIVectorMachineObject* self, PyObject* other, int op) {
  BOB_TRY

  if (!PyBobLearnMiscIVectorMachine_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobLearnMiscIVectorMachineObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  BOB_CATCH_MEMBER("cannot compare IVectorMachine objects", 0)
}

int PyBobLearnMiscIVectorMachine_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnMiscIVectorMachine_Type));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

/***** shape *****/
static auto shape = bob::extension::VariableDoc(
  "shape",
  "(int,int, int)",
  "A tuple that represents the number of gaussians, dimensionality of each Gaussian, dimensionality of the rT (total variability matrix) ``(#Gaussians, #Inputs, #rT)``.",
  ""
);
PyObject* PyBobLearnMiscIVectorMachine_getShape(PyBobLearnMiscIVectorMachineObject* self, void*) {
  BOB_TRY
  return Py_BuildValue("(i,i,i)", self->cxx->getNGaussians(), self->cxx->getNInputs(), self->cxx->getDimRt());
  BOB_CATCH_MEMBER("shape could not be read", 0)
}

/***** supervector_length *****/
static auto supervector_length = bob::extension::VariableDoc(
  "supervector_length",
  "int",

  "Returns the supervector length."
  "NGaussians x NInputs: Number of Gaussian components by the feature dimensionality",
  
  "@warning An exception is thrown if no Universal Background Model has been set yet."
);
PyObject* PyBobLearnMiscIVectorMachine_getSupervectorLength(PyBobLearnMiscIVectorMachineObject* self, void*) {
  BOB_TRY
  return Py_BuildValue("i", self->cxx->getSupervectorLength());
  BOB_CATCH_MEMBER("supervector_length could not be read", 0)
}


/***** T *****/
static auto T = bob::extension::VariableDoc(
  "t",
  "array_like <float, 2D>",
  "Returns the Total Variability matrix",
  ""
);
PyObject* PyBobLearnMiscIVectorMachine_getT(PyBobLearnMiscIVectorMachineObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getT());
  BOB_CATCH_MEMBER("`t` could not be read", 0)
}
int PyBobLearnMiscIVectorMachine_setT(PyBobLearnMiscIVectorMachineObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, T.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "t");
  if (!b) return -1;
  self->cxx->setT(*b);
  return 0;
  BOB_CATCH_MEMBER("`t` vector could not be set", -1)
}


/***** sigma *****/
static auto sigma = bob::extension::VariableDoc(
  "sigma",
  "array_like <float, 1D>",
  "The residual matrix of the model sigma",
  ""
);
PyObject* PyBobLearnMiscIVectorMachine_getSigma(PyBobLearnMiscIVectorMachineObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getSigma());
  BOB_CATCH_MEMBER("`sigma` could not be read", 0)
}
int PyBobLearnMiscIVectorMachine_setSigma(PyBobLearnMiscIVectorMachineObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 1D array of floats", Py_TYPE(self)->tp_name, sigma.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,1>(o, "sigma");
  if (!b) return -1;
  self->cxx->setSigma(*b);
  return 0;
  BOB_CATCH_MEMBER("`sigma` vector could not be set", -1)
}


/***** variance_threshold *****/
static auto variance_threshold = bob::extension::VariableDoc(
  "variance_threshold",
  "double",
  "Threshold for the variance contained in sigma",
  ""
);
PyObject* PyBobLearnMiscIVectorMachine_getVarianceThreshold(PyBobLearnMiscIVectorMachineObject* self, void*) {
  BOB_TRY
  return Py_BuildValue("d", self->cxx->getVarianceThreshold());
  BOB_CATCH_MEMBER("variance_threshold could not be read", 0)
}
int PyBobLearnMiscIVectorMachine_setVarianceThreshold(PyBobLearnMiscIVectorMachineObject* self, PyObject* value, void*){
  BOB_TRY

  if (!PyNumber_Check(value)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects an double", Py_TYPE(self)->tp_name, variance_threshold.name());
    return -1;
  }

  if (PyFloat_AS_DOUBLE(value) < 0){
    PyErr_Format(PyExc_TypeError, "variance_threshold must be greater than or equal to zero");
    return -1;
  }

  self->cxx->setVarianceThreshold(PyFloat_AS_DOUBLE(value));
  BOB_CATCH_MEMBER("variance_threshold could not be set", -1)
  return 0;
}



static PyGetSetDef PyBobLearnMiscIVectorMachine_getseters[] = { 
  {
   shape.name(),
   (getter)PyBobLearnMiscIVectorMachine_getShape,
   0,
   shape.doc(),
   0
  },
  
  {
   supervector_length.name(),
   (getter)PyBobLearnMiscIVectorMachine_getSupervectorLength,
   0,
   supervector_length.doc(),
   0
  },
  
  {
   T.name(),
   (getter)PyBobLearnMiscIVectorMachine_getT,
   (setter)PyBobLearnMiscIVectorMachine_setT,
   T.doc(),
   0
  },

  {
   variance_threshold.name(),
   (getter)PyBobLearnMiscIVectorMachine_getVarianceThreshold,
   (setter)PyBobLearnMiscIVectorMachine_setVarianceThreshold,
   variance_threshold.doc(),
   0
  },

  {
   sigma.name(),
   (getter)PyBobLearnMiscIVectorMachine_getSigma,
   (setter)PyBobLearnMiscIVectorMachine_setSigma,
   sigma.doc(),
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
  "Save the configuration of the IVectorMachine to a given HDF5 file"
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing");
static PyObject* PyBobLearnMiscIVectorMachine_Save(PyBobLearnMiscIVectorMachineObject* self,  PyObject* args, PyObject* kwargs) {

  BOB_TRY
  
  // get list of arguments
  char** kwlist = save.kwlist(0);  
  PyBobIoHDF5FileObject* hdf5;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &hdf5)) return 0;

  auto hdf5_ = make_safe(hdf5);
  self->cxx->save(*hdf5->f);

  BOB_CATCH_MEMBER("cannot save the data", 0)
  Py_RETURN_NONE;
}

/*** load ***/
static auto load = bob::extension::FunctionDoc(
  "load",
  "Load the configuration of the IVectorMachine to a given HDF5 file"
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading");
static PyObject* PyBobLearnMiscIVectorMachine_Load(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY
  
  char** kwlist = load.kwlist(0);  
  PyBobIoHDF5FileObject* hdf5;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &hdf5)) return 0;
  
  auto hdf5_ = make_safe(hdf5);  
  self->cxx->load(*hdf5->f);

  BOB_CATCH_MEMBER("cannot load the data", 0)
  Py_RETURN_NONE;
}


/*** is_similar_to ***/
static auto is_similar_to = bob::extension::FunctionDoc(
  "is_similar_to",
  
  "Compares this IVectorMachine with the ``other`` one to be approximately the same.",
  "The optional values ``r_epsilon`` and ``a_epsilon`` refer to the "
  "relative and absolute precision for the ``weights``, ``biases`` "
  "and any other values internal to this machine."
)
.add_prototype("other, [r_epsilon], [a_epsilon]","output")
.add_parameter("other", ":py:class:`bob.learn.misc.IVectorMachine`", "A IVectorMachine object to be compared.")
.add_parameter("r_epsilon", "float", "Relative precision.")
.add_parameter("a_epsilon", "float", "Absolute precision.")
.add_return("output","bool","True if it is similar, otherwise false.");
static PyObject* PyBobLearnMiscIVectorMachine_IsSimilarTo(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  char** kwlist = is_similar_to.kwlist(0);

  //PyObject* other = 0;
  PyBobLearnMiscIVectorMachineObject* other = 0;
  double r_epsilon = 1.e-5;
  double a_epsilon = 1.e-8;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!|dd", kwlist,
        &PyBobLearnMiscIVectorMachine_Type, &other,
        &r_epsilon, &a_epsilon)){

        is_similar_to.print_usage(); 
        return 0;        
  }

  if (self->cxx->is_similar_to(*other->cxx, r_epsilon, a_epsilon))
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}



/*** forward ***/
static auto forward = bob::extension::FunctionDoc(
  "forward",
  "Execute the machine",
  "", 
  true
)
.add_prototype("stats")
.add_parameter("stats", ":py:class:`bob.learn.misc.GMMStats`", "Statistics as input");
static PyObject* PyBobLearnMiscIVectorMachine_Forward(PyBobLearnMiscIVectorMachineObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  char** kwlist = forward.kwlist(0);

  PyBobLearnMiscGMMStatsObject* stats = 0;
  
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscGMMStats_Type, &stats))
    Py_RETURN_NONE;

   blitz::Array<double,1> ivector(self->cxx->getDimRt());
   self->cxx->forward(*stats->cxx, ivector);

  return PyBlitzArrayCxx_AsConstNumpy(ivector);
  
  BOB_CATCH_MEMBER("cannot forward", 0)

}


static PyMethodDef PyBobLearnMiscIVectorMachine_methods[] = {
  {
    save.name(),
    (PyCFunction)PyBobLearnMiscIVectorMachine_Save,
    METH_VARARGS|METH_KEYWORDS,
    save.doc()
  },
  {
    load.name(),
    (PyCFunction)PyBobLearnMiscIVectorMachine_Load,
    METH_VARARGS|METH_KEYWORDS,
    load.doc()
  },
  {
    is_similar_to.name(),
    (PyCFunction)PyBobLearnMiscIVectorMachine_IsSimilarTo,
    METH_VARARGS|METH_KEYWORDS,
    is_similar_to.doc()
  },

/*
  {
    forward.name(),
    (PyCFunction)PyBobLearnMiscIVectorMachine_Forward,
    METH_VARARGS|METH_KEYWORDS,
    forward.doc()
  },*/


  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the JFA type struct; will be initialized later
PyTypeObject PyBobLearnMiscIVectorMachine_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobLearnMiscIVectorMachine(PyObject* module)
{
  // initialize the type struct
  PyBobLearnMiscIVectorMachine_Type.tp_name      = IVectorMachine_doc.name();
  PyBobLearnMiscIVectorMachine_Type.tp_basicsize = sizeof(PyBobLearnMiscIVectorMachineObject);
  PyBobLearnMiscIVectorMachine_Type.tp_flags     = Py_TPFLAGS_DEFAULT;
  PyBobLearnMiscIVectorMachine_Type.tp_doc       = IVectorMachine_doc.doc();

  // set the functions
  PyBobLearnMiscIVectorMachine_Type.tp_new         = PyType_GenericNew;
  PyBobLearnMiscIVectorMachine_Type.tp_init        = reinterpret_cast<initproc>(PyBobLearnMiscIVectorMachine_init);
  PyBobLearnMiscIVectorMachine_Type.tp_dealloc     = reinterpret_cast<destructor>(PyBobLearnMiscIVectorMachine_delete);
  PyBobLearnMiscIVectorMachine_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobLearnMiscIVectorMachine_RichCompare);
  PyBobLearnMiscIVectorMachine_Type.tp_methods     = PyBobLearnMiscIVectorMachine_methods;
  PyBobLearnMiscIVectorMachine_Type.tp_getset      = PyBobLearnMiscIVectorMachine_getseters;
  PyBobLearnMiscIVectorMachine_Type.tp_call        = reinterpret_cast<ternaryfunc>(PyBobLearnMiscIVectorMachine_Forward);


  // check that everything is fine
  if (PyType_Ready(&PyBobLearnMiscIVectorMachine_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobLearnMiscIVectorMachine_Type);
  return PyModule_AddObject(module, "IVectorMachine", (PyObject*)&PyBobLearnMiscIVectorMachine_Type) >= 0;
}

