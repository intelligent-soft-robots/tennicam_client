#pragma once

#include <armadillo> // vec
#include <array>
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

namespace tennicam_client
{
  class Transform
  {
  public:
    Transform(std::array<double,3> translation,
	      std::array<double,3> rotation);
    std::array<double,3> apply(const std::array<double,3>& v) const;
  private:
    arma::vec translation_;
    arma::mat rotation_;
    mutable arma::vec tmp_;
  };

  std::tuple<std::array<double,3>,
	     std::array<double,3>> read_transform_from_memory(std::string segment_id);
  void write_transform_to_memory(std::string segment_id,
		      const std::array<double,3>& translation,
		      const std::array<double,3>& rotation);
  
}
