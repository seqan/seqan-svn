// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2018, Knut Reinert, FU Berlin
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
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)sven.boenigk@fu-berlin.de HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Sven Boenigk <sven.boenigk@fu-berlin.de>
// ==========================================================================
// Approximate String matching via search schemes on a substring index.
// ==========================================================================

//NOTE (svnbgnk) this file has to be include as <seqan/index/find2_index_approx_its.h>
#ifndef SEQAN_INDEX_FIND2_INDEX_APPROX_ITS_H_
#define SEQAN_INDEX_FIND2_INDEX_APPROX_ITS_H_

namespace seqan {
// Compare potential occurrences directly to genome if the range on the index is small enough.
template <typename TDelegateD,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks,
          typename TDir>
inline void directSearch(TDelegateD & delegateDirect,
                  Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > iter,
                  TNeedle const & needle,
                  uint32_t const needleLeftPos,
                  uint32_t const needleRightPos,
                  uint8_t const errors,
                  OptimalSearch<nbrBlocks> const & s,
                  uint8_t const blockIndex,
                  TDir const & /**/)
{
    auto const & genome = indexText(*iter.fwdIter.index);
    uint32_t needleL = length(needle);
    uint32_t blocks = s.pi.size();

    std::vector<uint32_t> blockStarts(blocks - blockIndex);
    std::vector<uint32_t> blockEnds(blocks - blockIndex);
    for(uint32_t j = blockIndex; j < s.pi.size(); ++j){
        uint32_t blockStart = (s.pi[j] - 1 == 0) ? 0 : s.chronBL[s.pi[j] - 2];
        blockStarts[j - blockIndex] = blockStart;
        blockEnds[j - blockIndex] = s.chronBL[s.pi[j] - 1];
    }

    //modifie blockStart or blockEnd if we are already inside a block
    if(std::is_same<TDir, Rev>::value){
        if(needleRightPos - 1 > blockStarts[0] && needleRightPos - 1 < blockEnds[0])
            blockStarts[0] = needleRightPos - 1;
    }else{
        if(needleLeftPos > blockStarts[0] && needleLeftPos < blockEnds[0])
            blockEnds[0] = needleLeftPos;
    }

    for(uint32_t i = iter.fwdIter.vDesc.range.i1; i < iter.fwdIter.vDesc.range.i2; ++i){
        bool valid = true;
        Pair<uint16_t, uint32_t> sa_info = iter.fwdIter.index->sa[i];
        //dont need look at the reverse index in this case since i dont use mappability
        uint32_t chromlength = length(genome[sa_info.i1]);
        if(!(needleLeftPos <= sa_info.i2 && chromlength - 1 >= sa_info.i2 - needleLeftPos + needleL - 1))
            continue;

        sa_info.i2 = sa_info.i2 - needleLeftPos;
        uint8_t errors2 = errors;
	//iterate over each block according to search scheme
        for(uint32_t j = 0; j < blockStarts.size(); ++j){
            // compare bases to needle
            for(uint32_t k = blockStarts[j]; k <  blockEnds[j]; ++k){
                if(needle[k] != genome[sa_info.i1][sa_info.i2 + k])
                    ++errors2;
            }
            if(errors2 < s.l[blockIndex + j] || errors2 > s.u[blockIndex + j]){
                valid = false;
                break;
            }
        }
        if(valid)
            delegateDirect(sa_info, needle, errors2);
    }
}


//added lambda functions "In Text Search" and ITS condition to _optimalSearchSchemeChildren, _optimalSearchSchemeExact
//and _optimalSearchScheme
//(from find2_index_approx.h)
template <typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks,
          typename TDir,
          typename TDistanceTag>
inline void _optimalSearchSchemeChildren(TDelegate & delegate,
                                         TDelegateD & delegateDirect,
                                         TCondition & iTSCondition,
                                         Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > iter,
                                         TNeedle const & needle,
                                         uint32_t const needleLeftPos,
                                         uint32_t const needleRightPos,
                                         uint8_t const errors,
                                         OptimalSearch<nbrBlocks> const & s,
                                         uint8_t const blockIndex,
                                         uint8_t const minErrorsLeftInBlock,
                                         TDir const &,
                                         TDistanceTag const & )
{
    bool goToRight = std::is_same<TDir, Rev>::value;
    if (goDown(iter, TDir()))
    {
        uint32_t charsLeft = s.blocklength[blockIndex] - (needleRightPos - needleLeftPos - 1);
        do
        {
            bool delta = !ordEqual(parentEdgeLabel(iter, TDir()),
                                   needle[goToRight ? needleRightPos - 1 : needleLeftPos - 1]);

            // NOTE (cpockrandt): this might not be optimal yet! we have more edges than in the theoretical model,
            // since we go down an edge before we check whether it can even work out!
            if (minErrorsLeftInBlock > 0 && charsLeft + delta < minErrorsLeftInBlock + 1u)
                // charsLeft - 1 < minErrorsLeftInBlock - delta
            {
                continue;
            }

            int32_t needleLeftPos2 = needleLeftPos - !goToRight;
            uint32_t needleRightPos2 = needleRightPos + goToRight;

            if (needleRightPos - needleLeftPos == s.blocklength[blockIndex])
            {
                uint8_t blockIndex2 = std::min(blockIndex + 1, static_cast<uint8_t>(s.u.size()) - 1);
                bool goToRight2 = s.pi[blockIndex2] > s.pi[blockIndex2 - 1];
                if (goToRight2)
                {
                    _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos2,
                                         needleRightPos2, errors + delta, s, blockIndex2, Rev(), TDistanceTag());
                }
                else
                {
                    _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos2,
                                         needleRightPos2, errors + delta, s, blockIndex2, Fwd(), TDistanceTag());
                }
            }
            else
            {
                _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos2,
                                     needleRightPos2, errors + delta, s, blockIndex, TDir(), TDistanceTag());
            }
        } while (goRight(iter, TDir()));
    }
}


template <typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks,
          typename TDir,
          typename TDistanceTag>
inline void _optimalSearchSchemeExact(TDelegate & delegate,
                                      TDelegateD & delegateDirect,
                                      TCondition & iTSCondition,
                                      Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > iter,
                                      TNeedle const & needle,
                                      uint32_t const needleLeftPos,
                                      uint32_t const needleRightPos,
                                      uint8_t const errors,
                                      OptimalSearch<nbrBlocks> const & s,
                                      uint8_t const blockIndex,
                                      TDir const & ,
                                      TDistanceTag const &)
{
    bool goToRight2 = (blockIndex < s.pi.size() - 1) ? s.pi[blockIndex + 1] > s.pi[blockIndex] :
    s.pi[blockIndex] > s.pi[blockIndex - 1];

    if (std::is_same<TDir, Rev>::value)
    {
        uint32_t infixPosLeft = needleRightPos - 1;
        uint32_t infixPosRight = needleLeftPos + s.blocklength[blockIndex] - 1;

        if (!goDown(iter, infix(needle, infixPosLeft, infixPosRight + 1), TDir()))
            return;

        if (goToRight2)
        {
            _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos,
                                 infixPosRight + 2, errors, s, std::min(blockIndex + 1,
                                 static_cast<uint8_t>(s.u.size()) - 1), Rev(), TDistanceTag());
        }
        else
        {
            _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos,
                                 infixPosRight + 2, errors, s,
                                 std::min(blockIndex + 1, static_cast<uint8_t>(s.u.size()) - 1), Fwd(), TDistanceTag());
        }
    }
    else
    {
        // has to be signed, otherwise we run into troubles when checking for -1 >= 0u
        int32_t infixPosLeft = needleRightPos - s.blocklength[blockIndex] - 1;
        int32_t infixPosRight = needleLeftPos - 1;

        while (infixPosRight >= infixPosLeft)
        {
            if (!goDown(iter, needle[infixPosRight], TDir()))
                return;
            --infixPosRight;
        }
        if (goToRight2)
        {
            _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, infixPosLeft, needleRightPos,
                                 errors, s, std::min(blockIndex + 1, static_cast<uint8_t>(s.u.size()) - 1), Rev(),
                                 TDistanceTag());
        }
        else
        {
            _optimalSearchScheme(delegate, delegateDirect, iTSCondition, iter, needle, infixPosLeft, needleRightPos,
                                 errors, s, std::min(blockIndex + 1, static_cast<uint8_t>(s.u.size()) - 1), Fwd(),
                                 TDistanceTag());
        }
    }
}

//added condition for using In Text Search
template <typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks,
          typename TDir,
          typename TDistanceTag>
inline void _optimalSearchScheme(TDelegate & delegate,
                                 TDelegateD & delegateDirect,
                                 TCondition & iTSCondition,
                                 Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > iter,
                                 TNeedle const & needle,
                                 uint32_t const needleLeftPos,
                                 uint32_t const needleRightPos,
                                 uint8_t const errors,
                                 OptimalSearch<nbrBlocks> const & s,
                                 uint8_t const blockIndex,
                                 TDir const & ,
                                 TDistanceTag const & )
{
    uint8_t const maxErrorsLeftInBlock = s.u[blockIndex] - errors;
    uint8_t const minErrorsLeftInBlock = (s.l[blockIndex] > errors) ? (s.l[blockIndex] - errors) : 0;

    if (minErrorsLeftInBlock == 0 && needleLeftPos == 0 && needleRightPos == length(needle) + 1)
    {
        delegate(iter, needle, errors, false);
    }
    // Exact search in current block.
    else if (maxErrorsLeftInBlock == 0 && needleRightPos - needleLeftPos - 1 != s.blocklength[blockIndex])
    {
        _optimalSearchSchemeExact(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos, needleRightPos,
                                  errors, s, blockIndex, TDir(), TDistanceTag());
    }

    else
    {
        //use lambda function to determine condition for In Text Search
        if(iTSCondition(iter, needleLeftPos, needleRightPos, errors, s, blockIndex))
        {
            directSearch(delegateDirect, iter, needle, needleLeftPos, needleRightPos, errors, s, blockIndex, TDir());
            return;
        }
        _optimalSearchSchemeChildren(delegate, delegateDirect, iTSCondition, iter, needle, needleLeftPos,
                                     needleRightPos, errors, s, blockIndex, minErrorsLeftInBlock, TDir(),
                                     TDistanceTag());
    }
}



template <typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks>
inline void _optimalSearchScheme(TDelegate & delegate,
                                 TDelegateD & delegateDirect,
                                 TCondition & iTSCondition,
                                 Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > it,
                                 TNeedle const & needle,
                                 OptimalSearch<nbrBlocks> const & s)
{
    bool initialDirection = s.pi[1] > s.pi[0];
    if(initialDirection)
        _optimalSearchScheme(delegate, delegateDirect, iTSCondition, it, needle, s.startPos, s.startPos + 1, 0, s, 0,
                             Rev(), HammingDistance());
    else
        _optimalSearchScheme(delegate, delegateDirect, iTSCondition, it, needle, s.startPos, s.startPos + 1, 0, s, 0,
                             Fwd(), HammingDistance());

}

template <typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndex, typename TIndexSpec,
          typename TNeedle,
          size_t nbrBlocks, size_t N>
inline void _optimalSearchScheme(TDelegate & delegate,
                                 TDelegateD & delegateDirect,
                                 TCondition & iTSCondition,
                                 Iter<Index<TText, BidirectionalIndex<TIndex> >, VSTree<TopDown<TIndexSpec> > > it,
                                 TNeedle const & needle,
                                 std::array<OptimalSearch<nbrBlocks>, N> const & ss)
{
    for (auto & s : ss)
        _optimalSearchScheme(delegate, delegateDirect, iTSCondition, it, needle, s);

}

// ----------------------------------------------------------------------------
// Function find()
// ----------------------------------------------------------------------------

template <size_t minErrors, size_t maxErrors,
          typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndexSpec,
          typename TChar, typename TStringSpec>
inline void
find(TDelegate & delegate,
     TDelegateD & delegateDirect,
     TCondition & iTSCondition,
     Index<TText, BidirectionalIndex<TIndexSpec> > & index,
     String<TChar, TStringSpec> const & needle)
{
    auto scheme = OptimalSearchSchemes<minErrors, maxErrors>::VALUE;
    _optimalSearchSchemeComputeFixedBlocklength(scheme, length(needle));
    _optimalSearchSchemeComputeChronBlocklength(scheme);
    Iter<Index<TText, BidirectionalIndex<TIndexSpec> >, VSTree<TopDown<> > > it(index);
    _optimalSearchScheme(delegate, delegateDirect, iTSCondition, it, needle, scheme);
}

// ----------------------------------------------------------------------------
// Function find()
// ----------------------------------------------------------------------------

template <size_t minErrors, size_t maxErrors,
          typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndexSpec,
          typename TNeedle, typename TStringSetSpec,
          typename TParallelTag>
inline void
find(TDelegate & delegate,
     TDelegateD & delegateDirect,
     TCondition & iTSCondition,
     Index<TText, BidirectionalIndex<TIndexSpec> > & index,
     StringSet<TNeedle, TStringSetSpec> const & needles,
     TParallelTag const & )
{
    typedef typename Iterator<StringSet<TNeedle, TStringSetSpec> const, Rooted>::Type TNeedleIt;
    typedef typename Reference<TNeedleIt>::Type                                       TNeedleRef;
    iterate(needles, [&](TNeedleIt const & needleIt)
    {
        TNeedleRef needle = value(needleIt);
        find<minErrors, maxErrors>(delegate, delegateDirect, iTSCondition, index, needle);
    },
    Rooted(), TParallelTag());
}

// ----------------------------------------------------------------------------
// Function find()
// ----------------------------------------------------------------------------

template <size_t minErrors, size_t maxErrors,
          typename TDelegate, typename TDelegateD, typename TCondition,
          typename TText, typename TIndexSpec,
          typename TNeedle, typename TStringSetSpec>
inline void
find(TDelegate & delegate,
     TDelegateD & delegateDirect,
     TCondition & iTSCondition,
     Index<TText, BidirectionalIndex<TIndexSpec> > & index,
     StringSet<TNeedle, TStringSetSpec> const & needles)
{
    find<minErrors, maxErrors>(delegate, delegateDirect, iTSCondition, index, needles, Serial());
}

}

#endif
