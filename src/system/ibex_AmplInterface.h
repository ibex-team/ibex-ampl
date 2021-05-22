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
#include "ibex/ibex_OptimizerConfig.h"

struct ASL;
struct expr;


namespace ibex {



class AmplInterface : public SystemFactory  {
public:
	AmplInterface(std::string nlfile);

	virtual ~AmplInterface();

	bool writeSolution(Optimizer& o);

	/** see #set_rel_eps_f(). */
	double get_rel_eps_f() const;

	/** see #set_abs_eps_f(). */
	double get_abs_eps_f() const;

	/** \see #set_eps_h(). */
	double get_eps_h() const;

	/** \see #set_rigor(). */
	int get_rigor() const;

	/** \see #set_inHC4(). */
	int get_inHC4() const;

	/** \see #set_kkt(). */
	int get_kkt() const;

	/** \see #set_random_seed(). */
	double get_random_seed() const;

	/** \see #set_trace(). */
	int get_trace() const;

	/** \see #set_timeout(). */
	double get_timeout() const;

	/** \see #set_init_obj_value(). */
	double get_init_obj_value() const;

	/** \see #set_obj_numb(). */
	int get_obj_numb() const;

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

	/** Absolute precision on the objective function. Default: 1.e-7. */
	double abs_eps_f;

	/** Relaxation value of the equality constraints. Default: 1.e-8.  */
	double eps_h;

	/** Initialization of the upper bound with a known value. Default: +infinity. */
	double init_obj_value;

	/** Activate KKT contractor.
	 * \see #set_kkt().  */
	int kkt;

	/** Default inHC4 mode.
	 * \see #set_inHC4().*/
	int inHC4;

	/** Choose which objective function of the AMPL model: 0 = none, 1 = first. Default: 1. */
	int obj_numb;

	/** Random seed (useful for reproducibility). Default: 1.  */
	double random_seed;

	/** Relative precision on the objective. Default value is 1e-3.  */
	double rel_eps_f;

	/** Activate rigor mode (certify feasibility of equalities). If true, feasibility of equalities is certified.
	 * \see #set_rigor(). */
	int rigor;

	/** Timeout (time in seconds). Default: -1 (none).  */
	double timeout;

	/** Activate trace. Updates of lower and upper bound are printed while minimizing. Default: 1.  */
	int trace;


	/**
	 * \brief Set relative precision on the objective.
	 */
	void set_rel_eps_f(double rel_eps_f);

	/**
	 * \brief Set absolute precision on the objective.
	 */
	void set_abs_eps_f(double abs_eps_f);

	/**
	 * \brief Set equality thickness.
	 *
	 * By default: #ExtendedSystem::default_eps_h.
	 *
	 */
	void set_eps_h(double eps_h);

	/**
	 * \brief Activate/deactivate rigor mode.
	 *
	 * Possible value:
	 * * -1 : By default: #default_rigor.
	 * *  0 : Deactivate rigor mode
	 * *  1 : Activate rigor mode, feasibility of equalities is certified.
	 *
	 */
	void set_rigor(int rigor);

	/**
	 * \brief Activate/deactivate inHC4.
	 *
	 * Possible value:
	 * * -1 : By default: #default_inHC4.
	 * *  0 : Deactivate inHC4
	 * *  1 : Activate inHC4, feasibility is also tried with LoupFinderInHC4.
	 *
	 * Currently, the mode may not be activated because
	 * inHC4 is not implemented for all operators.
	 * This can be checked using #with_inHC4.
	 */
	void set_inHC4(int inHC4);

	/**
	 * \brief Activate/deactivate KKT contractor.
	 *
	 * Possible value:
	 * * -1 : By default, we apply KKT only for unconstrained problems.
	 * *  0 : deactivate KKT contractor
	 * *  1 : activate KKT contractor
	 *
	 * \note If KKT is enabled for constrained problems, the rigor mode is
	 * automatically enforced.
	 */
	void set_kkt(int kkt);

	/**
	 * \brief Set random seed
	 *
	 * The sequence of random numbers is reinitialized with
	 * this seed before calling optimize(..) (useful for
	 * reproducibility).
	 *
	 * Set by default to #default_random_seed.
	 */
	void set_random_seed(double random_seed);


	/**
	 * \brief Set trace activation flag.
	 *
	 * The value can be fixed by the user.
	 * * 0 : nothing is printed
	 * * 1 : prints every loup/uplo update.
	 * * 2 : prints also each handled node (warning: can generate very long trace).
	 * Set by default to 1.
	 */
	void set_trace(int trace);

	/**
	 * \brief Set time limit.
	 *
	 * Maximum CPU time used by the strategy.
	 * This parameter allows to bound time consumption.
	 * The value can be fixed by the user.
	 */
	void set_timeout(double timeout);

	/**
	 * \brief Set the initial bound of the objective function
	 *
	 * Initialization of the upper bound with a known value.
	 * Default: +infinity.
	 */
	void set_init_obj_value(double bound);

	/** \brief Set the choice of the objective function
	 *  of the AMPL model
	 *  * 0 = none,
	 *  * 1 = first.
	 *  Default: 1.
	 */
	void set_obj_numb(int num);

};



inline double AmplInterface::get_rel_eps_f() const   { return rel_eps_f; }

inline double AmplInterface::get_abs_eps_f() const   { return abs_eps_f; }

inline double AmplInterface::get_eps_h() const       { return eps_h; }

inline int   AmplInterface::get_rigor() const      { return rigor; }

inline int   AmplInterface::get_inHC4() const      { return inHC4; }

inline int   AmplInterface::get_kkt() const        { return kkt; }

inline double AmplInterface::get_random_seed() const { return random_seed; }

inline int    AmplInterface::get_trace() const       { return trace; }

inline double AmplInterface::get_timeout() const     { return timeout; }

inline double AmplInterface::get_init_obj_value() const { return init_obj_value; }

inline int    AmplInterface::get_obj_numb() const       { return obj_numb; }



inline void AmplInterface::set_rel_eps_f(double _rel_eps_f)  { rel_eps_f = _rel_eps_f; }

inline void AmplInterface::set_abs_eps_f(double _abs_eps_f)  { abs_eps_f = _abs_eps_f; }

inline void AmplInterface::set_trace(int _trace)             { trace = _trace; }

inline void AmplInterface::set_timeout(double _timeout)      { timeout = _timeout; }

inline void AmplInterface::set_eps_h(double _eps_h)          { eps_h = _eps_h;}

inline void AmplInterface::set_rigor(int _rigor)            { rigor = _rigor;}

inline void AmplInterface::set_inHC4(int _inHC4)            { inHC4 = _inHC4;}

inline void AmplInterface::set_kkt(int _kkt)                { kkt = _kkt;}

inline void AmplInterface::set_random_seed(double _seed)     { random_seed = _seed;}

inline void AmplInterface::set_init_obj_value(double bound) { init_obj_value = bound; }

inline void AmplInterface::set_obj_numb(int num)        { obj_numb = num; }

} /* end namespace ibex */


#endif /* IBEX_AMPLINTERFACE_H_ */
