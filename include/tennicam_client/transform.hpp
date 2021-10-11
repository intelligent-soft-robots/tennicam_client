#pragma once

#include <armadillo> // vec
#include <array>

namespace tennicam_client
{
  class Transform
  {
  public:
    Transform(){};
    Transform(std::array<double,3> translation,
	      std::array<double,3> rotation);
    std::array<double,3> apply(const std::array<double,3>& v) const;
    private:
    arma::vec translation_;
    //arma::mat rotation_;
    //mutable arma::vec tmp_;
  };

}
