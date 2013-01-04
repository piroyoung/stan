#define _LOG_PROB_ neg_binomial_log
#include <stan/prob/distributions/univariate/discrete/neg_binomial.hpp>
#include <stan/prob/distributions/univariate/discrete/binomial.hpp>

#include <test/prob/distributions/distribution_test_fixture.hpp>
#include <test/prob/distributions/distribution_tests_1_discrete_2_params.hpp>

using std::vector;
using std::log;
using std::numeric_limits;

class ProbDistributionsNegBinomial : public DistributionTest {
public:
  void valid_values(vector<vector<double> >& parameters,
                    vector<double>& log_prob) {
    vector<double> param(3);

    param[0] = 10;           // n
    param[1] = 2.0;          // alpha
    param[2] = 1.5;          // beta
    parameters.push_back(param);
    log_prob.push_back(-7.786663); // expected log_prob

    param[0] = 100;          // n
    param[1] = 3.0;          // alpha
    param[2] = 3.5;          // beta
    parameters.push_back(param);
    log_prob.push_back(-142.6147); // expected log_prob

    param[0] = 13;
    param[1] = 1e11; // alpha > 1e10, causes redux to Poisson
    param[2] = 1e10; // equiv to Poisson(1e11/1e10) = Poisson(10)
    parameters.push_back(param);
    log_prob.push_back(-2.618558); // log poisson(13|10)
  }
 
  void invalid_values(vector<size_t>& index, 
                      vector<double>& value) {
    // n
    index.push_back(0U);
    value.push_back(-1);
    
    // alpha
    index.push_back(1U);
    value.push_back(0);
    
    // beta
    index.push_back(2U);
    value.push_back(0);
  }
};

INSTANTIATE_TYPED_TEST_CASE_P(ProbDistributionsNegBinomial,
                              DistributionTestFixture,
                              ProbDistributionsNegBinomial);


const double p = 0.57;
const double beta = p / (1.0 - p);

TEST(ProbDistributionsNegBinomialCDF,Values) {
    EXPECT_FLOAT_EQ(0.042817421, stan::prob::neg_binomial_cdf(30, 24, beta));
    
    // Consistency with implemented Binomial
    EXPECT_FLOAT_EQ(stan::prob::binomial_cdf(24, 54, p), stan::prob::neg_binomial_cdf(30, 24, beta));
}

/*




#include <gtest/gtest.h>
#include <stan/prob/distributions/univariate/discrete/neg_binomial.hpp>

TEST(ProbDistributionsNegBinomial,NegBinomial) {
  EXPECT_FLOAT_EQ(-7.786663, stan::prob::neg_binomial_log(10,2.0,1.5));
  EXPECT_FLOAT_EQ(-142.6147, stan::prob::neg_binomial_log(100,3.0,3.5));
}
TEST(ProbDistributionsNegBinomial,Propto) {
  EXPECT_FLOAT_EQ(0.0, stan::prob::neg_binomial_log<true>(10,2.0,1.5));
  EXPECT_FLOAT_EQ(0.0, stan::prob::neg_binomial_log<true>(100,3.0,3.5));
}

using boost::math::policies::policy;
using boost::math::policies::evaluation_error;
using boost::math::policies::domain_error;
using boost::math::policies::overflow_error;
using boost::math::policies::domain_error;
using boost::math::policies::pole_error;
using boost::math::policies::errno_on_error;

typedef policy<
  domain_error<errno_on_error>, 
  pole_error<errno_on_error>,
  overflow_error<errno_on_error>,
  evaluation_error<errno_on_error> 
  > errno_policy;

using stan::prob::neg_binomial_log;

TEST(ProbDistributionsNegBinomial,DefaultPolicy) {
  double nan = std::numeric_limits<double>::quiet_NaN();
  double inf = std::numeric_limits<double>::infinity();
  
  unsigned int n = 10;
  double alpha = 2.0;
  double beta = 1.5;

  EXPECT_NO_THROW(neg_binomial_log(n,alpha,beta));
  
  EXPECT_THROW(neg_binomial_log(n,nan,beta), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,0.0,beta), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,-1.0,beta), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,-inf,beta), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,inf,beta), std::domain_error);

  EXPECT_THROW(neg_binomial_log(n,alpha,nan), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,alpha,0.0), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,alpha,-1.0), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,alpha,-inf), std::domain_error);
  EXPECT_THROW(neg_binomial_log(n,alpha,inf), std::domain_error);
}
TEST(ProbDistributionsNegBinomial,ErrnoPolicy) {
  double nan = std::numeric_limits<double>::quiet_NaN();
  double inf = std::numeric_limits<double>::infinity();
  
  double result;
  unsigned int n = 10;
  double alpha = 2.0;
  double beta = 1.5;

  result = neg_binomial_log(n,alpha,beta, errno_policy());
  EXPECT_FALSE(std::isnan(result));

  result = neg_binomial_log(n,nan,beta, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,0.0,beta, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,-1.0,beta, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,-inf,beta, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,inf,beta, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  
  result = neg_binomial_log(n,alpha,nan, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,alpha,0.0, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,alpha,-1.0, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,alpha,-inf, errno_policy());
  EXPECT_TRUE(std::isnan(result));
  result = neg_binomial_log(n,alpha,inf, errno_policy());
  EXPECT_TRUE(std::isnan(result));
}
TEST(ProbDistributionsNegBinomial,check_values) {
  int n;
  double alpha;
  double beta;
  double result;
  

  alpha = 82.34;
  beta = 0.366114;
  n = 0;
  result = neg_binomial_log(n, alpha, beta);
  EXPECT_FLOAT_EQ(-108.423725446514, result);

  n = 1;
  result = neg_binomial_log(n, alpha, beta);
  EXPECT_FLOAT_EQ(-104.324838643183, result);
  
  n = 2;
  result = neg_binomial_log(n, alpha, beta);
  EXPECT_FLOAT_EQ(-100.907027410759, result);

  n = 4;
  result = neg_binomial_log(n, alpha, beta);
  EXPECT_FLOAT_EQ(-95.1343749604898, result);
  
  n = 5;
  result = neg_binomial_log(n, alpha, beta);
  EXPECT_FLOAT_EQ(-92.597490095807, result);
}
*/
