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

#include <algorithm>

#include "../utils/NSGDist.h"
#include "../utils/beamSearch.h"
#include "../utils/check_nn_recall.h"
#include "../utils/parse_results.h"
#include "../utils/mips_point.h"
#include "../utils/euclidian_point.h"
#include "../utils/stats.h"
#include "../utils/types.h"
#include "../utils/graph.h"
#include "index.h"
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/random.h"


template<typename Point, typename PointRange, typename indexType>
void ANN_build_index_(Graph<indexType> &G, BuildParams &BP, PointRange &Points) {
    parlay::internal::timer t("ANN");

    using findex = knn_index<PointRange, indexType>;
    findex I(BP);
    indexType start_point;
    double idx_time;
    // declare two array, visited and distances
    stats<unsigned int> BuildStats(G.size());
    I.build_index(G, Points, BuildStats);
    start_point = I.get_start();
    idx_time = t.next_time();
    std::cout << "start index = " << start_point << std::endl;

    std::string name = "Vamana";
    std::string params =
            "R = " + std::to_string(BP.R) + ", L = " + std::to_string(BP.L);
    auto [avg_deg, max_deg] = graph_stats_(G);
    auto vv = BuildStats.visited_stats();
    std::cout << "Average visited: " << vv[0] << ", Tail visited: " << vv[1]
              << std::endl;
    Graph_ G_(name, params, G.size(), avg_deg, max_deg, idx_time);
    G_.print();

    long build_num_distances = parlay::reduce(parlay::map(BuildStats.distances,
                                                          [](auto x) { return (long) x; }));

}

template<typename Point, typename PointRange_, typename indexType>
void ANN_build_index(Graph<indexType> &G, BuildParams &BP, PointRange_ &Points) {

    ANN_build_index_<Point, PointRange_, indexType>(G, BP, Points);
}


template<typename Point, typename PointRange, typename QPointRange, typename indexType>
void ANN_search_(Graph<indexType> &G, long k, BuildParams &BP,
                 PointRange &Query_Points, QPointRange &Q_Query_Points,
                 groundTruth<indexType> GT, char *res_file,
                 PointRange &Points, QPointRange &Q_Points) {
    parlay::internal::timer t("ANN");

    double idx_time = 0;
    indexType start_point = 0;
    // declare two array, visited and distances
    stats<unsigned int> BuildStats(G.size());
    std::cout << "start index = " << start_point << std::endl;

    std::string name = "Vamana";
    std::string params =
            "R = " + std::to_string(BP.R) + ", L = " + std::to_string(BP.L);
    auto [avg_deg, max_deg] = graph_stats_(G);
    auto vv = BuildStats.visited_stats();
    std::cout << "Average visited: " << vv[0] << ", Tail visited: " << vv[1]
              << std::endl;
    Graph_ G_(name, params, G.size(), avg_deg, max_deg, idx_time);
    G_.print();

    long build_num_distances = parlay::reduce(parlay::map(BuildStats.distances,
                                                          [](auto x) { return (long) x; }));

    assert(Query_Points.size() != 0);
    search_and_parse<Point, PointRange, QPointRange, indexType>(G_, G, Points, Query_Points,
                                                                Q_Points, Q_Query_Points, GT,
                                                                res_file, k, start_point,
                                                                BP.verbose);

}

template<typename Point, typename PointRange_, typename indexType>
void ANN_search(PointRange_ &Points, Graph<indexType> &G, BuildParams &BP,
                PointRange_ &Query_Points, long k,
                groundTruth<indexType> GT, char *res_file) {

    ANN_search_<Point, PointRange_, PointRange_, indexType>(G, k, BP,
                                                            Query_Points, Query_Points, GT, res_file,
                                                            Points, Points);
}
