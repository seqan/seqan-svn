// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
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
// Author: Enrico Siragusa <enrico.siragusa@fu-berlin.de>
//         David Weese <david.weese@fu-berlin.de>
// ==========================================================================
// Smart file for reading/writing files in Fasta or Fastq format.
// ==========================================================================

#ifndef SEQAN_SEQ_IO_SEQUENCE_FILE_H_
#define SEQAN_SEQ_IO_SEQUENCE_FILE_H_

namespace seqan {

// ============================================================================
// Classes
// ============================================================================

// ============================================================================
// Typedefs
// ============================================================================

// ----------------------------------------------------------------------------
// Typedef SeqFileIn
// ----------------------------------------------------------------------------

/*!
 * @class SeqFileIn
 * @extends SmartFile
 * @headerfile <seqan/gff_io.h>
 * @brief @link SmartFile @endlink for reading FASTA, FAST, and other sequence formats.
 *
 * @signature typedef SmartFile<Fastq, Input> SeqFileIn;
 */
typedef SmartFile<Fastq, Input>     SeqFileIn;

// ----------------------------------------------------------------------------
// Typedef SeqFileOut
// ----------------------------------------------------------------------------

/*!
 * @class SeqFileOut
 * @extends SmartFile
 * @headerfile <seqan/gff_io.h>
 * @brief @link SmartFile @endlink for writing FASTA and FASTQ files.
 *
 * @signature typedef SmartFile<Fastq, Output> SeqFileOut;
 */
typedef SmartFile<Fastq, Output>    SeqFileOut;

// --------------------------------------------------------------------------
// Tag AutoSeqFormat
// --------------------------------------------------------------------------
// if TagSelector is set to -1, the file format is auto-detected

/*!
 * @class AutoSeqFormat
 * @extends TagSelector
 * @headerfile <seqan/file.h>
 * @brief Auto-detects and stores a file format.
 *
 * @signature typedef TagList<Fastq, TagList<Fasta, TagList<Raw> > > SeqFormats;
 * @signature typedef TagSelector<SeqFormat> AutoSeqFormat;
 */
typedef
    TagList<Fastq,
    TagList<Fasta,
    TagList<Embl,
    TagList<GenBank,
    TagList<Raw
    > > > > >
    SeqInFormats;

typedef
    TagList<Fastq,
    TagList<Fasta,
    TagList<Raw
    > > >
    SeqOutFormats;

typedef TagSelector<SeqInFormats>   SeqInFormat;
typedef TagSelector<SeqOutFormats>  SeqOutFormat;

// deprecated
typedef SeqInFormat AutoSeqFormat;

// ============================================================================
// Metafunctions
// ============================================================================

// ----------------------------------------------------------------------------
// Metafunction SmartFileContext
// ----------------------------------------------------------------------------

template <typename TDirection>
struct SeqFileContext_;

template <>
struct SeqFileContext_<Input>
{
    Tuple<CharString, 3>    buffer;
    Dna5QString             hybrid;
};

template <>
struct SeqFileContext_<Output>
{
    SequenceOutputOptions   options;
};


template <typename TSpec, typename TDirection, typename TStorageSpec>
struct SmartFileContext<SmartFile<Fastq, TDirection, TSpec>, TStorageSpec>
{
    typedef SeqFileContext_<TDirection> Type;
};

// ----------------------------------------------------------------------------
// Metafunction FileFormats
// ----------------------------------------------------------------------------

template <typename TSpec>
struct FileFormat<SmartFile<Fastq, Input, TSpec> >
{
    typedef TagSelector<SeqInFormats> Type;
};

template <typename TSpec>
struct FileFormat<SmartFile<Fastq, Output, TSpec> >
{
    typedef TagSelector<SeqOutFormats> Type;
};

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function read(); Without qualities
// ----------------------------------------------------------------------------

/*!
 * @fn SeqFileIn#readRecord
 * @brief Read one sequence from a SeqFileIn
 *
 * @signature void readRecord(meta, seq[, qual], seqFile);
 *
 * @param[out]    meta    A @link StringConcept string @endlink to read the meta/id field into.
 * @param[out]    seq     A @link StringConcept string @endlink to read the sequence into.
 * @param[out]    qual    An optional @link StringConcept string @endlink to read the qualities into.
 * @param[in,out] seqFile The SeqFileIn to read from.
 *
 * @throws IOError if something went wrong.
 *
 * If the file contains base call qualities then these can be either (a) written into the given <tt>qual</tt>
 * parameter if it is provided, (b) written to <tt>seq</tt> if <tt>seq</tt> is of the concept
 * @link AlphabetWithQualitiesConcept @endlink, or (c) discarded.
 */

template <typename TSpec, typename TIdString, typename TSeqString>
inline SEQAN_FUNC_ENABLE_IF(And<Is<InputStreamConcept<typename SmartFile<Fastq, Input, TSpec>::TStream> >,
                                Not<HasQualities<typename Value<TSeqString>::Type> > >, void)
readRecord(TIdString & meta, TSeqString & seq, SmartFile<Fastq, Input, TSpec> & file)
{
    readRecord(meta, seq, file.iter, file.format);
}

template <typename TSpec, typename TIdString, typename TSeqString>
inline SEQAN_FUNC_ENABLE_IF(And<Is<InputStreamConcept<typename SmartFile<Fastq, Input, TSpec>::TStream> >,
                                HasQualities<typename Value<TSeqString>::Type> >, void)
readRecord(TIdString & meta, TSeqString & seq, SmartFile<Fastq, Input, TSpec> & file)
{

    readRecord(meta, seq, context(file).buffer[2], file.iter, file.format);
    assignQualities(seq, context(file).buffer[2]);
}

// ----------------------------------------------------------------------------
// Function readRecords(); Without qualities
// ----------------------------------------------------------------------------

template <typename TIdStringSet, typename TSeqStringSet, typename TSpec>
inline void readRecords(TIdStringSet & meta,
                        TSeqStringSet & seq,
                        SmartFile<Fastq, Input, TSpec> & file,
                        __uint64 maxRecords = MaxValue<__uint64>::VALUE)
{
    typedef typename Value<TSeqStringSet>::Type TSeqString;
    typedef typename Value<TSeqString>::Type TValue;

    String<TValue> seqBuffer;

    // reuse the memory of context(file).buffer for seqBuffer (which has a different type but same sizeof(Alphabet))
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_begin), context(file).buffer[1].data_begin);
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_end), context(file).buffer[1].data_end);
    seqBuffer.data_capacity = context(file).buffer[1].data_capacity;

    for (; !atEnd(file) && maxRecords > 0; --maxRecords)
    {
        readRecord(context(file).buffer[0], seqBuffer, file);
        appendValue(meta, context(file).buffer[0]);
        appendValue(seq, seqBuffer);
    }

    std::swap(reinterpret_cast<char* &>(seqBuffer.data_begin), context(file).buffer[1].data_begin);
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_end), context(file).buffer[1].data_end);
    context(file).buffer[1].data_capacity = seqBuffer.data_capacity;
    seqBuffer.data_capacity = 0;
}

// ----------------------------------------------------------------------------
// Function read(); With qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TIdString, typename TSeqString, typename TQualString>
inline SEQAN_FUNC_ENABLE_IF(Is<InputStreamConcept<typename SmartFile<Fastq, Input, TSpec>::TStream> >, void)
readRecord(TIdString & meta, TSeqString & seq, TQualString & qual, SmartFile<Fastq, Input, TSpec> & file)
{
    readRecord(meta, seq, qual, file.iter, file.format);
}

// ----------------------------------------------------------------------------
// Function readRecords(); With qualities
// ----------------------------------------------------------------------------

template <typename TIdStringSet, typename TSeqStringSet, typename TQualStringSet, typename TSpec>
inline void readRecords(TIdStringSet & meta,
                        TSeqStringSet & seq,
                        TQualStringSet & qual,
                        SmartFile<Fastq, Input, TSpec> & file,
                        __uint64 maxRecords = MaxValue<__uint64>::VALUE)
{
    typedef typename Value<TSeqStringSet>::Type TSeqString;

    TSeqString seqBuffer;

    // reuse the memory of context(file).buffer for seqBuffer (which has a different type but same sizeof(Alphabet))
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_begin), context(file).buffer[1].data_begin);
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_end), context(file).buffer[1].data_end);
    std::swap(seqBuffer.data_capacity, context(file).buffer[1].data_capacity);

    for (; !atEnd(file) && maxRecords > 0; --maxRecords)
    {
        readRecord(context(file).buffer[0], seqBuffer, context(file).buffer[2], file);
        appendValue(meta, context(file).buffer[0]);
        appendValue(seq, seqBuffer);
        appendValue(qual, context(file).buffer[2]);
    }

    std::swap(reinterpret_cast<char* &>(seqBuffer.data_begin), context(file).buffer[1].data_begin);
    std::swap(reinterpret_cast<char* &>(seqBuffer.data_end), context(file).buffer[1].data_end);
    std::swap(seqBuffer.data_capacity, context(file).buffer[1].data_capacity);
}

// ----------------------------------------------------------------------------
// Function writeRecord(); Without qualities
// ----------------------------------------------------------------------------

/*!
 * @fn SeqFileOut#writeRecord
 * @brief Write out a string to a sequence file.
 *
 * @signature void writeRecord(seqFile, meta, seq[, qual]);
 *
 * @param[in,out] seqFile The SeqFileIn to write to.
 * @param[out]    meta    A @link StringConcept string @endlink with the meta/id to write out.
 * @param[out]    seq     A @link StringConcept string @endlink with the sequence to write out.
 * @param[out]    qual    An optional @link StringConcept string @endlink 
 *
 * @throws IOError if something went wrong.
 *
 * Qualities are handled similar to @link SeqFileIn#readRecord @endlink.  Qualities are only written if
 * the format of <tt>seqFile</tt> supports it.  When given, the qualities are written out from <tt>qual</tt>.
 * Otherwise, they can be taken from <tt>seq</tt> if it is of concept @link AlphabetWithQualitiesConcept
 * @endlink.  By default, quality values with score 40 (ASCII representation <tt>I</tt>), are written.
 */

template <typename TSpec, typename TIdString, typename TSeqString>
inline SEQAN_FUNC_ENABLE_IF(Is<OutputStreamConcept<typename SmartFile<Fastq, Output, TSpec>::TStream> >, void)
writeRecord(SmartFile<Fastq, Output, TSpec> & file,
            TIdString const & meta,
            TSeqString const & seq)
{
    writeRecord(file.iter, meta, seq, file.format, context(file).options);
}

// ----------------------------------------------------------------------------
// Function writeRecord(); With qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TIdString, typename TSeqString, typename TQualString>
inline SEQAN_FUNC_ENABLE_IF(Is<OutputStreamConcept<typename SmartFile<Fastq, Output, TSpec>::TStream> >, void)
writeRecord(SmartFile<Fastq, Output, TSpec> & file,
            TIdString const & meta,
            TSeqString const & seq,
            TQualString const & qual)
{
    writeRecord(file.iter, meta, seq, qual, file.format, context(file).options);
}

// ----------------------------------------------------------------------------
// Function writeRecords(); Without qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TIdStringSet, typename TSeqStringSet>
inline void
writeRecords(SmartFile<Fastq, Output, TSpec> & file,
             TIdStringSet const & meta,
             TSeqStringSet const & seq)
{
    for (typename Size<TIdStringSet>::Type i = 0; i != length(seq); ++i)
        writeRecord(file, meta[i], seq[i]);
}

// ----------------------------------------------------------------------------
// Function writeRecords(); With qualities
// ----------------------------------------------------------------------------

template <typename TSpec, typename TIdStringSet, typename TSeqStringSet, typename TQualStringSet>
inline void
writeRecords(SmartFile<Fastq, Output, TSpec> & file,
             TIdStringSet const & meta,
             TSeqStringSet const & seq,
             TQualStringSet const & qual)
{
    for (typename Size<TIdStringSet>::Type i = 0; i != length(seq); ++i)
        writeRecord(file, meta[i], seq[i], qual[i]);
}

}  // namespace seqan

#endif // SEQAN_SEQ_IO_SEQUENCE_FILE_H_
