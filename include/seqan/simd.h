// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2016, Knut Reinert, FU Berlin
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
// Author: Marcel Ehrhardt <marcel.ehrhardt@fu-berlin.de>
// ==========================================================================
// generic SIMD interface for SSE3 / AVX2
// ==========================================================================

#ifndef SEQAN_INCLUDE_SEQAN_SIMD_H_
#define SEQAN_INCLUDE_SEQAN_SIMD_H_

// Define global macro to check if simd instructions are enabled.
#define SEQAN_SIMD_ENABLED 1

#define SEQAN_SEQANSIMD_ENABLED 1
//#define SEQAN_UMESIMD_ENABLED 1

#include "simd/simd_base.h"
#include "simd/simd_base_seqan_impl.h"

#if SEQAN_SEQANSIMD_ENABLED
    #if defined(SEQAN_SSE4)
    #include "simd/simd_base_seqan_impl_sse4.2.h"
    #endif // defined(SEQAN_SSE4)

    #if defined(__AVX2__)
    #include "simd/simd_base_seqan_impl_avx2.h"
    #endif // defined(__AVX2__)

    #include "simd/simd_base_seqan_interface.h"
#endif // SEQAN_SIMD_ENABLED

#if SEQAN_UMESIMD_ENABLED
    #include "simd/simd_base_umesimd_impl.h"
#endif

#endif // SEQAN_INCLUDE_SEQAN_SIMD_H_
