#ifndef QPPCAD_PYTHON_GEOM_VIEW
#define QPPCAD_PYTHON_GEOM_VIEW
#include <qppcad/ws_item/arrow_array/python_arrow_array.hpp>

using namespace qpp;
using namespace qpp::cad;

void py_arrow_array_view_reg_helper_t::reg(py::module &module,
     py::class_<ws_item_t, std::shared_ptr<ws_item_t>, py_ws_item_t> &ws_item_base)
{
  py::class_<arrow_array_view_t, std::shared_ptr<arrow_array_view_t> >
  py_arav_t(module, "arrow_array_view_t", ws_item_base);
  py_arav_t.def_readwrite("atom_vectors", &arrow_array_view_t::m_binded_vectors);
			  
}
  

#endif
