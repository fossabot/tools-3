/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE linalg_test

// Standard includes
#include <iostream>

// Third party includes
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/tools/eigenio_matrixmarket.h"
#include "votca/tools/linalg.h"

using namespace votca::tools;

BOOST_AUTO_TEST_SUITE(linalg_test)

BOOST_AUTO_TEST_CASE(linalg_constrained_qrsolve_test) {

  Eigen::VectorXd b = Eigen::VectorXd::Zero(3);
  b(0) = 11;
  b(1) = -3;
  b(2) = 8;
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero(3, 3);
  A(0, 0) = 1;
  A(0, 1) = 1;
  A(0, 2) = 1;
  A(1, 0) = 1;
  A(1, 1) = -1;
  A(2, 1) = 1;
  A(2, 2) = 1;

  Eigen::MatrixXd B = Eigen::MatrixXd::Zero(1, 3);
  B(0, 1) = -1;
  B(0, 2) = 3;
  Eigen::VectorXd x = linalg_constrained_qrsolve(A, b, B);
  Eigen::VectorXd x_ref = Eigen::VectorXd::Zero(3);
  x_ref(0) = 3;
  x_ref(1) = 6;
  x_ref(2) = 2;

  bool equal = x_ref.isApprox(x, 1e-7);

  if (!equal) {
    std::cout << "result" << std::endl;
    std::cout << x << std::endl;
    std::cout << "ref" << std::endl;
    std::cout << x_ref << std::endl;
  }
  BOOST_CHECK_EQUAL(equal, true);
}

BOOST_AUTO_TEST_CASE(linalg_mkl_test) {

  votca::Index nmax = 10;

  Eigen::MatrixXd H = EigenIO_MatrixMarket::ReadMatrix(
      std::string(TOOLS_TEST_DATA_FOLDER) + "/linalg/H.mm");

  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(H);
  Eigen::MatrixXd V_ref = es.eigenvectors().block(0, 0, H.rows(), nmax);
  Eigen::VectorXd E_ref = es.eigenvalues().segment(0, nmax);

  EigenSystem result = linalg_eigenvalues(H, nmax);
  bool check_energy = E_ref.isApprox(result.eigenvalues(), 0.0001);

  bool check_vector =
      result.eigenvectors().cwiseAbs().isApprox(V_ref.cwiseAbs(), 0.0001);

  if (!check_vector || !check_energy) {
    std::cout << "E_ref" << std::endl;
    std::cout << E_ref << std::endl;
    std::cout << "E" << std::endl;
    std::cout << result.eigenvalues() << std::endl;
    std::cout << "V_ref" << std::endl;
    std::cout << V_ref << std::endl;
    std::cout << "V" << std::endl;
    std::cout << result.eigenvectors() << std::endl;
  }

  BOOST_CHECK_EQUAL(check_energy, 1);
  BOOST_CHECK_EQUAL(check_vector, 1);
}

BOOST_AUTO_TEST_SUITE_END()
