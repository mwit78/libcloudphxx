// vim:filetype=cpp
/** @file
  * @copyright University of Warsaw
  * @section LICENSE
  * GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
  */

#include <libcloudph++/common/kappa_koehler.hpp>
#include <libcloudph++/common/const_cp.hpp>

namespace libcloudphxx
{
  namespace lgrngn
  {
    namespace detail
    {
      template <typename real_t>
      struct rw2_eq 
      {   
       const real_t RH_max;

       rw2_eq(const real_t &RH_max) : RH_max(RH_max) {}

       __device__ 
       real_t operator()(const thrust::tuple<real_t, real_t, real_t, real_t> &tpl)
       {
#if !defined(__NVCC__)
         using std::min;
         using std::pow;
#endif
         const quantity<si::volume,        real_t> rd3 = thrust::get<0>(tpl) * si::cubic_metres;
         const quantity<si::dimensionless, real_t> kpa = thrust::get<1>(tpl); 
         const quantity<si::dimensionless, real_t> RH  = min(thrust::get<2>(tpl), RH_max);
         const quantity<si::temperature,   real_t> T   = thrust::get<3>(tpl) * si::kelvins;
         return pow(common::kappa_koehler::rw3_eq( // TODO: include kelvin effect!
           rd3, kpa, RH, T 
         ) / si::cubic_metres, real_t(2./3));
       }
      }; 
    };

    template <typename real_t, int device>
    void particles<real_t, device>::impl::init_wet()
    {
      // memory allocation
      rw2.resize(n_part);

      // initialising values of rw2
      {
	// calculating rw_eq
        {
          typedef thrust::permutation_iterator<
            typename thrust_device::vector<real_t>::iterator,
            typename thrust_device::vector<thrust_size_t>::iterator
          > pi_t;

          typedef thrust::zip_iterator<
            thrust::tuple<
              typename thrust_device::vector<real_t>::iterator,
              typename thrust_device::vector<real_t>::iterator,
              pi_t,
              pi_t
            >
          > zip_it_t;

          zip_it_t zip_it(thrust::make_tuple(
            rd3.begin(), 
            kpa.begin(), 
            pi_t(RH.begin(), ijk.begin()),
            pi_t(T.begin(),  ijk.begin())
          ));

	  thrust::transform(
	    zip_it, zip_it + n_part, // input
	    rw2.begin(), // output
            detail::rw2_eq<real_t>(.95) // value seggested in Lebo and Seinfeld 2011 (TODO: not here!)
	  );
        }
      }
    }
  };
};
