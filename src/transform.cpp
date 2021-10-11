#include "tennicam_client/transform.hpp"


namespace tennicam_client
{

  Transform::Transform(std::array<double,3> translation,
		       std::array<double,3> rotation)
  /*: translation_{translation[0],translation[1],translation[2]},
      rotation_(3,3),
      tmp_(3,arma::fill::zeros)*/
    {

      /*arma::Mat<double> Rx(3,3,arma::fill::zeros);
      Rx(0,0)=1;
      Rx(1,1)=+cos(_rotation[0]);
      Rx(1,2)=+sin(_rotation[0]);
      Rx(2,1)=-sin(_rotation[0]);
      Rx(2,2)=+cos(_rotation[0]);

      arma::Mat<double> Ry(3,3,arma::fill::zeros);
      Ry(0,0)=+cos(_rotation[1]);
      Ry(0,2)=-sin(_rotation[1]);
      Ry(1,1)=1;
      Ry(2,0)=+sin(_rotation[1]);
      Ry(2,2)=+cos(_rotation[1]);

      arma::Mat<double> Rz(3,3,arma::fill::zeros);
      Rz(0,0)=+cos(_rotation[2]);
      Rz(0,1)=+sin(_rotation[2]);
      Rz(1,0)=-sin(_rotation[2]);
      Rz(1,1)=+cos(_rotation[2]);
      Rz(2,2)=1;
      
      rotation_ = Rz*Ry*Rx;*/
      
    }
    
  std::array<double,3> Transform::apply(const std::array<double,3>& v) const
  {
    /*for(std::size_t index=0;index<3;index++)
      tmp_[index] = v[index];
    arma::vec transformed = rotation_*tmp_ + translation_;
    return std::array<double,3>{transformed[0],transformed[1],transformed[2]};*/
    return std::array<double,3>();
  }

}
