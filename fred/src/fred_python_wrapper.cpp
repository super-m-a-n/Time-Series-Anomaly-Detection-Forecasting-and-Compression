/*
Copyright 2020-2021 Dennis Rohde

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "config.hpp"
#include "curve.hpp"
#include "point.hpp"
#include "frechet.hpp"
#include "jl_transform.hpp"
#include "clustering.hpp"
#include "coreset.hpp"
#include "grid.hpp"
#include "simplification.hpp"
#include "dynamic_time_warping.hpp"

namespace fc = Frechet::Continuous;
namespace fd = Frechet::Discrete;
namespace ddtw = Dynamic_Time_Warping::Discrete;

Curve minimum_error_simplification(const Curve &curve, const curve_size_t l) {
    Simplification::Subcurve_Shortcut_Graph graph(const_cast<Curve&>(curve));
    auto scurve = graph.minimum_error_simplification(l);
    scurve.set_name("Simplification of " + curve.get_name());
    return scurve;
}

Curve approximate_minimum_link_simplification(const Curve &curve, const distance_t epsilon) {
    auto scurve = Simplification::approximate_minimum_link_simplification(curve, epsilon);
    scurve.set_name("Simplification of " + curve.get_name());
    return scurve;
}

Curve approximate_minimum_error_simplification(const Curve &curve, const curve_size_t ell) {
    auto scurve = Simplification::approximate_minimum_error_simplification(curve, ell);
    scurve.set_name("Simplification of " + curve.get_name());
    return scurve;
}
