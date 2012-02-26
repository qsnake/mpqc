#ifndef util_class_psi3_libpsio_psifiles_h_
#define util_class_psi3_libpsio_psifiles_h_

#define PSI_DEFAULT_FILE_PREFIX "psi"

#define PSIF_CHKPT          32 /* new libpsio checkpoint file number */

#define PSIF_OPTKING        1
#define PSIF_DSCF           31
#define PSIF_SO_TEI         33
#define PSIF_OEI            35
#define PSIF_SO_R12         38
#define PSIF_SO_R12T1       39
#define PSIF_DERINFO        40
#define PSIF_SO_PRESORT     41
#define PSIF_OLD_CHKPT      42   /* Until we have flexible PSIF_CHKPT this will store previous calculation info */
#define PSIF_CIVECT         43   /* CI vector from DETCI along with string and determinant info */

#define PSIF_AO_DGDBX       44   /* B-field derivative AO integrals over GIAO Gaussians -- only bra-ket
                                    permutational symmetry holds */
#define PSIF_AO_DGDBY       45
#define PSIF_AO_DGDBZ       46

#define PSIF_MO_R12         79
#define PSIF_MO_R12T1       80

#define PSIF_SO_PKSUPER1    92
#define PSIF_SO_PKSUPER2    93

#define PSIF_MO_TEI         72
#define PSIF_MO_OPDM        73
#define PSIF_MO_TPDM        74
#define PSIF_MO_LAG         75
#define PSIF_AO_OPDM        76   /* PSIF_AO_OPDM also contains AO Lagrangian */
#define PSIF_AO_TPDM        77

/*
** MO Hessian File (also contains specialized integral and Fock lists.
** See programs STABLE and CPHF for more info.
** -TDC, 7/00
*/
#define PSIF_MO_HESS        78
#define PSIF_CPHF           78

/*
** Additions for UHF-based transformations.
** -TDC, 6/01
*/
#define PSIF_MO_AA_TEI      81
#define PSIF_MO_BB_TEI      82
#define PSIF_MO_AB_TEI      83
#define PSIF_MO_AA_TPDM     84
#define PSIF_MO_BB_TPDM     85
#define PSIF_MO_AB_TPDM     86
#define PSIF_AA_PRESORT     87   /* AA UHF twopdm presort file */
#define PSIF_BB_PRESORT     88   /* BB UHF twopdm presort file */
#define PSIF_AB_PRESORT     89   /* AB UHF twopdm presort file */

/* All of these one-electron quantities have been moved into PSIF_OEI
   Most integrals are real Hermitian hence only lower triangle of the matrix is written out */
/* These macros give libpsio TOC strings for easy identification.     */
#define PSIF_SO_S           "SO-basis Overlap Ints"
#define PSIF_SO_T           "SO-basis Kinetic Energy Ints"
#define PSIF_SO_V           "SO-basis Potential Energy Ints"
#define PSIF_AO_S           "AO-basis Overlap Ints"
#define PSIF_AO_MX          "AO-basis Mu-X Ints"
#define PSIF_AO_MY          "AO-basis Mu-Y Ints"
#define PSIF_AO_MZ          "AO-basis Mu-Z Ints"
#define PSIF_MO_MX          "MO-basis Mu-X Ints"
#define PSIF_MO_MY          "MO-basis Mu-Y Ints"
#define PSIF_MO_MZ          "MO-basis Mu-Z Ints"
#define PSIF_AO_QXX         "AO-basis Q-XX Ints"    /* Electric quadrupole moment integrals */
#define PSIF_AO_QXY         "AO-basis Q-XY Ints"
#define PSIF_AO_QXZ         "AO-basis Q-XZ Ints"
#define PSIF_AO_QYY         "AO-basis Q-YY Ints"
#define PSIF_AO_QYZ         "AO-basis Q-YZ Ints"
#define PSIF_AO_QZZ         "AO-basis Q-ZZ Ints"

/* These integrals are anti-Hermitian -- upper triangle has sign opposite of that of the lower triangle */
#define PSIF_AO_NablaX      "AO-basis Nabla-X Ints" /* integrals of nabla operator */
#define PSIF_AO_NablaY      "AO-basis Nabla-Y Ints"
#define PSIF_AO_NablaZ      "AO-basis Nabla-Z Ints"

/* These integrals are pure imaginary Hermitian. We write the full matrix of the imaginary part of these
integrals out (i.e. multiply by i=sqrt(-1) to get the integrals) */
#define PSIF_AO_LX          "AO-basis LX Ints"      /* integrals of angular momentum operator */
#define PSIF_AO_LY          "AO-basis LY Ints"
#define PSIF_AO_LZ          "AO-basis LZ Ints"
#define PSIF_AO_DSDB_X      "AO-basis dS/dBx Ints"      /* Overlap derivative integrals WRT B field */
#define PSIF_AO_DSDB_Y      "AO-basis dS/dBy Ints"
#define PSIF_AO_DSDB_Z      "AO-basis dS/dBz Ints"
#define PSIF_AO_DHDB_X      "AO-basis dh/dBx Ints"      /* One-electron derivative integrals WRT B field */
#define PSIF_AO_DHDB_Y      "AO-basis dh/dBy Ints"
#define PSIF_AO_DHDB_Z      "AO-basis dh/dBz Ints"
#define PSIF_AO_D2HDBDE_XX  "AO-basis d2h/dBxdEx Ints"  /* One-electron derivative integrals WRT E and B fields */
#define PSIF_AO_D2HDBDE_XY  "AO-basis d2h/dBxdEy Ints"
#define PSIF_AO_D2HDBDE_XZ  "AO-basis d2h/dBxdEz Ints"
#define PSIF_AO_D2HDBDE_YX  "AO-basis d2h/dBydEx Ints"
#define PSIF_AO_D2HDBDE_YY  "AO-basis d2h/dBydEy Ints"
#define PSIF_AO_D2HDBDE_YZ  "AO-basis d2h/dBydEz Ints"
#define PSIF_AO_D2HDBDE_ZX  "AO-basis d2h/dBzdEx Ints"
#define PSIF_AO_D2HDBDE_ZY  "AO-basis d2h/dBzdEy Ints"
#define PSIF_AO_D2HDBDE_ZZ  "AO-basis d2h/dBzdEz Ints"
#define PSIF_MO_DFDB_X      "AO-basis dF/dBx Ints"      /* Fock operator derivative integrals WRT B field */
#define PSIF_MO_DFDB_Y      "AO-basis dF/dBy Ints"
#define PSIF_MO_DFDB_Z      "AO-basis dF/dBz Ints"

#define PSIF_MO_OEI         "MO-basis One-electron Ints"
#define PSIF_MO_A_OEI       "MO-basis Alpha One-electron Ints"
#define PSIF_MO_B_OEI       "MO-basis Beta One-electron Ints"
#define PSIF_MO_FZC         "MO-basis Frozen-Core Operator"
#define PSIF_MO_A_FZC       "MO-basis Alpha Frozen-Core Oper"
#define PSIF_MO_B_FZC       "MO-basis Beta Frozen-Core Oper"

/* More macros */
#define PSIF_AO_OPDM_TRIANG "AO-basis OPDM triang"
#define PSIF_AO_LAG_TRIANG  "AO-basis Lagrangian triang"
#define PSIF_AO_OPDM_SQUARE "AO-basis OPDM square"
#define PSIF_SO_OPDM        "SO-basis OPDM"
#define PSIF_SO_OPDM_TRIANG "SO-basis triang"

/* PSI return codes --- for new PSI driver           */
#define PSI_RETURN_SUCCESS      0
#define PSI_RETURN_FAILURE      1
#define PSI_RETURN_ENDLOOP      2

#endif

