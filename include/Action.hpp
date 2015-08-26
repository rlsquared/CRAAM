#ifndef ACTION_H
#define ACTION_H

#include <utility>
#include <vector>
#include <limits>

#include "definitions.hpp"
#include "Transition.hpp"

using namespace std;

namespace craam {

class Action {

public:
    vector<Transition> outcomes;
    vector<prec_t> distribution;
    prec_t threshold;

    Action(){};
    Action(vector<Transition> outcomes){
        this->outcomes = outcomes;
        threshold = -1;
    }

    // plain solution
    pair<long,prec_t> maximal(vector<prec_t> const& valuefunction, prec_t discount) const;
    pair<long,prec_t> minimal(vector<prec_t> const& valuefunction, prec_t discount) const;

    // average
    prec_t average(vector<prec_t> const& valuefunction, prec_t discount, vector<prec_t> const& distribution) const;
    prec_t average(vector<prec_t> const& valuefunction, prec_t discount) const{
        return average(valuefunction,discount,distribution);
    }

    // fixed-outcome
    prec_t fixed(vector<prec_t> const& valuefunction, prec_t discount, int index) const;

    // **** weighted constrained
    template<pair<vector<prec_t>,prec_t> (*Nature)(vector<prec_t> const& z, vector<prec_t> const& q, prec_t t)>
    pair<vector<prec_t>,prec_t> maximal_cst(vector<prec_t> const& valuefunction, prec_t discount, vector<prec_t> const& distribution, prec_t t) const{
        /**
         * \brief Computes the maximal outcome distribution constraints on the nature's distribution
         *
         * \param valuefunction Value function reference
         * \param discount Discount factor
         *
         * \return Outcome distribution and the mean value for the maximal bounded solution
         */
        if(distribution.size() != outcomes.size())
            throw range_error("Outcome distribution has incorrect size.");
        if(outcomes.size() == 0)
            throw range_error("Action with no outcomes not allowed when maximizing.");

        vector<prec_t> outcomevalues(outcomes.size());

        for(size_t i = 0; i < outcomes.size(); i++){
            const auto& outcome = outcomes[i];
            outcomevalues[i] = - outcome.compute_value(valuefunction, discount);
        }

        auto result = Nature(outcomevalues, distribution, t);
        result.second = - result.second;

        return result;
    };

    template<pair<vector<prec_t>,prec_t> (*Nature)(vector<prec_t> const& z, vector<prec_t> const& q, prec_t t)>
    pair<vector<prec_t>,prec_t> minimal_cst(vector<prec_t> const& valuefunction, prec_t discount, vector<prec_t> const& distribution, prec_t t) const{
        /** \brief Computes the minimal outcome distribution given l1 constraints on the distribution
         *
         * \param valuefunction Value function reference
         * \param discount Discount factor
         * \param distribution Reference distribution for the L1 bound
         * \param threshold Bound on the L1 deviation
         *
         * \return Outcome distribution and the mean value for the minimal bounded solution
         */

        if(distribution.size() != outcomes.size())
            throw range_error("Outcome distribution has incorrect size.");
        if(outcomes.size() == 0)
            return make_pair(vector<prec_t>(0), -numeric_limits<prec_t>::infinity());

        vector<prec_t> outcomevalues(outcomes.size());

        for(size_t i = 0; i < outcomes.size(); i++){
            const auto& outcome = outcomes[i];
            outcomevalues[i] = outcome.compute_value(valuefunction, discount);
        }

        return Nature(outcomevalues, distribution, t);
    }

    // **** weighted l1
    pair<vector<prec_t>,prec_t> maximal_l1(vector<prec_t> const& valuefunction, prec_t discount, vector<prec_t> const& distribution, prec_t t) const{
        return maximal_cst<worstcase_l1>(valuefunction, discount, distribution, t);
    };
    pair<vector<prec_t>,prec_t> maximal_l1(vector<prec_t> const& valuefunction, prec_t discount) const{
        return maximal_l1(valuefunction,discount,distribution,this->threshold);
    };

    pair<vector<prec_t>,prec_t> minimal_l1(vector<prec_t> const& valuefunction, prec_t discount, vector<prec_t> const& distribution, prec_t t) const{
        return minimal_cst<worstcase_l1>(valuefunction, discount, distribution, t);
    }
    pair<vector<prec_t>,prec_t> minimal_l1(vector<prec_t> const& valuefunction, prec_t discount) const{
        return minimal_l1(valuefunction,discount,distribution,this->threshold);
    };

    void add_outcome(long outcomeid, long toid, prec_t probability, prec_t reward);

    void set_distribution(vector<prec_t> const& distribution, prec_t threshold);

    void set_threshold(prec_t threshold){
        this->threshold = threshold;
    }
};

}
#endif // ACTION_H
