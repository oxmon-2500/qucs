/*
 * nasolver.h - nodal analysis solver class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: nasolver.h,v 1.12 2004-11-29 19:03:39 raimi Exp $
 *
 */

#ifndef __NASOLVER_H__
#define __NASOLVER_H__

#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"

#define SAVE_OPS 1 // save operating points
#define SAVE_ALL 2 // also save subcircuit nodes and operating points

// Convergence helper definitions.
#define CONV_Attenuation     1
#define CONV_LineSearch      2
#define CONV_SteepestDescent 3
#define CONV_GMinStepping    4
#define CONV_SourceStepping  5

class analysis;
class circuit;
class nodelist;
class complex;
class vector;

template <class nr_type_t>
class nasolver : public analysis
{
 public:
  nasolver ();
  nasolver (char *);
  nasolver (nasolver &);
  ~nasolver ();
  int  solve_once (void);
  int  solve_nonlinear (void);
  int  solve_nonlinear_continuation (void);
  int  solve_linear (void);
  void solve_pre (void);
  void solve_post (void);
  void setDescription (char * n) { desc = n; }
  char * getDescription (void) { return desc; }
  void saveResults (char *, char *, int, vector * f = NULL);
  typedef void (* calculate_func_t) (nasolver<nr_type_t> *);
  void setCalculation (calculate_func_t f) { calculate_func = f; }
  void calculate (void) { if (calculate_func) (*calculate_func) (this); }

 protected:
  void savePreviousIteration (void);
  int  countNodes (void);
  void saveSolution (void);
  circuit * findVoltageSource (int);
  void applyNodeset (void);

 private:
  void createMatrix (void);
  void assignVoltageSources (void);
  void createGMatrix (void);
  void createBMatrix (void);
  void createCMatrix (void);
  void createDMatrix (void);
  void createIVector (void);
  void createEVector (void);
  void createZVector (void);
  void runMNA (void);
  void applyAttenuation (void);
  void lineSearch (void);
  void steepestDescent (void);
  char * createV (int, char *, int);
  char * createI (int, char *, int);
  char * createOP (char *, char *);
  void saveNodeVoltages (void);
  void saveBranchCurrents (void);
  int  checkConvergence (void);
  void saveVariable (char *, nr_type_t, vector * f = NULL);
  nr_type_t MatValX (complex, complex *);
  nr_type_t MatValX (complex, nr_double_t *);

 protected:
  tvector<nr_type_t> * z;
  tvector<nr_type_t> * x;
  tvector<nr_type_t> * xprev;
  tvector<nr_type_t> * zprev;
  tmatrix<nr_type_t> * A;
  int iterations;
  int convHelper;
  int fixpoint;
  int eqnAlgo;
  nr_double_t gMin, srcFactor;

 private:
  nodelist * nlist;
  eqnsys<nr_type_t> * eqns;
  nr_double_t reltol;
  nr_double_t abstol;
  nr_double_t vntol;
  int updateMatrix;

 private:
  char * desc;
  calculate_func_t calculate_func;
};

#include "nasolver.cpp"

#endif /* __NASOLVER_H__ */
