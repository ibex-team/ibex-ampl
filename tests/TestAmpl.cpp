/* ============================================================================
 * I B E X - System Tests
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Gilles Chabert
 * Created     : Aug 23, 2012
 * ---------------------------------------------------------------------------- */

#include "TestAmpl.h"
#include "ibex_AmplInterface.h"
#include "ibex_ExtendedSystem.h"
#include "ibex_NormalizedSystem.h"
#include "ibex_DefaultOptimizerConfig.h"

#include <sstream>

using namespace std;

namespace ibex {


void TestAmpl::factory01() {

	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex1.nl");
	System sys(inter);
	CPPUNIT_ASSERT(sys.nb_ctr==4);
	CPPUNIT_ASSERT(sys.nb_var==13);
	CPPUNIT_ASSERT(sys.args.size()==13);
	for (int i; i <13; i++)
		CPPUNIT_ASSERT(sys.args[i].dim==Dim::scalar());
	check(sys.goal->eval(IntervalVector(13,Interval(0))),Interval(-1));
	CPPUNIT_ASSERT(sameExpr(sys.goal->expr(),"((-exp(x[1]))+y)"));

	CPPUNIT_ASSERT(sys.box.size()==13);
	CPPUNIT_ASSERT(sys.box[12]==Interval(-1,1));

	CPPUNIT_ASSERT(sys.ctrs.size()==4);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==13);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==13);
	CPPUNIT_ASSERT(sys.f_ctrs.image_dim()==4);
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((((A[0,0]*x[0])+(A[0,1]*x[1]))+(A[0,2]*x[2]))-10)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"((((A[1,0]*x[0])+(A[1,1]*x[1]))+(A[1,2]*x[2]))-11)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"((((A[2,0]*x[0])+(A[2,1]*x[1]))+(A[2,2]*x[2]))-12)"));
	CPPUNIT_ASSERT(sys.ops[0]==EQ);
	CPPUNIT_ASSERT(sys.ops[1]==EQ);
	CPPUNIT_ASSERT(sys.ops[2]==EQ);
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[3].expr(),"((-x[0])+y)"));
	CPPUNIT_ASSERT(sys.ops[3]==GEQ);

}


void TestAmpl::factory02() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex3.nl");
	System sys(inter);

	CPPUNIT_ASSERT(sys.nb_ctr==5);
	CPPUNIT_ASSERT(sys.nb_var==2);
	CPPUNIT_ASSERT(sys.goal==NULL);
	CPPUNIT_ASSERT(sys.ctrs.size()==5);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.image_dim()==5);

	CPPUNIT_ASSERT(sameExpr(sys.ctrs[0].f.expr(),"((x+y)-1)"));
	CPPUNIT_ASSERT(sameExpr(sys.ctrs[1].f.expr(),"((x+y)+1)"));
	CPPUNIT_ASSERT(sys.ctrs[0].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[1].op==GEQ);


}


void TestAmpl::extend() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex2.nl");
	System _sys(inter);
	ExtendedSystem sys(_sys);

	CPPUNIT_ASSERT(sys.nb_ctr==4);
	CPPUNIT_ASSERT(sys.nb_var==5);
	CPPUNIT_ASSERT(sys.args.size()==5);
	for (int i; i <5; i++)
		CPPUNIT_ASSERT(sys.args[i].dim==Dim::scalar());
	CPPUNIT_ASSERT(sameExpr(sys.goal->expr(),"__goal__"));

	CPPUNIT_ASSERT(sys.box.size()==5);

	CPPUNIT_ASSERT(sys.ctrs.size()==4);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==5);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==5);
	CPPUNIT_ASSERT(sys.f_ctrs.image_dim()==4);
	CPPUNIT_ASSERT(sameExpr(sys.ctrs[0].f.expr(),"(((-cos(x[1]))+y)-__goal__)"));
	CPPUNIT_ASSERT(sys.ctrs[0].op==EQ);
	CPPUNIT_ASSERT(sameExpr(sys.ctrs[1].f.expr(),"(-(x[1]+x[0]))"));
	CPPUNIT_ASSERT(sys.ctrs[1].op==LEQ);
	CPPUNIT_ASSERT(sameExpr(sys.ctrs[2].f.expr(),"((x[1]+x[2])-y)"));
	CPPUNIT_ASSERT(sys.ctrs[2].op==LEQ);
	CPPUNIT_ASSERT(sameExpr(sys.ctrs[3].f.expr(),"(-((-x[0])+y))"));
	CPPUNIT_ASSERT(sys.ctrs[3].op==LEQ);

}


void TestAmpl::normalize() {

	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex3.nl");
	System _sys(inter);
	NormalizedSystem sys(_sys,0.5);

	CPPUNIT_ASSERT(sys.nb_ctr==6);
	CPPUNIT_ASSERT(sys.nb_var==2);
	CPPUNIT_ASSERT(sys.goal==NULL);
	CPPUNIT_ASSERT(sys.ctrs.size()==6);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.image_dim()==6);

	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((x+y)-1)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"(-((x+y)+1))"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"((x-y)-1)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[3].expr(),"(-((x-y)+1))"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[4].expr(),"((x-y)-0.5)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[5].expr(),"((-(x-y))+-0.5)") ||
				   sameExpr(sys.f_ctrs[5].expr(),"((-(x-y))-0.5)")   );
	CPPUNIT_ASSERT(sys.ctrs[0].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[1].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[2].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[3].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[4].op==LEQ);
	CPPUNIT_ASSERT(sys.ctrs[5].op==LEQ);
}


void TestAmpl::merge() {

	AmplInterface i1(SRCDIR_TESTS "/ex_ampl/ex7.nl");
	AmplInterface i2(SRCDIR_TESTS "/ex_ampl/ex8.nl");
	System sys1(i1);
	System sys2(i2);
	System sys3(sys1,sys2);

	for (int i=0; i<sys1.nb_ctr; i++)
		CPPUNIT_ASSERT(sameExpr(sys3.f_ctrs[i].expr(),sys1.f_ctrs[i].expr()));
	for (int i=0; i<sys2.nb_ctr; i++)
		CPPUNIT_ASSERT(sameExpr(sys3.f_ctrs[sys1.nb_ctr+i].expr(),sys2.f_ctrs[i].expr()));
}

void TestAmpl::variable1() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex4.nl" );
	System sys(inter);

	CPPUNIT_ASSERT(sameExpr(sys.goal->expr(),"(x+y)"));
	CPPUNIT_ASSERT(sys.ctrs.size()==3);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==2);
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"(x+y)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"x"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"(-y)"));
	CPPUNIT_ASSERT(sys.ops[0]==EQ);
	CPPUNIT_ASSERT(sys.ops[1]==LEQ);
	CPPUNIT_ASSERT(sys.ops[2]==GEQ);
}


void TestAmpl::variable2() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex5.nl" );
	System sys(inter);
	CPPUNIT_ASSERT(sameExpr(sys.goal->expr(),"(((x+y)*(x+y))+y)"));
	CPPUNIT_ASSERT(sys.ctrs.size()==3);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==2);
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((x+y)*(x+y))"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"(x+y)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"(-y)"));
	CPPUNIT_ASSERT(sys.ops[0]==LEQ);
	CPPUNIT_ASSERT(sys.ops[1]==EQ);
	CPPUNIT_ASSERT(sys.ops[2]==GEQ);
}

void TestAmpl::variable3() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex6.nl" );
	System sys(inter);

	CPPUNIT_ASSERT(sameExpr(sys.goal->expr(),"((((x+y)*(((((x+y)*(x+y))+((x+y)*(x+y)))-x)+y))+((((((x+y)*(x+y))+((x+y)*(x+y)))-x)+y)*(((x+y)*(x+y))+y)))-x)"));
	CPPUNIT_ASSERT(sys.ctrs.size()==3);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==2);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==2);
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((x+y)*(x+y))"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"(((-(((x+y)*(x+y))+((x+y)*(x+y))))+(2*x))-y)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"(x+y)"));
	CPPUNIT_ASSERT(sys.ops[0]==LEQ);
	CPPUNIT_ASSERT(sys.ops[1]==GEQ);
	CPPUNIT_ASSERT(sys.ops[2]==LEQ);
}

void TestAmpl::I5() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex8.nl" );
	System sys(inter);

	CPPUNIT_ASSERT(sys.ctrs.size()==11);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==13);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==13);
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((((x3^4*x9^7)+(-0.18324*((x4*x3)*x9)^3))+x1)-0.25428)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"((((x10^4*x6^7)+(-0.16275*((x1*x10)*x6)^3))+x2)-0.37842)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"((((x2^4*x10^7)+(-0.16955*((x1*x2)*x10)^3))+x3)-0.27162)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[3].expr(),"((((x1^4*x6^7)+(-0.15585*((x7*x1)*x6)^3))+x4)-0.19807)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[4].expr(),"((((x6^4*x3^7)+(-0.1995*((x7*x6)*x3)^3))+x5)-0.44166)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[5].expr(),"((((x5^4*x10^7)+(-0.18922*((x8*x5)*x10)^3))+x6)-0.14654)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[6].expr(),"((((x5^4*x8^7)+(-0.2118*((x2*x5)*x8)^3))+x7)-0.42937)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[7].expr(),"((((x7^4*x6^7)+(-0.17081*((x1*x7)*x6)^3))+x8)-0.07056)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[8].expr(),"((((x6^4*x8^7)+(-0.19612*((x10*x6)*x8)^3))+x9)-0.34504)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[9].expr(),"((((x8^4*x1^7)+(-0.21466*((x4*x8)*x1)^3))+x10)-0.42651)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[10].expr(),"((x11+x12)+x13)"));
	CPPUNIT_ASSERT(sys.ops[0]==EQ);
	CPPUNIT_ASSERT(sys.ops[1]==EQ);
	CPPUNIT_ASSERT(sys.ops[2]==EQ);
	CPPUNIT_ASSERT(sys.ops[3]==EQ);
	CPPUNIT_ASSERT(sys.ops[4]==EQ);
	CPPUNIT_ASSERT(sys.ops[5]==EQ);
	CPPUNIT_ASSERT(sys.ops[6]==EQ);
	CPPUNIT_ASSERT(sys.ops[7]==EQ);
	CPPUNIT_ASSERT(sys.ops[8]==EQ);
	CPPUNIT_ASSERT(sys.ops[9]==EQ);
	CPPUNIT_ASSERT(sys.ops[10]==EQ);
}

void TestAmpl::bearing() {
	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex7.nl" );
	System sys(inter);

	CPPUNIT_ASSERT(sys.ctrs.size()==12);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_arg()==13);
	CPPUNIT_ASSERT(sys.f_ctrs.nb_var()==13);
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[0].expr(),"((-1.42857*(x4*x6))+(10000*x8))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[1].expr(),"((10*(x7*x9))-(0.00968*(x3*(x1^4-x2^4))))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[2].expr(),"((143.3*(x10*x4))+(-10000*x7))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[3].expr(),"((3.14159*(x6*(0.001*x9)^3))-(6e-06*((x3*x4)*x13)))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[4].expr(),"((1010*(x12*x13))-(1.57079*(x6*x14)))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[5].expr(),"((-1096*exp((-3.55*log(x11))))+(56*log(((8.112*x3)+0.8))))"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[6].expr(),"((0.0307*x4^2)-(0.3864*((0.00628*(x1*x9))^2*x6)))"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[7].expr(),"((log(x2)-log(x1))+x13)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[8].expr(),"((x2^2-x1^2)+x14)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[9].expr(),"(((-0.5*x10)+x11)-560)"));
	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[10].expr(),"(x1-x2)"));
//	CPPUNIT_ASSERT(sameExpr(sys.f_ctrs[11].expr(),"((1010*x12)+(-15707.9*x14))"));

	CPPUNIT_ASSERT(sys.ops[0]==EQ);
	CPPUNIT_ASSERT(sys.ops[1]==EQ);
	CPPUNIT_ASSERT(sys.ops[2]==EQ);
	CPPUNIT_ASSERT(sys.ops[3]==EQ);
	CPPUNIT_ASSERT(sys.ops[4]==EQ);
	CPPUNIT_ASSERT(sys.ops[5]==EQ);
	CPPUNIT_ASSERT(sys.ops[6]==LEQ);
	CPPUNIT_ASSERT(sys.ops[7]==EQ);
	CPPUNIT_ASSERT(sys.ops[8]==EQ);
	CPPUNIT_ASSERT(sys.ops[9]==EQ);
	CPPUNIT_ASSERT(sys.ops[10]==GEQ);
	CPPUNIT_ASSERT(sys.ops[11]==LEQ);

}

void TestAmpl::option1() {

	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/ex1.nl" );

	CPPUNIT_ASSERT(inter.get_abs_eps_f()==OptimizerConfig::default_abs_eps_f);
	CPPUNIT_ASSERT(inter.get_rel_eps_f()==OptimizerConfig::default_rel_eps_f );
	CPPUNIT_ASSERT(inter.get_eps_h()==ExtendedSystem::default_eps_h );
	CPPUNIT_ASSERT(inter.get_timeout()==OptimizerConfig::default_timeout );
	CPPUNIT_ASSERT(inter.get_random_seed()==DefaultOptimizerConfig::default_random_seed );
	CPPUNIT_ASSERT(inter.get_init_obj_value()==POS_INFINITY);
	CPPUNIT_ASSERT(inter.get_rigor()==-1 );
	CPPUNIT_ASSERT(inter.get_kkt()==-1 );
	CPPUNIT_ASSERT(inter.get_inHC4()==-1 );
	CPPUNIT_ASSERT(inter.get_obj_numb()==1 );
	CPPUNIT_ASSERT(inter.get_trace()==1 );

}


void TestAmpl::option2() {

//	AmplInterface inter(SRCDIR_TESTS "/ex_ampl/option2.nl" );
//	std::cout << inter.option.abs_eps_f << std::endl;
//	CPPUNIT_ASSERT(inter.option.abs_eps_f==1.e-1);
//	CPPUNIT_ASSERT(inter.option.rel_eps_f==1.e-2 );
//	CPPUNIT_ASSERT(inter.option.eps_h==10);
//	CPPUNIT_ASSERT(inter.option.timeout==1000 );
//	CPPUNIT_ASSERT(inter.option.random_seed==20);
//	CPPUNIT_ASSERT(inter.option.simpl_level==2 );
//	CPPUNIT_ASSERT(inter.option.initial_loup==30);
//	CPPUNIT_ASSERT(inter.option.rigor==true );
//	CPPUNIT_ASSERT(inter.option.kkt==true );
//	CPPUNIT_ASSERT(inter.option.objno==2 );
//	CPPUNIT_ASSERT(inter.option.trace==3 );
}


} // end namespace
