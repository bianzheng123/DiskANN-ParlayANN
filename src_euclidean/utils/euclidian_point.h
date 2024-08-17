// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <algorithm>
#include <iostream>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/internal/file_map.h"
#include "../bench/parse_command_line.h"
#include "NSGDist.h"

#include "types.h"
// #include "common/time_loop.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


float euclidian_distance(const float *p, const float *q, unsigned d) {
    efanna2e::DistanceL2 distfunc;
    return distfunc.compare(p, q, d);
}

// this looks like the union of the array
struct Euclidian_Point {
    using dist_t = float;

    const static long range = (1l << sizeof(dist_t) * 8) - 1;

    struct parameters {
        float slope;
        int32_t offset;
        int dims;

        // the slop and offset are the parameter for scalar quantization
        parameters() : slope(0), offset(0), dims(0) {}

        parameters(int dims) : slope(0), offset(0), dims(dims) {}

        parameters(float min_val, float max_val, int dims)
                : slope(range / (max_val - min_val)),
                  offset((int32_t) round(min_val * slope)),
                  dims(dims) {}
    };

    static dist_t d_min() { return 0; }

    static bool is_metric() { return true; }

    dist_t operator[](long i) const { return *(values + i); }

    float distance(const Euclidian_Point &x) const {
        return euclidian_distance(this->values, x.values, params.dims);
    }

    void normalize() {
        double norm = 0.0;
        for (int j = 0; j < params.dims; j++)
            norm += values[j] * values[j];
        norm = std::sqrt(norm);
        if (norm == 0) norm = 1.0;
        for (int j = 0; j < params.dims; j++)
            values[j] = values[j] / norm;
    }

    void prefetch() const {
        int l = (params.dims * sizeof(dist_t) - 1) / 64 + 1;
        for (int i = 0; i < l; i++)
            __builtin_prefetch((char *) values + i * 64);
    }

    long id() const { return id_; }

    Euclidian_Point() : values(nullptr), id_(-1), params(0) {}

    Euclidian_Point(dist_t *values, long id, parameters params)
            : values(values), id_(id), params(params) {}

    bool operator==(const Euclidian_Point &q) const {
        for (int i = 0; i < params.dims; i++) {
            if (values[i] != q.values[i]) {
                return false;
            }
        }
        return true;
    }

    bool same_as(const Euclidian_Point &q) const {
        return values == q.values;
    }

    template<typename Point>
    static void translate_point(dist_t *values, const Point &p, const parameters &params) {
        float slope = params.slope;
        int32_t offset = params.offset;
        float min_val = std::floor(offset / slope);
        float max_val = std::ceil((range + offset) / slope);
        for (int j = 0; j < params.dims; j++) {
            auto x = p[j];
            if (x < min_val || x > max_val) {
                std::cout << x << " is out of range: [" << min_val << "," << max_val << "]" << std::endl;
                abort();
            }
            int64_t r = (int64_t) (std::round(x * slope)) - offset;
            if (r < 0 || r > range) {
                std::cout << "out of range: " << r << ", " << range << ", " << x << ", "
                          << std::round(x * slope) - offset << ", " << slope << ", " << offset << std::endl;
                abort();
            }
            values[j] = (dist_t) r;
        }
    }

    template<typename PR>
    static parameters generate_parameters(const PR &pr) {
        long n = pr.size();
        int dims = pr.dimension();
        parlay::sequence<typename PR::dist_t> mins(n, 0.0);
        parlay::sequence<typename PR::dist_t> maxs(n, 0.0);
        parlay::sequence<bool> ni(n, true);
        parlay::parallel_for(0, n, [&](long i) {
            for (int j = 0; j < dims; j++) {
                ni[i] = ni[i] && (pr[i][j] >= 0) && (pr[i][j] - (long) pr[i][j]) == 0;
                mins[i] = std::min(mins[i], pr[i][j]);
                maxs[i] = std::max(maxs[i], pr[i][j]);
            }
        });
        float min_val = *parlay::min_element(mins);
        float max_val = *parlay::max_element(maxs);
        bool all_ints = *parlay::min_element(ni);
        if (all_ints)
            if (sizeof(dist_t) == 1 && max_val < 256) max_val = 255;
            else if (sizeof(dist_t) == 2 && max_val < 65536) max_val = 65536;
        std::cout << "scalar quantization: min value = " << min_val
                  << ", max value = " << max_val << std::endl;
        return parameters(min_val, max_val, dims);
    }

    parameters params;

private:
    dist_t *values;
    long id_;
};
