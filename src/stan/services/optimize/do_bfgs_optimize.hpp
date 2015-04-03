#ifndef STAN_SERVICES_OPTIMIZE_DO_BFGS_OPTIMIZE_HPP
#define STAN_SERVICES_OPTIMIZE_DO_BFGS_OPTIMIZE_HPP

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include <stan/services/error_codes.hpp>
#include <stan/services/io.hpp>

namespace stan {
  namespace services {
    namespace optimize {

      template<typename ModelT,typename BFGSOptimizerT,typename RNGT,
               typename OutputWriter, typename InfoWriter, typename Interrupt>
      int do_bfgs_optimize(ModelT &model,
                           BFGSOptimizerT &bfgs,
                           RNGT &base_rng,
                           double &lp,
                           std::vector<double> &cont_vector,
                           std::vector<int> &disc_vector,
                           OutputWriter& output_stream,
                           InfoWriter& info,
                           bool save_iterations,
                           int refresh,
                           Interrupt& interrupt) {
        lp = bfgs.logp();
        
        std::stringstream msg;
        msg << "initial log joint probability = " << lp;
        info(msg.str());
        
        if (save_iterations) {
          io::write_iteration(output_stream, model, base_rng,
                              lp, cont_vector, disc_vector);
        }

        int ret = 0;
        
        while (ret == 0) {  
          interrupt();
          if (io::do_print(bfgs.iter_num(), 50 * refresh)) {
            msg.str(std::string());
            msg.clear();
            msg << "    Iter " << "     log prob " << "       ||dx|| "
                << "     ||grad|| " <<  "      alpha " << "     alpha0 "
                << " # evals " << " Notes ";
            info(msg.str());
          }
              
          ret = bfgs.step();
          lp = bfgs.logp();
          bfgs.params_r(cont_vector);
              
          if (io::do_print(bfgs.iter_num(), ret != 0 || !bfgs.note().empty(),refresh)) {
            msg.str(std::string());
            msg.clear();
            msg << " " << std::setw(7) << bfgs.iter_num() << " ";
            msg << " " << std::setw(12) << std::setprecision(6) << lp << " ";
            msg << " " << std::setw(12) << std::setprecision(6) << bfgs.prev_step_size() << " ";
            msg << " " << std::setw(12) << std::setprecision(6) << bfgs.curr_g().norm() << " ";
            msg << " " << std::setw(10) << std::setprecision(4) << bfgs.alpha() << " ";
            msg << " " << std::setw(10) << std::setprecision(4) << bfgs.alpha0() << " ";
            msg << " " << std::setw(7) << bfgs.grad_evals() << " ";
            msg << " " << bfgs.note();
            info(msg.str());
          }
             
          if (save_iterations) {
            io::write_iteration(output_stream, model, base_rng,
                               lp, cont_vector, disc_vector);
          }
        }
            
        int return_code;
        if (ret >= 0) {
          info("Optimization terminated normally: ");
          return_code = stan::services::error_codes::OK;
        } else {
          info("Optimization terminated with error: ");
          return_code = stan::services::error_codes::SOFTWARE;
        }
        info("  " + bfgs.get_code_string(ret));

        return return_code;
      }
      
    } // optimize
  } // services
} // stan
#endif

