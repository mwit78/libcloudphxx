// including it first not to require pthread option to nvcc
//#include <blitz/array.h>

#define THRUST_DEVICE_SYSTEM THRUST_DEVICE_SYSTEM_CUDA

#include "particles.tpp"

namespace libcloudphxx
{ 
  namespace common
  {
    namespace prtcls
    {
      // instantiation 
      template class particles<float, cuda>;
      template class particles<double, cuda>;
    };
  };
};