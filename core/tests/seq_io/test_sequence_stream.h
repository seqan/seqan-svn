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
// Author: Manuel Holtgrewe <manuel.holtgrewe@fu-berlin.de>
// ==========================================================================

#ifndef CORE_TESTS_SEQ_IO_TEST_EASY_SEQ_IO_H_
#define CORE_TESTS_SEQ_IO_TEST_EASY_SEQ_IO_H_

#include <seqan/basic.h>
#include <seqan/sequence.h>
#include <seqan/seq_io.h>

// ---------------------------------------------------------------------------
// Test recognition of supported file types.
// ---------------------------------------------------------------------------

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_recognize_file_type_gz_fasta)
{
#if SEQAN_HAS_ZLIB
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fa.gz");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    SEQAN_ASSERT(isEqual(format(seqIO), Fasta()));
#endif  // #if SEQAN_HAS_ZLIB
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_recognize_file_type_bz2_fasta)
{
#if SEQAN_HAS_BZIP2
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fa.bz2");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    SEQAN_ASSERT(isEqual(format(seqIO), Fasta()));
#endif  // #if SEQAN_HAS_BZIP2
}

// ---------------------------------------------------------------------------
// Test recognition of supported file formats.
// ---------------------------------------------------------------------------

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_recognize_file_format_text_fasta)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fa");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    SEQAN_ASSERT(isEqual(format(seqIO), Fasta()));
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_recognize_file_format_text_fastq)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fq");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    SEQAN_ASSERT(isEqual(format(seqIO), Fastq()));
}

// ---------------------------------------------------------------------------
// Test reading with different interfaces.
// ---------------------------------------------------------------------------

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_read_record_text_fasta)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fa");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::CharString id;
    seqan::Dna5String seq;

    readRecord(id, seq, seqIO);
    SEQAN_ASSERT_EQ(id, "seq1");
    SEQAN_ASSERT_EQ(seq, "CGATCGATAAT");

    readRecord(id, seq, seqIO);
    SEQAN_ASSERT_EQ(id, "seq2");
    SEQAN_ASSERT_EQ(seq, "CCTCTCTCTCCCT");

    readRecord(id, seq, seqIO);
    SEQAN_ASSERT_EQ(id, "seq3");
    SEQAN_ASSERT_EQ(seq, "CCCCCCCC");

    SEQAN_ASSERT(atEnd(seqIO));
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_read_all_text_fasta)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_PATH_TO_ROOT();
    append(filePath, "/core/tests/seq_io/test_dna.fa");

    // Create SequenceStream object.
    SeqFileIn seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    seqan::StringSet<seqan::Dna5String> seqs;

    readRecords(ids, seqs, seqIO);
    SEQAN_ASSERT_EQ(length(seqs), 3u);
    SEQAN_ASSERT_EQ(ids[0], "seq1");
    SEQAN_ASSERT_EQ(seqs[0], "CGATCGATAAT");
    SEQAN_ASSERT_EQ(ids[1], "seq2");
    SEQAN_ASSERT_EQ(seqs[1], "CCTCTCTCTCCCT");
    SEQAN_ASSERT_EQ(ids[2], "seq3");
    SEQAN_ASSERT_EQ(seqs[2], "CCCCCCCC");

    SEQAN_ASSERT(atEnd(seqIO));
}

// ---------------------------------------------------------------------------
// Test writing with different interfaces.
// ---------------------------------------------------------------------------

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_record_text_fasta)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fa");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");

    writeRecords(seqIO, ids, seqs);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fa");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_all_text_fasta)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fa");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");

    writeRecords(seqIO, ids, seqs);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fa");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_record_text_fastq_no_qual)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fq");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");

    writeRecords(seqIO, ids, seqs);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fq");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_record_text_fastq_with_qual)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fq");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");
    seqan::StringSet<seqan::CharString> quals;
    appendValue(quals, "IIIIIIIIIII");
    appendValue(quals, "IIIIIIIIIIIII");
    appendValue(quals, "IIIIIIII");

    writeRecords(seqIO, ids, seqs, quals);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fq");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_all_text_fastq_no_qual)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fq");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");

    writeRecords(seqIO, ids, seqs);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fq");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

SEQAN_DEFINE_TEST(test_seq_io_sequence_stream_write_all_text_fastq_with_qual)
{
    // Build path to file.
    seqan::CharString filePath = SEQAN_TEMP_FILENAME();
    append(filePath, ".fq");

    // Create SequenceStream object.
    SeqFileOut seqIO(toCString(filePath));

    // Check that the file type and format are set correctly.
    seqan::StringSet<seqan::CharString> ids;
    appendValue(ids, "seq1");
    appendValue(ids, "seq2");
    appendValue(ids, "seq3");
    seqan::StringSet<seqan::Dna5String> seqs;
    appendValue(seqs, "CGATCGATAAT");
    appendValue(seqs, "CCTCTCTCTCCCT");
    appendValue(seqs, "CCCCCCCC");
    seqan::StringSet<seqan::CharString> quals;
    appendValue(quals, "IIIIIIIIIII");
    appendValue(quals, "IIIIIIIIIIIII");
    appendValue(quals, "IIIIIIII");

    writeRecords(seqIO, ids, seqs, quals);

    close(seqIO);  // Make sure we can read this later.

    seqan::CharString pathToExpected = SEQAN_PATH_TO_ROOT();
    append(pathToExpected, "/core/tests/seq_io/test_dna.fq");
    SEQAN_ASSERT_MSG(seqan::_compareTextFiles(toCString(pathToExpected), toCString(filePath)), "Output should match example.");
}

#endif  // CORE_TESTS_SEQ_IO_TEST_EASY_SEQ_IO_H_
