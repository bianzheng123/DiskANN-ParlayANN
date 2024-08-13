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
#include "utils/mips_point.h"
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

using uint = unsigned int;


template<typename Point, typename PointRange, typename indexType>
void timeNeighbors(Graph<indexType> &G,
                   PointRange &Query_Points, long k,
                   BuildParams &BP, char *outFile,
                   groundTruth<indexType> GT, char *res_file, bool graph_built, PointRange &Points) {


    time_loop(1, 0,
              [&]() {},
              [&]() {
                  ANN<Point, PointRange, indexType>(G, k, BP, Query_Points, GT, res_file, graph_built, Points);
              },
              [&]() {});

    if (outFile != NULL) {
        G.save(outFile);
    }


}

int main(int argc, char *argv[]) {
    commandLine P(argc, argv,
                  "[-a <alpha>] [-R <deg>]"
                  "[-L <bm>] [-k <k> ]  [-gt_path <g>] [-query_path <qF>]"
                  "[-graph_path <gF>] [-graph_outfile <oF>] [-res_path <rF>]" "[-num_passes <np>]"
                  "[-dist_func <df>] [-base_path <b>] <inFile>");

    char *iFile = P.getOptionValue("-base_path");
    char *oFile = P.getOptionValue("-graph_outfile");
    char *gFile = P.getOptionValue("-graph_path");
    char *qFile = P.getOptionValue("-query_path");
    char *cFile = P.getOptionValue("-gt_path");
    char *rFile = P.getOptionValue("-res_path");
    long R = P.getOptionIntValue("-R", 0);
    if (R < 0) P.badArgument();
    long L = P.getOptionIntValue("-L", 0);
    if (L < 0) P.badArgument();
    long k = P.getOptionIntValue("-k", 0);
    if (k > 1000 || k < 0) P.badArgument();
    double alpha = P.getOptionDoubleValue("-alpha", 1.0);
    int num_passes = P.getOptionIntValue("-num_passes", 1);
    char *dfc = P.getOptionValue("-dist_func");
    bool verbose = P.getOption("-verbose");
    bool normalize = P.getOption("-normalize");
    int single_batch = P.getOptionIntValue("-single_batch", 0);

    std::string df = std::string(dfc);

    BuildParams BP = BuildParams(R, L, alpha, num_passes, verbose,
                                 single_batch);
    long maxDeg = BP.max_degree();

    if (df != "Euclidian" && df != "mips") {
        std::cout << "Error: specify distance type Euclidian or mips" << std::endl;
        abort();
    }

    bool graph_built = (gFile != NULL);

    groundTruth<uint> GT = groundTruth<uint>(cFile);

    if (df == "Euclidian") {
        PointRange<float, Euclidian_Point<float>> Points = PointRange<float, Euclidian_Point<float>>(iFile);
        PointRange<float, Euclidian_Point<float>> Query_Points = PointRange<float, Euclidian_Point<float>>(qFile);
        if (normalize) {
            std::cout << "normalizing data" << std::endl;
            for (int i = 0; i < Points.size(); i++)
                Points[i].normalize();
            for (int i = 0; i < Query_Points.size(); i++)
                Query_Points[i].normalize();
        }
        Graph<unsigned int> G;
        if (gFile == NULL) G = Graph<unsigned int>(maxDeg, Points.size());
        else G = Graph<unsigned int>(gFile);
        using Point = Euclidian_Point<float>;
        using PR = PointRange<float, Point>;
        timeNeighbors<Point, PR, uint>(G, Query_Points, k, BP, oFile, GT, rFile, graph_built, Points);

    } else if (df == "mips") {
        PointRange<float, Mips_Point<float>> Points = PointRange<float, Mips_Point<float>>(iFile);
        PointRange<float, Mips_Point<float>> Query_Points = PointRange<float, Mips_Point<float>>(qFile);
        if (normalize) {
            std::cout << "normalizing data" << std::endl;
            for (int i = 0; i < Points.size(); i++)
                Points[i].normalize();
            for (int i = 0; i < Query_Points.size(); i++)
                Query_Points[i].normalize();
        }
        Graph<unsigned int> G;
        if (gFile == NULL) G = Graph<unsigned int>(maxDeg, Points.size());
        else G = Graph<unsigned int>(gFile);
        using Point = Mips_Point<float>;
        using PR = PointRange<float, Point>;
        timeNeighbors<Point, PR, uint>(G, Query_Points, k, BP, oFile, GT, rFile, graph_built, Points);
    }

    return 0;
}
