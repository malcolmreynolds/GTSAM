/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file   testJacobianFactor.cpp
 *  @brief  Unit tests for Linear Factor
 *  @author Christian Potthast
 *  @author Frank Dellaert
 **/

#include <gtsam/base/TestableAssertions.h>
#include <CppUnitLite/TestHarness.h>

#include <gtsam/linear/JacobianFactor.h>
#include <gtsam/linear/HessianFactor.h>
#include <gtsam/linear/GaussianConditional.h>
#include <gtsam/linear/GaussianFactorGraph.h>

using namespace std;
using namespace gtsam;

static const Index _x0_=0, _x1_=1, _x2_=2,  _x_=5, _y_=6, _l11_=8;

static SharedDiagonal   constraintModel = noiseModel::Constrained::All(2);

/* ************************************************************************* */
TEST(JacobianFactor, constructor)
{
  Vector b = Vector_(3, 1., 2., 3.);
  SharedDiagonal noise = noiseModel::Diagonal::Sigmas(Vector_(3,1.,1.,1.));
  std::list<std::pair<Index, Matrix> > terms;
  terms.push_back(make_pair(_x0_, eye(3)));
  terms.push_back(make_pair(_x1_, 2.*eye(3)));
  JacobianFactor actual(terms, b, noise);
  JacobianFactor expected(_x0_, eye(3), _x1_, 2.*eye(3), b, noise);
  EXPECT(assert_equal(expected, actual));
}

/* ************************************************************************* */
TEST(JacobianFactor, constructor2)
{
  Vector b = Vector_(3, 1., 2., 3.);
  SharedDiagonal noise = noiseModel::Diagonal::Sigmas(Vector_(3,1.,1.,1.));
  std::list<std::pair<Index, Matrix> > terms;
  terms.push_back(make_pair(_x0_, eye(3)));
  terms.push_back(make_pair(_x1_, 2.*eye(3)));
  const JacobianFactor actual(terms, b, noise);

  JacobianFactor::const_iterator key0 = actual.begin();
  JacobianFactor::const_iterator key1 = key0 + 1;
  EXPECT(assert_equal(*key0, _x0_));
  EXPECT(assert_equal(*key1, _x1_));
  EXPECT(!actual.empty());
  EXPECT_LONGS_EQUAL(3, actual.Ab_.nBlocks());

  Matrix actualA0 = actual.getA(key0);
  Matrix actualA1 = actual.getA(key1);
  Vector actualb = actual.getb();

  EXPECT(assert_equal(eye(3), actualA0));
  EXPECT(assert_equal(2.*eye(3), actualA1));
  EXPECT(assert_equal(b, actualb));
}

/* ************************************************************************* */

JacobianFactor construct() {
  Matrix A = Matrix_(2,2, 1.,2.,3.,4.);
  Vector b = Vector_(2, 1.0, 2.0);
  SharedDiagonal s = noiseModel::Diagonal::Sigmas(Vector_(2, 3.0, 4.0));
  JacobianFactor::shared_ptr shared(
      new JacobianFactor(0, A, b, s));
  return *shared;
}

TEST(JacobianFactor, return_value)
{
  Matrix A = Matrix_(2,2, 1.,2.,3.,4.);
  Vector b = Vector_(2, 1.0, 2.0);
  SharedDiagonal s = noiseModel::Diagonal::Sigmas(Vector_(2, 3.0, 4.0));
  JacobianFactor copied = construct();
  EXPECT(assert_equal(b, copied.getb()));
  EXPECT(assert_equal(*s, *copied.get_model()));
  EXPECT(assert_equal(A, copied.getA(copied.begin())));
}

/* ************************************************************************* */
TEST(JabobianFactor, Hessian_conversion) {
  HessianFactor hessian(0, (Matrix(4,4) <<
        1.57,        2.695,         -1.1,        -2.35,
       2.695,      11.3125,        -0.65,      -10.225,
        -1.1,        -0.65,            1,          0.5,
       -2.35,      -10.225,          0.5,         9.25).finished(),
      (Vector(4) << -7.885, -28.5175, 2.75, 25.675).finished(),
      73.1725);

  JacobianFactor expected(0, (Matrix(2,4) <<
      1.2530,   2.1508,   -0.8779,  -1.8755,
           0,   2.5858,    0.4789,  -2.3943).finished(),
      (Vector(2) << -6.2929, -5.7941).finished(),
      noiseModel::Unit::Create(2));

  JacobianFactor actual(hessian);

  EXPECT(assert_equal(expected, actual, 1e-3));
}

/* ************************************************************************* */
TEST( JacobianFactor, constructor_combined){
  double sigma1 = 0.0957;
  Matrix A11(2,2);
  A11(0,0) = 1; A11(0,1) =  0;
  A11(1,0) = 0;       A11(1,1) = 1;
  Vector b(2);
  b(0) = 2; b(1) = -1;
  JacobianFactor::shared_ptr f1(new JacobianFactor(0, A11, b*sigma1, noiseModel::Isotropic::Sigma(2,sigma1)));

  double sigma2 = 0.5;
  A11(0,0) = 1; A11(0,1) =  0;
  A11(1,0) = 0; A11(1,1) = -1;
  b(0) = 4 ; b(1) = -5;
  JacobianFactor::shared_ptr f2(new JacobianFactor(0, A11, b*sigma2, noiseModel::Isotropic::Sigma(2,sigma2)));

  double sigma3 = 0.25;
  A11(0,0) = 1; A11(0,1) =  0;
  A11(1,0) = 0; A11(1,1) = -1;
  b(0) = 3 ; b(1) = -88;
  JacobianFactor::shared_ptr f3(new JacobianFactor(0, A11, b*sigma3, noiseModel::Isotropic::Sigma(2,sigma3)));

  // TODO: find a real sigma value for this example
  double sigma4 = 0.1;
  A11(0,0) = 6; A11(0,1) =  0;
  A11(1,0) = 0; A11(1,1) = 7;
  b(0) = 5 ; b(1) = -6;
  JacobianFactor::shared_ptr f4(new JacobianFactor(0, A11*sigma4, b*sigma4, noiseModel::Isotropic::Sigma(2,sigma4)));

  GaussianFactorGraph lfg;
  lfg.push_back(f1);
  lfg.push_back(f2);
  lfg.push_back(f3);
  lfg.push_back(f4);
  JacobianFactor combined(lfg);

  Vector sigmas = Vector_(8, sigma1, sigma1, sigma2, sigma2, sigma3, sigma3, sigma4, sigma4);
  Matrix A22(8,2);
  A22(0,0) = 1;   A22(0,1) =  0;
  A22(1,0) = 0;   A22(1,1) = 1;
  A22(2,0) = 1;   A22(2,1) =  0;
  A22(3,0) = 0;   A22(3,1) = -1;
  A22(4,0) = 1;   A22(4,1) =  0;
  A22(5,0) = 0;   A22(5,1) = -1;
  A22(6,0) = 0.6; A22(6,1) =  0;
  A22(7,0) = 0;   A22(7,1) =  0.7;
  Vector exb(8);
  exb(0) = 2*sigma1 ; exb(1) = -1*sigma1;  exb(2) = 4*sigma2 ; exb(3) = -5*sigma2;
  exb(4) = 3*sigma3 ; exb(5) = -88*sigma3; exb(6) = 5*sigma4 ; exb(7) = -6*sigma4;

  vector<pair<Index, Matrix> > meas;
  meas.push_back(make_pair(0, A22));
  JacobianFactor expected(meas, exb, noiseModel::Diagonal::Sigmas(sigmas));
  EXPECT(assert_equal(expected,combined));
}

/* ************************************************************************* */
TEST(JacobianFactor, linearFactorN){
  Matrix I = eye(2);
  GaussianFactorGraph f;
  SharedDiagonal model = noiseModel::Isotropic::Sigma(2,1.0);
  f.push_back(JacobianFactor::shared_ptr(new JacobianFactor(0, I, Vector_(2, 10.0, 5.0), model)));
  f.push_back(JacobianFactor::shared_ptr(new JacobianFactor(0, -10 * I, 1, 10 * I, Vector_(2, 1.0, -2.0), model)));
  f.push_back(JacobianFactor::shared_ptr(new JacobianFactor(1, -10 * I, 2, 10 * I, Vector_(2, 1.5, -1.5), model)));
  f.push_back(JacobianFactor::shared_ptr(new JacobianFactor(2, -10 * I, 3, 10 * I, Vector_(2, 2.0, -1.0), model)));

  JacobianFactor combinedFactor(f);

  vector<pair<Index, Matrix> > combinedMeasurement;
  combinedMeasurement.push_back(make_pair(0, Matrix_(8,2,
    1.0,  0.0,
    0.0,  1.0,
    -10.0,  0.0,
    0.0,-10.0,
    0.0,  0.0,
    0.0,  0.0,
    0.0,  0.0,
    0.0,  0.0)));
  combinedMeasurement.push_back(make_pair(1, Matrix_(8,2,
    0.0,  0.0,
    0.0,  0.0,
    10.0,  0.0,
    0.0, 10.0,
    -10.0,  0.0,
    0.0,-10.0,
    0.0,  0.0,
    0.0,  0.0)));
  combinedMeasurement.push_back(make_pair(2, Matrix_(8,2,
    0.0,  0.0,
    0.0,  0.0,
    0.0,  0.0,
    0.0,  0.0,
    10.0,  0.0,
    0.0, 10.0,
    -10.0,  0.0,
    0.0,-10.0)));
  combinedMeasurement.push_back(make_pair(3, Matrix_(8,2,
    0.0, 0.0,
    0.0, 0.0,
    0.0, 0.0,
    0.0, 0.0,
    0.0, 0.0,
    0.0, 0.0,
    10.0, 0.0,
    0.0,10.0)));
  Vector b = Vector_(8,
    10.0, 5.0, 1.0, -2.0, 1.5, -1.5, 2.0, -1.0);

  Vector sigmas = repeat(8,1.0);
  JacobianFactor expected(combinedMeasurement, b, noiseModel::Diagonal::Sigmas(sigmas));
  EXPECT(assert_equal(expected,combinedFactor));
}

/* ************************************************************************* */
TEST(JacobianFactor, error) {
  Vector b = Vector_(3, 1., 2., 3.);
  SharedDiagonal noise = noiseModel::Diagonal::Sigmas(Vector_(3,2.,2.,2.));
  std::list<std::pair<Index, Matrix> > terms;
  terms.push_back(make_pair(_x0_, eye(3)));
  terms.push_back(make_pair(_x1_, 2.*eye(3)));
  const JacobianFactor jf(terms, b, noise);

  VectorValues values(2, 3);
  values[0] = Vector_(3, 1.,2.,3.);
  values[1] = Vector_(3, 4.,5.,6.);

  Vector expected_unwhitened = Vector_(3, 8., 10., 12.);
  Vector actual_unwhitened = jf.unweighted_error(values);
  EXPECT(assert_equal(expected_unwhitened, actual_unwhitened));

  Vector expected_whitened = Vector_(3, 4., 5., 6.);
  Vector actual_whitened = jf.error_vector(values);
  EXPECT(assert_equal(expected_whitened, actual_whitened));

  // check behavior when there are more values than in this factor
  VectorValues largeValues(3, 3);
  largeValues[0] = Vector_(3, 1.,2.,3.);
  largeValues[1] = Vector_(3, 4.,5.,6.);
  largeValues[2] = Vector_(3, 7.,8.,9.);

  EXPECT(assert_equal(expected_unwhitened, jf.unweighted_error(largeValues)));
  EXPECT(assert_equal(expected_whitened, jf.error_vector(largeValues)));
}

/* ************************************************************************* */
TEST(JacobianFactor, operators )
{
  SharedDiagonal  sigma0_1 = noiseModel::Isotropic::Sigma(2,0.1);

  Matrix I = eye(2);
  Vector b = Vector_(2,0.2,-0.1);
  JacobianFactor lf(_x1_, -I, _x2_, I, b, sigma0_1);

  VectorValues c;
  c.insert(_x1_, Vector_(2,10.,20.));
  c.insert(_x2_, Vector_(2,30.,60.));

  // test A*x
  Vector expectedE = Vector_(2,200.,400.), e = lf*c;
  EXPECT(assert_equal(expectedE,e));

  // test A^e
  VectorValues expectedX;
  expectedX.insert(_x1_, Vector_(2,-2000.,-4000.));
  expectedX.insert(_x2_, Vector_(2, 2000., 4000.));
  VectorValues actualX = VectorValues::Zero(expectedX);
  lf.transposeMultiplyAdd(1.0, e, actualX);
  EXPECT(assert_equal(expectedX, actualX));
}

/* ************************************************************************* */
TEST(JacobianFactor, eliminate2 )
{
  // sigmas
  double sigma1 = 0.2;
  double sigma2 = 0.1;
  Vector sigmas = Vector_(4, sigma1, sigma1, sigma2, sigma2);

  // the combined linear factor
  Matrix Ax2 = Matrix_(4,2,
      // x2
      -1., 0.,
      +0.,-1.,
      1., 0.,
      +0.,1.
  );

  Matrix Al1x1 = Matrix_(4,4,
      // l1   x1
      1., 0., 0.00,  0., // f4
      0., 1., 0.00,  0., // f4
      0., 0., -1.,  0., // f2
      0., 0., 0.00,-1.  // f2
  );

  // the RHS
  Vector b2(4);
  b2(0) = -0.2;
  b2(1) =  0.3;
  b2(2) =  0.2;
  b2(3) = -0.1;

  vector<pair<Index, Matrix> > meas;
  meas.push_back(make_pair(_x2_, Ax2));
  meas.push_back(make_pair(_l11_, Al1x1));
  JacobianFactor combined(meas, b2, noiseModel::Diagonal::Sigmas(sigmas));

  // eliminate the combined factor
  GaussianConditional::shared_ptr actualCG_QR;
  JacobianFactor::shared_ptr actualLF_QR(new JacobianFactor(combined));
  actualCG_QR = actualLF_QR->eliminateFirst();

  // create expected Conditional Gaussian
  double oldSigma = 0.0894427; // from when R was made unit
  Matrix R11 = Matrix_(2,2,
      1.00,  0.00,
      0.00,  1.00
  )/oldSigma;
  Matrix S12 = Matrix_(2,4,
      -0.20, 0.00,-0.80, 0.00,
      +0.00,-0.20,+0.00,-0.80
  )/oldSigma;
  Vector d = Vector_(2,0.2,-0.14)/oldSigma;
  GaussianConditional expectedCG(_x2_,d,R11,_l11_,S12,ones(2));

  EXPECT_LONGS_EQUAL(0, actualCG_QR->rsd().firstBlock());
  EXPECT_LONGS_EQUAL(0, actualCG_QR->rsd().rowStart());
  EXPECT_LONGS_EQUAL(2, actualCG_QR->rsd().rowEnd());
  EXPECT_LONGS_EQUAL(3, actualCG_QR->rsd().nBlocks());
  EXPECT(assert_equal(expectedCG,*actualCG_QR,1e-4));

  // the expected linear factor
  double sigma = 0.2236;
  Matrix Bl1x1 = Matrix_(2,4,
      // l1          x1
      1.00, 0.00, -1.00,  0.00,
      0.00, 1.00, +0.00, -1.00
  )/sigma;
  Vector b1 = Vector_(2,0.0,0.894427);
  JacobianFactor expectedLF(_l11_, Bl1x1, b1, noiseModel::Isotropic::Sigma(2,1.0));
  EXPECT(assert_equal(expectedLF,*actualLF_QR,1e-3));
}

/* ************************************************************************* */
TEST(JacobianFactor, default_error )
{
  JacobianFactor f;
  vector<size_t> dims;
  VectorValues c(dims);
  double actual = f.error(c);
  EXPECT(actual==0.0);
}

//* ************************************************************************* */
TEST(JacobianFactor, empty )
{
  // create an empty factor
  JacobianFactor f;
  EXPECT(f.empty()==true);
}

/* ************************************************************************* */
// small aux. function to print out lists of anything
template<class T>
void print(const list<T>& i) {
  copy(i.begin(), i.end(), ostream_iterator<T> (cout, ","));
  cout << endl;
}

/* ************************************************************************* */
TEST(JacobianFactor, CONSTRUCTOR_GaussianConditional )
{
  Matrix R11 = eye(2);
  Matrix S12 = Matrix_(2,2,
      -0.200001, 0.00,
      +0.00,-0.200001
  );
  Vector d(2); d(0) = 2.23607; d(1) = -1.56525;
  Vector sigmas =repeat(2,0.29907);
  GaussianConditional::shared_ptr CG(new GaussianConditional(_x2_,d,R11,_l11_,S12,sigmas));

  // Call the constructor we are testing !
  JacobianFactor actualLF(*CG);

  JacobianFactor expectedLF(_x2_,R11,_l11_,S12,d, noiseModel::Diagonal::Sigmas(sigmas));
  EXPECT(assert_equal(expectedLF,actualLF,1e-5));
}

/* ************************************************************************* */
TEST ( JacobianFactor, constraint_eliminate1 )
{
  // construct a linear constraint
  Vector v(2); v(0)=1.2; v(1)=3.4;
  Index key = _x0_;
  JacobianFactor lc(key, eye(2), v, constraintModel);

  // eliminate it
  GaussianConditional::shared_ptr actualCG;
  JacobianFactor::shared_ptr actualLF(new JacobianFactor(lc));
  actualCG = actualLF->eliminateFirst();

  // verify linear factor
  EXPECT(actualLF->size() == 0);

  // verify conditional Gaussian
  Vector sigmas = Vector_(2, 0.0, 0.0);
  GaussianConditional expCG(_x0_, v, eye(2), sigmas);
  EXPECT(assert_equal(expCG, *actualCG));
}

/* ************************************************************************* */
TEST ( JacobianFactor, constraint_eliminate2 )
{
  // Construct a linear constraint
  // RHS
  Vector b(2); b(0)=3.0; b(1)=4.0;

  // A1 - invertible
  Matrix A1(2,2);
  A1(0,0) = 1.0 ; A1(0,1) = 2.0;
  A1(1,0) = 2.0 ; A1(1,1) = 1.0;

  // A2 - not invertible
  Matrix A2(2,2);
  A2(0,0) = 1.0 ; A2(0,1) = 2.0;
  A2(1,0) = 2.0 ; A2(1,1) = 4.0;

  JacobianFactor lc(_x_, A1, _y_, A2, b, constraintModel);

  // eliminate x and verify results
  GaussianConditional::shared_ptr actualCG;
  JacobianFactor::shared_ptr actualLF(new JacobianFactor(lc));
  actualCG = actualLF->eliminateFirst();

  // LF should be null
  JacobianFactor expectedLF;
  EXPECT(assert_equal(*actualLF, expectedLF));

  // verify CG
  Matrix R = Matrix_(2, 2,
      1.0,    2.0,
      0.0,    1.0);
  Matrix S = Matrix_(2,2,
      1.0,    2.0,
      0.0,    0.0);
  Vector d = Vector_(2, 3.0, 0.6666);
  GaussianConditional expectedCG(_x_, d, R, _y_, S, zero(2));
  EXPECT(assert_equal(expectedCG, *actualCG, 1e-4));
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr);}
/* ************************************************************************* */
