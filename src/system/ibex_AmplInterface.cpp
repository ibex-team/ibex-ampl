//============================================================================
//                                  I B E X
// File        : ibex_AmplInterface.cpp Adapted from CouenneAmplInterface
//				by Pietro Belotti, Leo Liberti and Sonia Caffieri
// Author      : Jordan Ninin
// License     : See the LICENSE file
// Created     : Nov 5, 2013
// Last Update : Nov 5, 2013
//============================================================================

#include "ibex_AmplInterface.h"
#include "ibex/ibex_Exception.h"
#include "ibex/ibex_ExtendedSystem.h"
#include <sstream>


#include "ibex/ibex_DefaultOptimizerConfig.h"

#include "asl/asl.h"
#include "asl/nlp.h"
#include "asl/getstub.h"
#include "asl/opcode.hd"
#include "asl/r_opn.hd" /* for N_OPS */
//#include <string.h>
//#include <math.h>
#include <stdint.h>


#define OBJ_DE    ((const ASL_fg *) asl) -> I.obj_de_
#define VAR_E     ((const ASL_fg *) asl) -> I.var_e_
#define CON_DE    ((const ASL_fg *) asl) -> I.con_de_
#define OBJ_sense ((const ASL_fg *) asl) -> i.objtype_
#define CEXPS1 ((const ASL_fg *) asl) -> I.cexps1_
#define CEXPS ((const ASL_fg *) asl) -> I.cexps_



// The different option of IBEXOPT in Ampl
double ibex_rel_eps_f=0, ibex_abs_eps_f=0, ibex_initial_loup=INFINITY, ibex_timeout=0, ibex_eps_x=0, ibex_bisect_ratio=0, ibex_eps_h=-1, ibex_relax_ratio=0;
int ibex_trace2=-10, ibex_random_seed=0, ibex_objno=1;
int	ibex_extended_COV=-1, ibex_anticipated_UB=-1, ibex_inHC4=-1, ibex_rigor=-1, ibex_kkt=-1 ;

static
keyword keywds[] = { // must be alphabetical order
		KW(const_cast<char*>("abs_eps_f"), D_val, &ibex_abs_eps_f, const_cast<char*>("Absolute precision on the objective function. Default: 1.e-7. ")),
//		KW(const_cast<char*>("anticipated_UB"), I_val, &ibex_anticipated_UB, const_cast<char*>("If true, the search space is not only contracted w.r.t. f(x)<=loup, but f(x)<=loup-eps. Default: 1 (enable). ")),
//		KW(const_cast<char*>("bisect_ratio"), D_val, &ibex_bisect_ratio, const_cast<char*>("Ratio for choosing bisection point. Default: 0.5. ")),
		KW(const_cast<char*>("eps_h"), D_val, &ibex_eps_h, const_cast<char*>("Relaxation value of the equality constraints. Default: 1.e-8. ")),
		KW(const_cast<char*>("eps_x"), D_val, &ibex_eps_x, const_cast<char*>("Precision on the variable (**Deprecated**). Default: 0. ")),
		KW(const_cast<char*>("extended_COV"), I_val, &ibex_extended_COV, const_cast<char*>("Extended COV output file. Default: 1 (extended).")),
		KW(const_cast<char*>("inHC4"), I_val, &ibex_inHC4, const_cast<char*>("Activate inHC4. Default: true. ")),
		KW(const_cast<char*>("initial_loup"), D_val, &ibex_initial_loup, const_cast<char*>("Initilization of the upper bound with a known value. ")),
//		KW(const_cast<char*>("kkt"), I_val, &ibex_kkt, const_cast<char*>("Activate KKT contractor. Default: true for unconstrained problems. ")),
		KW(const_cast<char*>("objno"),  I_val, &ibex_objno, const_cast<char*>("Choose which objective function of the AMPL model: 0 = none, 1 = first. Default: 1.")),
		KW(const_cast<char*>("random_seed"), I_val, &ibex_random_seed, const_cast<char*>("Random seed (useful for reproducibility). Default: 0. ")),
		KW(const_cast<char*>("rel_eps_f"), D_val, &ibex_rel_eps_f, const_cast<char*>("Relative precision on the objective function. Default: 1.e-3. ")),
//		KW(const_cast<char*>("relax_ratio"), D_val, &ibex_relax_ratio, const_cast<char*>("Fix-point ratio for contraction based on linear relaxation. Default: 0.2. ")),
		KW(const_cast<char*>("rigor"), I_val, &ibex_rigor, const_cast<char*>("Activate rigor mode (certify feasibility of equalities). If true, feasibility of equalities is certified. Default: false. ")),
		KW(const_cast<char*>("timeout"), D_val, &ibex_timeout, const_cast<char*>("Timeout (time in seconds). Default: -1 (none). ")),
		KW(const_cast<char*>("outlev"), I_val, &ibex_trace2, const_cast<char*>("Activate trace. Updates of lower and upper bound are printed while minimizing. Default: -1 (none). ")),
		KW(const_cast<char*>("version"), Ver_val, 0, const_cast<char*>("report version")),
		KW(const_cast<char*>("wantsol"), WS_val, 0, WS_desc_ASL+5)
/** TODO
	KW("timing",  I_val, &timing,  "report I/O and solution times: 1 = stdout, 2 = stderr, 3 = both"),
	*/
};


static std::string xxxvers = (std::string)("AMPL/IBEXopt interface Version ")+ (_IBEX_RELEASE_) + (std::string)("\n");

static
Option_Info Oinfo = {
		const_cast<char*>("ibexopt"),          /* invocation name of solver */
		const_cast<char*>("IBEXOPT "),         /* solver name in startup "banner" */
		const_cast<char*>("ibexopt_options"),  /* name of solver_options environment var */
		keywds,                                /* key words */
		nkeywds,                               /* number of key words */
		0,                                     /* whether funcadd will be called */
		const_cast<char*>(xxxvers.c_str())     /*  for -v and Ver_key_ASL() */
	};



// (C++) code starts here ///////////////////////////////////////////////////////////////////////////

namespace ibex {

//const double AmplInterface::default_max_bound= 1.e20;


AmplOption::AmplOption():
		abs_eps_f(OptimizerConfig::default_abs_eps_f),
//		anticipated_UB(OptimizerConfig::default_anticipated_UB),
//		bisect_ratio(DefaultOptimizerConfig::default_bisect_ratio),
		eps_h(ExtendedSystem::default_eps_h),
		eps_x(OptimizerConfig::default_eps_x),
		extended_COV(OptimizerConfig::default_extended_cov),
		inHC4(DefaultOptimizerConfig::default_inHC4),
		initial_loup(POS_INFINITY),
//		kkt(false),
		random_seed(DefaultOptimizerConfig::default_random_seed),
		rel_eps_f(OptimizerConfig::default_rel_eps_f),
//		relax_ratio(DefaultOptimizerConfig::default_relax_ratio),
		rigor(DefaultOptimizerConfig::default_rigor),
		trace(OptimizerConfig::default_trace),
		timeout(OptimizerConfig::default_timeout)  {}

AmplInterface::AmplInterface(std::string nlfile) : asl(NULL), _nlfile(nlfile), _x(NULL), option(){

	size_t i=0;
	while (i<N_OPS) {
		opmap[ Intcast (r_ops[i]) ] = i;
		i++;
	}

	if (!readASLfg()) {
		ibex_error("Fail to read the ampl file.\n");
	}

	if (!readnl()) {
		ibex_error("Fail to read the nl file.\n");
	}
	if (!readoption()) {
		ibex_error("Fail to read the option.\n");
	}
}

AmplInterface::~AmplInterface() {
	for (int i =0; i< n_var;i++) {
			if (_x[i]) delete _x[i];
	}

	var_data.clear();
	opmap.clear();
	if (asl) {
		ASL_free(&asl);
	}
}

bool AmplInterface::writeSolution(Optimizer& o) {
	std::stringstream message;
	message << "IBEXopt  "<< _IBEX_RELEASE_ << " finish : ";
	Optimizer::Status status =o.get_status();
	switch(status) {
		case Optimizer::SUCCESS:
			message << " OPTIMIZATION SUCCESS! \n The global minimum (with respect to the precision required) has been found. In particular, at least one feasible point has been found, less than obj_init_bound, and in the time limit." ;
			solve_result_num=0;
			break;
		case Optimizer::INFEASIBLE:
			message << " INFEASIBLE PORBLEM. \n No feasible point exist less than obj_init_bound. In particular, the function returns INFEASIBLE if the initial bound \"obj_init_bound\" is LESS than the true minimum (this case is only possible if obj_abs_prec and obj_rel_prec are 0). In the latter case, there may exist feasible points." ;
			solve_result_num=200;
			break;
		case Optimizer::NO_FEASIBLE_FOUND:
			message << " NO FEASIBLE POINT FOUND. \n No feasible point could be found less than obj_init_bound. Contrary to INFEASIBLE, infeasibility is not proven here. Warning: this return value is sensitive to the abs_eps_f and rel_eps_f parameters. The upperbounding makes the optimizer only looking for points less than min{ (1-rel_eps_f)*obj_init_bound, obj_init_bound - abs_eps_f }.";
			solve_result_num=201;
			break;
		case Optimizer::UNBOUNDED_OBJ:
			message << " UNBOUNDED OBJECTIVE FONCTION. \n The objective function seems unbounded (tends to -oo).";
			solve_result_num=300;
			break;
		case Optimizer::TIME_OUT:
			message << " time limit " << o.timeout << "s. reached";
			solve_result_num=400;
			break;
		case Optimizer::UNREACHED_PREC:
			message << " UNREACHED PRECISION. \n The search is over but the resulting interval [uplo,loup] does not satisfy the precision requirements. There are several possible reasons: the goal function may be too pessimistic or the constraints function may be too pessimistic with respect to the precision requirement (which can be too stringent). This results in tiny boxes that can neither be contracted nor used as new loup candidates. Finally, the eps_x parameter may be too large." ;
			solve_result_num=402;
			break;
		}

	std::string tmp = message.str();
	Vector sol = o.get_loup_point().mid();
	write_sol(tmp.c_str(), sol.raw(), NULL, NULL);

	return true;
}




// Reads a NLP from an AMPL .nl file through the ASL methods
bool AmplInterface::readASLfg() {
	assert(asl == NULL);

	if (_nlfile == "")
		return false;

	char** argv = new char*[3];
	argv[0] = const_cast<char*>("dummy");
	argv[1] = strdup(_nlfile.c_str());
	argv[2] = NULL;

	// Create the ASL structure
	asl = (ASL*) ASL_alloc (ASL_read_fg);

	char* stub = getstub (&argv, &Oinfo);
	getopts (argv, &Oinfo);
	//getstops =  getstub + getopts

	// Although very intuitive, we shall explain why the second argument
	// is passed with a minus sign: it is to tell the ASL to retrieve
	// the nonlinear information too.
	FILE* nl = jac0dim (stub, - (fint) strlen (stub));

	// Set options in the asl structure
	want_xpi0 = 1 | 2;  // allocate initial values for primal and dual if available
	obj_no = 0;         // always want to work with the first (and only?) objective

	// read the rest of the nl file
	fg_read (nl, ASL_return_read_err | ASL_findgroups);

	//FIXME freeing argv and argv[1] gives segfault !!!
	//  free(argv[1]);
	//  delete[] argv;

	return true;
}



// Reads the solver option from the .nl file through the ASL methods
bool AmplInterface::readoption() {
	if (ibex_abs_eps_f) {
		option.abs_eps_f = ibex_abs_eps_f;
	}
//	if (ibex_anticipated_UB!=-1) {
//		option.anticipated_UB = (ibex_anticipated_UB!=0);
//	}
//	if (ibex_bisect_ratio) {
//		option.bisect_ratio = ibex_bisect_ratio;
//	}
	if (ibex_eps_h!=-1) {
		option.eps_h = ibex_eps_h;
	}
	if (ibex_eps_x) {
		option.eps_x = ibex_eps_x;
	}
	if (ibex_extended_COV!=-1) {
		option.extended_COV = (ibex_extended_COV!=0);
	}
	if (ibex_inHC4!=-1) {
		option.inHC4 = (ibex_inHC4!=0);
	}
	if (ibex_initial_loup < POS_INFINITY) {
		option.initial_loup = ibex_initial_loup;
	}

//	if (ibex_kkt!=-1) {
//		option.kkt = (ibex_kkt!=0);
//	}
	if (ibex_random_seed) {
		option.random_seed = ibex_random_seed;
	}
	if (ibex_rel_eps_f) {
		option.rel_eps_f = ibex_rel_eps_f;
	}
//	if (ibex_relax_ratio) {
//		option.relax_ratio = ibex_relax_ratio;
//	}
	if (ibex_rigor!=-1) {
		option.rigor = (ibex_rigor!=0);
	}
	if (ibex_trace2!=-10) {
		option.trace = ibex_trace2;
	}
	if (ibex_timeout) {
		option.timeout = ibex_timeout;
	}

	return true;
}


// Reads a NLP from an AMPL .nl file through the ASL methods
bool AmplInterface::readnl() {


	// the variable /////////////////////////////////////////////////////////////
	// TODO only continuous variables for the moment

	_x= new const ExprSymbol*[n_var];
	for (int i =0; i< n_var; i++) {
		_x[i] = &(ExprSymbol::new_(var_name(i) , Dim::scalar()));
	}
	//_x =new Variable(n_var,"x");
	IntervalVector bound(n_var);

		// Each has a linear and a nonlinear part
		// thanks to Dominique Orban:
		//        http://www.gerad.ca/~orban/drampl/def-vars.html
		//        http://www.gerad.ca/~orban/drampl/dag.html

	try {

	// lower and upper bounds of the variables ///////////////////////////////////////////////////////////////
		if (LUv) {
			real *Uvx_copy = Uvx;
			// LUv is the variable lower bound if Uvx!=0, and the variable lower and upper bound if Uvx == 0
			if (!Uvx_copy)
				for (int i=0; i<n_var; i++) {
					//bound[i] = Interval(  ((LUv[2*i] <= -default_max_bound) ? -default_max_bound : LUv[2*i] ),
						//						((LUv[2*i+1] >= default_max_bound) ? default_max_bound : LUv[2*i+1]) );
					bound[i] = Interval( LUv[2*i], LUv[2*i+1]);
				}
			else
				for (int i=n_var; i--;) {
					//bound[i] = Interval(	(LUv [i] <= -default_max_bound ? -default_max_bound : LUv[i] ),
						//						(Uvx_copy [i] >= default_max_bound ? default_max_bound : Uvx_copy[i]) );
					bound[i] = Interval( LUv[i], Uvx_copy[i]);
				}
		} // else it is [-oo,+oo]
		for (int i =0; i< n_var; i++) {
			add_var(*(_x[i]),bound[i]);
		}
		//add_var(*_x, bound);

	// objective functions /////////////////////////////////////////////////////////////
		//if (n_obj>1) {ibex_error("Error AmplInterface: too much objective function in the ampl model."); return false;}

		//for (int i = 0; i < n_obj; i++) {
		if (n_obj>0 && ibex_objno>0) {
			int i = ibex_objno -1 ;
			///////////////////////////////////////////////////
			//  the nonlinear part
			const ExprNode *body = &(nl2expr ((OBJ_DE [i]).e));

			////////////////////////////////////////////////
			// The linear part
			// count nonzero terms in linear part
			double coeff;
			int index;
			for (ograd *objgrad = Ograd [i]; objgrad; objgrad = objgrad -> next) {
				if ((dynamic_cast<const ExprConstant*>(body))&&(((ExprConstant*)(body))->is_zero())) {
					delete body;
					if (fabs (objgrad -> coef) != 0.0) {
						coeff = objgrad -> coef;
						index = objgrad -> varno;
						if (coeff==1) {
							body = ((_x[index]));
						} else if (coeff==-1) {
							body = &( - (*(_x[index])));
						} else {
							body = &(coeff * (*(_x[index])));
						}
					}
				} else {
					if (fabs (objgrad -> coef) != 0.0) {
						coeff = objgrad -> coef;
						index = objgrad -> varno;
						if (coeff==1) {
							body = &(*body + (*(_x[index])));
						} else if (coeff==-1) {
							body = &(*body - (*(_x[index])));
						} else {
							body = &(*body +coeff * (*(_x[index])));
						}
					}
				}
			}

			////////////////////////////////////////////////
			// Max or Min
			// 3rd/ASL/solvers/asl.h, line 336: 0 is minimization, 1 is maximization
			if (OBJ_sense [i] == 0) {
				add_goal(*body, obj_name(0));
			} else {
				add_goal(-(*body), obj_name(0));
			}
		}

	// constraints ///////////////////////////////////////////////////////////////////
		// allocate space for argument list of all constraints' summations
		// of linear and nonlinear terms
		const ExprNode **body_con = new const ExprNode*[n_con];
		///////////////////////////////////////////////////
		// The nonlinear part :
		//init array of each constraint with the nonlinear part
		for (int i = 0; i<n_con;i++)
			body_con[i] = &(nl2expr (CON_DE [i] . e));

		///////////////////////////////////////////////////
		// count all linear terms
		if (A_colstarts && A_vals)    {      // Constraints' linear info is stored in A_vals
			for (int j = 0; j < n_var; j++){
				for (int i = A_colstarts [j], k = A_colstarts [j+1] - i; k--; i++) {
					if ((dynamic_cast<const ExprConstant*>(body_con[A_rownos[i]]))&&(((ExprConstant*)(body_con[A_rownos[i]]))->is_zero())) {
						delete body_con[A_rownos[i]];
						if (A_vals[i]==1) {
							body_con[A_rownos[i]] = (((_x[j])));
						} else if (A_vals[i]==-1) {
							body_con[A_rownos[i]] = &(- (*(_x[j])));
						} else {
							body_con[A_rownos[i]] = &((A_vals[i]) * (*(_x[j])));
						}
					} else {
						if (A_vals[i]==1) {
							body_con[A_rownos[i]] = &(*(body_con[A_rownos[i]]) + (*(_x[j])));
						} else if (A_vals[i]==-1) {
							body_con[A_rownos[i]] = &(*(body_con[A_rownos[i]]) - (*(_x[j])));
						} else {
							body_con[A_rownos[i]] = &(*(body_con[A_rownos[i]]) + (A_vals[i]) * (*(_x[j])));
						}
					}

				}
			}
		} else {		// Constraints' linear info is stored in Cgrad
			cgrad *congrad;
			double coeff;
			int index;
			for ( int i = 0; i < n_con; i++)
				for (congrad = Cgrad [i]; congrad; congrad = congrad -> next) {
					if ((dynamic_cast<const ExprConstant*>(body_con[i]))&&(((ExprConstant*)(body_con[i]))->is_zero())) {
						delete body_con[i];
						coeff = congrad -> coef;
						if (fabs (coeff) != 0.0) {
							index = congrad -> varno;
							if (coeff==1) {
								body_con[i] = ( ((_x[index])));
							} else if (coeff==-1) {
								body_con[i] = &(-(*(_x[index])));
							} else {
								body_con[i] = &((coeff) * (*(_x[index])));
							}
						}

					} else {
						coeff = congrad -> coef;
						if (fabs (coeff) != 0.0) {
							index = congrad -> varno;
							if (coeff==1) {
								body_con[i] = &(*(body_con[i]) + (*(_x[index])));
							} else if (coeff==-1) {
								body_con[i] = &(*(body_con[i]) - (*(_x[index])));
							} else {
								body_con[i] = &(*(body_con[i]) + (coeff) * (*(_x[index])));
							}
						}
					}
				}
		}
		///////////////////////////////////////////////////
		// Kind of constraints : equality, inequality
		for (int i = 0; i < n_con; i++) {
			int sig;
			double lb, ub;

			/* LUrhs is the constraint lower bound if Urhsx!=0, and the constraint lower and upper bound if Uvx == 0 */
			if (Urhsx) {
				lb = LUrhs [i];
				ub = Urhsx [i];
			} else {
				lb = LUrhs [2*i];
				ub = LUrhs [2*i+1];
			}

			// set constraint sign
			if (negInfinity < lb)
				if (ub < Infinity)  sig =1; // EQ;
				else                sig =2; // GEQ;
			else                    sig =3; // LEQ;

			// add them (and set lower-upper bound)
			switch (sig) {

			case  1:  {
				if (lb==ub) {
					if (lb==0) {
						add_ctr_eq((*(body_con[i])));
					} else if (lb<0) {
						add_ctr_eq((*(body_con[i])+(-lb)));
					} else {
						add_ctr_eq((*(body_con[i])-lb));
					}
				} else  {
					 std::string name1 = "_1";
					 name1 = con_name(i)+name1;
					 std::string name2 = "_2";
					 name2 = con_name(i)+name2;
					 add_ctr(ExprCtr(*(body_con[i])-ub, LEQ), name1.c_str());
					 add_ctr(ExprCtr(*(body_con[i])-lb, GEQ), name2.c_str());
				}
				break;
			}
			case  2:  {
				if (lb==0) {
					add_ctr(ExprCtr(*(body_con[i]),GEQ),con_name(i));
				} else if (lb<0) {
					add_ctr(ExprCtr(*(body_con[i])+(-lb),GEQ),con_name(i));
				} else {
					add_ctr(ExprCtr(*(body_con[i])-lb,GEQ),con_name(i));
				}
				break;
			}
			case  3: {
				if (ub==0) {
					add_ctr(ExprCtr(*(body_con[i]),LEQ),con_name(i));
				} else if (ub<0) {
					add_ctr(ExprCtr(*(body_con[i])+(-ub),LEQ),con_name(i));
				} else {
					add_ctr(ExprCtr(*(body_con[i])-ub,LEQ),con_name(i));
				}
				break;
			}
			default: ibex_error("Error: could not recognize a constraint\n"); return false;
			}
		}


		delete[] body_con;

	} catch (...) {
		return false;
	}

	return true;
}

// converts an AMPL expression (sub)tree into an expression* (sub)tree
// thank to Dominique Orban for the explication of the DAG inside AMPL:
// http://www.gerad.ca/~orban/drampl/dag.html
const ExprNode& AmplInterface::nl2expr(expr *e) {

	switch (opmap[Intcast (e -> op)]) {

	case OPNUM:    return  (ExprConstant::new_scalar(((expr_n *)e)->v));
	case OPPLUS:   {
		if (opmap[Intcast (e->R.e->op)]==OPUMINUS) {
			return  (( nl2expr(e -> L.e)) - nl2expr((e->R.e)->L.e) );
		} else {
			if (opmap[Intcast (e->L.e->op)]==OPUMINUS) {
				return  (nl2expr(e->R.e) - nl2expr((e->L.e)->L.e) );
			} else {
				return  (nl2expr(e->R.e) + nl2expr(e->L.e) );
			}
		}
	}
	case OPMINUS:  {
		if (opmap[Intcast (e->R.e->op)]==OPUMINUS)  {
			return (( nl2expr(e->L.e)) + nl2expr((e->R.e)->L.e) );
		} else {
			return  (( nl2expr(e->L.e)) - nl2expr(e->R.e));
		}
	}
	case OPDIV:    return  (( nl2expr(e -> L.e)) / (nl2expr(e -> R.e)));
	case OPMULT:   return  (operator*((nl2expr (e -> L.e)) , (nl2expr(e -> R.e))));
	case OPPOW:    return  pow( nl2expr(e -> L.e), nl2expr(e -> R.e));
	case OP1POW:   {
		if (((int) (((expr_n *)e->R.e)->v) )==(((expr_n *)e->R.e)->v)) {
			return pow( nl2expr(e -> L.e), (int) (((expr_n *)e->R.e)->v));
		} else
			return pow( nl2expr(e -> L.e), ExprConstant::new_scalar(((expr_n *)e->R.e)->v));
	}
	case OP2POW:   return sqr( nl2expr(e -> L.e));
	case OPCPOW:   return pow(ExprConstant::new_scalar(((expr_n *)e->L.e)->v), nl2expr(e -> R.e));
	case MINLIST: {
		expr **ep = e->L.ep;
		const ExprNode* ee = &(nl2expr(*ep));
		ep++;
		while (ep < e->R.ep) {
			ee = &(min(*ee , nl2expr(*ep)));
			ep++;
		}
		return *ee;
	}
	case MAXLIST:  {
		expr **ep = e->L.ep;
		const ExprNode* ee = &(nl2expr(*ep));
		ep++;
		while (ep < e->R.ep) {
			ee = &(max(*ee , nl2expr(*ep)));
			ep++;
		}
		return *ee;
	}
	case OPSUMLIST: {
		expr **ep = e->L.ep;
		const ExprNode* ee = &(nl2expr(*ep));
		ep++;
		while (ep < e->R.ep) {
			if (opmap[Intcast ((*ep)->op)]==OPUMINUS) {
				ee = &(*ee - nl2expr((*ep)->L.e) );
			} else {
				ee = &(*ee + nl2expr(*ep) );
			}
			ep++;
		}
		return *ee;
	}
	case ABS:      return abs( nl2expr(e -> L.e));
	case OPUMINUS: {
		if (opmap[Intcast (e->L.e->op)]==OPUMINUS)  {
			return  (nl2expr((e -> L.e)->L.e));
		} else {
			return  (operator-(nl2expr(e -> L.e)));
		}
	}
	case OP_sqrt:  return sqrt(nl2expr(e -> L.e));
	case OP_exp:   return exp( nl2expr(e -> L.e));
	case OP_log:   return log( nl2expr(e -> L.e));
	case OP_log10: return ((ExprConstant::new_scalar(1.0/log(Interval(10.0)))) * log(nl2expr (e -> L.e)));
	case OP_cos:   return cos( nl2expr(e -> L.e));
	case OP_sin:   return sin( nl2expr(e -> L.e));
	case OP_tan:   return tan( nl2expr(e -> L.e));
	case OP_cosh:  return cosh(nl2expr(e -> L.e));
	case OP_sinh:  return sinh(nl2expr(e -> L.e));
	case OP_tanh:  return tanh(nl2expr(e -> L.e));
	case OP_acos:  return acos(nl2expr(e -> L.e));
	case OP_asin:  return asin(nl2expr(e -> L.e));
	case OP_atan:  return atan(nl2expr(e -> L.e));
	//case OP_asinh: notimpl ("asinh");
	//case OP_acosh: notimpl ("acosh");
	//case OP_atanh: notimpl ("atanh");
	case OP_atan2: return atan2(nl2expr(e -> L.e), nl2expr(e -> R.e));
	//case OPintDIV: notimpl ("intdiv");
	//case OPprecision: notimpl ("precision");
	//case OPround:  notimpl ("round");
	//case OPtrunc:  notimpl ("trunc");
	//case FLOOR:   notimpl ("floor");
	//case CEIL:    notimpl ("ceil");
	//case OPFUNCALL: notimpl ("function call");
	//case OPPLTERM:  notimpl ("plterm");
	//case OPIFSYM:   notimpl ("ifsym");
	//case OPHOL:     notimpl ("hol");
	//case OPREM:   notimpl ("remainder");
	//case OPLESS:  notimpl ("less");
	//case OPIFnl:  // TODO return (chi(nl2expr(????))) BoolInterval??
	                // see ASL/solvers/rops.c, see f_OPIFnl and  expr_if
	case OPVARVAL:  {
		int j = ((expr_v *) e) -> a;
		if (j<n_var) {
			return (*(_x[j]));
		}
		else {
			// http://www.gerad.ca/~orban/drampl/def-vars.html
			// common expression | defined variable
			int k = (expr_v *)e - VAR_E;
			double coeff;
			int index;

			if( k >= n_var ) {
				const ExprNode* body;
				// This is a common expression. Find pointer to its root.

				// Check if the common expression are already construct
				if (var_data.find(k)!=var_data.end()) {
					body = var_data[k];
				}
				else {

					// Constract the common expression
					j = k - n_var;
					if( j < ncom0 ) 	{
						cexp *common = CEXPS +j;
						// init with the nonlinear part
						body = &(nl2expr(common->e));

						int nlin = common->nlin; // Number of linear terms
						if( nlin > 0 ) {
							linpart * L = common->L;
							for(int i = 0; i < nlin; i++ ) {
								if ((dynamic_cast<const ExprConstant*>(body))&&(((ExprConstant*)(body))->is_zero())) {
									coeff = (L[i]).fac;
									index = ((uintptr_t) (L[i].v.rp) - (uintptr_t) VAR_E) / sizeof (expr_v);
									if (coeff==1) {
										body = &((*(_x[index])));
									} else if (coeff==-1) {
										body = &( - (*(_x[index])));
									} else if (coeff != 0) {
										body = &(coeff * (*(_x[index])));
									}
								} else {
									coeff = (L[i]).fac;
									index = ((uintptr_t) (L[i].v.rp) - (uintptr_t) VAR_E) / sizeof (expr_v);
									if (coeff==1) {
										body = &(*body + (*(_x[index])));
									} else if (coeff==-1) {
										body = &(*body - (*(_x[index])));
									} else if (coeff != 0) {
										body = &(*body +coeff * (*(_x[index])));
									}
								}
							}
						}
					}
					else {
						cexp1 *common = (CEXPS1 - ncom0) +j ;
						// init with the nonlinear part
						body = &(nl2expr(common->e));

						int nlin = common->nlin; // Number of linear terms
						if( nlin > 0 ) {
							linpart * L = common->L;
							for(int i = 0; i < nlin; i++ ) {
								if ((dynamic_cast<const ExprConstant*>(body))&&(((ExprConstant*)(body))->is_zero())) {
									coeff = (L[i]).fac;
									index = ((uintptr_t) (L[i].v.rp) - (uintptr_t) VAR_E) / sizeof (expr_v);
									if (coeff==1) {
										body = &( (*(_x[index])));
									} else if (coeff==-1) {
										body = &( - (*(_x[index])));
									} else if (coeff != 0) {
										body = &(coeff * (*(_x[index])));
									}
								} else {
									coeff = (L[i]).fac;
									index = ((uintptr_t) (L[i].v.rp) - (uintptr_t) VAR_E) / sizeof (expr_v);
									if (coeff==1) {
										body = &(*body + (*(_x[index])));
									} else if (coeff==-1) {
										body = &(*body - (*(_x[index])));
									} else if (coeff != 0) {
										body = &(*body +coeff * (*(_x[index])));
									}

								}
							}
						}
					}
					// Store the temporary variable, in case of reuse it late, to construct a DAG (and not just a tree.
					var_data[k] = body;;
				}
				return *body;

			} else {
				ibex_error("Error AmplInterface: unknown defined variable \n");
				throw -1;
			}
		}
	}

	default: {
		std::cout << opmap[Intcast (e -> op)] << std::endl;
		ibex_error( "Error AmplInterface: unknown operator or not implemented \n " );
		throw -2;
	}
	}
	return ExprConstant::new_scalar(0.);
}


}

