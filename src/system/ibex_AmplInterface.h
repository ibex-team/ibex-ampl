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

	/** Goal absolute precision: 1e-7. */
	double abs_eps_f;
//	/** Anticipated upper bounding : true (enabled). */
//	bool anticipated_UB;
//	/** Ratio for choosing bisection point. */
//	double bisect_ratio;
	/**  Equality thickness. */
	double eps_h;
	/** Bisection precision: 0. */
	double eps_x;
	/** COV output mode: extended (enabled). */
	bool extended_COV;
	/** inHC4 mode: true (enabled). */
	bool inHC4;
	/** Initial value of the upper bound */
	double initial_loup;
//	bool kkt;
	int random_seed;
	/** Goal relative precision: 1e-3. */
	double rel_eps_f;
//	/** Fix-point ratio for contraction based on linear relaxation. */
//	double relax_ratio;
	/** Activate/deactivate rigor mode. If true, feasibility of equalities is certified. */
	bool rigor;
	/** Trace mode: 0 (none). */
	int trace;
	/** Timeout: -1 (none). */
	double timeout;
};


class AmplInterface : public SystemFactory  {
private:

	ASL*     asl;
	std::string _nlfile;
	const ExprSymbol ** _x;

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
