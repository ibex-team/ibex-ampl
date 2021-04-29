//============================================================================
//                                  I B E X
// File        : ibex_AmplInterface.h Adapted from CouenneAmplInterface
// Author      : Jordan Ninin
// License     : See the LICENSE file
// Created     : Nov 5, 2020
// Last Update : Nov 5, 2020
//============================================================================


#ifndef __IBEX_AMPL_INTERFACE_H__
#define __IBEX_AMPL_INTERFACE_H__


#include "ibex/ibex_SystemFactory.h"
#include "ibex/ibex_Expr.h"
#include "ibex/ibex_Interval.h"
#include "ibex/ibex_Setting.h"
#include <string>

#ifdef __GNUC__
#include <ciso646> // just to initialize _LIBCPP_VERSION
#ifdef _LIBCPP_VERSION
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#else
#if (_MSC_VER >= 1600)
#include <unordered_map>
#else
#include <unordered_map>
#endif // (_MSC_VER >= 1600)
#endif

#include "ibex/ibex_Optimizer.h"


struct ASL;
struct expr;

namespace ibex {
class AmplOption {
public:
	AmplOption();
	/** Absolute precision on the objective function. Default: 1.e-7. */
	double abs_eps_f;

	/** Relaxation value of the equality constraints. Default: 1.e-8.  */
	double eps_h;

	/** Initialization of the upper bound with a known value. Default: +infinity. */
	double initial_loup;

	/** Activate KKT contractor. Default: 0.  */
	bool kkt;

	/** Choose which objective function of the AMPL model: 0 = none, 1 = first. Default: 1. */
	int objno;

	/** Random seed (useful for reproducibility). Default: 1.  */
	double random_seed;

	/** Relative precision on the objective. Default value is 1e-3.  */
	double rel_eps_f;

	/** Activate rigor mode (certify feasibility of equalities). If true, feasibility of equalities is certified. Default: 0.  */
	bool rigor;

	/** Expression simplification level. Possible values are:\n \t\t* 0:\t no simplification at all (fast).
	 *  * 1: basic simplifications (fairly fast). E.g. x+1+1 --> x+2
	 *  * 2: more advanced simplifications without developing (can be slow). E.g. x*x + x^2 --> 2x^2
	 *  * 3: simplifications with full polynomial developing (can blow up!). E.g. x*(x-1) + x --> x^2
	 *  Default value is : 1. */
	int simpl_level;

	/** Timeout (time in seconds). Default: -1 (none).  */
	double timeout;

	/** Activate trace. Updates of lower and upper bound are printed while minimizing. Default: -1 (none).  */
	int trace;

};


class AmplInterface : public SystemFactory  {
private:

	ASL*     asl;
	std::string _nlfile;
	const ExprSymbol ** _x;

	/**  var_data: map which containts expressions of the temporary variable already defined */
	/**  opmap: map to convert efunc* of AMPL to an operator */
#ifdef __GNUC__
#include <ciso646> // just to initialize _LIBCPP_VERSION
#ifdef _LIBCPP_VERSION
	std::unordered_map<int, const ExprNode*> var_data;
	std::unordered_map<size_t, size_t> opmap;
#else
	std::tr1::unordered_map<int, const ExprNode*> var_data;
	std::tr1::unordered_map<size_t, size_t> opmap;
#endif
#else
#if (_MSC_VER >= 1600)
	std::unordered_map<int, const ExprNode*> var_data
	std::unordered_map<size_t, size_t> opmap;
#else
	std::tr1::unordered_map<int, const ExprNode*> var_data;
	std::tr1::unordered_map<size_t, size_t> opmap;
#endif // (_MSC_VER >= 1600)
#endif

	bool readnl();
	bool readoption();
	bool readASLfg();
	const ExprNode& nl2expr(expr *e);


public:
	AmplInterface(std::string nlfile);

	AmplOption option;
	virtual ~AmplInterface();

	bool writeSolution(Optimizer& o);

	//static const double default_max_bound;

};

}


#endif /* IBEX_AMPLINTERFACE_H_ */
