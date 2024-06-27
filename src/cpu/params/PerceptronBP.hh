#ifndef __PARAMS__PerceptronBP__
#define __PARAMS__PerceptronBP__

class PerceptronBP;

#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"

#include "params/BranchPredictor.hh"

struct PerceptronBPParams
    : public BranchPredictorParams
{
    PerceptronBP * create();
    int count_perc;
    int n;
    int theta;
};

#endif // __PARAMS__PerceptronBP__
//test