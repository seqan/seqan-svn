// ==========================================================================
//                                   ANISE
// ==========================================================================
// Copyright (c) 2006-2014, Knut Reinert, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Manuel Holtgrewe <manuel.holtgrewe@fu-berlin.de>
// ==========================================================================

#ifndef SANDBOX_HERBARIUM_APPS_ANISE_TEST_UNITIGGER_H_
#define SANDBOX_HERBARIUM_APPS_ANISE_TEST_UNITIGGER_H_

#include <functional>
#include <memory>
#include <vector>

#include <seqan/sequence.h>

namespace assembler {

// ============================================================================
// Forwards
// ============================================================================

class Overlap;
class UnitiggerImpl;
struct ContigGraph;

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ----------------------------------------------------------------------------
// Class Unitigger
// ----------------------------------------------------------------------------

class Unitigger
{
public:
    enum Method { BEST_OVERLAP_GRAPH, MIRA_LIKE };

    Unitigger(Method method, bool logging);
    ~Unitigger();  // for pimpl

    std::unique_ptr<ContigGraph> run(seqan::StringSet<seqan::Dna5String> const & seqs,
                                     std::vector<Overlap> const & overlaps) const;

private:
    Unitigger::Method method;
    std::unique_ptr<UnitiggerImpl> impl;
};

// ============================================================================
// Metafunctions
// ============================================================================

// ============================================================================
// Functions
// ============================================================================

}  // namespace assembler

#endif  // #ifndef SANDBOX_HERBARIUM_APPS_ANISE_TEST_UNITIGGER_H_
