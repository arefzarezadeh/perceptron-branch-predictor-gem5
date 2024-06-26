/*
 * Copyright (c) 2022-2023 The University of Edinburgh
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu/pred/perceptron.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/Fetch.hh"

namespace gem5
{

namespace branch_prediction
{

PerceptronBP::PerceptronBP(const PerceptronBPParams &params)
    :   n(params.n),
        count_perc(params.count_perc),
        perceptronTable(perceptronCount, std::vector<int>(n + 1)),
        perceptronTable(perceptronCount, int)
{
    // n = params.n;
    // count_perc = params.count_perc;
    // for (int i = 0; i < count_perc; i++) {  
    //     std::vector<SatCounter8> v1;
    //     for (int j = 0; j <= n; j++) { 
    //         v1.push_back(0); 
    //     }
    //     weights.push_back(v1); 
    // }
    // for (int i = 0; i < n; i++) {
    //     history.push_back(0);
    // }
}

void PerceptronBP::updateHistories(ThreadID tid, Addr pc, bool uncond,
                         bool taken, Addr target, void * &bp_history)
{
    for (int i = n - 2; i >= 0; i--)
    {
        history[i + 1] = history[i];
    }
    history[0] = taken ? 1 : -1;
}


bool
PerceptronBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{
    return getPrediction(getLocalIndex(branch_addr));
}

void PerceptronBP::update(ThreadID tid, Addr branch_addr, bool taken, void *&bp_history,
                bool squashed, const StaticInstPtr & inst, Addr target)
{
    if (taken != getPrediction(getLocalIndex(branch_addr))) {
        int t = taken ? 1 : -1;
        for (int i = 1; i <= n; i++) {
            weights[current_perc][i] += t * history[i - 1];
        }
        weights[current_perc][i] += t;
    }
}

inline
bool
PerceptronBP::getPrediction(int current_perc)
{
    int sum = weights[current_perc][0];
    for (int i = 0; i < n; i++) {
        sum += weights[current_perc][i + 1] * history[i];
    }
    return sum >= 0;
}

inline
unsigned
PerceptronBP::getLocalIndex(Addr &branch_addr)
{
    return (branch_addr >> 2) & (count_perc - 1);
}


} // namespace branch_prediction
} // namespace gem5