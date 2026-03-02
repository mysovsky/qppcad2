#ifndef FMT_QPPCAD_TYPES_HPP
#define FMT_QPPCAD_TYPES_HPP

#include <memory>
#include <fmt/format.h>

//Forward Declaration to isolate infinite loop when including <qppcad/sflow/sflow_parameter.hpp>
namespace qpp { namespace cad { enum sflow_parameter_e : int; } }

namespace fmt {

//Sflow Parameter 
template <>
struct formatter<qpp::cad::sflow_parameter_e> : formatter<int> {
    auto format(qpp::cad::sflow_parameter_e e, format_context& ctx) const {
        return formatter<int>::format(static_cast<int>(e), ctx);
    }
};

//Shared Pointer (Yeah, I don't know why, but fmt/std.h don't have that one ;(
template <typename T>
struct formatter<std::shared_ptr<T>> : formatter<const void*> {
    auto format(const std::shared_ptr<T>& ptr, format_context& ctx) const {
        return formatter<const void*>::format(static_cast<const void*>(ptr.get()), ctx);
    }
};

} // namespace fmt

#endif
