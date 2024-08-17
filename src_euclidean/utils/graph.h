// This code is part of the Parlay Project
// Copyright (c) 2024 Guy Blelloch, Magdalen Dobson and the Parlay team
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
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/internal/file_map.h"

#include "../bench/parse_command_line.h"
#include "types.h"

template<typename index_t>
struct edgeRange {

    size_t size() const { return edges[0]; }

    index_t id() const { return id_; }

    edgeRange() : edges(parlay::make_slice<index_t *, index_t *>(nullptr, nullptr)) {}

    edgeRange(index_t *start, index_t *end, index_t id)
            : edges(parlay::make_slice<index_t *, index_t *>(start, end)), id_(id) {
        maxDeg = edges.size() - 1;
    }

    index_t operator[](index_t j) const {
        if (j > edges[0]) {
            std::cout << "ERROR: index exceeds degree while accessing neighbors" << std::endl;
            abort();
        } else return edges[j + 1];
    }

    void append_neighbor(index_t nbh) {
        if (edges[0] == maxDeg) {
            std::cout << "ERROR in append_neighbor: cannot exceed max degree "
                      << maxDeg << std::endl;
            abort();
        } else {
            edges[edges[0] + 1] = nbh;
            edges[0] += 1;
        }
    }

    template<typename rangeType>
    void update_neighbors(const rangeType &r) {
        if (r.size() > maxDeg) {
            std::cout << "ERROR in update_neighbors: cannot exceed max degree "
                      << maxDeg << std::endl;
            abort();
        }
        edges[0] = r.size();
        for (int i = 0; i < r.size(); i++) {
            edges[i + 1] = r[i];
        }
    }

    template<typename rangeType>
    void append_neighbors(const rangeType &r) {
        if (r.size() + edges[0] > maxDeg) {
            std::cout << "ERROR in append_neighbors for point " << id_
                      << ": cannot exceed max degree " << maxDeg << std::endl;
            std::cout << edges[0] << std::endl;
            std::cout << r.size() << std::endl;
            abort();
        }
        for (int i = 0; i < r.size(); i++) {
            edges[edges[0] + i + 1] = r[i];
        }
        edges[0] += r.size();
    }

    void clear_neighbors() {
        edges[0] = 0;
    }

    void prefetch() {
        int l = ((edges[0] + 1) * sizeof(index_t)) / 64;
        for (int i = 0; i < l; i++)
            __builtin_prefetch((char *) edges.begin() + i * 64);
    }

    template<typename F>
    void sort(F &&less) {
        std::sort(edges.begin() + 1, edges.begin() + 1 + edges[0], less);
    }

    index_t *begin() { return edges.begin() + 1; }

    index_t *end() { return edges.end() + 1 + edges[0]; }

private:
    parlay::slice<index_t *, index_t *> edges;
    long maxDeg;
    index_t id_;
};

template<typename index_t>
struct Graph {
    long max_degree() const { return maxDeg; }

    size_t size() const { return n; }

    Graph() {}

    void allocate_graph(long maxDeg, size_t n) {
        long cnt = n * (maxDeg + 1);
        long num_bytes = cnt * sizeof(index_t);
        index_t *ptr = (index_t *) aligned_alloc(1l << 21, num_bytes);
        madvise(ptr, num_bytes, MADV_HUGEPAGE);
        parlay::parallel_for(0, cnt, [&](long i) { ptr[i] = 0; });
        graph = std::shared_ptr<index_t[]>(ptr, std::free);
    }

    Graph(long maxDeg, size_t n) : maxDeg(maxDeg), n(n) {
        allocate_graph(maxDeg, n);
    }

    Graph(char *gFile) {
        std::ifstream reader(gFile);
        assert(reader.is_open());

        //read num points and max degree
        index_t num_points;
        index_t max_deg;
        reader.read((char *) (&num_points), sizeof(index_t));
        n = num_points;
        reader.read((char *) (&max_deg), sizeof(index_t));
        maxDeg = max_deg;
        std::cout << "Detected " << num_points
                  << " points with max degree " << max_deg << std::endl;

        //read degrees and perform scan to find offsets
        index_t *degrees_start = new index_t[n];
        reader.read((char *) (degrees_start), sizeof(index_t) * n);
        index_t *degrees_end = degrees_start + n;
        parlay::slice<index_t *, index_t *> degrees0 =
                parlay::make_slice(degrees_start, degrees_end);
        auto degrees = parlay::tabulate(degrees0.size(), [&](size_t i) {
            return static_cast<size_t>(degrees0[i]);
        });
        auto [offsets, total] = parlay::scan(degrees);
        std::cout << "Total edges read from file: " << total << std::endl;
        offsets.push_back(total);

        allocate_graph(max_deg, n);

        //write 1000000 vertices at a time
        size_t BLOCK_SIZE = 1000000;
        size_t index = 0;
        size_t total_size_read = 0;
        while (index < n) {
            size_t g_floor = index;
            size_t g_ceiling = g_floor + BLOCK_SIZE <= n ? g_floor + BLOCK_SIZE : n;
            size_t total_size_to_read = offsets[g_ceiling] - offsets[g_floor];
            index_t *edges_start = new index_t[total_size_to_read];
            reader.read((char *) (edges_start), sizeof(index_t) * total_size_to_read);
            index_t *edges_end = edges_start + total_size_to_read;
            parlay::slice<index_t *, index_t *> edges =
                    parlay::make_slice(edges_start, edges_end);
            index_t *gr = graph.get();
            parlay::parallel_for(g_floor, g_ceiling, [&](size_t i) {
                gr[i * (maxDeg + 1)] = degrees[i];
                for (size_t j = 0; j < degrees[i]; j++) {
                    gr[i * (maxDeg + 1) + 1 + j] = edges[offsets[i] - total_size_read + j];
                }
            });
            total_size_read += total_size_to_read;
            index = g_ceiling;
            delete[] edges_start;
        }
        delete[] degrees_start;
    }

    void save(char *oFile) {
        std::cout << "Writing graph with " << n
                  << " points and max degree " << maxDeg
                  << std::endl;
        parlay::sequence<index_t> preamble =
                {static_cast<index_t>(n), static_cast<index_t>(maxDeg)};
        parlay::sequence<index_t> sizes = parlay::tabulate(n, [&](size_t i) {
            return static_cast<index_t>((*this)[i].size());
        });
        std::ofstream writer;
        writer.open(oFile, std::ios::binary | std::ios::out);
        writer.write((char *) preamble.begin(), 2 * sizeof(index_t));
        writer.write((char *) sizes.begin(), sizes.size() * sizeof(index_t));
        size_t BLOCK_SIZE = 1000000;
        size_t index = 0;
        while (index < n) {
            size_t floor = index;
            size_t ceiling = index + BLOCK_SIZE <= n ? index + BLOCK_SIZE : n;
            auto edge_data = parlay::tabulate(ceiling - floor, [&](size_t i) {
                return parlay::tabulate(sizes[i + floor], [&](size_t j) {
                    return (*this)[i + floor][j];
                });
            });
            parlay::sequence<index_t> data = parlay::flatten(edge_data);
            writer.write((char *) data.begin(), data.size() * sizeof(index_t));
            index = ceiling;
        }
        writer.close();
    }

    edgeRange<index_t> operator[](index_t i) {
        if (i > n) {
            std::cout << "ERROR: graph index out of range: " << i << std::endl;
            abort();
        }
        return edgeRange<index_t>(graph.get() + i * (maxDeg + 1),
                                    graph.get() + (i + 1) * (maxDeg + 1),
                                    i);
    }

    ~Graph() {}

private:
    size_t n;
    long maxDeg;
    std::shared_ptr<index_t[]> graph;
};
