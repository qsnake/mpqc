// 
// File:          MPQC_IntegralEvaluator2_Impl.hh
// Symbol:        MPQC.IntegralEvaluator2-v0.2
// Symbol Type:   class
// Babel Version: 0.10.2
// Description:   Server-side implementation for MPQC.IntegralEvaluator2
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.2
// 

#ifndef included_MPQC_IntegralEvaluator2_Impl_hh
#define included_MPQC_IntegralEvaluator2_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_IntegralEvaluator2_IOR_h
#include "MPQC_IntegralEvaluator2_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_QC_GaussianBasis_DerivCenters_hh
#include "Chemistry_QC_GaussianBasis_DerivCenters.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_Molecular_hh
#include "Chemistry_QC_GaussianBasis_Molecular.hh"
#endif
#ifndef included_MPQC_IntegralEvaluator2_hh
#include "MPQC_IntegralEvaluator2.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator2._includes)
#include <chemistry/qc/basis/gaussbas.h>
#include <chemistry/qc/intv3/cartitv3.h>
#include <chemistry/qc/intv3/intv3.h>
#ifdef HAVE_CINTS
  #include <chemistry/qc/cints/cints.h>
#endif
using namespace sc;
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator2._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.IntegralEvaluator2" (version 0.2)
   */
  class IntegralEvaluator2_impl
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator2._inherits)

  /** IntegralEvaluator2_impl implements a class interface for
      supplying 2-center molecular integrals.

      This is an implementation of a SIDL interface.
      The stub code is generated by the Babel tool.  Do not make
      modifications outside of splicer blocks, as these will be lost.
      This is a server implementation for a Babel class, the Babel
      client code is provided by the cca-chem-generic package.
   */

  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator2._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    IntegralEvaluator2 self;

    // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator2._implementation)
    Chemistry::Molecule molecule_;
    std::string evaluator_label_;
    Ref<GaussianBasisSet> bs1_, bs2_;
    Ref<Integral> integral_;
    Ref<OneBodyInt> eval_;
    Ref<OneBodyDerivInt> deriv_eval_;
    int max_nshell2_;
    int maxam_;
    sidl::array<double> sidl_buffer_;
    const double *sc_buffer_;
    double *temp_buffer_;
    double *buf_;
    enum { one_body, one_body_deriv};
    int int_type_;
    int deriv_level_;
    std::string package_;
    int **reorder_;
    Chemistry::QC::GaussianBasis::DerivCenters deriv_centers_;

    void reorder_intv3(int64_t, int64_t);
    void initialize_reorder_intv3();
    void reorder_doublet( sc::GaussianShell*, sc::GaussianShell*, int, int, int );
    // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator2._implementation)

  private:
    // private default constructor (required)
    IntegralEvaluator2_impl() 
    {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    IntegralEvaluator2_impl( struct MPQC_IntegralEvaluator2__object * s ) : 
      self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~IntegralEvaluator2_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

    // static class initializer
    static void _load();

  public:

    /**
     * user defined non-static method.
     */
    void
    set_integral_package (
      /* in */ const ::std::string& label
    )
    throw () 
    ;


    /**
     * Initialize the evaluator.
     * @param bs1 Molecular basis on center 1.
     * @param bs2 Molecular basis on center 2.
     * @param label String specifying integral type.
     * @param max_deriv Max derivative to compute. 
     */
    void
    initialize (
      /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
      /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
      /* in */ const ::std::string& label,
      /* in */ int64_t max_deriv
    )
    throw () 
    ;


    /**
     * Get the buffer pointer
     * @return Buffer pointer 
     */
    void*
    get_buffer() throw () 
    ;

    /**
     * Allows a DerivCenters object to be passed to 
     * an evaluator, so that derivatives can be taken 
     * with respect to a specified atom (needed for
     * derivatives with non-Hellman-Feynman contributions). 
     */
    void
    set_derivcenters (
      /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters dc
    )
    throw () 
    ;


    /**
     * Compute a shell doublet of integrals.
     * @param shellnum1 Gaussian shell number 1.
     * @param shellnum2 Gaussian shell number 2.
     * @param deriv_level Derivative level. 
     * @param deriv_ctr Derivative center descriptor. 
     */
    void
    compute (
      /* in */ int64_t shellnum1,
      /* in */ int64_t shellnum2,
      /* in */ int64_t deriv_level,
      /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters deriv_ctr
    )
    throw () 
    ;


    /**
     * Compute a shell doublet of integrals and return as a borrowed
     * sidl array.
     * @param shellnum1 Gaussian shell number 1.
     * @param shellnum2 Gaussian shell number 2.
     * @param deriv_level Derivative level.
     * @param deriv_ctr Derivative center descriptor.
     * @return Borrowed sidl array buffer. 
     */
    ::sidl::array<double>
    compute_array (
      /* in */ int64_t shellnum1,
      /* in */ int64_t shellnum2,
      /* in */ int64_t deriv_level,
      /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters deriv_ctr
    )
    throw () 
    ;

  };  // end class IntegralEvaluator2_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator2._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator2._misc)

#endif