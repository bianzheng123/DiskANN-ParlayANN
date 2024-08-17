//
// Created by bianzheng on 2024/8/14.
//
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

#include <iostream>
#include <algorithm>
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "bench/parse_command_line.h"
#include "bench/time_loop.h"
#include "utils/NSGDist.h"
#include "utils/euclidian_point.h"
#include "utils/point_range.h"
#include "utils/graph.h"

//#include "vamana/index.h"
#include "vamana/neighbors.h"


#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


// *************************************************************
//  TIMING
// *************************************************************


template<typename Point, typename PointRange, typename indexType>
void time_search(PointRange &Points, Graph<indexType> &G, BuildParams &BP,
                 PointRange &Query_Points, long k,
                 groundTruth<indexType> GT, char *res_file) {


    time_loop(1, 0,
              [&]() {},
              [&]() {
                  ANN_search<Point, PointRange, indexType>(Points, G, BP,
                                                           Query_Points, k,
                                                           GT, res_file);
              },
              [&]() {});

}

int main(int argc, char *argv[]) {
    commandLine P(argc, argv,
                  "[-R <deg>] [-L <bm>] [-a <alpha>]"
                  "[-k <k> ]  [-gt_path <g>] [-query_path <qF>]"
                  "[-graph_path <gF>] [-res_path <rF>]" "[-num_passes <np>]"
                  "[-base_path <b>] <inFile>");

    long R = P.getOptionIntValue("-R", 0);
    if (R < 0) P.badArgument();
    long L = P.getOptionIntValue("-L", 0);
    if (L < 0) P.badArgument();
    double alpha = P.getOptionDoubleValue("-alpha", 1.0);

    char *iFile = P.getOptionValue("-base_path");
    char *gFile = P.getOptionValue("-graph_path");
    char *qFile = P.getOptionValue("-query_path");
    char *cFile = P.getOptionValue("-gt_path");
    char *rFile = P.getOptionValue("-res_path");

    long k = P.getOptionIntValue("-k", 0);
    if (k > 1000 || k < 0) P.badArgument();
    int num_passes = P.getOptionIntValue("-num_passes", 1);
    bool verbose = P.getOption("-verbose");
    bool normalize = P.getOption("-normalize");
    int single_batch = P.getOptionIntValue("-single_batch", 0);

    BuildParams BP = BuildParams(R, L, alpha, num_passes, verbose,
                                 single_batch);

    groundTruth<uint32_t> GT = groundTruth<uint32_t>(cFile);

    PointRange Points = PointRange(iFile);
    PointRange Query_Points = PointRange(qFile);
    if (normalize) {
        std::cout << "normalizing data" << std::endl;
        for (int i = 0; i < Points.size(); i++)
            Points[i].normalize();
        for (int i = 0; i < Query_Points.size(); i++)
            Query_Points[i].normalize();
    }
    Graph<uint32_t> G = Graph<uint32_t>(gFile);
    using Point = Euclidian_Point;
    using PR = PointRange;
    time_search<Point, PR, uint32_t>(Points, G, BP,
                                     Query_Points, k,
                                     GT, rFile);


    return 0;
}
