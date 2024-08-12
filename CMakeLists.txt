cmake_minimum_required(VERSION 3.27)
project(DiskANN_ParlayANN)

set(CMAKE_CXX_STANDARD 14)

#include_directories(algorithms/bench)
#include_directories(algorithms/bench/common)
#include_directories(algorithms/HCNNG)
#include_directories(algorithms/HNSW)
#include_directories(algorithms/pyNNDescent)
#include_directories(algorithms/utils)
#include_directories(algorithms/vamana)
#include_directories(algorithms/vamanaRange)
#include_directories(parlaylib/benchmark)
#include_directories(parlaylib/examples)
#include_directories(parlaylib/examples/helper)
#include_directories(parlaylib/include)
#include_directories(parlaylib/include/parlay)
#include_directories(parlaylib/include/parlay/internal)
#include_directories(parlaylib/include/parlay/internal/concurrency)
#include_directories(parlaylib/include/parlay/internal/delayed)
#include_directories(parlaylib/include/parlay/internal/posix)
#include_directories(parlaylib/include/parlay/internal/scheduler_plugins)
#include_directories(parlaylib/include/parlay/internal/windows)
#include_directories(parlaylib/test)
#include_directories(rangeSearch/bench)
#include_directories(rangeSearch/vamanaRange)

#add_executable(DiskANN_ParlayANN
#        algorithms/bench/common/atomics.h
#        algorithms/bench/common/dataGen.h
#        algorithms/bench/common/geometry.h
#        algorithms/bench/common/geometryIO.h
#        algorithms/bench/common/get_time.h
#        algorithms/bench/common/glue.h
#        algorithms/bench/common/graph.h
#        algorithms/bench/common/graphIO.h
#        algorithms/bench/common/graphUtils.h
#        algorithms/bench/common/IO.h
#        algorithms/bench/common/ligraLight.h
#        algorithms/bench/common/parse_command_line.h
#        algorithms/bench/common/parseCommandLine.h
#        algorithms/bench/common/sequenceIO.h
#        algorithms/bench/common/speculative_for.h
#        algorithms/bench/common/time_loop.h
#        algorithms/bench/common/topology.h
#        algorithms/bench/common/topology_from_triangles.h
#        algorithms/bench/benchUtils.h
#        algorithms/bench/get_time.h
#        algorithms/bench/IO.h
#        algorithms/bench/neighborsTime.C
#        algorithms/bench/parse_command_line.h
#        algorithms/bench/time_loop.h
#        algorithms/HCNNG/clusterEdge.h
#        algorithms/HCNNG/hcnng_index.h
#        algorithms/HCNNG/neighbors.h
#        algorithms/HNSW/debug.hpp
#        algorithms/HNSW/dist.hpp
#        algorithms/HNSW/h5_ops.hpp
#        algorithms/HNSW/HNSW.hpp
#        algorithms/HNSW/type_point.hpp
#        algorithms/pyNNDescent/clusterPynn.h
#        algorithms/pyNNDescent/neighbors.h
#        algorithms/pyNNDescent/pynn_index.h
#        algorithms/utils/beamSearch.h
#        algorithms/utils/check_nn_recall.h
#        algorithms/utils/check_range_recall.h
#        algorithms/utils/csvfile.h
#        algorithms/utils/euclidian_point.h
#        algorithms/utils/graph.h
#        algorithms/utils/mips_point.h
#        algorithms/utils/mmap.h
#        algorithms/utils/NSGDist.h
#        algorithms/utils/parse_results.h
#        algorithms/utils/point_range.h
#        algorithms/utils/stats.h
#        algorithms/utils/types.h
#        algorithms/utils/union.h
#        algorithms/vamana/index.h
#        algorithms/vamana/index_test.cc
#        algorithms/vamana/neighbors.h
#        algorithms/vamana/neighbors_test.cc
#        algorithms/vamanaRange/index.h
#        algorithms/vamanaRange/neighbors.h
#        data_tools/compute_groundtruth.cpp
#        data_tools/compute_range_groundtruth.cpp
#        data_tools/crop.cpp
#        data_tools/random_sample.cpp
#        data_tools/vec_to_bin.cpp
#        parlaylib/benchmark/bench_delayed.cpp
#        parlaylib/benchmark/bench_examples.cpp
#        parlaylib/benchmark/bench_parsing.cpp
#        parlaylib/benchmark/bench_sequence.cpp
#        parlaylib/benchmark/bench_standard.cpp
#        parlaylib/benchmark/common.h
#        parlaylib/benchmark/trigram_words.h
#        parlaylib/benchmark/util.h
#        parlaylib/examples/helper/graph_utils.h
#        parlaylib/examples/helper/heap_tree.h
#        parlaylib/examples/helper/ligra_light.h
#        parlaylib/examples/helper/speculative_for.h
#        parlaylib/examples/helper/spherical.h
#        parlaylib/examples/helper/union_find.h
#        parlaylib/examples/2d_linear_program.cpp
#        parlaylib/examples/2d_linear_program.h
#        parlaylib/examples/3d_range.cpp
#        parlaylib/examples/3d_range.h
#        parlaylib/examples/bellman_ford.cpp
#        parlaylib/examples/bellman_ford.h
#        parlaylib/examples/betweenness_centrality.cpp
#        parlaylib/examples/betweenness_centrality.h
#        parlaylib/examples/BFS.cpp
#        parlaylib/examples/BFS.h
#        parlaylib/examples/BFS_ligra.cpp
#        parlaylib/examples/BFS_ligra.h
#        parlaylib/examples/bigint_add.cpp
#        parlaylib/examples/bigint_add.h
#        parlaylib/examples/box_kdtree.cpp
#        parlaylib/examples/box_kdtree.h
#        parlaylib/examples/bucketed_dijkstra.cpp
#        parlaylib/examples/bucketed_dijkstra.h
#        parlaylib/examples/cartesian_tree.cpp
#        parlaylib/examples/cartesian_tree.h
#        parlaylib/examples/cycle_count.cpp
#        parlaylib/examples/cycle_count.h
#        parlaylib/examples/decision_tree_c45.cpp
#        parlaylib/examples/decision_tree_c45.h
#        parlaylib/examples/delaunay.cpp
#        parlaylib/examples/delaunay.h
#        parlaylib/examples/fast_fourier_transform.cpp
#        parlaylib/examples/fast_fourier_transform.h
#        parlaylib/examples/filter.cpp
#        parlaylib/examples/filter.h
#        parlaylib/examples/filter_kruskal.cpp
#        parlaylib/examples/filter_kruskal.h
#        parlaylib/examples/find_if.cpp
#        parlaylib/examples/find_if.h
#        parlaylib/examples/flatten.cpp
#        parlaylib/examples/flatten.h
#        parlaylib/examples/graph_color.cpp
#        parlaylib/examples/graph_color.h
#        parlaylib/examples/hash_map.cpp
#        parlaylib/examples/hash_map.h
#        parlaylib/examples/huffman_tree.cpp
#        parlaylib/examples/huffman_tree.h
#        parlaylib/examples/karatsuba.cpp
#        parlaylib/examples/karatsuba.h
#        parlaylib/examples/kcore.cpp
#        parlaylib/examples/kcore.h
#        parlaylib/examples/kmeans_pp.cpp
#        parlaylib/examples/kmeans_pp.h
#        parlaylib/examples/knn.cpp
#        parlaylib/examples/knn.h
#        parlaylib/examples/knuth_morris_pratt.cpp
#        parlaylib/examples/knuth_morris_pratt.h
#        parlaylib/examples/knuth_shuffle.cpp
#        parlaylib/examples/knuth_shuffle.h
#        parlaylib/examples/kruskal.cpp
#        parlaylib/examples/kruskal.h
#        parlaylib/examples/kth_smallest.cpp
#        parlaylib/examples/kth_smallest.h
#        parlaylib/examples/lasso_regression.cpp
#        parlaylib/examples/lasso_regression.h
#        parlaylib/examples/le_list.cpp
#        parlaylib/examples/le_list.h
#        parlaylib/examples/linefit.cpp
#        parlaylib/examples/linefit.h
#        parlaylib/examples/longest_common_prefix.cpp
#        parlaylib/examples/longest_common_prefix.h
#        parlaylib/examples/longest_repeated_substring.cpp
#        parlaylib/examples/longest_repeated_substring.h
#        parlaylib/examples/low_diameter_decomposition.cpp
#        parlaylib/examples/low_diameter_decomposition.h
#        parlaylib/examples/maximal_independent_set.cpp
#        parlaylib/examples/maximal_independent_set.h
#        parlaylib/examples/maximal_matching.cpp
#        parlaylib/examples/maximal_matching.h
#        parlaylib/examples/mcss.cpp
#        parlaylib/examples/mcss.h
#        parlaylib/examples/mergesort.cpp
#        parlaylib/examples/mergesort.h
#        parlaylib/examples/minimum_edit_distance.cpp
#        parlaylib/examples/minimum_edit_distance.h
#        parlaylib/examples/nbody_fmm.cpp
#        parlaylib/examples/nbody_fmm.h
#        parlaylib/examples/oct_tree.cpp
#        parlaylib/examples/oct_tree.h
#        parlaylib/examples/pagerank.cpp
#        parlaylib/examples/pagerank.h
#        parlaylib/examples/primes.cpp
#        parlaylib/examples/primes.h
#        parlaylib/examples/push_relabel_max_flow.cpp
#        parlaylib/examples/push_relabel_max_flow.h
#        parlaylib/examples/quickhull.cpp
#        parlaylib/examples/quickhull.h
#        parlaylib/examples/quicksort.cpp
#        parlaylib/examples/quicksort.h
#        parlaylib/examples/rabin_karp.cpp
#        parlaylib/examples/rabin_karp.h
#        parlaylib/examples/radix_tree.cpp
#        parlaylib/examples/radix_tree.h
#        parlaylib/examples/range_min.cpp
#        parlaylib/examples/range_min.h
#        parlaylib/examples/ray_trace.cpp
#        parlaylib/examples/ray_trace.h
#        parlaylib/examples/rectangle_intersection.cpp
#        parlaylib/examples/rectangle_intersection.h
#        parlaylib/examples/reduce.cpp
#        parlaylib/examples/reduce.h
#        parlaylib/examples/samplesort.cpp
#        parlaylib/examples/samplesort.h
#        parlaylib/examples/scan.cpp
#        parlaylib/examples/scan.h
#        parlaylib/examples/set_cover.cpp
#        parlaylib/examples/set_cover.h
#        parlaylib/examples/spanning_tree.cpp
#        parlaylib/examples/spanning_tree.h
#        parlaylib/examples/spectral_separator.cpp
#        parlaylib/examples/spectral_separator.h
#        parlaylib/examples/suffix_array.cpp
#        parlaylib/examples/suffix_array.h
#        parlaylib/examples/suffix_tree.cpp
#        parlaylib/examples/suffix_tree.h
#        parlaylib/examples/tokens.cpp
#        parlaylib/examples/tokens.h
#        parlaylib/examples/triangle_count.cpp
#        parlaylib/examples/triangle_count.h
#        parlaylib/examples/word_counts.cpp
#        parlaylib/examples/word_counts.h
#        parlaylib/include/parlay/internal/concurrency/acquire_retire.h
#        parlaylib/include/parlay/internal/concurrency/hazptr_stack.h
#        parlaylib/include/parlay/internal/delayed/common.h
#        parlaylib/include/parlay/internal/delayed/filter.h
#        parlaylib/include/parlay/internal/delayed/filter_op.h
#        parlaylib/include/parlay/internal/delayed/flatten.h
#        parlaylib/include/parlay/internal/delayed/map.h
#        parlaylib/include/parlay/internal/delayed/scan.h
#        parlaylib/include/parlay/internal/delayed/terminal.h
#        parlaylib/include/parlay/internal/delayed/zip.h
#        parlaylib/include/parlay/internal/posix/file_map_impl_posix.h
#        parlaylib/include/parlay/internal/scheduler_plugins/cilkplus.h
#        parlaylib/include/parlay/internal/scheduler_plugins/omp.h
#        parlaylib/include/parlay/internal/scheduler_plugins/opencilk.h
#        parlaylib/include/parlay/internal/scheduler_plugins/sequential.h
#        parlaylib/include/parlay/internal/scheduler_plugins/tbb.h
#        parlaylib/include/parlay/internal/windows/file_map_impl_windows.h
#        parlaylib/include/parlay/internal/atomic_wait.h
#        parlaylib/include/parlay/internal/binary_search.h
#        parlaylib/include/parlay/internal/block_allocator.h
#        parlaylib/include/parlay/internal/block_delayed.h
#        parlaylib/include/parlay/internal/bucket_sort.h
#        parlaylib/include/parlay/internal/collect_reduce.h
#        parlaylib/include/parlay/internal/counting_sort.h
#        parlaylib/include/parlay/internal/debug_uninitialized.h
#        parlaylib/include/parlay/internal/file_map.h
#        parlaylib/include/parlay/internal/get_time.h
#        parlaylib/include/parlay/internal/group_by.h
#        parlaylib/include/parlay/internal/heap_tree.h
#        parlaylib/include/parlay/internal/integer_sort.h
#        parlaylib/include/parlay/internal/memory_size.h
#        parlaylib/include/parlay/internal/merge.h
#        parlaylib/include/parlay/internal/merge_sort.h
#        parlaylib/include/parlay/internal/pool_allocator.h
#        parlaylib/include/parlay/internal/quicksort.h
#        parlaylib/include/parlay/internal/sample_sort.h
#        parlaylib/include/parlay/internal/sequence_base.h
#        parlaylib/include/parlay/internal/sequence_ops.h
#        parlaylib/include/parlay/internal/stream_delayed.h
#        parlaylib/include/parlay/internal/transpose.h
#        parlaylib/include/parlay/internal/uninitialized_sequence.h
#        parlaylib/include/parlay/internal/uninitialized_storage.h
#        parlaylib/include/parlay/internal/work_stealing_deque.h
#        parlaylib/include/parlay/internal/work_stealing_job.h
#        parlaylib/include/parlay/alloc.h
#        parlaylib/include/parlay/delayed.h
#        parlaylib/include/parlay/delayed_sequence.h
#        parlaylib/include/parlay/hash_table.h
#        parlaylib/include/parlay/io.h
#        parlaylib/include/parlay/monoid.h
#        parlaylib/include/parlay/parallel.h
#        parlaylib/include/parlay/portability.h
#        parlaylib/include/parlay/primitives.h
#        parlaylib/include/parlay/random.h
#        parlaylib/include/parlay/range.h
#        parlaylib/include/parlay/scheduler.h
#        parlaylib/include/parlay/sequence.h
#        parlaylib/include/parlay/slice.h
#        parlaylib/include/parlay/type_traits.h
#        parlaylib/include/parlay/utilities.h
#        parlaylib/test/range_utils.h
#        parlaylib/test/sorting_utils.h
#        parlaylib/test/test_allocator.cpp
#        parlaylib/test/test_bucket_sort.cpp
#        parlaylib/test/test_collect_reduce.cpp
#        parlaylib/test/test_counting_sort.cpp
#        parlaylib/test/test_delayed_filter.cpp
#        parlaylib/test/test_delayed_filter_op.cpp
#        parlaylib/test/test_delayed_flatten.cpp
#        parlaylib/test/test_delayed_for_each.cpp
#        parlaylib/test/test_delayed_map.cpp
#        parlaylib/test/test_delayed_reduce.cpp
#        parlaylib/test/test_delayed_scan.cpp
#        parlaylib/test/test_delayed_sequence.cpp
#        parlaylib/test/test_delayed_zip.cpp
#        parlaylib/test/test_file_map.cpp
#        parlaylib/test/test_formatting.cpp
#        parlaylib/test/test_group_by.cpp
#        parlaylib/test/test_hash_table.cpp
#        parlaylib/test/test_hazptr_stack.cpp
#        parlaylib/test/test_integer_sort.cpp
#        parlaylib/test/test_io.cpp
#        parlaylib/test/test_merge_sort.cpp
#        parlaylib/test/test_monoid.cpp
#        parlaylib/test/test_padded.cpp
#        parlaylib/test/test_parallel.cpp
#        parlaylib/test/test_parsing.cpp
#        parlaylib/test/test_primitives.cpp
#        parlaylib/test/test_quicksort.cpp
#        parlaylib/test/test_random.cpp
#        parlaylib/test/test_range.cpp
#        parlaylib/test/test_relocate.cpp
#        parlaylib/test/test_sample_sort.cpp
#        parlaylib/test/test_separate_compilation1.cpp
#        parlaylib/test/test_sequence.cpp
#        parlaylib/test/test_slice.cpp
#        parlaylib/test/test_sorting_primitives.cpp
#        parlaylib/test/test_transpose.cpp
#        parlaylib/test/test_uninitialized_memory.cpp
#        python/builder.cpp
#        python/graph_index.cpp
#        python/module.cpp
#        rangeSearch/bench/get_time.h
#        rangeSearch/bench/IO.h
#        rangeSearch/bench/rangeTime.C
#        rangeSearch/vamanaRange/range.h)

find_package(Parlay REQUIRED)

#add_subdirectory(parlaylib)
include_directories(parlaylib/include)

add_executable(main src/neighborsTime.cpp)
target_link_libraries(main PRIVATE Parlay::parlay)

