// ==========================================================================
//                              generalProcessing.h
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
// Author: Sebastian Roskosch <serosko@zedat.fu-berlin.de>
// Author: Benjamin Menkuec <benjamin@menkuec.de>
// ==========================================================================
// This file provides functions used by different parts of seqan-flexbar
// which is based in the implementation of the original flexbar program
// in [1].
// [1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, C.  FLEXBAR—Flexible
// Barcode and Adapter Processing for Next-Generation Sequencing Platforms.
// Biology 2012, 1, 895-905.
// ==========================================================================



#ifndef SANDBOX_GROUP3_APPS_SEQDPT_GENERALPROCESSING_H_
#define SANDBOX_GROUP3_APPS_SEQDPT_GENERALPROCESSING_H_

#ifdef _OPENMP
#include <omp.h>
#endif
#include <seqan/basic.h>
#include <seqan/sequence.h>
#include <seqan/parallel.h>

#include <seqan/stream.h>

#include "helper_functions.h"
#include "general_stats.h"
#include <initializer_list>


// ============================================================================
// Tags, Classes, Enums
// ============================================================================


// ============================================================================
// Functions
// ============================================================================

struct NoSubstitute {};

template<typename TSeqChar, typename TSub>
void replaceN(TSeqChar& seqChar, const TSub sub) noexcept
{
    seqChar = sub;
}

template<typename TSeqChar>
void replaceN(TSeqChar& seqChar, const NoSubstitute) noexcept
{
    (void)seqChar;
}

template<typename TSeq, typename TSub>
inline int findNUniversal(TSeq& seq, unsigned allowed, const TSub substitute) noexcept
{
    const TSeq wanted = 'N';
    unsigned c = 0;
    for (auto& seqChar : seq)
    {
        if (seqChar == wanted)
        {
            replaceN(seqChar, substitute);
            ++c;
            if (c > allowed)
                return -1;       //sequence will be removed
        }
    }
    return c;                   //sequence not deleted, number of substitutions returned
}

template<template<typename> class TRead, typename TSeq, typename TSub,
    typename = std::enable_if_t<std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>> ::value >>
inline int findNMultiplex(TRead<TSeq>& read, unsigned allowed, const TSub substitute, bool = false) noexcept
{
    (void)read;
    (void)allowed;
    (void)substitute;
    return 0;
}

template<template<typename> class TRead, typename TSeq, typename TSub,
    typename = std::enable_if_t<std::is_same<TRead<TSeq>, ReadMultiplex<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplexPairedEnd<TSeq >> ::value >>
inline int findNMultiplex(TRead<TSeq>& read, unsigned allowed, const TSub substitute) noexcept
{
    return findNUniversal(read.demultiplex, allowed, substitute);
}

template<template<typename> class TRead, typename TSeq, typename TSub,
    typename = std::enable_if_t<std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplex<TSeq >> ::value >>
inline int findNPairedEnd(TRead<TSeq>& read, unsigned allowed, const TSub substitute, bool = false) noexcept
{
    (void)read;
    (void)allowed;
    (void)substitute;
    return 0;
}

template<template<typename> class TRead, typename TSeq, typename TSub,
    typename = std::enable_if_t<std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplexPairedEnd<TSeq >> ::value >>
inline int findNPairedEnd(TRead<TSeq>& read, unsigned allowed, const TSub substitute) noexcept
{
    return findNUniversal(read.seqRev, allowed, substitute);
}

template<template<typename> class TRead, typename TSeq, typename TSub>
int findN(TRead<TSeq>& read, unsigned allowed, const TSub substitute) noexcept
{
    auto c = findNUniversal(read.seq, allowed, substitute);
    if (c == -1)
        return -1;
    const auto c2 = findNMultiplex(read, allowed, substitute);
    if (c2 == -1)
        return -1;
    c += c2;
    const auto c3 = findNPairedEnd(read, allowed, substitute);
    if (c3 == -1)
        return -1;
    c += c3;
    return c;                   //sequence not deleted, number of substitutions returned
}

//universal function for all combinations of options
template<template <typename> typename TRead, typename TSeq, typename TSub, typename TStats>
void processN(std::vector<TRead<TSeq>>& reads, unsigned allowed, TSub substitute, TStats& stats) noexcept
{
    int limit = length(reads);
    std::vector<int> res(limit);
    SEQAN_OMP_PRAGMA(parallel for default(shared)schedule(static))
        for (int i = 0; i < limit; ++i)
        {
            res[i] = findN(reads[i], allowed, substitute);
        }

    stats.removedN += _eraseSeqs(res, -1, reads);
    for (int i = length(res) - 1; i >= 0; --i)
    {
        if (res[i] != -1)
            stats.uncalledBases += res[i];
    }
}

template<template <typename> class TRead, typename TSeq>
    unsigned int postTrim(std::vector<TRead<TSeq>>& reads, const unsigned min) noexcept
{
    const auto numReads = (int)length(reads);
    reads.erase(std::remove_if(reads.begin(), reads.end(), [min](const auto& read) {return read.minSeqLen() < min;}), reads.end());
    return numReads - length(reads);
}

// main preTrim function
template<template <typename> class TRead, typename TSeq, bool tagTrimming,
    typename = std::enable_if_t < std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same < TRead<TSeq>, ReadMultiplex < TSeq >> ::value >>
unsigned int _preTrim(std::vector<TRead<TSeq>>& readSet, const unsigned head, const unsigned tail, const unsigned min, bool = false) noexcept(!tagTrimming)
{
    std::transform(readSet.begin(), readSet.end(), readSet.begin(), [head, tail](auto& read) -> auto 
    {
        const auto seqLen = length(read.seq);
        if (seqLen > (head + tail))
        {
            std::string insertToken;
            if (head > 0)
            {
                if (tagTrimming)
                    insertToken = ":TL:" + std::string(prefix(read.seq, head));
                erase(read.seq, 0, head);
            }
            if (tail > 0)
            {
                const auto seqLen = length(read.seq);
                if (tagTrimming)
                    insertToken += ":TR:" + std::string(suffix(read.seq, seqLen - tail));
                erase(read.seq, seqLen - tail, seqLen);
            }
            if (insertToken.size() != 0)
                insertAfterFirstToken(read.id, std::move(insertToken));
        }
        return read;
    });
    unsigned int oldLen = readSet.size();
    readSet.erase(std::remove_if(readSet.begin(), readSet.end(), [min](auto& read) {return read.minSeqLen() < min;}), readSet.end());
    return oldLen - readSet.size();
}

template<template <typename> class TRead, typename TSeq, bool tagTrimming,
    typename = std::enable_if_t < std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value || std::is_same < TRead<TSeq>, ReadMultiplexPairedEnd < TSeq >> ::value >>
    unsigned int _preTrim(std::vector<TRead<TSeq>>& readSet, const unsigned head, const unsigned tail, const unsigned min) noexcept(!tagTrimming)
{
    std::transform(readSet.begin(), readSet.end(), readSet.begin(), [head, tail](auto& read) -> auto
    {
        const auto seqLen = length(read.seq);
        if (seqLen >(head + tail))
        {
            std::string insertToken;
            if (head > 0)
            {
                if (tagTrimming)
                {
                    std::string tempString = ":TL:";
                    append(tempString, prefix(read.seq, head));
                    insertAfterFirstToken(read.id, std::move(tempString));
                    tempString = ":TL:";
                    append(tempString, prefix(read.seqRev, head));
                    insertAfterFirstToken(read.idRev, std::move(tempString));
                }
                erase(read.seq, 0, head);
                erase(read.seqRev, 0, head);
            }
            if (tail > 0)
            {
                const auto seqLen = length(read.seq);
                if (tagTrimming)
                {
                    std::string tempString = ":TR:";
                    append(tempString, suffix(read.seq, seqLen - tail));
                    insertAfterFirstToken(read.id, std::move(tempString));
                    tempString = ":TR:";
                    append(tempString, suffix(read.seqRev, seqLen - tail));
                    insertAfterFirstToken(read.idRev, std::move(tempString));
                }
                erase(read.seq, seqLen - tail, seqLen);
                erase(read.seqRev, seqLen - tail, seqLen);
            }
            if (insertToken.size() != 0)
                insertAfterFirstToken(read.id, std::move(insertToken));
        }
        return read;
    });
    unsigned int oldLen = readSet.size();
    readSet.erase(std::remove_if(readSet.begin(), readSet.end(), [min](auto& read) {return read.minSeqLen() < min;}), readSet.end());
    return oldLen - readSet.size();
}

template <template<typename> class TRead, typename TSeq, typename TStats>
void preTrim(std::vector<TRead<TSeq>>& reads, unsigned head, unsigned tail, unsigned min, const bool tagTrimming, TStats& stats)
{
    if(tagTrimming)
        stats.removedShort += _preTrim<TRead, TSeq, true>(reads, head, tail, min);
    else
        stats.removedShort += _preTrim<TRead, TSeq, false>(reads, head, tail, min);
}

//Trims sequences to specific length and deletes to short ones together with their IDs
template<template <typename> class TRead, typename TSeq, typename TStats,
    typename = std::enable_if_t < std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same < TRead<TSeq>, ReadMultiplex < TSeq >> ::value >>
    void trimTo(std::vector<TRead<TSeq>>& reads, const unsigned len, TStats& stats, bool = true) noexcept
{
    const auto limit = (int)length(reads);
    std::vector<bool> rem(limit);
    SEQAN_OMP_PRAGMA(parallel for default(shared)schedule(static))
        for (int i = 0; i < limit; ++i)
        {
            if (length(reads[i].seq) < len)
            {
                rem[i] = true;
            }
            else
            {
                rem[i] = false;
                if (length(reads[i].seq) > len)
                {
                    erase(reads[i].seq, len, length(reads[i].seq));
                }
            }
        }
    stats.removedShort += _eraseSeqs(rem, true, reads);
}

template<template <typename> class TRead, typename TSeq, typename TStats,
    typename = std::enable_if_t < std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value || std::is_same < TRead<TSeq>, ReadMultiplexPairedEnd < TSeq >> ::value >>
    void trimTo(std::vector<TRead<TSeq>>& reads, const unsigned len, TStats& stats) noexcept
{
    const auto limit = (int)length(reads);
    std::vector<bool> rem(limit);
    SEQAN_OMP_PRAGMA(parallel for default(shared)schedule(static))
        for (int i = 0; i < limit; ++i)
        {
            if (std::min(length(reads[i].seq), length(reads[i].seqRev)) < len)
            {
                rem[i] = true;
            }
            else
            {
                rem[i] = false;
                if (length(reads[i].seq) > len)
                    erase(reads[i].seq, len, length(reads[i].seq));
                if (length(reads[i].seqRev) > len)
                    erase(reads[i].seqRev, len, length(reads[i].seqRev));
            }
        }
    stats.removedShort += _eraseSeqs(rem, true, reads);
}


#endif  // #ifndef SANDBOX_GROUP3_APPS_SEQDPT_GENERALPROCESSING_H_
