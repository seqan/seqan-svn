// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2017, Knut Reinert, FU Berlin
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
// Author: Kristin Knorr <kristin.knorr@fu-berlin.de>
// ==========================================================================
// Buchfink11 reduction tables
// ==========================================================================

#ifndef SEQAN_REDUCED_AMINOACID_BUCHFINK11_TABLES_H_
#define SEQAN_REDUCED_AMINOACID_BUCHFINK11_TABLES_H_

namespace seqan {

// ============================================================================
// Forwards
// ============================================================================

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ============================================================================
// Metafunctions
// ============================================================================

template <typename TSpec>
struct TranslateTableRedAAToChar_<Buchfink11, TSpec>
{
    static const char VALUE[ValueSize<SimpleType<unsigned char, ReducedAminoAcid_<Buchfink11> > >::VALUE];
};

template <typename TSpec>
struct TranslateTableCharToRedAA_<Buchfink11, TSpec>
{
    static const char VALUE[256];
};

template <typename TSpec>
struct TranslateTableAAToRedAA_<Buchfink11, TSpec>
{
    static const char VALUE[27];
};

template <typename TSpec>
struct TranslateTableByteToRedAA_<Buchfink11, TSpec>
{
    static const char VALUE[256];
};

// ---------------------------------- N = 11 ------------------------------

template <typename TVoidSpec>
char const TranslateTableRedAAToChar_<Buchfink11, TVoidSpec>::VALUE[11] =
{
    'A', // A S T X
    'B', // B D E K N O Q R Z
    'C', // C U
    'F', // F *
    'G', // G
    'H', // H
    'I', // I J L V
    'M', // M
    'P', // P
    'W', // W
    'Y'  // Y
};

template <typename TVoidSpec>
char const TranslateTableCharToRedAA_<Buchfink11, TVoidSpec>::VALUE[256] =
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  1,  2,  1,  1,  3,  4,  5,  6,  6,
     1,  6,  7,  1,  1,  8,  1,  1,  0,  0,  2,  6,  9,  0, 10,
     1,  0,  0,  0,  0,  0,  0,  0,  1,  2,  1,  1,  3,  4,  5,
     6,  6,  1,  6,  7,  1,  1,  8,  1,  1,  0,  0,  2,  6,  9,
     0, 10,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0
};

//lexicographically
template <typename TVoidSpec>
char const TranslateTableAAToRedAA_<Buchfink11, TVoidSpec>::VALUE[27] =
{
     0,  1,  2,  1,  1,  3,  4,  5,  6,  6,  1,  6,  7,
     1,  1,  8,  1,  1,  0,  0,  2,  6,  9,  10, 1,  0,  3
};

template <typename TVoidSpec>
char const TranslateTableByteToRedAA_<Buchfink11, TVoidSpec>::VALUE[256] =
{
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0
};

// ============================================================================
// Functions
// ============================================================================

} // namespace

#endif // SEQAN_REDUCED_AMINOACID_BUCHFINK11_TABLES_H_
