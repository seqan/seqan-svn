// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2013 NVIDIA Corporation
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
//     * Neither the name of NVIDIA Corporation nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Enrico Siragusa <enrico.siragusa@fu-berlin.de>
// ==========================================================================

#ifndef INDEX_FM_RANK_DICTIONARY_LEVELS_H_
#define INDEX_FM_RANK_DICTIONARY_LEVELS_H_

namespace seqan {

// ============================================================================
// Forwards
// ============================================================================

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBitMask_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec = void>
struct RankDictionaryBitMask_;

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryWordSize_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryWordSize_;

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBitsPerBlock_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryBitsPerBlock_;

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBlock_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryBlock_;

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryValues_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryValues_;

// ----------------------------------------------------------------------------
// Struct RankDictionaryEntry_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryEntry_;

// ============================================================================
// Tags
// ============================================================================

// ----------------------------------------------------------------------------
// Tag TwoLevels
// ----------------------------------------------------------------------------

template <typename TSpec = void>
struct TwoLevels {};

// ============================================================================
// Metafunctions
// ============================================================================

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBitMask_
// ----------------------------------------------------------------------------

template <typename TSpec>
struct RankDictionaryBitMask_<__uint32, TSpec>
{
    static const __uint32 VALUE = 0x55555555;
};

template <typename TSpec>
struct RankDictionaryBitMask_<__uint64, TSpec>
{
    static const __uint64 VALUE = 0x5555555555555555;
};

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryWordSize_
// ----------------------------------------------------------------------------

//#ifdef CUDA_DISABLED
//template <typename TValue, typename TSpec>
//struct RankDictionaryWordSize_<TValue, TwoLevels<TSpec> > :
//    BitsPerValue<unsigned long> {};
//#else
template <typename TValue, typename TSpec>
struct RankDictionaryWordSize_<TValue, TwoLevels<TSpec> > :
    BitsPerValue<__uint32> {};
//#endif

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBitsPerBlock_
// ----------------------------------------------------------------------------
// The number of bits per block equals the number of bits of the block summary.

template <typename TValue, typename TSpec>
struct RankDictionaryBitsPerBlock_<TValue, TwoLevels<TSpec> > :
    BitsPerValue<typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type> {};

//#ifdef CUDA_DISABLED
//template <typename TSpec>
//struct RankDictionaryBitsPerBlock_<Dna, TwoLevels<TSpec> > :
//    RankDictionaryWordSize_<Dna, TwoLevels<TSpec> > {};
//#endif

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryBlock_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryBlock_<TValue, TwoLevels<TSpec> >
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >               TRankDictionary_;
    typedef typename Size<TRankDictionary_>::Type                   TSize_;

    typedef Tuple<TSize_, ValueSize<TValue>::VALUE>                 Type;
};

template <typename TSpec>
struct RankDictionaryBlock_<bool, TwoLevels<TSpec> >
{
    typedef RankDictionary<bool, TwoLevels<TSpec> >                 TRankDictionary_;

    typedef typename Size<TRankDictionary_>::Type                   Type;
};

// ----------------------------------------------------------------------------
// Metafunction RankDictionaryValues_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryValues_<TValue, TwoLevels<TSpec> >
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                       TRankDictionary_;
    
    typedef Tuple<TValue, TRankDictionary_::_VALUES_PER_WORD, BitPacked<> > TValues;
    typedef typename TValues::TBitVector                                    TWord;
    typedef Tuple<TValues, TRankDictionary_::_WORDS_PER_BLOCK>              Type;
};

// ----------------------------------------------------------------------------
// Metafunction Fibre
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct Fibre<RankDictionary<TValue, TwoLevels<TSpec> >, FibreRanks>
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                   TRankDictionary_;
    typedef RankDictionaryEntry_<TValue, TwoLevels<TSpec> >             TRankDictionaryEntry_;
    typedef typename RankDictionaryFibreSpec<TRankDictionary_>::Type    TRankDictionaryFibreSpec_;

    typedef String<TRankDictionaryEntry_, TRankDictionaryFibreSpec_>    Type;
};

// ============================================================================
// Classes
// ============================================================================

// ----------------------------------------------------------------------------
// Struct RankDictionaryEntry_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec = TwoLevels<> >
struct RankDictionaryEntry_ {};

// ----------------------------------------------------------------------------
// Struct TwoLevels RankDictionaryEntry_
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionaryEntry_<TValue, TwoLevels<TSpec> >
{
    // A bit-compressed block of TValue symbols.
    typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::Type   values;

    // A summary of counts for each block of TValue symbols.
    typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type    block;
};

// ----------------------------------------------------------------------------
// Class TwoLevels RankDictionary
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
struct RankDictionary<TValue, TwoLevels<TSpec> >
{
    // ------------------------------------------------------------------------
    // Constants
    // ------------------------------------------------------------------------

    static const unsigned _BITS_PER_VALUE   = BitsPerValue<TValue>::VALUE;
    static const unsigned _BITS_PER_BLOCK   = RankDictionaryBitsPerBlock_<TValue, TwoLevels<TSpec> >::VALUE;
    static const unsigned _BITS_PER_WORD    = Min<RankDictionaryWordSize_<TValue, TwoLevels<TSpec> >::VALUE, _BITS_PER_BLOCK>::VALUE;
    static const unsigned _VALUES_PER_WORD  = _BITS_PER_WORD  / _BITS_PER_VALUE;
    static const unsigned _VALUES_PER_BLOCK = _BITS_PER_BLOCK / _BITS_PER_VALUE;
    static const unsigned _WORDS_PER_BLOCK  = _BITS_PER_BLOCK / _BITS_PER_WORD;

    // ------------------------------------------------------------------------
    // Fibres
    // ------------------------------------------------------------------------

    typename Fibre<RankDictionary, FibreRanks>::Type    ranks;
    typename Size<RankDictionary>::Type                 _length;
    // TODO(esiragusa): open/save _length or remove it.

    // ------------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------------

    RankDictionary() :
        _length(0)
    {}

    template <typename TText>
    RankDictionary(TText const & text) :
        _length(0)
    {
        createRankDictionary(*this, text);
    }
};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function loadAndCache()
// ----------------------------------------------------------------------------
// TODO(esiragusa): move loadAndCache() in misc_cuda.h

template <typename TValue>
SEQAN_HOST_DEVICE inline TValue
loadAndCache(TValue const & value)
{
#if __CUDA_ARCH__ >= 350
    return __ldg(&value);
#else
    return value;
#endif
}

// ----------------------------------------------------------------------------
// Function loadAndCache()                                              [Tuple]
// ----------------------------------------------------------------------------
// TODO(esiragusa): move loadAndCache() in misc_cuda.h

template <typename TValue, unsigned SIZE, typename TSpec>
SEQAN_HOST_DEVICE inline Tuple<TValue, SIZE, TSpec>
loadAndCache(Tuple<TValue, SIZE, TSpec> const & tuple)
{
#if __CUDA_ARCH__ >= 350
    typedef Tuple<TValue, SIZE, TSpec>  TTuple;

    const unsigned UINTS = BytesPerValue<TTuple>::VALUE / 4;

    union { TTuple x; uint4 y[UINTS]; } tmp;

    for (unsigned u = 0; u < UINTS; ++u)
        tmp.y[u] = __ldg(reinterpret_cast<uint4 const *>(&tuple) + u);

    return tmp.x;
#else
    return tuple;
#endif
}

// ----------------------------------------------------------------------------
// Function loadAndCache()                               [RankDictionaryEntry_]
// ----------------------------------------------------------------------------
//
//template <typename TValue, typename TSpec>
//SEQAN_HOST_DEVICE inline RankDictionaryEntry_<TValue, TwoLevels<TSpec> >
//loadAndCache(RankDictionaryEntry_<TValue, TwoLevels<TSpec> > const & entry)
//{
//#if __CUDA_ARCH__ >= 350
//    typedef RankDictionaryEntry_<TValue, TwoLevels<TSpec> >   TEntry;
//
//    const unsigned UINTS = BytesPerValue<TEntry>::VALUE / 4;
//
//    union { TEntry x; uint4 y[UINTS]; } tmp;
//
//    for (unsigned u = 0; u < UINTS; ++u)
//        tmp.y[u] = __ldg(reinterpret_cast<uint4 const *>(&entry) + u);
//
//    return tmp.x;
//#else
//    return entry;
//#endif
//}

//template <unsigned SIZE, typename TSpec>
//SEQAN_HOST_DEVICE inline Tuple<Tuple<Dna, SIZE, BitPacked<> >, 4, TSpec>
//loadAndCache(Tuple<Tuple<Dna, SIZE, BitPacked<> >, 4, TSpec> const & values)
//{
//#if __CUDA_ARCH__ >= 350
//    Tuple<Tuple<Dna, SIZE, BitPacked<> >, 4, TSpec> tmp;
//
//    uint4 t = __ldg((uint4 *)values.i);
//    tmp.i[0].i = t.x;
//    tmp.i[1].i = t.y;
//    tmp.i[2].i = t.z;
//    tmp.i[3].i = t.w;
//
//    return tmp;
//#else
//    return values;
//#endif
//}

//template <unsigned SIZE, typename TSpec>
//SEQAN_HOST_DEVICE inline Tuple<Tuple<bool, SIZE, BitPacked<> >, 1, TSpec>
//loadAndCache(Tuple<Tuple<bool, SIZE, BitPacked<> >, 1, TSpec> const & values)
//{
//#if __CUDA_ARCH__ >= 350
//    Tuple<Tuple<bool, SIZE, BitPacked<> >, 1, TSpec> tmp;
//
//    uint2 t = __ldg((uint2 *)values.i);
//    tmp.i[0].i = t.x;
//    tmp.i[1].i = t.y;
//
//    return tmp;
//#else
//    return values;
//#endif
//}

// ----------------------------------------------------------------------------
// Function _toPosInWord()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
_toPosInWord(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TPos posInBlock)
{
    return posInBlock % RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_WORD;
}

// ----------------------------------------------------------------------------
// Function _toWordPos()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
_toWordPos(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TPos posInBlock)
{
    return posInBlock / RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_WORD;
}

// ----------------------------------------------------------------------------
// Function _toPosInBlock()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
_toPosInBlock(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TPos pos)
{
    return pos % RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_BLOCK;
}

// ----------------------------------------------------------------------------
// Function _toBlockPos()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
_toBlockPos(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TPos pos)
{
    return pos / RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_BLOCK;
}

// ----------------------------------------------------------------------------
// Function _toPos()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TBlockPos>
inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
_toPos(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TBlockPos blockPos)
{
    return blockPos * RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_BLOCK;
}

// ----------------------------------------------------------------------------
// Function _valuesAt()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TBlockPos, typename TWordPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::TValues &
_valuesAt(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TBlockPos blockPos, TWordPos wordPos)
{
    return dict.ranks[blockPos].values[wordPos];
}

template <typename TValue, typename TSpec, typename TBlockPos, typename TWordPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::TValues const &
_valuesAt(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TBlockPos blockPos, TWordPos wordPos)
{
    return dict.ranks[blockPos].values[wordPos];
}

// ----------------------------------------------------------------------------
// Function _valuesAt()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::Type &
_valuesAt(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TPos pos)
{
    return dict.ranks[_toBlockPos(dict, pos)].values;
}

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::Type const &
_valuesAt(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TPos pos)
{
    return dict.ranks[_toBlockPos(dict, pos)].values;
}

// ----------------------------------------------------------------------------
// Function _blockAt()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type &
_blockAt(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TPos pos)
{
    return dict.ranks[_toBlockPos(dict, pos)].block;
}

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type const &
_blockAt(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TPos pos)
{
    return dict.ranks[_toBlockPos(dict, pos)].block;
}

// ----------------------------------------------------------------------------
// Function _padValues()
// ----------------------------------------------------------------------------
// Set values beyond length(dict) but still within the end of the ranks fibre.

template <typename TValue, typename TSpec>
inline void _padValues(RankDictionary<TValue, TwoLevels<TSpec> > & dict)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >               TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                    TSize;

    TSize beginPos = length(dict);
    TSize endPos   = length(dict.ranks) * TRankDictionary::_VALUES_PER_BLOCK;

    for (TSize pos = beginPos; pos < endPos; ++pos)
        setValue(dict, pos, TValue());
}

// ----------------------------------------------------------------------------
// Function _clearBlockAt()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
inline void _clearBlockAt(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TPos pos)
{
    clear(_blockAt(dict, pos));
}

// ----------------------------------------------------------------------------
// Function _clearBlockAt(bool)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TPos>
inline void _clearBlockAt(RankDictionary<bool, TwoLevels<TSpec> > & dict, TPos pos)
{
    _blockAt(dict, pos) = 0u;
}

// ----------------------------------------------------------------------------
// Function _getBlockRank()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TBlock, typename TPos, typename TChar>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
_getBlockRank(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */, TBlock const & block, TPos /* pos */, TChar c)
{
    return loadAndCache(block[ordValue(c)]);
//    return block[ordValue(c)];
}

// ----------------------------------------------------------------------------
// Function _getBlockRank(bool)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TBlock, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<bool, TwoLevels<TSpec> > const>::Type
_getBlockRank(RankDictionary<bool, TwoLevels<TSpec> > const & dict, TBlock const & block, TPos pos, bool c)
{
    TBlock rank = loadAndCache(block);

    // If c == false then return the complementary rank.
    return c ? rank : pos - _toPosInBlock(dict, pos) - rank;

//    return c ? block : pos - _toPosInBlock(dict, pos) - block;
}

// ----------------------------------------------------------------------------
// Function _getWordRank()
// ----------------------------------------------------------------------------
// NOTE(esiragusa): This version is generic but absymally slow - only for testing purposes.

template <typename TValue, typename TSpec, typename TWord, typename TPosInWord>
inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
_getWordRank(RankDictionary<TValue, TwoLevels<TSpec> > const & /* dict */,
             TWord const & word,
             TPosInWord posInWord,
             TValue c)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                           TRankDictionary;
    typedef typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::TValues  TRankDictionaryValues;
    typedef typename Size<TRankDictionary>::Type                                TSize;

    TRankDictionaryValues values;
    values.i = word;

    TSize valueRank = 0;

    for (TSize i = 0; i <= posInWord; ++i)
        valueRank += isEqual(values[i], c);

    return valueRank;
}

// ----------------------------------------------------------------------------
// Function _getWordRank(Dna)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TWord, typename TPosInWord>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<Dna, TwoLevels<TSpec> > const>::Type
_getWordRank(RankDictionary<Dna, TwoLevels<TSpec> > const & /* dict */,
             TWord const & values,
             TPosInWord posInWord,
             Dna c)
{
    typedef RankDictionary<Dna, TwoLevels<TSpec> >                      TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                        TSize;

    // Clear the last positions.
    TWord word = hiBits(values, TRankDictionary::_BITS_PER_VALUE * (posInWord + 1));

    // And matches when c == G|T.
    TWord odd  = ((ordValue(c) & ordValue(Dna('G'))) ? word : ~word) >> 1;

    // And matches when c == C|T.
    TWord even = ((ordValue(c) & ordValue(Dna('C'))) ? word : ~word);

    // Apply the interleaved mask.
    TWord mask = odd & even & RankDictionaryBitMask_<TWord>::VALUE;

    // The rank is the sum of the bits on.
    TSize valueRank = popCount(mask);

    // If c == A then masked character positions must be subtracted from the count.
    if (c == Dna('A')) valueRank -= TRankDictionary::_VALUES_PER_WORD - (posInWord + 1);

    return valueRank;
}

// ----------------------------------------------------------------------------
// Function _getWordRank(bool)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TWord, typename TPosInWord>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<bool, TwoLevels<TSpec> > const>::Type
_getWordRank(RankDictionary<bool, TwoLevels<TSpec> > const & /* dict */,
             TWord const & values,
             TPosInWord posInWord,
             bool c)
{
    // Negate the values to compute the rank of zero.
    TWord word = c ? values : ~values;

    // Clear the last positions.
    TWord mask = hiBits(word, posInWord + 1);

    // Get the sum of the bits on.
    return popCount(mask);
}

// ----------------------------------------------------------------------------
// Function _getWordRank()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TWord>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
_getWordRank(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TWord const & values, TValue c)
{
    return _getWordRank(dict, values, RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_WORD - 1, c);
}

// ----------------------------------------------------------------------------
// Function _getValueRank()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TValues, typename TPosInBlock>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
_getValueRank(RankDictionary<TValue, TwoLevels<TSpec> > const & dict,
              TValues const & values,
              TPosInBlock posInBlock,
              TValue c)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >       TRankDictionary;
    typedef typename Size<TRankDictionary>::Type            TSize;

    TSize wordPos    = _toWordPos(dict, posInBlock);
    TSize posInWord  = _toPosInWord(dict, posInBlock);

    TSize valueRank = 0;

    // NOTE(esiragusa): writing the loop in this form prevents the compiler from unrolling it.
//    for (TSize wordPrevPos = 0; wordPrevPos < wordPos; ++wordPrevPos)
//      valueRank += _getWordRank(dict, values[wordPrevPos].i, c);

    for (TSize wordPrevPos = 0; wordPrevPos < TRankDictionary::_WORDS_PER_BLOCK; ++wordPrevPos)
      if (wordPrevPos < wordPos) valueRank += _getWordRank(dict, values[wordPrevPos].i, c);

    valueRank += _getWordRank(dict, values[wordPos].i, posInWord, c);

    return valueRank;
}

// ----------------------------------------------------------------------------
// Function _getValuesRanks()
// ----------------------------------------------------------------------------
// TODO(esiragusa): Specialize _getValuesRanks() for Dna.

template <typename TValue, typename TSpec, typename TPos>
inline typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type
_getValuesRanks(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TPos pos)
{
    typedef typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type  TBlock;
    typedef typename ValueSize<TValue>::Type                                TValueSize;

    TBlock blockRank;

    for (TValueSize c = 0; c < ValueSize<TValue>::VALUE; ++c)
        assignValue(blockRank, c, _getValueRank(dict, _valuesAt(dict, pos), _toPosInBlock(dict, pos), TValue(c)));

    return blockRank;
}

// ----------------------------------------------------------------------------
// Function _getValuesRanks(bool)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TPos>
inline typename RankDictionaryBlock_<bool, TwoLevels<TSpec> >::Type
_getValuesRanks(RankDictionary<bool, TwoLevels<TSpec> > const & dict, TPos pos)
{
    return _getValueRank(dict, _valuesAt(dict, pos), _toPosInBlock(dict, pos), true);
}

// ----------------------------------------------------------------------------
// Function getRank()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos, typename TChar>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
getRank(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TPos pos, TChar c)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> > const                 TRankDictionary;
    typedef typename Fibre<TRankDictionary, FibreRanks>::Type               TFibreRanks;
    typedef typename Value<TFibreRanks>::Type                               TRankEntry;
//    typedef typename RankDictionaryBlock_<TValue, TwoLevels<TSpec> >::Type  TRankBlock;
    typedef typename RankDictionaryValues_<TValue, TwoLevels<TSpec> >::Type TRankValues;
    typedef typename Size<TRankDictionary>::Type                            TSize;

    TSize blockPos   = _toBlockPos(dict, pos);
    TSize posInBlock = _toPosInBlock(dict, pos);

//    TRankEntry entry = loadAndCache(dict.ranks[blockPos]);
    TRankEntry const & entry = dict.ranks[blockPos];

//    TRankBlock block = loadAndCache(entry.block);
    TRankValues values = loadAndCache(entry.values);

    return _getBlockRank(dict, entry.block, pos, static_cast<TValue>(c)) +
           _getValueRank(dict, values, posInBlock, static_cast<TValue>(c));
}

// ----------------------------------------------------------------------------
// Function getRank(bool)
// ----------------------------------------------------------------------------

template <typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Size<RankDictionary<bool, TwoLevels<TSpec> > const>::Type
getRank(RankDictionary<bool, TwoLevels<TSpec> > const & dict, TPos pos)
{
    return getRank(dict, pos, true);
}

// ----------------------------------------------------------------------------
// Function getValue()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Value<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
getValue(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TPos pos)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                   TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                        TSize;

    TSize blockPos   = _toBlockPos(dict, pos);
    TSize posInBlock = _toPosInBlock(dict, pos);
    TSize wordPos    = _toWordPos(dict, posInBlock);
    TSize posInWord  = _toPosInWord(dict, posInBlock);

    return _valuesAt(dict, blockPos, wordPos)[posInWord];
}

template <typename TValue, typename TSpec, typename TPos>
SEQAN_HOST_DEVICE inline typename Value<RankDictionary<TValue, TwoLevels<TSpec> > const>::Type
getValue(RankDictionary<TValue, TwoLevels<TSpec> > const & dict, TPos pos)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                   TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                        TSize;

    TSize blockPos   = _toBlockPos(dict, pos);
    TSize posInBlock = _toPosInBlock(dict, pos);
    TSize wordPos    = _toWordPos(dict, posInBlock);
    TSize posInWord  = _toPosInWord(dict, posInBlock);

    return _valuesAt(dict, blockPos, wordPos)[posInWord];
}

// ----------------------------------------------------------------------------
// Function setValue()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TPos, typename TChar>
inline void setValue(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TPos pos, TChar c)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >                   TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                        TSize;

    TSize blockPos   = _toBlockPos(dict, pos);
    TSize posInBlock = _toPosInBlock(dict, pos);
    TSize wordPos    = _toWordPos(dict, posInBlock);
    TSize posInWord  = _toPosInWord(dict, posInBlock);

    assignValue(_valuesAt(dict, blockPos, wordPos), posInWord, static_cast<TValue>(c));
}

// ----------------------------------------------------------------------------
// Function appendValue()
// ----------------------------------------------------------------------------
// NOTE(esiragusa): Better not to have appendValue() - it is not efficient - and thus neither length().

template <typename TValue, typename TSpec, typename TChar, typename TExpand>
inline void appendValue(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TChar c, Tag<TExpand> const tag)
{
    resize(dict, length(dict) + 1, tag);
    setValue(dict, length(dict) - 1, c);
}

// ----------------------------------------------------------------------------
// Function updateRanks()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
inline void updateRanks(RankDictionary<TValue, TwoLevels<TSpec> > & dict)
{
    typedef RankDictionary<TValue, TwoLevels<TSpec> >               TRankDictionary;
    typedef typename Size<TRankDictionary>::Type                    TSize;
    typedef typename Fibre<TRankDictionary, FibreRanks>::Type       TFibreRanks;
    typedef typename Iterator<TFibreRanks, Standard>::Type          TFibreRanksIter;

    if (empty(dict)) return;

    TFibreRanksIter ranksBegin = begin(dict.ranks, Standard());
    TFibreRanksIter ranksEnd = end(dict.ranks, Standard());

    // Insures the first block ranks start from zero.
    _clearBlockAt(dict, 0u);

    // Clear the uninitialized values.
    _padValues(dict);

    // Iterate through the blocks.
    for (TFibreRanksIter ranksIt = ranksBegin; ranksIt != ranksEnd - 1; ++ranksIt)
    {
        TSize blockPos = ranksIt - ranksBegin;
        TSize curr = _toPos(dict, blockPos);
        TSize next = _toPos(dict, blockPos + 1);

        _blockAt(dict, next) = _blockAt(dict, curr) + _getValuesRanks(dict, next - 1);
    }
}

// ----------------------------------------------------------------------------
// Function length()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec>
inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
length(RankDictionary<TValue, TwoLevels<TSpec> > const & dict)
{
    return dict._length;
}

// ----------------------------------------------------------------------------
// Function reserve()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TSize, typename TExpand>
inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
reserve(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TSize newCapacity, Tag<TExpand> const tag)
{
    return reserve(dict.ranks, std::ceil(newCapacity /
                   static_cast<double>(RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_BLOCK)), tag);
}

// ----------------------------------------------------------------------------
// Function resize()
// ----------------------------------------------------------------------------

template <typename TValue, typename TSpec, typename TSize, typename TExpand>
inline typename Size<RankDictionary<TValue, TwoLevels<TSpec> > >::Type
resize(RankDictionary<TValue, TwoLevels<TSpec> > & dict, TSize newLength, Tag<TExpand> const tag)
{
    dict._length = newLength;
    return resize(dict.ranks, std::ceil(newLength /
                  static_cast<double>(RankDictionary<TValue, TwoLevels<TSpec> >::_VALUES_PER_BLOCK)), tag);
}

}

#endif  // INDEX_FM_RANK_DICTIONARY_LEVELS_H_
