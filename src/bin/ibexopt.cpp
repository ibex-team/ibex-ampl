//============================================================================
//                                  I B E X
//
//                               ************
//                                  IbexOpt
//                               ************
//
// Author      : Gilles Chabert
// Copyright   : IMT Atlantique (France)
// License     : See the LICENSE file
// Last Update : Jul 09, 2017
//============================================================================

#include "ibex.h"
#include "parse_args.h"
#include "ibex_AmplInterface.h"

#include <sstream>

using namespace std;
using namespace ibex;

int main(int argc, char** argv) {

#ifdef __IBEX_NO_LP_SOLVER__
	ibex_error("ibexopt requires a LP Solver (use --lp-lib with waf or -DLP_LIB with cmake)");
	exit(1);
#endif

	stringstream _rel_eps_f, _abs_eps_f, _eps_h, _random_seed, _eps_x;
	_rel_eps_f << "Relative precision on the objective. Default value is 1e" << round(::log10(OptimizerConfig::default_rel_eps_f)) << ".";
	_abs_eps_f << "Absolute precision on the objective. Default value is 1e" << round(::log10(OptimizerConfig::default_abs_eps_f)) << ".";
	_eps_h << "Equality relaxation value. Default value is 1e" << round(::log10(NormalizedSystem::default_eps_h)) << ".";
	_random_seed << "Random seed (useful for reproducibility). Default value is " << DefaultOptimizerConfig::default_random_seed << ".";
	_eps_x << "Precision on the variable (**Deprecated**). Default value is 0.";

	args::ArgumentParser parser("********* IbexOpt (defaultoptimizer) *********.", "Solve a Minibex file.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag version(parser, "version", "Display the version of this plugin (same as the version of Ibex).", {'v',"version"});
	args::ValueFlag<double> rel_eps_f(parser, "float", _rel_eps_f.str(), {'r', "rel-eps-f"});
	args::ValueFlag<double> abs_eps_f(parser, "float", _abs_eps_f.str(), {'a', "abs-eps-f"});
	args::ValueFlag<double> eps_h(parser, "float", _eps_h.str(), {"eps-h"});
	args::ValueFlag<double> timeout(parser, "float", "Timeout (time in seconds). Default value is +oo.", {'t', "timeout"});
	args::ValueFlag<double> random_seed(parser, "float", _random_seed.str(), {"random-seed"});
	args::ValueFlag<double> eps_x_arg(parser, "float", _eps_x.str(), {"eps-x"});
	args::ValueFlag<int>    simpl_level(parser, "int", "Expression simplification level. Possible values are:\n"
			"\t\t* 0:\tno simplification at all (fast).\n"
			"\t\t* 1:\tbasic simplifications (fairly fast). E.g. x+1+1 --> x+2\n"
			"\t\t* 2:\tmore advanced simplifications without developing (can be slow). E.g. x*x + x^2 --> 2x^2\n"
			"\t\t* 3:\tsimplifications with full polynomial developing (can blow up!). E.g. x*(x-1) + x --> x^2\n"
			"Default value is : 1.", {"simpl"});
	args::ValueFlag<double> initial_loup(parser, "float", "Intial \"loup\" (a priori known upper bound).", {"initial-loup"});
	args::ValueFlag<string> input_file(parser, "filename", "COV input file. The file contains "
			"optimization data in the COV (binary) format.", {'i',"input"});
	args::ValueFlag<string> output_file(parser, "filename", "COV output file. The file will contain the "
			"optimization data in the COV (binary) format. See --format", {'o',"output"});
	args::Flag rigor(parser, "rigor", "Activate rigor mode (certify feasibility of equalities).", {"rigor"});
	args::Flag kkt(parser, "kkt", "Activate contractor based on Kuhn-Tucker conditions.", {"kkt"});
	args::Flag output_no_obj(parser, "output-no-obj", "Generate a COV with domains of variables only (not objective values).", {"output-no-obj"});
	args::Flag trace(parser, "trace", "Activate trace. Updates of loup/uplo are printed while minimizing.", {"trace"});
	args::Flag format(parser, "format", "Give a description of the COV format used by IbexOpt", {"format"});
	args::ValueFlag<string> no_split_arg(parser, "vars","Prevent some variables to be bisected, separated by '+'.\nExample: --no-split=x+y",{"no-split"});
	args::Flag fquiet(parser, "quiet", "Print no report on the standard output.",{'q',"quiet"});
	args::ValueFlag<std::string> ampl1(parser, "MPL", "option -AMPL activate the AMPL output.", {'A'});
	args::Flag ampl2(parser, "AMPL",  "option -AMPL activate the AMPL output.",{"AMPL"});

	args::Positional<std::string> filename(parser, "filename", "The name of the MINIBEX file of the AMPL file.");

	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (args::Help&)
	{
		std::cout << parser;
		return 0;
	}
	catch (args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	catch (args::ValidationError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	if (version) {
		cout << "IbexOpt Release " << _IBEX_RELEASE_ << endl;
		exit(0);
	}

	if (format) {
		cout << CovOptimData::format();
		exit(0);
	}

	if (filename.Get()=="") {
		ibex_error("no input file (try ibexopt --help)");
		exit(1);
	}

	if (args::get(ampl1)!="MPL") {
		std::cerr << parser;
		return 1;
	}
	bool quiet =false;
	if (fquiet) {
		quiet = true;
	}

	bool option_ampl=false;
	double initial_loup1=POS_INFINITY;

	if (args::get(ampl1)=="MPL" || ampl2) {
			option_ampl = true;
			quiet =true;
	}

	try {

		System *sys;
		AmplInterface *ampl;

		cout << "IbexOpt  "<< _IBEX_RELEASE_ << " is running..."<< endl;

		string extension = filename.Get().substr(filename.Get().find_last_of('.')+1);
		if (extension == "nl" || option_ampl) {


			ampl = new AmplInterface(filename.Get());
			if (simpl_level)
				ampl->set_simplification_level(simpl_level.Get());
			sys = new System(*ampl);

		}
		else {
			// Load a system of equations
			sys = new System(filename.Get().c_str(), simpl_level? simpl_level.Get() : ExprNode::default_simpl_level);
		}
		
		DefaultOptimizerConfig config(*sys);

		if (extension == "nl" || option_ampl) {

			config.set_rel_eps_f(ampl->get_rel_eps_f());
			config.set_abs_eps_f(ampl->get_abs_eps_f());
			config.set_eps_h(ampl->get_eps_h());
			config.set_timeout(ampl->get_timeout());
			config.set_random_seed(ampl->get_random_seed());

			if (ampl->get_rigor() >=0) {
				config.set_rigor(ampl->get_rigor());
			}
			if (ampl->get_kkt() >=0) {
				config.set_kkt(ampl->get_kkt());
			}
			if (ampl->get_inHC4() >=0) {
				config.set_inHC4(ampl->get_inHC4());
			}

			config.set_trace(ampl->get_trace());
			initial_loup1 = ampl->get_init_obj_value();

		}


		string output_cov_file; // cov output file
		bool overwitten=false;  // is it overwritten?
		string cov_copy;

		if (!sys->goal) {
			ibex_error(" input file has not goal (it is not an optimization problem).");
		}

		if (!quiet) {
			cout << endl << "************************ setup ************************" << endl;
			cout << "  file loaded:\t\t" << filename.Get() << endl;
		}

		if (rel_eps_f) {
			config.set_rel_eps_f(rel_eps_f.Get());

			if (!quiet)
				cout << "  rel_eps_f:\t\t" << rel_eps_f.Get() << "\t(relative precision on objective)" << endl;
		}

		if (abs_eps_f) {
			config.set_abs_eps_f(abs_eps_f.Get());
			if (!quiet)
				cout << "  abs_eps_f:\t\t" << abs_eps_f.Get() << "\t(absolute precision on objective)" << endl;
		}

		if (eps_h) {
			config.set_eps_h(eps_h.Get());
			if (!quiet)
				cout << "  eps_h:\t\t" << eps_h.Get() << "\t(equality thickening)" << endl;
		}

		if (eps_x_arg) {
			if (!quiet)
				cout << "  eps_x:\t\t" << eps_x_arg.Get() << "\t(precision on variables domain)" << endl;
		}

		Vector eps_x(sys->nb_var, eps_x_arg ? eps_x_arg.Get() : OptimizerConfig::default_eps_x);

		if (no_split_arg) {
			if (!quiet)
				cout << "  don't split:\t\t";

			vector<const ExprNode*> no_split = parse_symbols_list(sys->args, no_split_arg.Get());

			if (!quiet) {
				for (vector<const ExprNode*>::const_iterator it=no_split.begin(); it!=no_split.end(); ++it)
					cout << **it << ' ';
				cout << endl;
			}

			if (!no_split.empty()) {
				// we use VarSet for convenience (handling of indexed symbols)
				VarSet varset(sys->f_ctrs,no_split,true);

				for (int i=0; i<varset.nb_var; i++) {
					eps_x[varset.var(i)]=POS_INFINITY;
				}
				for (vector<const ExprNode*>::iterator it=no_split.begin(); it!=no_split.end(); ++it) {
					cleanup(**it,false);
				}
			}
		}

		config.set_eps_x(eps_x);

		// This option certifies feasibility with equalities
		if (rigor) {
			config.set_rigor(rigor.Get());
			if (!quiet)
				cout << "  rigor mode:\t\tON\t(feasibility of equalities certified)" << endl;
		}

		if (kkt) {
			config.set_kkt(kkt.Get());
			if (!quiet)
				cout << "  KKT contractor:\tON" << endl;
		}

		if (simpl_level)
			if (!quiet)
				cout << "  symbolic simpl level:\t" << simpl_level.Get() << "\t" << endl;

		if (initial_loup) {
			initial_loup1 = initial_loup.Get();
			if (!quiet)
				cout << "  initial loup:\t\t" << initial_loup.Get() << " (a priori upper bound of the minimum)" << endl;
		}

		// Fix the random seed for reproducibility.
		if (random_seed) {
			config.set_random_seed(random_seed.Get());
			if (!quiet)
				cout << "  random seed:\t\t" << random_seed.Get() << endl;
		}

		if (input_file) {
			if (!quiet) {
				cout << "  input COV file:\t" << input_file.Get().c_str() << "\n";
			}
		}

		if (output_file) {
			output_cov_file = output_file.Get();
		} else if (!option_ampl) {
			// got from stackoverflow.com:
			string::size_type const p(filename.Get().find_last_of('.'));
			// filename without extension
			string filename_no_ext=filename.Get().substr(0, p);
			stringstream ss;
			ss << filename_no_ext << ".cov";
			output_cov_file=ss.str();

			ifstream file;
			file.open(output_cov_file.c_str(), ios::in); // to check if it exists

			if (file.is_open()) {
				overwitten = true;
				stringstream ss;
				ss << output_cov_file << "~";
				cov_copy=ss.str();
				// got from stackoverflow.com:
				ofstream dest(cov_copy, ios::binary);

			    istreambuf_iterator<char> begin_source(file);
			    istreambuf_iterator<char> end_source;
			    ostreambuf_iterator<char> begin_dest(dest);
			    copy(begin_source, end_source, begin_dest);
			}
			file.close();
		}

		if (!quiet) {
			cout << "  output COV file:\t" << output_cov_file << "\n";
		}

		// This option limits the search time
		if (timeout) {
			if (!quiet)
				cout << "  timeout:\t\t" << timeout.Get() << "s" << endl;
			config.set_timeout(timeout.Get());
		}

		// This option prints each better feasible point when it is found
		if (trace) {
			if (!quiet)
				cout << "  trace:\t\tON" << endl;
			config.set_trace(trace.Get());
		}

		// Question: is really inHC4 good?
		config.set_inHC4(true);

		if (!config.with_inHC4()) {
			cerr << "\n  \033[33mwarning: inHC4 disabled\033[0m (unimplemented operator)" << endl;
		}

		if (output_no_obj) {
			if (!quiet)
				cout << "  Generates COV with:\tvariable domains only\n";
			config.set_extended_cov(false);
		}

		if (!quiet) {
			cout << "*******************************************************" << endl << endl;
		}


		// Build the default optimizer
		Optimizer o(config);

		// display solutions with up to 12 decimals
		cout.precision(12);

		if (!quiet)
			cout << "running............" << endl << endl;

		// Search for the optimum
		// Get the solutions
		if (input_file)
			o.optimize(input_file.Get().c_str(), initial_loup1);
		else
			o.optimize(sys->box, initial_loup1);

		if (trace) cout << endl;

		// Report some information (computation time, etc.)

		if (!quiet)
			o.report();

		if (!option_ampl) {
			o.get_data().save(output_cov_file.c_str());

			if (!quiet) {
				cout << " results written in " << output_cov_file << "\n";
				if (overwitten)
					cout << " (old file saved in " << cov_copy << ")\n";
			}
		} else {

			//  si l'option -AMPL est présent, ecrire le fichier .sol pour ampl

			o.report();
			ampl->writeSolution(o);

			if (ampl) {
				delete ampl;
			}

		}
		delete sys;

		return 0;

	}
	catch(ibex::UnknownFileException& e) {
		cerr << "Error: cannot read file '" << filename.Get() << "'" << endl;
	}
	catch(ibex::SyntaxError& e) {
		cout << e << endl;
	}
}
