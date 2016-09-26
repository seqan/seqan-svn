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
// SIMD implementation of umesimd
// ==========================================================================

#ifndef SEQAN_INCLUDE_SEQAN_SIMD_SIMD_BASE_UMESIMD_IMPL_H_
#define SEQAN_INCLUDE_SEQAN_SIMD_SIMD_BASE_UMESIMD_IMPL_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#include "umesimd/UMESimd.h"
#pragma GCC diagnostic pop

namespace seqan
{

template <typename TSimdVector>
struct SimdIndexVectorImpl<TSimdVector, True>
{
    using Type = typename UME::SIMD::SIMDTraits<TSimdVector>::SWIZZLE_T;
};

template <typename TValue, int LENGTH>
struct SimdVector
{
    typedef UME::SIMD::SIMDVec<TValue, LENGTH> Type;
};

// ============================================================================
// SIMDSwizzle
// ============================================================================

template <uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDSwizzle<LENGTH>),       (SimdVectorConcept));

template <uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDSwizzle<LENGTH> const), (SimdVectorConcept));

template <uint32_t LENGTH>
struct Value<UME::SIMD::SIMDSwizzle<LENGTH> >
{
    typedef uint32_t Type;
};

template <uint32_t LENGTH_>
struct LENGTH<UME::SIMD::SIMDSwizzle<LENGTH_> > {
    enum { VALUE = LENGTH_ };
};

template <uint32_t LENGTH, typename TPosition>
inline typename Value<UME::SIMD::SIMDSwizzle<LENGTH> >::Type
getValue(UME::SIMD::SIMDSwizzle<LENGTH> const &vector, TPosition const pos)
{
    return vector[pos];
}

template <uint32_t LENGTH, typename TPosition>
inline typename Value<UME::SIMD::SIMDSwizzle<LENGTH> >::Type
value(UME::SIMD::SIMDSwizzle<LENGTH> const &vector, TPosition const pos)
{

    return vector[pos];
}

template <uint32_t LENGTH, typename TPosition, typename TValue2>
inline void
assignValue(UME::SIMD::SIMDSwizzle<LENGTH> &vector, TPosition const pos, TValue2 const value)
{
    vector.insert(pos, value);
}

// ============================================================================
// SIMDVec_u
// ============================================================================

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_u<TValue, LENGTH>),       (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_u<TValue, LENGTH> const), (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
struct Value<UME::SIMD::SIMDVec_u<TValue, LENGTH> >
{
    typedef TValue Type;
};

template <typename TValue, uint32_t LENGTH_>
struct LENGTH<UME::SIMD::SIMDVec_u<TValue, LENGTH_> > {
    enum { VALUE = LENGTH_ };
};

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
getValue(UME::SIMD::SIMDVec_u<TValue, LENGTH> const &vector, TPosition const pos)
{
    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
value(UME::SIMD::SIMDVec_u<TValue, LENGTH> const &vector, TPosition const pos)
{

    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition, typename TValue2>
inline void
assignValue(UME::SIMD::SIMDVec_u<TValue, LENGTH> &vector, TPosition const pos, TValue2 const value)
{
    vector[pos] = value;
}

// ============================================================================
// SIMDVec_i
// ============================================================================

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_i<TValue, LENGTH>),       (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_i<TValue, LENGTH> const), (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
struct Value<UME::SIMD::SIMDVec_i<TValue, LENGTH> >
{
    typedef TValue Type;
};

template <typename TValue, uint32_t LENGTH_>
struct LENGTH<UME::SIMD::SIMDVec_i<TValue, LENGTH_> > {
    enum { VALUE = LENGTH_ };
};

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
getValue(UME::SIMD::SIMDVec_i<TValue, LENGTH> const &vector, TPosition const pos)
{
    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
value(UME::SIMD::SIMDVec_i<TValue, LENGTH> const &vector, TPosition const pos)
{

    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition, typename TValue2>
inline void
assignValue(UME::SIMD::SIMDVec_i<TValue, LENGTH> &vector, TPosition const pos, TValue2 const value)
{
    vector[pos] = value;
}

// ============================================================================
// SIMDVec_f
// ============================================================================

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_f<TValue, LENGTH>),       (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
SEQAN_CONCEPT_IMPL((typename UME::SIMD::SIMDVec_f<TValue, LENGTH> const), (SimdVectorConcept));

template <typename TValue, uint32_t LENGTH>
struct Value<UME::SIMD::SIMDVec_f<TValue, LENGTH> >
{
    typedef TValue Type;
};

template <typename TValue, uint32_t LENGTH_>
struct LENGTH<UME::SIMD::SIMDVec_f<TValue, LENGTH_> > {
    enum { VALUE = LENGTH_ };
};

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
getValue(UME::SIMD::SIMDVec_f<TValue, LENGTH> const &vector, TPosition const pos)
{
    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition>
inline TValue
value(UME::SIMD::SIMDVec_f<TValue, LENGTH> const &vector, TPosition const pos)
{

    return vector[pos];
}

template <typename TValue, uint32_t LENGTH, typename TPosition, typename TValue2>
inline void
assignValue(UME::SIMD::SIMDVec_f<TValue, LENGTH> &vector, TPosition const pos, TValue2 const value)
{
    vector[pos] = value;
}

} // namespace seqan

namespace UME
{
namespace SIMD
{
    template <typename TStream,
              typename TVector, typename TScalar>
    inline TStream &
    operator<<(TStream & stream,
               IntermediateIndex<TVector, TScalar> const & pInterIndex)
    {
        stream << static_cast<TScalar>(pInterIndex);
        return stream;
    }
}
}

namespace seqan
{

// --------------------------------------------------------------------------
// Function clearVector()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, void)
clearVector(TSimdVector &vector)
{
    vector = 0;
}


// --------------------------------------------------------------------------
// Function createVector()
// --------------------------------------------------------------------------

template <typename TSimdVector, typename TValue>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
createVector(TValue x)
{
    return TSimdVector(x);
}

// --------------------------------------------------------------------------
// Function fillVector()
// --------------------------------------------------------------------------

template <typename TSimdVector, typename ...TValue>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, void)
fillVector(TSimdVector &vector, TValue const... args)
{
    vector = TSimdVector(args...);
}

// --------------------------------------------------------------------------
// Function cmpEq()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
cmpEq (TSimdVector const &a, TSimdVector const &b)
{
    using TValue = typename UME::SIMD::SIMDTraits<TSimdVector>::SCALAR_T;
    return UME::SIMD::SCALAR_EMULATION::xtoy<TSimdVector, TValue>(a.cmpeq(b));
}

// --------------------------------------------------------------------------
// Function operator==()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator == (TSimdVector const &a, TSimdVector const &b)
{
    using TValue = typename UME::SIMD::SIMDTraits<TSimdVector>::SCALAR_T;
    return UME::SIMD::SCALAR_EMULATION::xtoy<TSimdVector, TValue>(a.cmpeq(b));
}

// --------------------------------------------------------------------------
// Function operatorGt()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
cmpGt (TSimdVector const &a, TSimdVector const &b)
{
    using TValue = typename UME::SIMD::SIMDTraits<TSimdVector>::SCALAR_T;
    return UME::SIMD::SCALAR_EMULATION::xtoy<TSimdVector, TValue>(a.cmpgt(b));
}

// --------------------------------------------------------------------------
// Function operator>()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator > (TSimdVector const &a, TSimdVector const &b)
{
    using TValue = typename UME::SIMD::SIMDTraits<TSimdVector>::SCALAR_T;
    return UME::SIMD::SCALAR_EMULATION::xtoy<TSimdVector, TValue>(a.cmpgt(b));
}

// --------------------------------------------------------------------------
// Function max()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
max(TSimdVector const &a, TSimdVector const &b)
{
    return a.max(b);
}

// --------------------------------------------------------------------------
// Function operator|()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator | (TSimdVector const &a, TSimdVector const &b)
{
    return a.bor(b);
}

// --------------------------------------------------------------------------
// Function operator|=()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector &)
operator |= (TSimdVector &a, TSimdVector const &b)
{
    return a.bora(b);
}

// --------------------------------------------------------------------------
// Function operator&()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator & (TSimdVector const &a, TSimdVector const &b)
{
    return a.band(b);
}

// --------------------------------------------------------------------------
// Function operator&=()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector &)
operator &= (TSimdVector &a, TSimdVector const &b)
{
    return a.banda(b);
}

// --------------------------------------------------------------------------
// Function operator~()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator ~ (TSimdVector const &a)
{
    return a.bnot();
}

// --------------------------------------------------------------------------
// Function operator+()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator + (TSimdVector const &a, TSimdVector const &b)
{
    return a.add(b);
}

// --------------------------------------------------------------------------
// Function operator-()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator - (TSimdVector const &a, TSimdVector const &b)
{
    return a.sub(b);
}

// --------------------------------------------------------------------------
// Function operator*()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator * (TSimdVector const &a, TSimdVector const &b)
{
    return a.mul(b);
}

// --------------------------------------------------------------------------
// Function operator/()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
operator/ (TSimdVector const &a, TSimdVector const &b)
{
    return a.div(b);
}

// --------------------------------------------------------------------------
// Function andNot
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
andNot(TSimdVector const &a, TSimdVector const &b)
{
    return a.bandnot(b);
}


// --------------------------------------------------------------------------
// Function shiftRightLogical()
// --------------------------------------------------------------------------

template <typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
shiftRightLogical(TSimdVector const &vector, const int imm)
{
    return vector.rsh(imm);
}

// --------------------------------------------------------------------------
// Function blend()
// --------------------------------------------------------------------------

template <typename TSimdVector, typename TSimdVectorMask>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
blend(TSimdVector const &a, TSimdVector const &b, TSimdVectorMask const & mask)
{
    using TMaskType = typename UME::SIMD::SIMDTraits<TSimdVector>::MASK_T;

    return a.blend(
        UME::SIMD::SCALAR_EMULATION::xtoy<TMaskType, bool>(mask),
        b
    );
}

// --------------------------------------------------------------------------
// Function storeu()
// --------------------------------------------------------------------------

template <typename T, typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, void)
storeu(T * memAddr, TSimdVector const &vec)
{
    vec.store(memAddr);
}

// --------------------------------------------------------------------------
// Function load()
// --------------------------------------------------------------------------

template <typename TSimdVector, typename T>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
load(T const * memAddr)
{
    return TSimdVector(memAddr);
}

// --------------------------------------------------------------------------
// Function gather()
// --------------------------------------------------------------------------

template <typename TValue, typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(IsSameType<TValue, typename Value<TSimdVector>::Type>, TSimdVector)
_gather(TValue const * memAddr, TSimdVector const & idx)
{
    using TIndexVector = typename UME::SIMD::SIMDTraits<TSimdVector>::UINT_VEC_T;

    TSimdVector a;
    a.gather(memAddr, static_cast<TIndexVector>(idx));
    return a;
}

template <typename TValue, typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Not<IsSameType<TValue, typename Value<TSimdVector>::Type> >, TSimdVector)
_gather(TValue const * memAddr, TSimdVector const & idx)
{
    using TIndexVector = typename UME::SIMD::SIMDTraits<TSimdVector>::UINT_VEC_T;

    TSimdVector a;
    for (auto i = 0u; i < TIndexVector::length(); ++i)
    {
        a[i] = memAddr[idx[i]];
    }
    return a;
}

template <typename TValue, typename TSimdVector>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector> >, TSimdVector)
gather(TValue const * memAddr, TSimdVector const & idx)
{
    return _gather(memAddr, idx);
}

// --------------------------------------------------------------------------
// Function shuffleVector()
// --------------------------------------------------------------------------

template <typename TSimdVector1, typename TSimdVector2>
inline SEQAN_FUNC_ENABLE_IF(Is<SimdVectorConcept<TSimdVector1> >, TSimdVector1)
shuffleVector(TSimdVector1 const &vector, TSimdVector2 const &indices)
{
    return vector.swizzle(indices);
}

}

#endif // SEQAN_INCLUDE_SEQAN_SIMD_SIMD_BASE_UMESIMD_IMPL_H_