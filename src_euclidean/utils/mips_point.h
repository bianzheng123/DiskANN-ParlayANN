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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


float mips_distance(const float *p, const float *q, unsigned d) {
    float result = 0;
    for (int i = 0; i < d; i++) {
        result += (q[i]) * (p[i]);
    }
    return -result;
}

template<typename T>
struct Mips_Point {
    using distanceType = float;
    //template<typename C, typename range> friend struct Quantized_Mips_Point;

    struct parameters {
        int dims;

        parameters() : dims(0) {}

        parameters(int dims) : dims(dims) {}
    };

    static distanceType d_min() { return -std::numeric_limits<float>::max(); }

    static bool is_metric() { return false; }

    T operator[](long i) const { return *(values + i); }

    float distance(const Mips_Point<T> &x) const {
        return mips_distance(this->values, x.values, params.dims);
    }

    void prefetch() const {
        int l = (params.dims * sizeof(T) - 1) / 64 + 1;
        for (int i = 0; i < l; i++)
            __builtin_prefetch((char *) values + i * 64);
    }

    long id() const { return id_; }

    Mips_Point() : values(nullptr), id_(-1), params(0) {}

    Mips_Point(T *values, long id, parameters params)
            : values(values), id_(id), params(params) {}

    bool operator==(const Mips_Point<T> &q) const {
        for (int i = 0; i < params.dims; i++) {
            if (values[i] != q.values[i]) {
                return false;
            }
        }
        return true;
    }

    bool same_as(const Mips_Point<T> &q) const {
        return values == q.values;
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

    template<typename Point>
    static void translate_point(T *values, const Point &p, const parameters &params) {
        for (int j = 0; j < params.dims; j++) values[j] = (T) p[j];
    }

    template<typename PR>
    static parameters generate_parameters(const PR &pr) {
        return parameters(pr.dimension());
    }

private:
    T *values;
    long id_;
    parameters params;
};

