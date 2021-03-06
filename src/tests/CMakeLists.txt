find_package(Boost 1.53.0 REQUIRED COMPONENTS unit_test_framework)

# Each test listed in Alphabetical order
foreach(PROG
    test_calculator
    test_constants
    test_correlate
    test_crosscorrelate
    test_cubicspline
    test_datacollection
    test_edge_base
    test_edgecontainer
    test_elements
    test_filesystem
    test_floatingpointcomparison
    test_graphalgorithm
    test_graph_base
    test_graph_bf_visitor
    test_graph_df_visitor
    test_graphdistvisitor
    test_graphnode
    test_graphvisitor
    test_histogramnew
    test_identity
    test_linalg
    test_name
    test_property
    test_reducededge
    test_reducedgraph
    test_structureparameters
    test_table
    test_thread
    test_tokenizer
    test_random
    test_akimaspline
    test_linspline
    test_unitconverter
    test_eigenio_matrixmarket)

  file(GLOB ${PROG}_SOURCES ${PROG}*.cc)
  add_executable(unit_${PROG} ${${PROG}_SOURCES})
  target_compile_definitions(unit_${PROG} PRIVATE TOOLS_TEST_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/DataFiles")
  target_link_libraries(unit_${PROG} votca_tools Boost::unit_test_framework Boost::filesystem)
  target_compile_definitions(unit_${PROG} PRIVATE BOOST_TEST_DYN_LINK)
  add_test(unit_${PROG} unit_${PROG})
  # run tests for tools (for coverage) as well
  set_tests_properties(unit_${PROG} PROPERTIES LABELS "tools;votca;unit")

  if(VALGRIND_FOUND)
    # https://github.com/votca/tools/issues/268
    # tests seem to trigger a 'Address 0x0 is not stack'd, malloc'd or (recently) free'd' in valgrind
    set(BROCKEN_MKL_TEST test_crosscorrelate test_cubicspline test_linalg test_linspline)
    if(MKL_FOUND AND PROG IN_LIST BROCKEN_MKL_TEST)
      continue()
    endif()
    add_test(NAME memory_${PROG} COMMAND "${VALGRIND_EXECUTABLE}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --error-exitcode=1 $<TARGET_FILE:unit_${PROG}>)
    set_tests_properties(memory_${PROG} PROPERTIES LABELS "tools;votca;memory")
  endif(VALGRIND_FOUND) 
endforeach(PROG)
