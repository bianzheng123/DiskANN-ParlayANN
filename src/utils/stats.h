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
#include <queue>
#include <set>

#include "parlay/parallel.h"
#include "parlay/primitives.h"

// template <typename T>
// std::pair<double, int> graph_stats(parlay::sequence<Tvec_point<T> *> &v) {
//   auto od = parlay::delayed_seq<size_t>(
//       v.size(), [&](size_t i) { return size_of(v[i]->out_nbh); });
//   size_t j = parlay::max_element(od) - od.begin();
//   int maxDegree = od[j];
//   size_t sum1 = parlay::reduce(od);
//   double avg_deg = sum1 / ((double)v.size());
//   return std::make_pair(avg_deg, maxDegree);
// }

std::pair<double, int> graph_stats_(Graph<unsigned int> &G) {
    auto od = parlay::delayed_seq<size_t>(
            G.size(), [&](size_t i) { return G[i].size(); });
    size_t j = parlay::max_element(od) - od.begin();
    int maxDegree = od[j];
    size_t sum1 = parlay::reduce(od);
    double avg_deg = sum1 / ((double) G.size());
    return std::make_pair(avg_deg, maxDegree);
}

template<typename indexType>
struct stats {

    stats() {}

    stats(size_t n) {
        visited = parlay::sequence<indexType>(n, 0);
        distances = parlay::sequence<indexType>(n, 0);
    }

    parlay::sequence<indexType> visited;
    parlay::sequence<indexType> distances;

    void increment_dist(indexType i, indexType j) { distances[i] += j; }

    void increment_visited(indexType i, indexType j) { visited[i] += j; }

    parlay::sequence<indexType> visited_stats() { return statistics(this->visited); }

    parlay::sequence<indexType> dist_stats() { return statistics(this->distances); }

    void clear() {
        size_t n = visited.size();
        visited = parlay::sequence<indexType>(n, 0);
        distances = parlay::sequence<indexType>(n, 0);
    }

    parlay::sequence<indexType> statistics(parlay::sequence<indexType> s) {
        parlay::sequence<indexType> stats = parlay::tabulate(s.size(), [&](size_t i) { return s[i]; });
        parlay::sort_inplace(stats);
        indexType avg = (int) parlay::reduce(stats) / ((double) s.size());
        indexType tail_index = .99 * ((float) s.size());
        indexType tail = stats[tail_index];
        auto result = {avg, tail};
        return result;
    }

};
