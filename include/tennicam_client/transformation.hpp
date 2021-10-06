#pragma once

#include <armadillo>
#include <array>

namespace tennicam_ball
{

  class Transform
  {
  public:
    Transform(std::array<double,3> translation,
	      std::array<double,3> rotation>)
      : translation_{translation.data,3}
    {

      Mat<double> Rx(3,3,fill::zeros);
      Rx(0,0)=1;
      Rx(1,1)=+cos(rotation[0]);
      Rx(1,2)=+sin(rotation[0]);
      Rx(2,1)=-sin(rotation[0]);
      Rx(2,2)=+cos(rotation[0]);

      Mat<double> Ry(3,3,fill::zeros);
      Ry(0,0)=+cos(rotation[1]);
      Ry(0,2)=-sin(rotation[1]);
      Ry(1,1)=1;
      Ry(2,0)=+sin(rotation[1]);
      Ry(2,2)=+cos(rotation[1]);

      Mat<double> Rz(3,3,fill::zeros);
      Rz(0,0)=+cos(rotation[2]);
      Rz(0,1)=+sin(rotation[2]);
      Rz(1,0)=-sin(rotation[2]);
      Rz(1,1)=+cos(rotation[2]);
      Rz(2,2)=1;
      
      rotation_ = Rz*Ry*Rx;
      
    }
    
    std::array<double,3> apply(const std::array<double,3>& v) const
    {
      vec transformed = rotation_*v.data + translation;
      return std::array<double,3>{transformed[0],transformed[1],transformed[2]}
    }
    
    
  private:
    vec translation_;
    vec rotation_;
      
  }

}
