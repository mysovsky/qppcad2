#ifndef QPPCAD_PYTHON_ARROW_ARRAY_VIEW
#define QPPCAD_PYTHON_ARROW_ARRAY_VIEW

#pragma push_macro("slots")
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#pragma pop_macro("slots")

#include <qppcad/ws_item/geom_view/geom_view.hpp>
#include <qppcad/ws_item/geom_view/geom_view_labels_subsys.hpp>
#include <qppcad/ws_item/geom_view/geom_view_anim_subsys.hpp>
#include <qppcad/ws_item/geom_view/geom_view_sel_groups_subsys.hpp>
#include <qppcad/ws_item/ws_item_trampoline.hpp>
#include <qppcad/ws_item/arrow_array/arrow_array.hpp>

namespace py = pybind11;

namespace qpp {

  namespace cad {

    struct py_arrow_array_view_reg_helper_t
    {
      static void reg(
		      py::module &module,
		      py::class_<ws_item_t, std::shared_ptr<ws_item_t>, py_ws_item_t> &ws_item_base);
      
    };

  } // namespace cad

} // namespace qpp

#endif
