//
// ebc_contribs.cc
//
// Copyright (C) 2004 Edward Valeev
//
// Author: Edward Valeev <edward.valeev@chemistry.gatech.edu>
// Maintainer: EV
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <scconfig.h>
#include <util/misc/formio.h>
#include <util/misc/timer.h>
#include <util/class/class.h>
#include <util/state/state.h>
#include <util/state/state_text.h>
#include <util/state/state_bin.h>
#include <math/scmat/matrix.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/qc/basis/integral.h>
#include <chemistry/qc/mbptr12/blas.h>
#include <chemistry/qc/mbptr12/r12ia.h>
#include <chemistry/qc/mbptr12/vxb_eval_info.h>
#include <chemistry/qc/mbptr12/pairiter.h>
#include <chemistry/qc/mbptr12/r12int_eval.h>
#include <chemistry/qc/mbptr12/print_scmat_norms.h>

using namespace std;
using namespace sc;

#define TEST_T2 0
#define TEST_A 0
// if set to 1 then use f+k rather than f to compute A
#define A_DIRECT_EXCLUDE_K 0

//
// these are for testing purposes only
//
// use the commutator form A
#define USE_A_COMM_IN_B_EBC 0
#define ACOMM_INCLUDE_TR_ONLY 0
#define ACOMM_INCLUDE_R_ONLY 0

void
R12IntEval::compute_T2_()
{
  if (evaluated_)
    return;

  Ref<TwoBodyMOIntsTransform> ipjq_tform = get_tform_("(ip|jq)");
  Ref<R12IntsAcc> ijpq_acc = ipjq_tform->ints_acc();
  if (!ijpq_acc->is_committed())
    ipjq_tform->compute();
  if (!ijpq_acc->is_active())
    ijpq_acc->activate();

  tim_enter("mp2 t2 amplitudes");

  ExEnv::out0() << endl << indent
                << "Entered MP2 T2 amplitude evaluator" << endl;
  ExEnv::out0() << incindent;

  Ref<MessageGrp> msg = r12info_->msg();
  int me = msg->me();
  int nproc = msg->n();

  const int noso = r12info_->mo_space()->rank();
  const int nocc = r12info_->nocc();
  const int nfzv = r12info_->nfzv();
  Ref<MOIndexSpace> mo_space = r12info_->obs_space();
  Ref<MOIndexSpace> act_occ_space = r12info_->act_occ_space();
  Ref<MOIndexSpace> act_vir_space = r12info_->act_vir_space();
  
  SpatialMOPairIter_eq ij_iter(act_occ_space);
  SpatialMOPairIter_eq ab_iter(act_vir_space);
  int naa = ij_iter.nij_aa();          // Number of alpha-alpha pairs (i > j)
  int nab = ij_iter.nij_ab();          // Number of alpha-beta pairs
  if (debug_) {
    ExEnv::out0() << indent << "naa = " << naa << endl;
    ExEnv::out0() << indent << "nab = " << nab << endl;
  }
  
  // Compute the number of tasks that have full access to the integrals
  // and split the work among them
  vector<int> proc_with_ints;
  int nproc_with_ints = tasks_with_ints_(ijpq_acc,proc_with_ints);
  
  //////////////////////////////////////////////////////////////
  //
  // Evaluation of the MP2 T2 amplitudes proceeds as follows:
  //
  //    loop over batches of ij,
  //      load (ijxy)=(ix|jy) into memory
  //
  //      loop over xy, 0<=x<nvir_act, 0<=y<nvir_act
  //        compute T2_aa[ij][xy] = [ (ijxy) - (ijyx) ] / denom
  //        compute T2_ab[ij][xy] = [ (ijxy) ] / denom
  //      end xy loop
  //    end ij loop
  //
  /////////////////////////////////////////////////////////////////////////////////

  for(ij_iter.start();int(ij_iter);ij_iter.next()) {

    const int ij = ij_iter.ij();
    // Figure out if this task will handle this ij
    int ij_proc = ij%nproc_with_ints;
    if (ij_proc != proc_with_ints[me])
      continue;
    const int i = ij_iter.i();
    const int j = ij_iter.j();
    const int ij_aa = ij_iter.ij_aa();
    const int ij_ab = ij_iter.ij_ab();
    const int ji_ab = ij_iter.ij_ba();

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": working on (i,j) = " << i << "," << j << " " << endl;

    // Get (|1/r12|) integrals
    tim_enter("MO ints retrieve");
    double *ijxy_buf_eri = ijpq_acc->retrieve_pair_block(i,j,R12IntsAcc::eri);
    tim_exit("MO ints retrieve");

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": obtained ij blocks" << endl;

    // Compute MP2 energies
    RefDiagSCMatrix act_occ_evals = r12info_->act_occ_space()->evals();
    RefDiagSCMatrix all_evals = r12info_->obs_space()->evals();
    double T2_aa_ijab = 0.0;
    double T2_ab_ijab = 0.0;

    for(ab_iter.start();int(ab_iter);ab_iter.next()) {

      const int a = ab_iter.i();
      const int b = ab_iter.j();
      const int ab_aa = ab_iter.ij_aa();
      const int ab_ab = ab_iter.ij_ab();
      const int ba_ab = ab_iter.ij_ba();

      const int aa = a + nocc;
      const int bb = b + nocc;

      const int ab_offset = aa*noso+bb;
      const int ba_offset = bb*noso+aa;
#if TEST_T2
      const double oo_delta_ijab = -1.0/sqrt(-act_occ_evals(i)-act_occ_evals(j)+all_evals(aa)+all_evals(bb));
#else
      const double oo_delta_ijab = -1.0/(-act_occ_evals(i)-act_occ_evals(j)+all_evals(aa)+all_evals(bb));
#endif
      const double eri_iajb = ijxy_buf_eri[ab_offset];
      const double eri_ibja = ijxy_buf_eri[ba_offset];
      const double T2_ab_ijab = eri_iajb * oo_delta_ijab;
      const double T2_ab_ijba = eri_ibja * oo_delta_ijab;
      T2ab_.set_element(ij_ab,ab_ab,T2_ab_ijab);
      T2ab_.set_element(ji_ab,ba_ab,T2_ab_ijab);
      T2ab_.set_element(ji_ab,ab_ab,T2_ab_ijba);
      T2ab_.set_element(ij_ab,ba_ab,T2_ab_ijba);

      if (ij_aa != -1 && ab_aa != -1) {
        const double T2_aa_ijab = (eri_iajb - eri_ibja) * oo_delta_ijab;
        T2aa_.set_element(ij_aa,ab_aa,T2_aa_ijab);
      }

    }
    
    ijpq_acc->release_pair_block(i,j,R12IntsAcc::eri);
  }

  globally_sum_intermeds_();

#if TEST_T2
  // As a test -- compute MP2 energies
  RefSCMatrix emp2pair_aa = T2aa_*T2aa_.t();
  RefSCMatrix emp2pair_ab = T2ab_*T2ab_.t();
  emp2pair_aa.scale(-2.0);
  emp2pair_ab.scale(-1.0);
  emp2pair_aa.print("Alpha-alpha MP2 energies");
  emp2pair_ab.print("Alpha-beta MP2 energies");
#endif

  ExEnv::out0() << decindent;
  ExEnv::out0() << indent << "Exited MP2 T2 amplitude evaluator" << endl;

  tim_exit("mp2 t2 amplitudes");
}


void
R12IntEval::compute_R_()
{
  if (evaluated_)
    return;

  // This functions assumes that virtuals are expanded in the same basis
  // as the occupied orbitals
  if (!r12info_->basis_vir()->equiv(r12info_->basis()))
    throw std::runtime_error("R12IntEval::compute_R_() -- should not be called when the basis set for virtuals \
differs from the basis set for occupieds");

  Ref<TwoBodyMOIntsTransform> ipjq_tform = get_tform_("(ip|jq)");
  Ref<R12IntsAcc> ijpq_acc = ipjq_tform->ints_acc();
  if (!ijpq_acc->is_committed())
    ipjq_tform->compute();
  if (!ijpq_acc->is_active())
    ijpq_acc->activate();

  tim_enter("R intermediate");

  Ref<MessageGrp> msg = r12info_->msg();
  int me = msg->me();
  int nproc = msg->n();
  ExEnv::out0() << endl << indent
    << "Entered R amplitude evaluator" << endl;
  ExEnv::out0() << incindent;

  const int noso = r12info_->mo_space()->rank();
  const int nocc = r12info_->nocc();
  const int nfzv = r12info_->nfzv();
  Ref<MOIndexSpace> mo_space = r12info_->obs_space();
  Ref<MOIndexSpace> act_occ_space = r12info_->act_occ_space();
  Ref<MOIndexSpace> act_vir_space = r12info_->act_vir_space();

  SpatialMOPairIter_eq ij_iter(act_occ_space);
  SpatialMOPairIter_eq ab_iter(act_vir_space);
  int naa = ij_iter.nij_aa();          // Number of alpha-alpha pairs (i > j)
  int nab = ij_iter.nij_ab();          // Number of alpha-beta pairs
  if (debug_) {
    ExEnv::out0() << indent << "naa = " << naa << endl;
    ExEnv::out0() << indent << "nab = " << nab << endl;
  }

  // Compute the number of tasks that have full access to the integrals
  // and split the work among them
  vector<int> proc_with_ints;
  int nproc_with_ints = tasks_with_ints_(ijpq_acc,proc_with_ints);

  for(ij_iter.start();int(ij_iter);ij_iter.next()) {

    const int ij = ij_iter.ij();
    // Figure out if this task will handle this ij
    int ij_proc = ij%nproc_with_ints;
    if (ij_proc != proc_with_ints[me])
      continue;
    const int i = ij_iter.i();
    const int j = ij_iter.j();
    const int ij_aa = ij_iter.ij_aa();
    const int ij_ab = ij_iter.ij_ab();
    const int ji_ab = ij_iter.ij_ba();

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": working on (i,j) = " << i << "," << j << " " << endl;

    // Get (|1/r12|) integrals
    tim_enter("MO ints retrieve");
    double *ijxy_buf_r12 = ijpq_acc->retrieve_pair_block(i,j,R12IntsAcc::r12);
    tim_exit("MO ints retrieve");

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": obtained ij blocks" << endl;

    // Compute MP2 energies
    RefDiagSCMatrix act_occ_evals = r12info_->act_occ_space()->evals();
    RefDiagSCMatrix all_evals = r12info_->obs_space()->evals();
    double R_aa_ijab = 0.0;
    double R_ab_ijab = 0.0;

    for(ab_iter.start();int(ab_iter);ab_iter.next()) {

      const int a = ab_iter.i();
      const int b = ab_iter.j();
      const int ab_aa = ab_iter.ij_aa();
      const int ab_ab = ab_iter.ij_ab();
      const int ba_ab = ab_iter.ij_ba();

      const int aa = a + nocc;
      const int bb = b + nocc;

      const int ab_offset = aa*noso+bb;
      const int ba_offset = bb*noso+aa;
      const double r12_iajb = ijxy_buf_r12[ab_offset];
      const double r12_ibja = ijxy_buf_r12[ba_offset];
      Rab_.set_element(ij_ab,ab_ab,r12_iajb);
      Rab_.set_element(ji_ab,ba_ab,r12_iajb);
      Rab_.set_element(ji_ab,ab_ab,r12_ibja);
      Rab_.set_element(ij_ab,ba_ab,r12_ibja);

      if (ij_aa != -1 && ab_aa != -1) {
        const double R_aa_ijab = (r12_iajb - r12_ibja);
        Raa_.set_element(ij_aa,ab_aa,R_aa_ijab);
      }

    }

    ijpq_acc->release_pair_block(i,j,R12IntsAcc::r12);
  }

  globally_sum_intermeds_();

  ExEnv::out0() << decindent;
  ExEnv::out0() << indent << "Exited R amplitude evaluator" << endl;

  tim_exit("R intermediate");
}


void
R12IntEval::compute_A_simple_()
{
  if (abs_method_ == LinearR12::ABS_ABS || abs_method_ == LinearR12::ABS_ABSPlus)
    throw std::runtime_error("R12IntEval::compute_A_simple_() -- A intermediate can only be computed using a CABS (or CABS+) approach");

  if (evaluated_)
    return;

  tim_enter("A intermediate");

  const int num_te_types = 2;
  
  Ref<MessageGrp> msg = r12info_->msg();
  int me = msg->me();
  int nproc = msg->n();
  ExEnv::out0() << endl << indent
    << "Entered A intermediate evaluator" << endl;
  ExEnv::out0() << incindent;

  const int noso = r12info_->mo_space()->rank();
  const int nocc = r12info_->nocc();
  const int nfzv = r12info_->nfzv();
  const int nvir_act = noso - nocc - nfzv;
  Ref<MOIndexSpace> act_occ_space = r12info_->act_occ_space();
  Ref<MOIndexSpace> mo_space = r12info_->obs_space();
  Ref<MOIndexSpace> act_vir_space = r12info_->act_vir_space();

  // compute the Fock matrix between the complement and virtuals and
  // create the new Fock-weighted space
  Ref<MOIndexSpace> ribs_space = r12info_->ribs_space();
#if A_DIRECT_EXCLUDE_K
  RefSCMatrix F_ri_v = fock_(r12info_->occ_space(),ribs_space,act_vir_space,1.0,0.0);
#else
  RefSCMatrix F_ri_v = fock_(r12info_->occ_space(),ribs_space,act_vir_space);
#endif
  if (debug_ > 1)
    F_ri_v.print("Fock matrix (RI-BS/act.virt.)");
  if (debug_ > 0)
    print_scmat_norms(F_ri_v,"Fock matrix (RI-BS/act.virt.)");

  Ref<MOIndexSpace> act_fvir_space = new MOIndexSpace("Fock-weighted active unoccupied MOs sorted by energy",
                                                      act_vir_space, ribs_space->coefs()*F_ri_v, ribs_space->basis());

  // Do the AO->MO transform
  Ref<MOIntsTransformFactory> tfactory = r12info_->tfactory();
  tfactory->set_spaces(act_occ_space,act_vir_space,
                       act_occ_space,act_fvir_space);
  Ref<TwoBodyMOIntsTransform> iajBf_tform = tfactory->twobody_transform_13("(ia|jB_f)");
  iajBf_tform->set_num_te_types(num_te_types);
  iajBf_tform->compute();
  Ref<R12IntsAcc> ijaBf_acc = iajBf_tform->ints_acc();
  
  SpatialMOPairIter_eq ij_iter(act_occ_space);
  SpatialMOPairIter_eq ab_iter(act_vir_space);
  int naa = ij_iter.nij_aa();          // Number of alpha-alpha pairs (i > j)
  int nab = ij_iter.nij_ab();          // Number of alpha-beta pairs
  if (debug_) {
    ExEnv::out0() << indent << "naa = " << naa << endl;
    ExEnv::out0() << indent << "nab = " << nab << endl;
  }

  // Compute the number of tasks that have full access to the integrals
  // and split the work among them
  vector<int> proc_with_ints;
  int nproc_with_ints = tasks_with_ints_(ijaBf_acc,proc_with_ints);

  //////////////////////////////////////////////////////////////
  //
  // Evaluation of A intermedates proceeds as follows:
  //
  //    loop over batches of ij,
  //      load (ijxy)=(ix|jy) into memory
  //
  //      loop over xy, 0<=x<nvir_act, 0<=y<nvir_act
  //        compute T2_aa[ij][xy] = [ (ijxy) - (ijyx) ] / denom
  //        compute T2_ab[ij][xy] = [ (ijxy) ] / denom
  //      end xy loop
  //    end ij loop
  //
  /////////////////////////////////////////////////////////////////////////////////

  for(ij_iter.start();int(ij_iter);ij_iter.next()) {

    const int ij = ij_iter.ij();
    // Figure out if this task will handle this ij
    int ij_proc = ij%nproc_with_ints;
    if (ij_proc != proc_with_ints[me])
      continue;
    const int i = ij_iter.i();
    const int j = ij_iter.j();
    const int ij_aa = ij_iter.ij_aa();
    const int ij_ab = ij_iter.ij_ab();
    const int ji_ab = ij_iter.ij_ba();

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": working on (i,j) = " << i << "," << j << " " << endl;

    // Get (|r12|) integrals
    tim_enter("MO ints retrieve");
    double *ijaBf_buf_r12 = ijaBf_acc->retrieve_pair_block(i,j,R12IntsAcc::r12);
    double *jiaBf_buf_r12 = ijaBf_acc->retrieve_pair_block(j,i,R12IntsAcc::r12);
    tim_exit("MO ints retrieve");

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": obtained ij blocks" << endl;

    // Compute contributions to A
    for(ab_iter.start();int(ab_iter);ab_iter.next()) {

      const int a = ab_iter.i();
      const int b = ab_iter.j();
      const int ab_aa = ab_iter.ij_aa();
      const int ab_ab = ab_iter.ij_ab();
      const int ba_ab = ab_iter.ij_ba();

      const int ab_offset = a*nvir_act+b;
      const int ba_offset = b*nvir_act+a;

      const double r12_iajBf = ijaBf_buf_r12[ab_offset];
      const double r12_jaiBf = jiaBf_buf_r12[ab_offset];
      const double r12_ibjAf = ijaBf_buf_r12[ba_offset];
      const double r12_jbiAf = jiaBf_buf_r12[ba_offset];

      const double A_ab_ijab = 0.5*(r12_jbiAf + r12_iajBf);
      const double A_ab_ijba = 0.5*(r12_ibjAf + r12_jaiBf);
      Aab_.set_element(ij_ab,ab_ab,A_ab_ijab);
      Aab_.set_element(ji_ab,ba_ab,A_ab_ijab);
      Aab_.set_element(ji_ab,ab_ab,A_ab_ijba);
      Aab_.set_element(ij_ab,ba_ab,A_ab_ijba);

      if (ij_aa != -1 && ab_aa != -1) {
        const double A_aa_ijab = 0.5*(r12_jbiAf - r12_ibjAf + r12_iajBf - r12_jaiBf);
        Aaa_.set_element(ij_aa,ab_aa,A_aa_ijab);
      }

    }

    ijaBf_acc->release_pair_block(i,j,R12IntsAcc::r12);
    ijaBf_acc->release_pair_block(j,i,R12IntsAcc::r12);
  }

  globally_sum_intermeds_();

#if TEST_A
  Aaa_.print("Alpha-alpha A intermediate");
  Aab_.print("Alpha-beta A intermediate");
#endif

  ExEnv::out0() << decindent;
  ExEnv::out0() << indent << "Exited A intermediate evaluator" << endl;

  tim_exit("A intermediate");
}

void
R12IntEval::compute_A_via_commutator_()
{
  if (evaluated_)
    return;

  // This functions assumes that virtuals are expanded in the same basis
  // as the occupied orbitals
  if (!r12info_->basis_vir()->equiv(r12info_->basis()))
    throw std::runtime_error("R12IntEval::compute_A_via_commutator_() -- should not be called when the basis set for virtuals \
differs from the basis set for occupieds");

  Ref<TwoBodyMOIntsTransform> ipjq_tform = get_tform_("(ip|jq)");
  Ref<R12IntsAcc> ijpq_acc = ipjq_tform->ints_acc();
  if (!ijpq_acc->is_committed())
    ipjq_tform->compute();
  if (!ijpq_acc->is_active())
    ijpq_acc->activate();

  tim_enter("A intermediate via [T,r]");

  Ref<MessageGrp> msg = r12info_->msg();
  int me = msg->me();
  int nproc = msg->n();
  ExEnv::out0() << endl << indent
    << "Entered A amplitude (via [T,r]) evaluator" << endl;
  ExEnv::out0() << incindent;

  const int noso = r12info_->mo_space()->rank();
  const int nocc = r12info_->nocc();
  const int nfzv = r12info_->nfzv();
  Ref<MOIndexSpace> mo_space = r12info_->obs_space();
  Ref<MOIndexSpace> act_occ_space = r12info_->act_occ_space();
  Ref<MOIndexSpace> act_vir_space = r12info_->act_vir_space();

  SpatialMOPairIter_eq ij_iter(act_occ_space);
  SpatialMOPairIter_eq ab_iter(act_vir_space);
  int naa = ij_iter.nij_aa();          // Number of alpha-alpha pairs (i > j)
  int nab = ij_iter.nij_ab();          // Number of alpha-beta pairs
  if (debug_) {
    ExEnv::out0() << indent << "naa = " << naa << endl;
    ExEnv::out0() << indent << "nab = " << nab << endl;
  }

  // Compute the number of tasks that have full access to the integrals
  // and split the work among them
  vector<int> proc_with_ints;
  int nproc_with_ints = tasks_with_ints_(ijpq_acc,proc_with_ints);

  for(ij_iter.start();int(ij_iter);ij_iter.next()) {

    const int ij = ij_iter.ij();
    // Figure out if this task will handle this ij
    int ij_proc = ij%nproc_with_ints;
    if (ij_proc != proc_with_ints[me])
      continue;
    const int i = ij_iter.i();
    const int j = ij_iter.j();
    const int ij_aa = ij_iter.ij_aa();
    const int ij_ab = ij_iter.ij_ab();
    const int ji_ab = ij_iter.ij_ba();

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": working on (i,j) = " << i << "," << j << " " << endl;

    // Get (|1/r12|) integrals
    tim_enter("MO ints retrieve");
    double *ijxy_buf_r12 = ijpq_acc->retrieve_pair_block(i,j,R12IntsAcc::r12);
    double *ijxy_buf_r12t1 = ijpq_acc->retrieve_pair_block(i,j,R12IntsAcc::r12t1);
    double *jixy_buf_r12t1 = ijpq_acc->retrieve_pair_block(j,i,R12IntsAcc::r12t1);
    tim_exit("MO ints retrieve");

    if (debug_)
      ExEnv::outn() << indent << "task " << me << ": obtained ij blocks" << endl;

    RefDiagSCMatrix act_occ_evals = r12info_->act_occ_space()->evals();
    RefDiagSCMatrix all_evals = r12info_->obs_space()->evals();
    double R_aa_ijab = 0.0;
    double R_ab_ijab = 0.0;
    double TR_aa_ijab = 0.0;
    double TR_ab_ijab = 0.0;

    for(ab_iter.start();int(ab_iter);ab_iter.next()) {

      const int a = ab_iter.i();
      const int b = ab_iter.j();
      const int ab_aa = ab_iter.ij_aa();
      const int ab_ab = ab_iter.ij_ab();
      const int ba_ab = ab_iter.ij_ba();

      const int aa = a + nocc;
      const int bb = b + nocc;

      const int ab_offset = aa*noso+bb;
      const int ba_offset = bb*noso+aa;
      const double r12_iajb = ijxy_buf_r12[ab_offset];
      const double r12_ibja = ijxy_buf_r12[ba_offset];
      const double t1r12_iajb = -ijxy_buf_r12t1[ab_offset];
      const double t1r12_ibja = -ijxy_buf_r12t1[ba_offset];
      const double t2r12_iajb = -jixy_buf_r12t1[ba_offset];
      const double t2r12_ibja = -jixy_buf_r12t1[ab_offset];
#if ACOMM_INCLUDE_TR_ONLY
      double Aab_ij_ab = 0.5 * ( -(t1r12_iajb + t2r12_iajb) );
      double Aab_ij_ba = 0.5 * ( -(t1r12_ibja + t2r12_ibja) );
#elif ACOMM_INCLUDE_R_ONLY
      double Aab_ij_ab = 0.5 * ( r12_iajb );
      double Aab_ij_ba = 0.5 * ( r12_ibja );
#else
      double Aab_ij_ab = 0.5 * ( -(t1r12_iajb + t2r12_iajb) - (all_evals(aa) + all_evals(bb) -
                                                              act_occ_evals(i) - act_occ_evals(j))*r12_iajb );
      double Aab_ij_ba = 0.5 * ( -(t1r12_ibja + t2r12_ibja) - (all_evals(aa) + all_evals(bb) -
                                                              act_occ_evals(i) - act_occ_evals(j))*r12_ibja );
#endif
      Ac_ab_.set_element(ij_ab,ab_ab,Aab_ij_ab);
      Ac_ab_.set_element(ji_ab,ba_ab,Aab_ij_ab);
      Ac_ab_.set_element(ji_ab,ab_ab,Aab_ij_ba);
      Ac_ab_.set_element(ij_ab,ba_ab,Aab_ij_ba);

      if (ij_aa != -1 && ab_aa != -1) {
        Ac_aa_.set_element(ij_aa,ab_aa,Aab_ij_ab - Aab_ij_ba);
      }

    }

    ijpq_acc->release_pair_block(i,j,R12IntsAcc::r12);
    ijpq_acc->release_pair_block(i,j,R12IntsAcc::r12t1);
    ijpq_acc->release_pair_block(j,i,R12IntsAcc::r12t1);
  }

  globally_sum_intermeds_();

#if TEST_A
  Ac_aa_.print("Alpha-alpha A intermediate");
  Ac_ab_.print("Alpha-beta A intermediate");
#endif

  ExEnv::out0() << decindent;
  ExEnv::out0() << indent << "Exited A amplitude (via [T,r]) evaluator" << endl;

  tim_exit("A intermediate via [T,r]");
}


void
R12IntEval::AT2_contrib_to_V_()
{
  if (evaluated_)
    return;
  if (r12info_->msg()->me() == 0) {
    RefSCMatrix Vaa = Aaa_*T2aa_.t();
    Vaa_.accumulate(Vaa);
    RefSCMatrix Vab = Aab_*T2ab_.t();
    Vab_.accumulate(Vab);  
    if (debug_ > 0) {
      print_scmat_norms(Vaa,"Alpha-alpha AT2 contribution to V");
      print_scmat_norms(Vab,"Alpha-beta AT2 contribution to V");
    }
  }
  globally_sum_intermeds_();
}

void
R12IntEval::AR_contrib_to_B_()
{
  if (evaluated_)
    return;
  if (r12info_->msg()->me() == 0) {
#if USE_A_COMM_IN_B_EBC
    RefSCMatrix AR_aa = Ac_aa_*Raa_.t();
    RefSCMatrix AR_ab = Ac_ab_*Rab_.t();
    double scale = -0.5;
#else
    RefSCMatrix AR_aa = Aaa_*Raa_.t();
    RefSCMatrix AR_ab = Aab_*Rab_.t();
    double scale = -0.5;
#endif
    RefSCMatrix Baa = Baa_.clone();  Baa.assign(0.0);
    RefSCMatrix Bab = Bab_.clone();  Bab.assign(0.0);
    AR_aa.scale(scale); Baa.accumulate(AR_aa);
    RefSCMatrix AR_aa_t = AR_aa.t();
    Baa.accumulate(AR_aa_t);
    AR_ab.scale(scale); Bab.accumulate(AR_ab);
    RefSCMatrix AR_ab_t = AR_ab.t();
    Bab.accumulate(AR_ab_t);
    if (debug_ > 1) {
      Baa.print("Alpha-alpha B^{EBC} contribution");
      Bab.print("Alpha-beta B^{EBC} contribution");
    }
    Baa_.accumulate(Baa);
    Bab_.accumulate(Bab);
    if (debug_ > 0) {
      print_scmat_norms(Baa,"Alpha-alpha AR contribution to B");
      print_scmat_norms(Bab,"Alpha-beta AR contribution to B");
    }
  }
  globally_sum_intermeds_();
}

////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
