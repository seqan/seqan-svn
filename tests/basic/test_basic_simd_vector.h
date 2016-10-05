// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2015, Knut Reinert, FU Berlin
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
// Author: David Weese <david.weese@fu-berlin.de>
//         Marcel Ehrhardt <marcel.ehrhardt@fu-berlin.de>
// ==========================================================================
// Tests for SIMD vectors.
// ==========================================================================

#ifndef SEQAN_CORE_TESTS_BASIC_TEST_BASIC_SIMD_VECTOR_H_
#define SEQAN_CORE_TESTS_BASIC_TEST_BASIC_SIMD_VECTOR_H_

#include <seqan/simd.h>
#include <random>
#include <seqan/sequence.h>
#include <seqan/misc/bit_twiddling.h>

#if defined(SEQAN_SIMD_ENABLED)
namespace seqan {

template <int ROWS, typename TVector>
inline void test_matrix_transpose()
{
    typedef typename Value<TVector>::Type TValue;
    typedef TVector TMatrix[LENGTH<TVector>::VALUE];
    const int COLS = LENGTH<TVector>::VALUE;

    String<TValue> random;
    resize(random, ROWS * COLS);

    std::mt19937 rng;
    std::uniform_int_distribution<TValue> pdf(0, MaxValue<TValue>::VALUE);
    for (unsigned i = 0; i < length(random); ++i)
        random[i] = pdf(rng);

    TMatrix tmp;
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            tmp[i][j] = random[i * COLS + j];

//    for(int i=0;i<ROWS;++i)
//        print(std::cout, tmp[i]) << std::endl;

    transpose<ROWS>(tmp);

//    std::cout << std::endl;
//    std::cout << std::endl;
//    for(int i=0;i<DIM;++i)
//        print(std::cout, tmp[i]) << std::endl;

    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            SEQAN_ASSERT_EQ(tmp[i][j], random[j * ROWS + i]);
}

template <typename TSimdVector>
void fillVectors(TSimdVector & a, TSimdVector & b)
{
    using namespace seqan;
    constexpr auto length = LENGTH<TSimdVector>::VALUE;

    for (auto i = 0; i < length; ++i)
    {
        a[i] = i * 3;
        b[i] = length - i;
    }
}

} // namespace seqan

// ----------------------------------------------------------------------------
// Configuration of typed tests for simd vectors.
// ----------------------------------------------------------------------------

template <typename TSimdVector_>
class SimdVectorTestCommon : public seqan::Test
{
public:
    using TValue = typename seqan::Value<TSimdVector_>::Type;
    constexpr static auto const LENGTH = seqan::LENGTH<TSimdVector_>::VALUE;
    using TSimdVector = TSimdVector_;
};

typedef
        seqan::TagList<seqan::SimdVector<int8_t, 16>::Type,
        seqan::TagList<seqan::SimdVector<int16_t, 8>::Type,
        seqan::TagList<seqan::SimdVector<int32_t, 4>::Type,
        // seqan::TagList<seqan::SimdVector<int64_t, 2>::Type,
        seqan::TagList<seqan::SimdVector<uint8_t, 16>::Type,
        seqan::TagList<seqan::SimdVector<uint16_t, 8>::Type,
        seqan::TagList<seqan::SimdVector<uint32_t, 4>::Type
        // seqan::TagList<seqan::SimdVector<uint64_t, 2>::Type
        > > > > > >
        // > >
        SimdVectorCommonCommonTypes;

SEQAN_TYPED_TEST_CASE(SimdVectorTestCommon, SimdVectorCommonCommonTypes);

#ifdef __SSE4_1__

SEQAN_TYPED_TEST(SimdVectorTestCommon, ClearVector)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    auto zero = static_cast<TValue>(0);
    TSimdVector a, b;
    fillVectors(a, b);

    clearVector(a);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)a[i] << " = " << 0 << std::endl;
        SEQAN_ASSERT_EQ(a[i], zero);
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, CreateVector)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    auto scalar = static_cast<TValue>(23);
    auto a = createVector<TSimdVector>(scalar);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)a[i] << " = " << 23 << std::endl;
        SEQAN_ASSERT_EQ(a[i], scalar);
    }
}

template <typename TSimdVector, int L>
inline void
test_fill_vector(TSimdVector & a, seqan::SimdParams_<2, L>)
{
    using namespace seqan;
    fillVector(a, 0, 1);
}

template <typename TSimdVector, int L>
inline void
test_fill_vector(TSimdVector & a, seqan::SimdParams_<4, L>)
{
    using namespace seqan;
    fillVector(a, 0, 1, 2, 3);
}

template <typename TSimdVector, int L>
inline void
test_fill_vector(TSimdVector & a, seqan::SimdParams_<8, L>)
{
    using namespace seqan;
    fillVector(a, 0, 1, 2, 3, 4, 5, 6, 7);
}

template <typename TSimdVector, int L>
inline void
test_fill_vector(TSimdVector & a, seqan::SimdParams_<16, L>)
{
    using namespace seqan;
    fillVector(a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
}

template <typename TSimdVector, int L>
inline void
test_fill_vector(TSimdVector & a, seqan::SimdParams_<32, L>)
{
    using namespace seqan;
    fillVector(a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
               16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, FillVector)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a;
    test_fill_vector(a, SimdParams_<length, 0>());

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)a[i] << " = " << i << std::endl;
        SEQAN_ASSERT_EQ(a[i], static_cast<TValue>(i));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, CmpEqual)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    constexpr auto length = TestFixture::LENGTH;

    int zeros = 0, ones = ~0;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a == b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " == " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] == b[i] ? ones : zeros);
        SEQAN_ASSERT_EQ(c[i], (i * 3) == (length - i) ? ones : zeros);
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, CmpGt)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    constexpr auto length = TestFixture::LENGTH;

    int zeros = 0, ones = ~0;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a > b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " > " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] > b[i] ? ones : zeros);
        SEQAN_ASSERT_EQ(c[i], (i * 3) > (length - i) ? ones : zeros);
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Max)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = max(a, b);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = max (" << (int)a[i] << ", " << (int)b[i] << ")" << std::endl;
        SEQAN_ASSERT_EQ(c[i], std::max(a[i], b[i]));
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(std::max(i * 3, length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseOr)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a | b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " | " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] | b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) | (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseOrAssign)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b, c;
    fillVectors(a, b);

    c = a;
    c |= b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " | " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] | b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) | (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseAnd)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a & b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " & " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] & b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) & (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseAndAssign)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b, c;
    fillVectors(a, b);

    c = a;
    c &= b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " & " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] & b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) & (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseNot)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = ~a;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = ~" << (int)a[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(~a[i]));
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(~(i * 3)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Addition)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a + b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " + " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] + b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(length + 2*i));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Subtraction)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a - b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " - " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(a[i] - b[i]));
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(i * 3 - (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Multiplication)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a * b;

    for (size_t i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " * " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(a[i] * b[i]));
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(i * 3 * (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Division)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = a / b;

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " * " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] / b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(i * 3 / (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, BitwiseAndNot)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = andNot(a, b);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = (~" << (int)a[i] << ") & " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(~a[i] & b[i]));
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(~(i * 3) & (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, ShiftRightLogical)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = shiftRightLogical(a, 2);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " >> " << (int)2 << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] >> 2);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) >> 2));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Blend)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    auto c = blend(b, a, a > b);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << " > " << (int)b[i] << " ? " << (int)a[i] << " : " << (int)b[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i] > b[i] ? a[i] : b[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>((i * 3) > (length - i) ? (i * 3) : (length - i)));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Storeu)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, b;
    fillVectors(a, b);

    TValue c[length];
    storeu(c, a);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(i * 3));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Load)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, c;
    fillVectors(a, c);

    alignas(sizeof(TSimdVector)) TValue b[length];
    storeu(b, a);
    c = load<TSimdVector>(b);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[i] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[i]);
        SEQAN_ASSERT_EQ(c[i], static_cast<TValue>(i * 3));
    }
}

SEQAN_TYPED_TEST(SimdVectorTestCommon, Gather)
{
    using namespace seqan;
    using TSimdVector = typename TestFixture::TSimdVector;
    using TValue = typename TestFixture::TValue;
    constexpr auto length = TestFixture::LENGTH;

    TSimdVector a, idx;
    fillVectors(a, idx);

    for (auto i = 0u; i < length; ++i)
        idx[i] = length - i - 1;

    TValue b[length];
    storeu(b, a);
    auto c = gather(b, idx);

    for (auto i = 0; i < length; ++i)
    {
        // std::cout << i << " / " << length << ": " << (int)c[i] << " = " << (int)a[idx[i]] << std::endl;
        SEQAN_ASSERT_EQ(c[i], a[idx[i]]);
        SEQAN_ASSERT_EQ(c[i], a[length - i - 1]);
    }
}


SEQAN_DEFINE_TEST(test_basic_simd_transpose_8x8)
{
    seqan::test_matrix_transpose<8, seqan::SimdVector<unsigned char, 8>::Type>();
}

SEQAN_DEFINE_TEST(test_basic_simd_transpose_16x16)
{
    seqan::test_matrix_transpose<16, seqan::SimdVector<unsigned char, 16>::Type>();
}

#endif  // #ifdef __SSE4_1__
#ifdef __AVX2__

SEQAN_DEFINE_TEST(test_basic_simd_shuffle_avx)
{
    seqan::SimdVector<unsigned short, 16>::Type vec;
    seqan::SimdVector<unsigned char, 16>::Type  indices;

    const int perm[] = {1,4,2,6,3,5,0,7};

    for (int i = 0; i < 8; ++i)
    {
        vec[i]   = i * 259 + 3;
        vec[i+8] = i * 432 + 9;
    }

    for (int i = 0; i < 8; ++i)
    {
        indices[i]   = 7 - i;
        indices[i+8] = perm[i];
    }

    vec = seqan::shuffleVector(vec, indices);

    for (int i = 0; i < 8; ++i)
    {
        SEQAN_ASSERT_EQ(vec[i],   (7 - i) * 259 + 3);
        SEQAN_ASSERT_EQ(vec[i+8], perm[i] * 432 + 9);
    }
}

SEQAN_DEFINE_TEST(test_basic_simd_transpose_32x32)
{
    seqan::test_matrix_transpose<32, seqan::SimdVector<unsigned char, 32>::Type >();
}

#endif  // #ifdef __AVX2__
#endif  // SEQAN_SIMD_ENABLED

#endif  // #ifndef SEQAN_CORE_TESTS_BASIC_TEST_BASIC_SIMD_VECTOR_H_
