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
// Author: Lily Shellhammer
// ==========================================================================


#ifndef SEQAN_RNA_FORMAT_RECORD_H_
#define SEQAN_RNA_FORMAT_RECORD_H_

namespace seqan {

// ============================================================================
// Tags, Classes, Enums
// ============================================================================

// ----------------------------------------------------------------------------
// Class RnaRecord
// ----------------------------------------------------------------------------

class RnaRecord
{
public:
        static const int INVALID_POS = -1;


    // Amount of records.
    int32_t amount;    
    //beginning and ending positions of the sequence
    int32_t begPos;
    int32_t endPos;
    //energy
    float energy;    
    // Record's name.
    CharString name;
    
    //beginning and ending positions
    String<int>  index;

    //string of base at each position in Rna strand
    Rna5String base;   

    // Position of n base's pair.
    String<int>  pair;

    //Qual, and information specific to other file formats. Will be set to default value in constructor when I figure out what to set them to.
    CharString qual;


    ////////RDAT FILES
    int offset;

    String<CharString> seqpos;

    String<CharString> annotation;

    CharString comment;

    //Annotation data 1
    //annotation data 2

    String<float> reactivity;

    String<float> reactivity_error;

    String<float> xsel;

    String<float> xsel_refine;

    //mutpos
  
    // Default constructor.
    RnaRecord() : energy(0), offset(0), comment(""), begPos(INVALID_POS), endPos(INVALID_POS), name(" "), amount(0)
    {}                                                                                      

};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function clear()
// ----------------------------------------------------------------------------


inline void clear(RnaRecord & record)
{
    clear(record.name);
    clear(record.index);
    clear(record.base);
    clear(record.pair);
    clear(record.qual);

    clear(record.seqpos);
    clear(record.annotation);
    clear(record.comment);
    clear(record.reactivity);
    clear(record.reactivity_error);
    clear(record.xsel);    
    clear(record.xsel_refine);
}

}  // namespace seqan

#endif  //SEQAN_RNA_FORMAT_RECORD_H_
