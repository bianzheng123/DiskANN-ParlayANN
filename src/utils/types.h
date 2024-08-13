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

#ifndef TYPES
#define TYPES

#include <algorithm>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "mmap.h"

template<typename T>
struct groundTruth {
    parlay::slice<T *, T *> coords;
    parlay::slice<float *, float *> dists;
    long dim;
    size_t n;

    groundTruth() : coords(parlay::make_slice<T *, T *>(nullptr, nullptr)),
                    dists(parlay::make_slice<float *, float *>(nullptr, nullptr)) {}

    groundTruth(char *gtFile) : coords(parlay::make_slice<T *, T *>(nullptr, nullptr)),
                                dists(parlay::make_slice<float *, float *>(nullptr, nullptr)) {
        if (gtFile == NULL) {
            n = 0;
            dim = 0;
        } else {
            auto [fileptr, length] = mmapStringFromFile(gtFile);

            int num_vectors = *((T *) fileptr);
            int d = *((T *) (fileptr + 4));

            std::cout << "Detected " << num_vectors << " points with num results " << d << std::endl;

            T *start_coords = (T *) (fileptr + 8);
            T *end_coords = start_coords + d * num_vectors;

            float *start_dists = (float *) (end_coords);
            float *end_dists = start_dists + d * num_vectors;

            n = num_vectors;
            dim = d;
            coords = parlay::make_slice(start_coords, end_coords);
            dists = parlay::make_slice(start_dists, end_dists);
        }
    }

    groundTruth(parlay::sequence<parlay::sequence<T>> gt) : coords(parlay::make_slice<T *, T *>(nullptr, nullptr)),
                                                            dists(parlay::make_slice<float *, float *>(nullptr,
                                                                                                       nullptr)) {
        n = gt.size();
        dim = gt[0].size();
        auto flat_gt = parlay::flatten(gt);
        coords = parlay::make_slice(flat_gt.begin(), flat_gt.end());
        parlay::sequence<float> dummy_ds = parlay::sequence<float>(dim * n, 0.0);
        dists = parlay::make_slice(dummy_ds.begin(), dummy_ds.end());
    }

    //saves in binary format
    //assumes gt is not so big that it needs block saving
    void save(char *save_path) {
        std::cout << "Writing groundtruth for " << n << " points and num results " << dim
                  << std::endl;
        parlay::sequence<T> preamble = {static_cast<T>(n), static_cast<T>(dim)};
        std::ofstream writer;
        writer.open(save_path, std::ios::binary | std::ios::out);
        writer.write((char *) preamble.begin(), 2 * sizeof(T));
        writer.write((char *) coords.begin(), dim * n * sizeof(T));
        writer.write((char *) dists.begin(), dim * n * sizeof(float));
        writer.close();
    }

    T coordinates(long i, long j) { return *(coords.begin() + i * dim + j); }

    float distances(long i, long j) { return *(dists.begin() + i * dim + j); }

    size_t size() { return n; }

    long dimension() { return dim; }

};

struct BuildParams {
    long L; //vamana
    long R; //vamana
    double alpha; //vamana
    int num_passes; //vamana
    int single_batch; //vamana

    bool verbose;

    std::string alg_type;

    BuildParams(long R, long L, double a, int num_passes,
                bool verbose = false,
                int single_batch = 0)
            : R(R), L(L), alpha(a), num_passes(num_passes),
              verbose(verbose),
              single_batch(single_batch) {
        assert(R != 0 && L != 0 && alpha != 0);
        alg_type = "Vamana";
    }

    BuildParams() {}

    BuildParams(long R, long L, double a, int num_passes, bool verbose = false)
            : R(R), L(L), alpha(a), num_passes(num_passes), single_batch(0), verbose(verbose) { alg_type = "Vamana"; }

    long max_degree() {
        return R;
    }
};


struct QueryParams {
    long k;
    long beamSize;
    double cut;
    long limit;
    long degree_limit;
    float pad = 1.0;

    QueryParams(long k, long Q, double cut, long limit, long dg) : k(k), beamSize(Q), cut(cut), limit(limit),
                                                                   degree_limit(dg) {}

    QueryParams() {}

};

#endif
