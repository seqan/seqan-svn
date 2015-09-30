// ==========================================================================
//                                SeqAn-Flexbar
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
// Author: Benjamin Menkuec <benjamin@menkuec.de>
// ==========================================================================

#include "seqan_flexbar.h"

#define SEQAN_PROFILE
#ifdef SEQAN_ENABLE_DEBUG
#define DEBUG_MSG(str) do { std::cerr << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#include <iostream>
#include <seqan/basic.h>
#include <seqan/sequence.h>
#include <seqan/seq_io.h>
#include <seqan/arg_parse.h>

#ifdef _WIN32
#include <direct.h>
#endif

// Headers for creating subdirectories.
#include <errno.h>

#include "read_trimming.h"
#include "adapter_trimming.h"
#include "demultiplex.h"
#include "general_processing.h"

// Global variables are evil, this is for adaption and should be removed
// after refactorization.
FlexiProgram flexiProgram;

using namespace seqan;

// --------------------------------------------------------------------------
// Class ArgumentParserBuilder
// --------------------------------------------------------------------------

class ArgumentParserBuilder
{
public:
    virtual seqan::ArgumentParser build() = 0;

protected:
    void addGeneralOptions(seqan::ArgumentParser & parser);
    void addFilteringOptions(seqan::ArgumentParser & parser);
    void addDemultiplexingOptions(seqan::ArgumentParser & parser);
    void addAdapterTrimmingOptions(seqan::ArgumentParser & parser);
    void addReadTrimmingOptions(seqan::ArgumentParser & parser);
};

void ArgumentParserBuilder::addGeneralOptions(seqan::ArgumentParser & parser)
{
    // GENERAL OPTIONS -----------------------------------------
    addSection(parser, "General Options");

    seqan::ArgParseOption showSpeedOpt = seqan::ArgParseOption(
        "ss", "showSpeed", "Show speed in base pairs per second");
    addOption(parser, showSpeedOpt);

    seqan::ArgParseOption writeStatsOpt = seqan::ArgParseOption(
        "st", "writeStats", "Write statistics into a file");
    addOption(parser, writeStatsOpt);
    
    seqan::ArgParseOption recordOpt = seqan::ArgParseOption(
        "r", "records", "Number of records to be read in one run.",
        seqan::ArgParseOption::INTEGER, "VALUE");
    setDefaultValue(recordOpt, 10000);
    setMinValue(recordOpt, "1");
    addOption(parser, recordOpt);

    seqan::ArgParseOption noQualOpt = seqan::ArgParseOption(
        "nq", "noQualities", "Force .fa format for output files.");
        addOption(parser, noQualOpt);

	seqan::ArgParseOption firstReadsOpt = seqan::ArgParseOption(
			"fr", "reads", "Process only first n reads.",
			seqan::ArgParseOption::INTEGER, "VALUE");
	setMinValue(firstReadsOpt, "1");
	addOption(parser, firstReadsOpt);

    seqan::ArgParseOption threadOpt = seqan::ArgParseOption(
        "tnum", "threads", "Number of threads used.",
        seqan::ArgParseOption::INTEGER, "THREADS");
    setDefaultValue(threadOpt, 1);
    setMinValue(threadOpt, "1");
    addOption(parser, threadOpt);

    if(flexiProgram == ADAPTER_REMOVAL || flexiProgram == FILTERING || flexiProgram == QUALITY_CONTROL)
    {
        seqan::ArgParseOption outputOpt = seqan::ArgParseOption(
            "o", "output", "Name of the output file.",
            seqan::ArgParseOption::OUTPUT_FILE, "OUTPUT");
        setValidValues(outputOpt, SeqFileOut::getFileExtensions());
        addOption(parser, outputOpt);
    }
    else
    {
        seqan::ArgParseOption outputOpt = seqan::ArgParseOption(
            "o", "output", "Prefix and file ending of output files (prefix$.fa - $: placeholder which will be determined by the program.).",
            seqan::ArgParseOption::OUTPUT_PREFIX, "OUTPUT");
        setValidValues(outputOpt, SeqFileOut::getFileExtensions());
        addOption(parser, outputOpt);
    }

    if (flexiProgram == ALL_STEPS)
    {
        seqan::ArgParseOption adTagOpt = seqan::ArgParseOption(
            "t", "tag", "Tags IDs of sequences which had adapters removed and/or were quality-trimmed.");
        addOption(parser, adTagOpt);
    }

    seqan::ArgParseOption adInfoOpt = seqan::ArgParseOption(
        "ni", "noInfo", "Don't print paramter overview to console.");
    addOption(parser, adInfoOpt);
}

void ArgumentParserBuilder::addFilteringOptions(seqan::ArgumentParser & parser)
{
    // Preprocessing and Filtering
    addSection(parser, "Preprocessing and Filtering");

    seqan::ArgParseOption leftTrimOpt = seqan::ArgParseOption(
        "tl", "trimLeft", "Number of Bases to be trimmed from the 5'end(s) before further processing.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setDefaultValue(leftTrimOpt, 0);
    setMinValue(leftTrimOpt, "0");
    addOption(parser, leftTrimOpt);

	seqan::ArgParseOption tagTrimmingOpt = seqan::ArgParseOption(
		"tt", "tagTrimming", "Write trimmed-out segments into id");
	addOption(parser, tagTrimmingOpt);

    seqan::ArgParseOption rigthTrimOpt = seqan::ArgParseOption(
        "tr", "trimRight", "Number of Bases to be trimmed from the 3'end(s) before further processing.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setDefaultValue(rigthTrimOpt, 0);
    setMinValue(rigthTrimOpt, "0");
    addOption(parser, rigthTrimOpt);

    seqan::ArgParseOption minLenOpt = seqan::ArgParseOption(
        "ml", "minLength", "Required minimal length of reads after all PREprocessing steps.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setDefaultValue(minLenOpt, 0);
    setMinValue(minLenOpt, "0");
    addOption(parser, minLenOpt);

    seqan::ArgParseOption uncalledOpt = seqan::ArgParseOption(
        "u", "uncalled", "Number of allowed uncalled bases per sequence.",
        seqan::ArgParseOption::INTEGER, "VALUE");
    setDefaultValue(uncalledOpt, 0);
    setMinValue(uncalledOpt, "0");
    addOption(parser, uncalledOpt);

    seqan::ArgParseOption substituteOption = seqan::ArgParseOption(
        "s", "substitute", "Substitue Dna-character for N's.",
        seqan::ArgParseOption::STRING, "BASE");
    setDefaultValue(substituteOption, "A");
    setValidValues(substituteOption, "A C G T");
    addOption(parser, substituteOption);

     seqan::ArgParseOption finMinLenOpt = seqan::ArgParseOption(
        "fm", "finalMinLength", "Deletes read (and mate)"
        " if on of them is shorter than the given value after the complete worflow.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setMinValue(finMinLenOpt, "1");
    addOption(parser, finMinLenOpt);

    seqan::ArgParseOption finLenOpt = seqan::ArgParseOption(
        "fl", "finalLength", "Trims reads to desired length after the complete workflow.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setDefaultValue(finLenOpt, 0);
    setMinValue(finLenOpt, "1");
    addOption(parser, finLenOpt);

    addTextSection(parser, "EXAMPLE");
    std::string appName;
    assign(appName, getAppName(parser));
    addText(parser, appName + " READS.fq -tr r -u 1 -o RESULT.fq");

}

void ArgumentParserBuilder::addDemultiplexingOptions(seqan::ArgumentParser & parser)
{
    // Barcode Demultiplexing
    addSection(parser, "Demultiplexing Options");
    
    seqan::ArgParseOption barcodeFileOpt = seqan::ArgParseOption(
        "b", "barcodes", "FastA file containing the used barcodes and their IDs. Necessary for demutiplexing.",
        seqan::ArgParseArgument::INPUT_FILE, "BARCODE_FILE");
    setValidValues(barcodeFileOpt, SeqFileIn::getFileExtensions());
    addOption(parser, barcodeFileOpt);

    seqan::ArgParseOption multiplexFileOpt = seqan::ArgParseOption(
        "x", "multiplex", "FastA/FastQ file containing the barcode for each read.",
        seqan::ArgParseArgument::INPUT_FILE, "MULTIPLEX_FILE");
    setValidValues(multiplexFileOpt, SeqFileIn::getFileExtensions());
    addOption(parser, multiplexFileOpt);
    
    addOption(parser, seqan::ArgParseOption(
        "app", "approximate", "Select approximate barcode demultiplexing, allowing one mismatch."));

    addOption(parser, seqan::ArgParseOption(
        "hc", "hardClip", "Select hardClip option, clipping the first length(barcode) bases in any case."));

    addOption(parser, seqan::ArgParseOption(
        "ex", "exclude", "Exclude unidentified reads from further processing."));
    
    addTextSection(parser, "EXAMPLE");
    std::string appName;
    assign(appName, getAppName(parser));
    addText(parser, appName + " READS.fq -b BARCODES.fa -o RESULT.fq");
}

void ArgumentParserBuilder::addAdapterTrimmingOptions(seqan::ArgumentParser & parser)
{
    // ADAPTER TRIMMING
    addSection(parser, "Adapter removal options");
    seqan::ArgParseOption adapterFileOpt = seqan::ArgParseOption(
        "a", "adapters", "FastA file containing the two adapter sequences. "
        "The adapters according to the layout: 5'-adapter1-read-adapter2-3'.",
        seqan::ArgParseArgument::INPUT_FILE, "ADAPTER_FILE");
    setValidValues(adapterFileOpt, SeqFileIn::getFileExtensions());
    addOption(parser, adapterFileOpt);

    seqan::ArgParseOption noAdapterOpt = seqan::ArgParseOption(
        "pa", "paired-adapterTrimming", "Trim adapters from paired-end reads without using reference adapters.");
    addOption(parser, noAdapterOpt);

    seqan::ArgParseOption errorOpt = seqan::ArgParseOption(
        "e", "errors", "Allowed errors in adapter detection.",
        seqan::ArgParseOption::INTEGER, "VALUE");
    setDefaultValue(errorOpt, 0);
    addOption(parser, errorOpt);

	seqan::ArgParseOption rateOpt = seqan::ArgParseOption(
		"er", "error rate", "Allowed errors per overlap in adapter detection.",
		seqan::ArgParseOption::DOUBLE, "VALUE");
	setDefaultValue(rateOpt, 0);
	addOption(parser, rateOpt);

    seqan::ArgParseOption overlapOpt = seqan::ArgParseOption(
        "ol", "overlap", "Minimum length of overlap for a significant adapter match.",
        seqan::ArgParseOption::INTEGER, "VALUE");
    setDefaultValue(overlapOpt, 0);
    addOption(parser, overlapOpt);

    if (flexiProgram != ALL_STEPS)
    {
        seqan::ArgParseOption adTagOpt = seqan::ArgParseOption(
            "t", "tag", "Tags IDs of sequences which had adapters removed.");
        addOption(parser, adTagOpt);
    }

    addTextSection(parser, "EXAMPLE");
    std::string appName;
    assign(appName, getAppName(parser));
    addText(parser, appName + " READS.fq -a ADAPTER.fa -o RESULT.fq");

}

void ArgumentParserBuilder::addReadTrimmingOptions(seqan::ArgumentParser & parser)
{
    // READ TRIMMING
    addSection(parser, "Quality trimming options");
    seqan::ArgParseOption qualOpt = seqan::ArgParseOption(
        "q", "quality", "Quality threshold for read trimming.",
        seqan::ArgParseArgument::INTEGER, "PHRED");
    setMinValue(qualOpt, "0");
    setMaxValue(qualOpt, "40");
    addOption(parser, qualOpt);

    seqan::ArgParseOption lenOpt = seqan::ArgParseOption(
        "l", "length", 
        "Minimum read length after trimming. " 
        "Shorter reads will be substituted by a single N or removed if the paired read is too short as well.",
        seqan::ArgParseArgument::INTEGER, "LENGTH");
    setDefaultValue(lenOpt, 1);
    setMinValue(lenOpt, "1");
    addOption(parser, lenOpt);

    seqan::ArgParseOption trimOpt = seqan::ArgParseOption(
        "m", "method", "Method for trimming reads.",
        seqan::ArgParseArgument::STRING, "METHOD");
    setDefaultValue(trimOpt, "WIN");
    setValidValues(trimOpt, "WIN BWA TAIL");
    addOption(parser, trimOpt);

    if (flexiProgram != ALL_STEPS)
    {
        seqan::ArgParseOption adTagOpt = seqan::ArgParseOption(
            "t", "tag", "Tags IDs of sequences which were quality-trimmed.");
        addOption(parser, adTagOpt);
    }

    addTextSection(parser, "EXAMPLE");
    std::string appName;
    assign(appName, getAppName(parser));
    addText(parser, appName + " READS.fq -q 20 -l 80 BARCODES -app -o RESULT.fq");
}

// --------------------------------------------------------------------------
// Class FilteringParserBuilder
// --------------------------------------------------------------------------

class FilteringParserBuilder : public ArgumentParserBuilder
{
public:
    seqan::ArgumentParser build();

private:
    void addHeader(seqan::ArgumentParser & parser);
};

void FilteringParserBuilder::addHeader(seqan::ArgumentParser & parser)
{
    setCategory(parser, "NGS Quality Control");
    setShortDescription(parser, "The SeqAn Filtering Toolkit of seqan_flexbar.");
    addUsageLine(parser, " \\fI<READ_FILE1>\\fP \\fI<[READ_FILE2]>\\fP \\fI[OPTIONS]\\fP");
    addDescription(parser,
       "This program is a sub-routine of Flexbar++ (a new implementation and extension of"
       " the original flexbar[1]) and can be used to filter reads and apply "
       "sequence independent trimming options");

    addDescription(parser, "[1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, "
            "C.  FLEXBAR—Flexible Barcode and Adapter Processing for "
            "Next-Generation Sequencing Platforms. Biology 2012, 1, 895-905.");

    seqan::setVersion(parser, SEQAN_APP_VERSION " [" SEQAN_REVISION "]");
    setDate(parser, SEQAN_DATE);

    seqan::ArgParseArgument fileArg(seqan::ArgParseArgument::INPUT_FILE, "READS", true);
    setValidValues(fileArg, SeqFileIn::getFileExtensions());
    addArgument(parser, fileArg);
    setHelpText(parser, 0, "Either one (single-end) or two (paired-end) read files.");
}

seqan::ArgumentParser FilteringParserBuilder::build()
{
    seqan::ArgumentParser parser("sflexFilter");

    addHeader(parser);
    addGeneralOptions(parser);
    addFilteringOptions(parser);

    return parser;
}

// --------------------------------------------------------------------------
// Class AdapterRemovalParserBuilder
// --------------------------------------------------------------------------

class AdapterRemovalParserBuilder : public ArgumentParserBuilder
{
public:
    seqan::ArgumentParser build();

private:
    void addHeader(seqan::ArgumentParser & parser);
};

void AdapterRemovalParserBuilder::addHeader(seqan::ArgumentParser & parser)
{
    setCategory(parser, "NGS Quality Control");
    setShortDescription(parser, "The Adapter Removal Toolkit of Flexbar++.");
    addUsageLine(parser, " \\fI<READ_FILE1>\\fP \\fI<[READ_FILE2]>\\fP \\fI[OPTIONS]\\fP");
    addDescription(parser,
        "This program is a sub-routine of Flexbar++ (a new implementation and extension of"
        " the original flexbar[1]) and removes adapter sequences from reads.");

    addDescription(parser, "[1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, "
            "C.  FLEXBAR—Flexible Barcode and Adapter Processing for "
            "Next-Generation Sequencing Platforms. Biology 2012, 1, 895-905.");

    seqan::setVersion(parser, SEQAN_APP_VERSION " [" SEQAN_REVISION "]");
#ifdef SEQAN_DATE
    seqan::setDate(parser, SEQAN_DATE);
#endif

    seqan::ArgParseArgument fileArg(seqan::ArgParseArgument::INPUT_FILE, "READS", true);
    setValidValues(fileArg, SeqFileIn::getFileExtensions());
    addArgument(parser, fileArg);
    setHelpText(parser, 0, "Either one (single-end) or two (paired-end) read files.");
}

seqan::ArgumentParser AdapterRemovalParserBuilder::build()
{
    seqan::ArgumentParser parser("sflexAR");

    addHeader(parser);
    addGeneralOptions(parser);
    addAdapterTrimmingOptions(parser);

    return parser;
}

// --------------------------------------------------------------------------
// Class DemultiplexingParserBuilder
// --------------------------------------------------------------------------

class DemultiplexingParserBuilder : public ArgumentParserBuilder
{
public:
    seqan::ArgumentParser build();

private:
    void addHeader(seqan::ArgumentParser & parser);
};

void DemultiplexingParserBuilder::addHeader(seqan::ArgumentParser & parser)
{
    setCategory(parser, "NGS Quality Control");
    setShortDescription(parser, "The SeqAn Demultiplexing Toolkit of Flexbar++.");
    addUsageLine(parser, " \\fI<READ_FILE1>\\fP \\fI<[READ_FILE2]>\\fP \\fI[OPTIONS]\\fP");
    addDescription(parser,
        "This program is a sub-routine of Flexbar++ (a new implementation and extension of"
        " the original flexbar[1]) and can be used for demultiplexing of reads.");

    addDescription(parser, "[1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, "
            "C.  FLEXBAR—Flexible Barcode and Adapter Processing for "
            "Next-Generation Sequencing Platforms. Biology 2012, 1, 895-905.");


    seqan::setVersion(parser, SEQAN_APP_VERSION " [" SEQAN_REVISION "]");
    setDate(parser, SEQAN_DATE);

    seqan::ArgParseArgument fileArg(seqan::ArgParseArgument::INPUT_FILE, "READS", true);
    setValidValues(fileArg, SeqFileIn::getFileExtensions());
    addArgument(parser, fileArg);
    setHelpText(parser, 0, "Either one (single-end) or two (paired-end) read files.");
}

seqan::ArgumentParser DemultiplexingParserBuilder::build()
{
    seqan::ArgumentParser parser("sflexDMulti");

    addHeader(parser);
    addGeneralOptions(parser);
    addDemultiplexingOptions(parser);

    return parser;
}


// --------------------------------------------------------------------------
// Class QualityControlParserBuilder
// --------------------------------------------------------------------------

class QualityControlParserBuilder : public ArgumentParserBuilder
{
public:
    seqan::ArgumentParser build();

private:
    void addHeader(seqan::ArgumentParser & parser);
};

void QualityControlParserBuilder::addHeader(seqan::ArgumentParser & parser)
{
    setCategory(parser, "NGS Quality Control");
    setShortDescription(parser, "The SeqAn Quality Control Toolkit of Flexbar++.");
    addUsageLine(parser, " \\fI<READ_FILE1>\\fP \\fI<[READ_FILE2]>\\fP \\fI[OPTIONS]\\fP");
    addDescription(parser,
        "This program is a sub-routine of Flexbar++ (a new implementation and extension of"
        " the original flexbar [1]) and can be used for quality controlling of reads.");

    addDescription(parser, "[1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, "
            "C.  FLEXBAR—Flexible Barcode and Adapter Processing for "
            "Next-Generation Sequencing Platforms. Biology 2012, 1, 895-905.");

    seqan::setVersion(parser, SEQAN_APP_VERSION " [" SEQAN_REVISION "]");
    setDate(parser, SEQAN_DATE);

    seqan::ArgParseArgument fileArg(seqan::ArgParseArgument::INPUT_FILE, "READS", true);
    setValidValues(fileArg, SeqFileIn::getFileExtensions());
    addArgument(parser, fileArg);
    setHelpText(parser, 0, "Either one (single-end) or two (paired-end) read files.");
}

seqan::ArgumentParser QualityControlParserBuilder::build()
{
    seqan::ArgumentParser parser("sflexQC");

    addHeader(parser);
    addGeneralOptions(parser);
    addReadTrimmingOptions(parser);

    return parser;
}

// --------------------------------------------------------------------------
// Class AllStepsParserBuilder
// --------------------------------------------------------------------------

class AllStepsParserBuilder : public ArgumentParserBuilder
{
public:
    seqan::ArgumentParser build();

private:
    void addHeader(seqan::ArgumentParser & parser);
};

void AllStepsParserBuilder::addHeader(seqan::ArgumentParser & parser)
{
    setCategory(parser, "NGS Quality Control");
    setShortDescription(parser, "The Flexbar++ NGS-Data Processing Toolkit");
    addUsageLine(parser, " \\fI<READ_FILE1>\\fP \\fI<[READ_FILE2]>\\fP \\fI[OPTIONS]\\fP");
    addDescription(parser,
        "Flexbar++ is a toolkit for the processing of sequenced NGS reads. "
        "It is a reimplementation and extension of the original Flexbar implementation of Dodt [1]. It is "
        "possible to demultiplex the reads and order them according to "
        "different kind of barcodes, to remove adapter contamination from "
        "reads, to trim low quality bases, filter N's or trim whole reads. The "
        "different tools are controlled through command line parameters and can "
        "operate on both single- and paired-end read data.");

    addDescription(parser, "[1] Dodt, M.; Roehr, J.T.; Ahmed, R.; Dieterich, "
            "C.  FLEXBAR—Flexible Barcode and Adapter Processing for "
            "Next-Generation Sequencing Platforms. Biology 2012, 1, 895-905.");

    addDescription(parser, "(c) Copyright 2015 by Benjamin Menkuec.");

    seqan::setVersion(parser, SEQAN_APP_VERSION " [" SEQAN_REVISION "]");
    setDate(parser, SEQAN_DATE);

    seqan::ArgParseArgument fileArg(seqan::ArgParseArgument::INPUT_FILE, "READS", true);
    setValidValues(fileArg, SeqFileIn::getFileExtensions());
    addArgument(parser, fileArg);
    setHelpText(parser, 0, "Either one (single-end) or two (paired-end) read files.");
}

seqan::ArgumentParser AllStepsParserBuilder::build()
{
    seqan::ArgumentParser parser("seqan_flexbar");

    addHeader(parser);
    addGeneralOptions(parser);
    addFilteringOptions(parser);
    addDemultiplexingOptions(parser);
    addAdapterTrimmingOptions(parser);
    addReadTrimmingOptions(parser);

    return parser;
}

// --------------------------------------------------------------------------
// Function initParser()
// --------------------------------------------------------------------------

//Defining the the argument parser
seqan::ArgumentParser initParser()
{
    std::auto_ptr<ArgumentParserBuilder> argParseBuilder;

    switch (flexiProgram)
    {
        case ADAPTER_REMOVAL:
            argParseBuilder.reset(new AdapterRemovalParserBuilder());
            break;
        case DEMULTIPLEXING:
            argParseBuilder.reset(new DemultiplexingParserBuilder());
            break;
        case FILTERING:
            argParseBuilder.reset(new FilteringParserBuilder());
            break;
        case QUALITY_CONTROL:
            argParseBuilder.reset(new QualityControlParserBuilder());
            break;
        case ALL_STEPS:
            argParseBuilder.reset(new AllStepsParserBuilder());
            break;
        default:
            SEQAN_FAIL("Invalid program type.");
    }

    return argParseBuilder->build();
}

// --------------------------------------------------------------------------
// Class ProcessingParams
// --------------------------------------------------------------------------

struct ProcessingParams
{
    seqan::Dna substitute;
    unsigned uncalled;
    unsigned trimLeft;
    unsigned trimRight;
    unsigned minLen;
    unsigned finalMinLength;
    unsigned finalLength;
    bool tagTrimming;
    bool runPre;
    bool runPost;
    bool runSubstitute;
    bool runCheckUncalled;

    ProcessingParams() :
        substitute('A'), 
        uncalled(0),
        trimLeft(0),
        trimRight(0),
        minLen(0),
        finalMinLength(0),
        finalLength(0),
        tagTrimming(false),
        runPre(false),
        runPost(false), 
        runSubstitute(false),
        runCheckUncalled(false) {};
};

enum TrimmingMode
{
    E_WINDOW,
    E_BWA,
    E_TAIL
};

enum MatchMode
{
    E_AUTO,
    E_USER
};

struct AdapterTrimmingParams
{
    bool pairedNoAdapterFile;
    bool run;
    std::array<TAdapterSet, 2> adapters;
    AdapterMatchSettings mode;
    MatchMode mmode;
    bool tag;
    AdapterTrimmingParams() : pairedNoAdapterFile(false), run(false), mmode(E_AUTO), tag(false) {};
};

struct QualityTrimmingParams
{
    TrimmingMode trim_mode;
    int cutoff;
    int min_length;
    bool run;
    bool tag;
       QualityTrimmingParams() : trim_mode(E_WINDOW), cutoff(-1), min_length(1), run(false), tag(false) {};
};

struct InputFileStreams
{
    seqan::SeqFileIn fileStream1, fileStream2, fileStreamMultiplex;
};

struct ProgramParams
{
    unsigned int fileCount;
    bool showSpeed;
    unsigned int firstReads;
    unsigned records;
    unsigned int num_threads;

    ProgramParams() : fileCount(0), showSpeed(false), firstReads(0), records(0), num_threads(0) {};
};

//TODO(singer): THIS NEEDS TO BE REDONE/DELETED
class OutputStreams
{
    typedef seqan::SeqFileOut * PSeqStream;
    typedef seqan::Pair<PSeqStream, PSeqStream> TStreamPair;
    std::map<int, TStreamPair> pairedFileStreams;
    std::map<int, PSeqStream> fileStreams;
    const seqan::CharString basePath;
    seqan::CharString extension;

public:
    // Constructor saves a base directory path used for all outputs.
    // The correct file extension is determined from the base path, according to the available
    // file extensions of the SeqFileOut and used for all stored files.
    OutputStreams(seqan::CharString const & base, bool /*noQuality*/) : basePath(base)
    {
        std::vector<std::string> tmpExtensions = seqan::SeqFileOut::getFileExtensions();
        for (unsigned i = 0; i < length(tmpExtensions); ++i)
        {
            if (endsWith(basePath, tmpExtensions[i]))
            {
                extension = tmpExtensions[i];
                break;
            }
        }
    }

     //Checks whether a key exists in a map. 
    template <typename TKey, typename TMap>
    bool exists(TKey& key, TMap& map)
    {
        return map.find(key) == map.end();
    }

    seqan::CharString getBaseFilename(void)
    {
        return prefix(basePath, length(basePath) - length(extension));
    }

    //Adds a new output streams to the collection of streams.
    void addStream(seqan::CharString fileName, int id, bool useDefault)
    {
        //Prepend basePath and append file extension to the filename.
        seqan::CharString path = getBaseFilename();
        if (fileName != "")
            seqan::append(path, "_");
        if (useDefault)
            append(path, "_result");

        seqan::append(path, fileName);
        seqan::append(path, extension);
        char* file = seqan::toCString(path);
        PSeqStream stream = new seqan::SeqFileOut(file);
        fileStreams[id] = stream;
    }
    
    void addStream(seqan::CharString fileName, int id)
    {
         addStream(fileName, id, false);
    }
     //Adds two new output streams to the collection of streams.
    void addStreams(seqan::CharString fileName1, seqan::CharString fileName2, int id, bool useDefault)
    {
        //Prepend basePath and append file extension to the filename.
        seqan::CharString path1 = prefix(basePath, length(basePath) - length(extension));
        seqan::CharString path2 = prefix(basePath, length(basePath) - length(extension));
        if (fileName1 != "")
            seqan::append(path1, "_");
        if (fileName2 != "")
            seqan::append(path2, "_");

        if (useDefault)
        {
            append(path1, "_result");
            append(path2, "_result");
        }

        seqan::append(path1, fileName1);
        seqan::append(path1, extension);
        seqan::append(path2, fileName2);
        seqan::append(path2, extension);
        char* file1 = seqan::toCString(path1);
        char* file2 = seqan::toCString(path2);
        PSeqStream stream1 = new seqan::SeqFileOut(file1);
        PSeqStream stream2 = new seqan::SeqFileOut(file2);
        pairedFileStreams[id] = TStreamPair(stream1, stream2);
    }

    void addStreams(seqan::CharString fileName1, seqan::CharString fileName2, int id)
    {
        addStreams(fileName1, fileName2, id, false);
    }

    //This method takes a String of integers and checks if these integers are
    //already associated with a stream. If not, a new stream is added and the opened
    //file is named according to the list of names. One or two files are created.
    template <typename TNames>
    void updateStreams(TNames& names, bool pair)
    {

        for (unsigned i = 0; i < length(names) + 1; ++i)
        {
            unsigned streamIndex = i;
            bool missing = pair ? exists(streamIndex, pairedFileStreams) : exists(streamIndex, fileStreams);
            // If no stream for this id exists, create one.
            if (missing)
            {
                // If the index is 0 (unidentified) create special stream.
                // Otherwise use index to get appropriate name for output file.
                seqan::CharString file;
                if (streamIndex > 0)
                {
                    file = names[streamIndex - 1];
                }
                else
                {
                    file = seqan::CharString("unidentified");
                }
                // Add file extension to stream and create it.
                if (pair)
                {
                    // Create a new subfolder at basePath/[barcodeID, unidentified].
                    seqan::CharString folderPath(basePath);
                    seqan::append(folderPath, file);
                    /*
                    int result = 0;
                    #ifdef __linux__
                    result = mkdir(seqan::toCString(folderPath), 0777);
                    #elif _WIN32
                    result = _mkdir(seqan::toCString(folderPath));
                    #endif
                    if (result==-1)
                    {
                    if (errno == EEXIST)
                    {
                    std::cerr << "Warning: Directory " << folderPath << " already exists.\n";
                    }
                    }
                    */
                    // Turn file target from [barcodeID,unidentified]
                    // to subfolder [barcodeID, unidentified]/[barcodeID, unidentified]
                    seqan::CharString filePath(file);
                    seqan::append(file, "/");
                    seqan::append(file, filePath);
                    // To differentiate between the paired reads, add index to the file name.
                    seqan::CharString file2 = file;
                    seqan::append(file, seqan::CharString("_1"));
                    seqan::append(file2, seqan::CharString("_2"));
                    this->addStreams(file, file2, streamIndex);
                }
                else
                {
                    //std::cerr << file << " " << streamIndex << std::endl;
                    this->addStream(file, streamIndex);
                }
            }
        }
    }



    template <typename TMap, typename TNames>
    void updateStreams(TMap& map, TNames& names, bool pair)
    {

        for (unsigned i=0; i < length(map); ++i)
        {
            unsigned streamIndex = map[i];
            bool missing = pair ? exists(streamIndex, pairedFileStreams) : exists(streamIndex, fileStreams);
            // If no stream for this id exists, create one.
            if (missing)
            {
                // If the index is 0 (unidentified) create special stream.
                // Otherwise use index to get appropriate name for output file.
                seqan::CharString file;
                if (streamIndex > 0)
                {
                    file = names[streamIndex-1];
                }
                else
                {
                    file = seqan::CharString("unidentified");
                }
                // Add file extension to stream and create it.
                if (pair)
                {
                    // Create a new subfolder at basePath/[barcodeID, unidentified].
                    seqan::CharString folderPath(basePath);
                    seqan::append(folderPath, file);
                    /*
                    int result = 0;
                    #ifdef __linux__
                        result = mkdir(seqan::toCString(folderPath), 0777);
                    #elif _WIN32
                        result = _mkdir(seqan::toCString(folderPath));
                    #endif
                        if (result==-1)
                    {
                        if (errno == EEXIST)
                        {
                            std::cerr << "Warning: Directory " << folderPath << " already exists.\n";
                        }
                    }
                    */
                    // Turn file target from [barcodeID,unidentified]
                    // to subfolder [barcodeID, unidentified]/[barcodeID, unidentified]
                    seqan::CharString filePath(file);
                    seqan::append(file, "/");
                    seqan::append(file, filePath);
                    // To differentiate between the paired reads, add index to the file name.
                    seqan::CharString file2 = file;
                    seqan::append(file, seqan::CharString("_1"));
                    seqan::append(file2, seqan::CharString("_2"));
                    this->addStreams(file, file2, streamIndex);
                }
                else
                {
                    //std::cerr << file << " " << streamIndex << std::endl;
                    this->addStream(file, streamIndex);
                }
            }
        }
    }
    //Writes the sets of ids and sequences to their corresponding files. Used for single-end data.
    template <template <typename> class TRead, typename TSeq, typename TNames, 
        typename = std::enable_if_t<std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplex<TSeq>>::value >>
    void writeSeqs(std::vector<TRead<TSeq>>&& reads, TNames& names, bool = false)
    {
        updateStreams(names, false);
        for (unsigned i = 0; i < length(reads); ++i)
        {
            unsigned streamIndex = reads[i].demuxResult;
            writeRecord(*fileStreams[streamIndex], std::move(reads[i].id), std::move(reads[i].seq));
        }
    }

    template <template <typename> class TRead, typename TSeq, typename TNames,
        typename = std::enable_if_t<std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplexPairedEnd<TSeq>>::value >>
        void writeSeqs(std::vector<TRead<TSeq>>&& reads, TNames& names)
    {
        updateStreams(names, false);
        for (unsigned i = 0; i < length(reads); ++i)
        {
            unsigned streamIndex = reads[i].demuxResult;
            writeRecord(*pairedFileStreams[streamIndex].i1, std::move(reads[i].id), std::move(reads[i].seq));
            writeRecord(*pairedFileStreams[streamIndex].i2, std::move(reads[i].idRev), std::move(reads[i].seqRev));
        }
    }

    //Destructor of the object holding the output streams. Needed
     //to destroy the streams properly after they have been created with new.
    ~OutputStreams()
    {
        // Delete all created file streams.
        for (unsigned i=0; i < length(fileStreams); ++i)
        {
            delete fileStreams[i];
        }
        for (unsigned i=0; i < length(pairedFileStreams); ++i)
        {
            TStreamPair tmp = pairedFileStreams[i];
            delete tmp.i1;
            delete tmp.i2;
        }
    }
};

// ============================================================================
// Functions
// ============================================================================

int loadBarcodes(char const * path, DemultiplexingParams& params)
{
    seqan::SeqFileIn bcFile;

    if (!open(bcFile, path, OPEN_RDONLY))
    {
        std::cerr << "Error while opening file'" << params.barcodeFile << "'.\n";
        return 1;
    }

    while (!atEnd(bcFile))
    {
        std::string id;
        std::string barcode;
        readRecord(id, barcode, bcFile);
        params.barcodeIds.emplace_back(id);
        params.barcodes.emplace_back(barcode);
    }

    if (params.approximate)                            //modifies the barcodes for approximate matching
    {
        buildAllVariations(params.barcodes);
    }
    return 0;
}

template<template <typename> class TRead, typename TSeq, typename = std::enable_if_t < std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value>>
inline void loadMultiplex(std::vector<TRead<TSeq>>& reads, unsigned records, seqan::SeqFileIn& multiplexFile)
{
    (void)reads;
    (void)multiplexFile;
    (void)records;
}

template<template <typename> class TRead, typename TSeq, typename = std::enable_if_t < std::is_same<TRead<TSeq>, ReadMultiplex<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplexPairedEnd<TSeq>>::value>>
inline void loadMultiplex(std::vector<TRead<TSeq>>& reads, unsigned records, seqan::SeqFileIn& multiplexFile, bool = false)
{
    seqan::String<char> id;
    unsigned int i = 0;
    while (i < records && !atEnd(multiplexFile))
    {
        readRecord(id, reads[i].demultiplex, multiplexFile);
        ++i;
    }
}

int openStream(seqan::CharString const & file, seqan::SeqFileIn & inFile)
{
    if (!open(inFile, seqan::toCString(file)))
    {
        std::cerr << "Error while opening input file '" << file << "'.\n";
        return 1;
    }
    return 0;
}

int loadDemultiplexingParams(seqan::ArgumentParser const& parser, DemultiplexingParams& params)
{
    // APPROXIMATE/EXACT MATCHING---------------------    
    params.approximate = seqan::isSet(parser, "app");
    // HARD CLIP MODE --------------------------------
    params.hardClip = seqan::isSet(parser, "hc") && !(isSet(parser, "ex"));
    // EXCLUDE UNIDENTIFIED --------------------------
    params.exclude = isSet(parser, "ex") && isSet(parser, "b");
    // RUN -------------------------------------------
    params.run = isSet(parser, "b");
    if (isSet(parser, "x"))
        params.runx = true;
    else
        params.runx = false;
    // BARCODES --------------------------------------
    if (isSet(parser, "b"))
    {
        getOptionValue(params.barcodeFile, parser, "b");
        if (loadBarcodes(toCString(params.barcodeFile), params) != 0)
            return 1;
    }
    return 0;
}

int loadAdapterTrimmingParams(seqan::ArgumentParser const& parser, AdapterTrimmingParams & params)
{
    // PAIRED-END ------------------------------
    int fileCount =  getArgumentValueCount(parser, 0);
    // Only consider paired-end mode if two files are given and user wants paired mode.
    params.pairedNoAdapterFile = fileCount == 2 && isSet(parser, "pa");
    // Set run flag, depending on essential parameters.
    params.run = isSet(parser,"a") || (isSet(parser, "pa") && fileCount == 2);
    // ADAPTER SEQUENCES ----------------------------
    seqan::CharString adapterFile, id;
    // If adapter sequences are given, we read them in any case.
    if (isSet(parser, "a"))
    {
        getOptionValue(adapterFile, parser, "a");
        seqan::SeqFileIn adapterInFile;
        if (!open(adapterInFile, toCString(adapterFile), OPEN_RDONLY))
        {
            std::cerr << "Error while opening file'" << adapterFile << "'.\n";
            return 1;
        }
        TAdapterSequence tempAdapter;
        while (!atEnd(adapterInFile))
        {
            readRecord(id, tempAdapter, adapterInFile);
            appendValue(params.adapters[0], tempAdapter);
            readRecord(id, tempAdapter, adapterInFile);
            appendValue(params.adapters[1], tempAdapter);
        }
    }
    // If they are not given, but we would need them (single-end trimming), output error.
    else if ((isSet(parser, "pa") && fileCount == 1))
    {
        std::cerr << "Unpaired adapter removal requires paired reads.\n";
        return 1;
    }
    else if (isSet(parser, "pa"))
    {
        std::cerr << "You can not specify adapters for paired adapter removal.\n";
        return 1;
    }
    // TRIMMING MODE ----------------------------
    // User must fully specify mode, if he wants to. (But not specifying both is ok too.)
    if ((seqan::isSet(parser, "er") || seqan::isSet(parser, "e")) != seqan::isSet(parser, "overlap"))
    {
        std::cerr << "User must define both error (-e) or error rate (-er) and minimum overlap (-o).\n";
        return 1;
    }
    // Read both options (if one is given, the above check guarantees that the other is too.)
	if (seqan::isSet(parser, "overlap")) {
		// If user tried to specify alignment parameters, but didn't activate adapter trimming, warn and exit.
		if (!params.run)
		{
			std::cerr << "Adapter removal alignment parameters require adapters or --no-adapter flag.\n";
			return 1;
		}
		int o;
		int e;
		double er;
		getOptionValue(o, parser, "overlap");
		getOptionValue(e, parser, "e");
		getOptionValue(er, parser, "er");
		params.mode = AdapterMatchSettings(o, e, er);
        params.mmode = E_USER;
    }
    // Otherwise use the automatic configuration.
    else
    {
        params.mode = AdapterMatchSettings();
        params.mmode = E_AUTO;
    }
    return 0;
}

int loadQualityTrimmingParams(seqan::ArgumentParser const & parser, QualityTrimmingParams & params)
{
    // TRIMMING METHOD ----------------------------
    std::string method;
    getOptionValue(method, parser, "m");
    if (method == "WIN")
    {
        params.trim_mode = E_WINDOW;
    }
    else if (method == "BWA")
    {
        params.trim_mode = E_BWA;
    }
    else
    {
        params.trim_mode = E_TAIL;
    }
    // QUALITY CUTOFF ----------------------------
    if (isSet(parser, "q"))
    {
        getOptionValue(params.cutoff, parser, "q");
    }
    // MINIMUM SEQUENCE LENGTH -------------------
    getOptionValue(params.min_length, parser, "l");
    // Set run flag, depending on essential parameters (which are in a valid state at this point).
    params.run = isSet(parser, "q");
    return 0;
}

int loadProgramParams(seqan::ArgumentParser const & parser, ProgramParams& params, InputFileStreams& vars)
{
    params.fileCount = getArgumentValueCount(parser, 0);
    // Load files.
    seqan::CharString fileName1, fileName2;
    getArgumentValue(fileName1, parser, 0, 0);
    if (openStream(fileName1, vars.fileStream1) != 0)
    {
        return 1;
    }
    if (params.fileCount == 2)
    {
        getArgumentValue(fileName2, parser, 0, 1);
        if (openStream(fileName2, vars.fileStream2) != 0)
        {
            return 1;
        }
        if (value(format(vars.fileStream1)) != value(format(vars.fileStream2)))
        {
            std::cerr << "Input files must have the same file format.\n";
            return 1;
        }
    }
    params.showSpeed = isSet(parser, "ss");

    params.firstReads = std::numeric_limits<unsigned>::max();
    if (seqan::isSet(parser, "fr"))
        getOptionValue(params.firstReads, parser, "fr");

    params.num_threads = 1;
    getOptionValue(params.num_threads, parser, "tnum");
    omp_set_num_threads(params.num_threads);

    getOptionValue(params.records, parser, "r");
    return 0;
}

int checkParams(ProgramParams const & programParams, InputFileStreams const& inputFileStreams, ProcessingParams const & processingParams,
    DemultiplexingParams const & demultiplexingParams, AdapterTrimmingParams const & adapterTrimmingParams,
    QualityTrimmingParams & qualityTrimmingParams)
{
    // Were there options that activated at least one processing stage?

    if (!(adapterTrimmingParams.run || qualityTrimmingParams.run || demultiplexingParams.run || processingParams.runPre
        || processingParams.runPost))
    {
        std::cerr << "\nNo processing stage was specified.\n";
        return 1;
    }
    // If quality trimming was selected, check if file format includes qualities.
    if (qualityTrimmingParams.run)
    {
        if ((value(format(inputFileStreams.fileStream1)) != Find<FileFormat<seqan::SeqFileIn>::Type, Fastq>::VALUE) ||
            ((programParams.fileCount == 2) &&
            (value(format(inputFileStreams.fileStream2)) != Find<FileFormat<seqan::SeqFileIn>::Type, Fastq>::VALUE)))
        {
            std::cerr << "\nQuality trimming requires quality information, please specify fq files." << std::endl;
            return 1;
        }
    }
    // If we don't demultiplex (and therefore take file names from the barcode IDs), set file names.
    if (!demultiplexingParams.run)
    {
        //std::cout << basename(toCString(fileName1)) << "\n\n";
    }
    return 0;
}

// PROGRAM STAGES ---------------------
//Preprocessing Stage
template<typename TReadSet, typename TStats>
void preprocessingStage(const ProcessingParams& processingParams, TReadSet& readSet, TStats& generalStats)
{
    if (processingParams.runPre)
    {
        //Trimming and filtering
        if (processingParams.trimLeft + processingParams.trimRight + processingParams.minLen != 0)
            preTrim(readSet, processingParams.trimLeft, processingParams.trimRight,
                processingParams.minLen, processingParams.tagTrimming, generalStats);
       // Detecting uncalled Bases
        if (processingParams.runCheckUncalled)
        {
            if (processingParams.runSubstitute)
                processN(readSet, processingParams.uncalled, processingParams.substitute, generalStats);
            else
                processN(readSet, processingParams.uncalled, NoSubstitute(), generalStats);
        }
    }
}

// DEMULTIPLEXING
template <typename TRead, typename TFinder, typename TStats>
int demultiplexingStage(const DemultiplexingParams& params, std::vector<TRead>& reads, TFinder& esaFinder,
    TStats& generalStats)
{
    if (!params.run)
        return 0;
    if (!params.approximate)
    {
        demultiplex(reads, esaFinder, params.hardClip, generalStats, ExactBarcodeMatching(), params.exclude);
    }
    else
    {
        if (!check(reads, params.barcodes, generalStats))            // On Errors with barcodes return 1;
            return 1;
        demultiplex(reads, esaFinder, params.hardClip, generalStats, ApproximateBarcodeMatching(), params.exclude);
    }
    return 0;
}

// ADAPTER TRIMMING
template <typename TRead, typename TStats>
void adapterTrimmingStage(const AdapterTrimmingParams& params, std::vector<TRead>& reads, TStats& stats)
{
    if (!params.run)
        return;
    if(params.tag)
        stripAdapterBatch(reads, params.adapters, params.mode, params.pairedNoAdapterFile, stats.adapterTrimmingStats, TagAdapter<true>());
    else
        stripAdapterBatch(reads, params.adapters, params.mode, params.pairedNoAdapterFile, stats.adapterTrimmingStats, TagAdapter<false>());
}

// QUALITY TRIMMING
//Version for single-ende data
template <typename TRead, typename TStats>
void qualityTrimmingStage(const QualityTrimmingParams& params, std::vector<TRead>& reads, TStats& stats)
{
    if (params.run)
    {
        switch (params.trim_mode)
        {
        case E_WINDOW:
        {
            trimBatch(reads, params.cutoff, Mean(5), params.tag);
            break;
        }
        case E_BWA:
        {
            trimBatch(reads, params.cutoff, BWA(), params.tag);
        }
        case E_TAIL:
        {
            trimBatch(reads, params.cutoff, Tail(), params.tag);
        }
        }
    }
    stats.removedQuality += removeShortSeqs(reads, params.min_length);
}

//Postprocessing
template<typename TRead, typename TStats>
void postprocessingStage(const ProcessingParams& params, std::vector<TRead>& reads, TStats& stats)
{
    if (params.runPost)
    {
        if ((params.finalMinLength != 0) && (params.finalLength == 0))
            stats.removedShort += removeShortSeqs(reads, params.finalMinLength);
        else if (params.finalLength != 0)
            trimTo(reads, params.finalLength, stats);
    }
}

// END PROGRAM STAGES ---------------------
template <typename TOutStream, typename TStats>
void printStatistics(const ProgramParams& programParams, const TStats& generalStats, DemultiplexingParams& demultiplexParams,
                const AdapterTrimmingParams& adapterParams, const bool timing, TOutStream &outStream)
{
    bool paired = programParams.fileCount == 2;
    bool adapter = adapterParams.run;
    outStream << std::endl;
    outStream << "\r\rRead statistics\n";
    outStream << "===============\n";
    outStream << "Reads processed:\t" << generalStats.readCount;
    if (paired) 
    {
        outStream << " (2 * " << generalStats.readCount << " single reads)";
    }
    outStream << std::endl;
    double dropped = generalStats.removedQuality + generalStats.removedN
        + generalStats.removedShort + (demultiplexParams.exclude * generalStats.removedDemultiplex);
    outStream << "  Reads dropped:\t" << dropped << "\t(" << std::setprecision(3) 
        << dropped / double(generalStats.readCount) * 100 << "%)\n";
    if (dropped != 0.0)
    {
        if (generalStats.removedN != 0)
        {
            outStream << "    Due to N content:\t" << generalStats.removedN << "\t(" << std::setprecision(3)
                << double(generalStats.removedN) / dropped * 100 << "%)\n";
        }
        if (generalStats.removedQuality != 0)
        {
            outStream << "    Due to quality:\t" << generalStats.removedQuality << "\t("
                << std::setprecision(3) << double(generalStats.removedQuality) / dropped * 100 << "%)\n";
        }
        if (generalStats.removedShort != 0)
        {
            outStream << "    Due to shortness:\t" << generalStats.removedShort << "\t("
                << std::setprecision(3) << double(generalStats.removedShort) / dropped * 100 << "%)\n";
        }
    }
    if (generalStats.uncalledBases != 0)
        {
            outStream << "  Remaining uncalled (or masked) bases: " << generalStats.uncalledBases << "\n";
        }
    //Statistics for Demultiplexing
    if (demultiplexParams.run)    
    {
        outStream << "\nBarcode Demultiplexing statistics\n";
        outStream << "=================================\n";

        unsigned barcodesTotal = length(demultiplexParams.barcodes);
        if (demultiplexParams.approximate)
        {
            barcodesTotal = barcodesTotal/(length(demultiplexParams.barcodes[0])*5);
        }

        outStream << "Reads per barcode:\n";
        outStream << "Unidentified:\t" << generalStats.matchedBarcodeReads[0];
        if (generalStats.readCount != 0)
        {
            outStream  << "\t\t(" << std::setprecision(3) << (double)generalStats.matchedBarcodeReads[0] /
                ((double)generalStats.readCount) * 100 << "%)";
        }  
        outStream << "\n";
        for (unsigned i = 1; i <= barcodesTotal; ++i)
        {
            outStream << demultiplexParams.barcodeIds[i-1]<<":\t" << generalStats.matchedBarcodeReads[i];
            if (generalStats.readCount != 0)
            {
                outStream  << "\t\t(" << std::setprecision(3) << (double)generalStats.matchedBarcodeReads[i] /
                    ((double)generalStats.readCount) * 100 << "%)";
            }
            outStream << "\n";
        }
        outStream << std::endl;
    }
    outStream << "File statistics\n";
    outStream << "===============\n";
    // How many reads are left.
    int survived = generalStats.readCount - generalStats.removedN
        - generalStats.removedQuality - generalStats.removedShort
        - demultiplexParams.exclude * generalStats.removedDemultiplex;
    // In percentage points.
    double surv_proc = (double)survived / (double)generalStats.readCount * 100;
    outStream << "File 1:\n";
    outStream << "-------\n";
    outStream << "  Surviving: " << survived << "/" << generalStats.readCount
              << " (" << std::setprecision(3) << surv_proc << "%)\n";
    if (adapter)
    {
            outStream << "   Adapters: " << generalStats.adapterTrimmingStats.a2count << "\n";
    }
    outStream << std::endl;
    if (adapter && (generalStats.adapterTrimmingStats.a1count + generalStats.adapterTrimmingStats.a2count != 0))
    {
        int mean = generalStats.adapterTrimmingStats.overlapSum/(generalStats.adapterTrimmingStats.a1count + generalStats.adapterTrimmingStats.a2count);
        outStream << "Adapter sizes:\n";
        outStream << "Min: " << generalStats.adapterTrimmingStats.minOverlap << ", Mean: " << mean
                << ", Max: " << generalStats.adapterTrimmingStats.maxOverlap << "\n\n";
    }
    // Print processing and IO time. IO is (approx.) the whole loop without the processing part.
    if (timing)
    {
        outStream << "Time statistics:\n";
        outStream << "==================\n";
        outStream << "Processing time: " << std::setw(5) << generalStats.processTime << " seconds.\n";
        outStream << "       I/O time: " << std::setw(5) << generalStats.ioTime << " seconds.\n";
        outStream << std::endl;
    }
}

template < template<typename> class TRead, typename TSeq, typename = std::enable_if_t<std::is_same<TRead<TSeq>, Read<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplex<TSeq>>::value> >
unsigned int readReads(std::vector<TRead<TSeq>>& reads, const unsigned int records, InputFileStreams& inputFileStreams, bool = false)
{
    reads.resize(records);
    unsigned int i = 0;
    while (i < records && !atEnd(inputFileStreams.fileStream1))
    {
        readRecord(reads[i].id, reads[i].seq, inputFileStreams.fileStream1);
        ++i;
    }
    reads.resize(i);
    return i;
}

template < template<typename> class TRead, typename TSeq, typename = std::enable_if_t<std::is_same<TRead<TSeq>, ReadPairedEnd<TSeq>>::value || std::is_same<TRead<TSeq>, ReadMultiplexPairedEnd<TSeq>>::value> >
unsigned int readReads(std::vector<TRead<TSeq>>& reads, const unsigned int records, InputFileStreams& inputFileStreams)
{
    reads.resize(records);
    unsigned int i = 0;
    while (i < records && !atEnd(inputFileStreams.fileStream1))
    {
        readRecord(reads[i].id, reads[i].seq, inputFileStreams.fileStream1);
        readRecord(reads[i].idRev, reads[i].seqRev, inputFileStreams.fileStream2);
        ++i;
    }
    reads.resize(i);
    return i;
}

template<template<typename> class TRead, typename TSeq, typename TWriteItem>
struct ReadWriter
{
    ReadWriter(const ProgramParams& programParams, OutputStreams& outputStreams, unsigned int sleepMS)
        : _programParams(programParams), _outputStreams(outputStreams), _tlsReadSets(_programParams.num_threads + 1),
        _run(false), _sleepMS(sleepMS), _startTime(std::chrono::steady_clock::now()), _lastScreenUpdate()
    {}
    ~ReadWriter() 
    {
        _run = false;
        if (_thread.joinable())
            _thread.join();
    }
    void start()
    {
        _run = true;
        _thread = std::thread([this]()
        {
            while (_run)
            {
                bool nothingToDo = true;
                for (auto& readSet : _tlsReadSets)
                {
                    if (readSet.first.try_lock())
                    {
                        if (std::get<0>(readSet.second))
                        {
                            nothingToDo = false;
                            const auto t1 = std::chrono::steady_clock::now();
                            _outputStreams.writeSeqs(std::move(*std::get<0>(readSet.second)), std::get<1>(readSet.second));
                            std::get<0>(readSet.second).reset(nullptr); // delete written data
                            _stats += std::get<2>(readSet.second);
                            const auto ioTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - t1).count();
                            _stats.ioTime += ioTime;
                            const auto deltaLastScreenUpdate = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - _lastScreenUpdate).count();
                            if (deltaLastScreenUpdate > 1)
                            {
                                const auto deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - _startTime).count();
                                if (_programParams.showSpeed)
                                    std::cout << "\rreads processed: " << _stats.readCount << "   (" << static_cast<int>(_stats.readCount / deltaTime) << " Reads/s)";
                                else
                                    std::cout << "\rreads processed: " << _stats.readCount;
                                _lastScreenUpdate = std::chrono::steady_clock::now();
                            }
                        }
                        readSet.first.unlock();
                    }
                }
                if (nothingToDo)
                {
                    //std::cout << "4" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
                }
            }
        });
    }
    void writeReads(TWriteItem&& writeItem) noexcept    // blocks until item could be added
    {
        while (true)
        {
            for (auto& reads : _tlsReadSets)
            {
                if (reads.first.try_lock())
                {
                    if (!std::get<0>(reads.second))
                    {
                        reads.second = std::move(writeItem);
                        reads.first.unlock();
                        return;
                    }
                    reads.first.unlock();
                }
            }
            //std::cout << "3" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
        }
    }
    void getStats(std::tuple_element_t<2, TWriteItem>& stats)
    {
        _run = false;
        if (_thread.joinable())
            _thread.join();
        stats = _stats;
    }
    bool idle() noexcept
    {
        unsigned int numEmpty = 0;
        for (auto& readSet : _tlsReadSets)
            if (readSet.first.try_lock())
            {
                if (!std::get<0>(readSet.second))
                    ++numEmpty;
                readSet.first.unlock();
            }
        return numEmpty == _tlsReadSets.size();
    }
    private:
        const ProgramParams& _programParams;
        OutputStreams& _outputStreams;
        std::vector<std::pair<std::mutex, TWriteItem>> _tlsReadSets;
        std::thread _thread;
        std::atomic_bool _run;
        unsigned int _sleepMS;
        std::chrono::time_point<std::chrono::steady_clock> _startTime;
        std::chrono::time_point<std::chrono::steady_clock> _lastScreenUpdate;
        std::tuple_element_t<2, TWriteItem> _stats;
};

template<template<typename> class TRead, typename TSeq>
struct ReadReader
{
    ReadReader(const ProgramParams& programParams, InputFileStreams& inputFileStreams, unsigned int sleepMS)
        :_programParams(programParams), _inputFileStreams(inputFileStreams), _tlsReadSets(_programParams.num_threads + 1) ,
        _run(false), _eof(false), _sleepMS(sleepMS), _numReads(0)
    {}
    ~ReadReader() 
    {
        _run = false;
        if (_thread.joinable())
            _thread.join();
    }
    void start()
    {
        _run = true;
        _thread = std::thread([this]()
        {
            while (_run)
            {
                bool nothingToDo = true;
                for (auto& readSet : _tlsReadSets)
                {
                    if (readSet.first.try_lock())
                    {
                        if (!readSet.second && !_eof) // empty slot -> start new Read
                        {
                            nothingToDo = false;
                            readSet.second = std::make_unique<std::vector<TRead<TSeq>>>(_programParams.records);
                            readReads(*(readSet.second), _programParams.records, _inputFileStreams);
                            loadMultiplex(*(readSet.second), _programParams.records, _inputFileStreams.fileStreamMultiplex);
                            _numReads += readSet.second->size();
                            if (readSet.second->empty() || _numReads >= _programParams.firstReads)    // no more reads available or maximum read number reached -> dont do further reads
                                _eof = true;
                        }
                        readSet.first.unlock();
                    }
                }
                if (nothingToDo)
                {
                    //std::cout << "1" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
                }
            }
        });
    }
    bool eof() const noexcept
    {
        return _eof;
    }
    bool idle() noexcept
    {
        if (!_eof)
            return false;
        for (auto& readSet : _tlsReadSets)
            if (readSet.first.try_lock())
            {
                if (readSet.second)
                {
                    readSet.first.unlock();
                    return false;
                }
                readSet.first.unlock();
            }
        return true;
    }
    bool getReads(std::unique_ptr<std::vector<TRead<TSeq>>>& reads) noexcept
    {
        while (true)
        {
            for (auto& readSet : _tlsReadSets)
            {
                if (readSet.first.try_lock())
                {
                    if (readSet.second)
                    {
                        reads = std::move(readSet.second);
                        readSet.first.unlock();
                        return reads->empty() ? false : true;
                    }
                    readSet.first.unlock();
                }
            }
            if (_eof)
                return false;
            //std::cout << "2" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
        }
        return false;
    };
private:
    const ProgramParams& _programParams;
    InputFileStreams& _inputFileStreams;
    std::vector<std::pair<std::mutex,std::unique_ptr<std::vector<TRead<TSeq>>>>> _tlsReadSets;
    std::thread _thread;
    std::atomic_bool _run;
    std::atomic_bool _eof;
    unsigned int _sleepMS;
    unsigned int _numReads;
};

template <typename TRead, typename TFinder, typename TReadReader, typename TReadWriter>
struct ProcessingUnit
{
    using TpReads = std::unique_ptr<std::vector<TRead>>;

    ProcessingUnit(const ProgramParams& programParams, const ProcessingParams& processingParams, const DemultiplexingParams& demultiplexingParams, const AdapterTrimmingParams& adapterTrimmingParams,
        const QualityTrimmingParams& qualityTrimmingParams, TFinder &finder, TReadReader& readReader, TReadWriter& readWriter, unsigned int sleepMS) : _programParams(programParams), _processingParams(processingParams), _demultiplexingParams(demultiplexingParams),
        _adapterTrimmingParams(adapterTrimmingParams), _qualityTrimmingParams(qualityTrimmingParams), _finder(finder), _readReader(readReader), _readWriter(readWriter), _threads(_programParams.num_threads), _sleepMS(sleepMS){};

    void start()
    {
        for (auto& _thread : _threads)
        {
            _thread = std::thread([this]() 
            {
                TpReads reads;
                while (_readReader.getReads(reads))
                {
                    _readWriter.writeReads(doProcessing(reads.release()));
                }
            });
        }
    }
    void shutDown()
    {
        for (auto& _thread : _threads)
            if (_thread.joinable())
                _thread.join();
    }
    bool finished() noexcept
    {
        for (auto& _thread : _threads)
            if (_thread.joinable())
                return false;
        return true;
    }

    std::tuple<TpReads, std::vector<std::string>, GeneralStats> doProcessing(std::vector<TRead>* reads)
    {
        GeneralStats generalStats(length(_demultiplexingParams.barcodeIds) + 1);
        generalStats.readCount = reads->size();
        
        // Preprocessing and Filtering
        preprocessingStage(_processingParams, *reads, generalStats);

        // Demultiplexing
        if (demultiplexingStage(_demultiplexingParams, *reads, _finder, generalStats) != 0)
            throw("");

        // Adapter trimming
        adapterTrimmingStage(_adapterTrimmingParams, *reads, generalStats);

        // Quality trimming
        qualityTrimmingStage(_qualityTrimmingParams, *reads, generalStats);

        // Postprocessing
        postprocessingStage(_processingParams, *reads, generalStats);
        return std::make_tuple(std::unique_ptr<std::vector<TRead>>(reads), _demultiplexingParams.barcodeIds, generalStats);
    }
private:
    const ProgramParams& _programParams;
    const ProcessingParams& _processingParams;
    const DemultiplexingParams& _demultiplexingParams;
    const AdapterTrimmingParams& _adapterTrimmingParams;
    const QualityTrimmingParams& _qualityTrimmingParams;
    const TFinder& _finder;
    const unsigned int _sleepMS;

    // main thread variables
    TReadReader& _readReader;
    TReadWriter& _readWriter;
    std::vector<std::thread> _threads;

};

// END FUNCTION DEFINITIONS ---------------------------------------------
template<template <typename> class TRead, typename TSeq, typename TEsaFinder, typename TStats>
int mainLoop(TRead<TSeq>, const ProgramParams& programParams, InputFileStreams& inputFileStreams, const DemultiplexingParams& demultiplexingParams, const ProcessingParams& processingParams, const AdapterTrimmingParams& adapterTrimmingParams,
    const QualityTrimmingParams& qualityTrimmingParams, TEsaFinder& esaFinder,
    OutputStreams& outputStreams, TStats& stats)
{
    const unsigned int threadIdleSleepTimeMS = 10;
    using TReadWriter = ReadWriter<TRead, TSeq, std::tuple<std::unique_ptr<std::vector<TRead<TSeq>>>, decltype(DemultiplexingParams::barcodeIds), GeneralStats>>;
    using TReadReader = ReadReader<TRead, TSeq>;
    using TProcessingUnit = ProcessingUnit<TRead<TSeq>, TEsaFinder, TReadReader, TReadWriter>;

    TReadWriter readWriter(programParams, outputStreams, threadIdleSleepTimeMS);
    TReadReader readReader(programParams, inputFileStreams, threadIdleSleepTimeMS);
    TProcessingUnit processingUnit(programParams, processingParams, demultiplexingParams, adapterTrimmingParams, qualityTrimmingParams, esaFinder, readReader, readWriter, threadIdleSleepTimeMS);

    TStats generalStats(length(demultiplexingParams.barcodeIds) + 1);

#ifdef _MULTITHREADED_IO
    readReader.start();
    readWriter.start();
    processingUnit.start();
    while (!(readReader.eof() && readReader.idle() && processingUnit.finished() && readWriter.idle())) // shortcut is used most of the time -> xxx.idle() get called only after eof is set
    {
        if (readReader.eof())
            processingUnit.shutDown();
        std::this_thread::sleep_for(std::chrono::milliseconds(10*threadIdleSleepTimeMS));
    }
    readWriter.getStats(stats);
#else
    const auto tMain = std::chrono::steady_clock::now();
    while (generalStats.readCount < programParams.firstReads)
    {
        readSet.reset(new std::vector<TRead<TSeq>>(programParams.records));
        auto t1 = std::chrono::steady_clock::now();
        const auto numReadsRead = readReads(*readSet, programParams.records, inputFileStreams);
        generalStats.ioTime += std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - t1).count();
        if (numReadsRead == 0)
            break;

        auto res = processingUnit.doProcessing(readSet.release());
        generalStats += std::get<1>(res);

        t1 = std::chrono::steady_clock::now();
        outputStreams.writeSeqs(std::move(*(std::get<0>(res))), demultiplexingParams.barcodeIds);
        generalStats.ioTime += std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - t1).count();

        // Print information
        const auto deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - tMain).count();
        if (programParams.showSpeed)
            std::cout << "\rreads processed: " << _stats.readCount << "   (" << static_cast<int>(generalStats.readCount / deltaTime) << " Reads/s)";
        else
            std::cout << "\rreads processed: " << _stats.readCount;
    }
    stats = generalStats;
#endif
    return 0;
}

// ----------------------------------------------------------------------------
// Function main()
// ----------------------------------------------------------------------------
// Program entry point.

int flexbarMain(int argc, char const ** argv)
{
    SEQAN_PROTIMESTART(loopTime);
    seqan::ArgumentParser parser = initParser();

    // Additional checks
    seqan::ArgumentParser::ParseResult res = seqan::parse(parser, argc, argv);

    // Check if input was successfully parsed.
    if (res != seqan::ArgumentParser::PARSE_OK)
        return res == seqan::ArgumentParser::PARSE_ERROR;

    // Check if one or two input files (single or paired-end) were given.
    int fileCount = getArgumentValueCount(parser, 0);
    if (!(fileCount == 1 || fileCount == 2)){
        printShortHelp(parser);
        return 1;
    }

    //--------------------------------------------------
    // Parse general parameters.
    //--------------------------------------------------

    seqan::CharString output;
    getOptionValue(output, parser, "output");

    //--------------------------------------------------
    // Parse pre- and postprocessing parameters.
    //--------------------------------------------------
    ProcessingParams processingParams;

    if(flexiProgram == FILTERING || flexiProgram == ALL_STEPS)
    {
        seqan::CharString substituteString;
        getOptionValue(substituteString, parser, "s");
        processingParams.substitute = substituteString[0];
        processingParams.runCheckUncalled = seqan::isSet(parser, "u");
        processingParams.runSubstitute = seqan::isSet(parser, "s");
        getOptionValue(processingParams.uncalled, parser, "u");
        if (seqan::isSet(parser, "s") && (!seqan::isSet(parser, "u") || processingParams.uncalled == 0))
        {
            std::cout << "\nWarning: Substitute for uncalled bases is set, but will be ineffective unless number of allowed"
                << " uncalled bases is set to a value higher than 0 by using the paramter -u [VALUE]\n";
            return 1;
        }
        getOptionValue(processingParams.trimLeft, parser, "tl");
        processingParams.tagTrimming = seqan::isSet(parser, "tt");
		getOptionValue(processingParams.trimRight, parser, "tr");
        getOptionValue(processingParams.minLen, parser, "ml");
        if (seqan::isSet(parser, "fl"))
        {
            getOptionValue(processingParams.finalLength , parser, "fl");
        }
        if (seqan::isSet(parser, "fm"))
        {
            getOptionValue(processingParams.finalMinLength , parser, "fm");
        }
        processingParams.runPre = ((processingParams.minLen + processingParams.trimLeft + processingParams.trimRight != 0)
            || isSet(parser, "u"));
        processingParams.runPost = (processingParams.finalLength + processingParams.finalMinLength != 0);
        if(flexiProgram == FILTERING)
        {
            processingParams.runPre = true;
            processingParams.runPost = true;
        }

    }
    //--------------------------------------------------
    // Parse demultiplexing parameters.
    //--------------------------------------------------

    DemultiplexingParams demultiplexingParams;
    seqan::SeqFileIn multiplexInFile;    //Initialising the SequenceStream for the multiplex file

    if(flexiProgram == DEMULTIPLEXING || flexiProgram == ALL_STEPS)
    {
        getOptionValue(demultiplexingParams.multiplexFile, parser, "x");
        if (isSet(parser, "x"))
        {
            if (!open(multiplexInFile, seqan::toCString(demultiplexingParams.multiplexFile)))
            {
                std::cerr << "Could not open file " << demultiplexingParams.multiplexFile << " for reading!" << std::endl;
                return 1;
            }
        }

        if (loadDemultiplexingParams(parser, demultiplexingParams) != 0)
            return 1;

        if(flexiProgram == DEMULTIPLEXING)
            demultiplexingParams.run = true;
    }

    //--------------------------------------------------
    // Process Barcodes
    //--------------------------------------------------

    BarcodeMatcher esaFinder(demultiplexingParams.barcodes);

    if(flexiProgram == DEMULTIPLEXING && (!isSet(parser, "x") && !isSet(parser, "b")))
    {
        std::cerr << "No Barcodefile was provided." << std::endl;
        return 1;
    }

    //--------------------------------------------------
    // Parse quality trimming parameters.
    //--------------------------------------------------

    QualityTrimmingParams qualityTrimmingParams;
    if(flexiProgram == QUALITY_CONTROL || flexiProgram == ALL_STEPS)
    {
        if ( loadQualityTrimmingParams(parser, qualityTrimmingParams) != 0 )
            return 1;

        if(flexiProgram == QUALITY_CONTROL)
            qualityTrimmingParams.run = true;
    }

    //--------------------------------------------------
    // Parse adapter trimming parameters.
    //--------------------------------------------------

    AdapterTrimmingParams adapterTrimmingParams;

    if(flexiProgram == ADAPTER_REMOVAL || flexiProgram == QUALITY_CONTROL|| flexiProgram == ALL_STEPS)
    {
        if(flexiProgram == ADAPTER_REMOVAL || flexiProgram == ALL_STEPS)
        {
            if (loadAdapterTrimmingParams(parser, adapterTrimmingParams) != 0)
            {
                return 1;
            }
        }
        const bool tagOpt = seqan::isSet(parser, "t");
        if (tagOpt && !(qualityTrimmingParams.run || adapterTrimmingParams.run))
        {
            std::cout << "\nWarning: Tag option was selected without selecting adapter removal or quality-trimming stage.\n";
                return 1;
        }
        adapterTrimmingParams.tag = qualityTrimmingParams.tag = tagOpt;
        if(flexiProgram == ADAPTER_REMOVAL)
            adapterTrimmingParams.run = true;
    }

    //--------------------------------------------------
    // General program parameters and additional checks.
    //--------------------------------------------------

    ProgramParams programParams;
    InputFileStreams inputFileStreams;
    if (loadProgramParams(parser, programParams, inputFileStreams) != 0)
        return 1;

    if (checkParams(programParams, inputFileStreams, processingParams, demultiplexingParams, adapterTrimmingParams, qualityTrimmingParams) != 0)
        return 1;

    //--------------------------------------------------
    // Processing
    //--------------------------------------------------

    // Prepare output stream object and initial mapping from StringSets to files.
    bool noQuality = (value(format(inputFileStreams.fileStream1)) ==
                      Find<FileFormat<seqan::SeqFileIn>::Type, Fasta>::VALUE);
    if (isSet(parser, "nq"))
    {
        noQuality = true;
    }

    seqan::CharString filename1;
    getArgumentValue(filename1, parser, 0, 0);

    bool useDefault = false;
    if (output == "")
    {
        output = filename1;
        useDefault = true;
    }

    OutputStreams outputStreams(output, noQuality);
    seqan::String<unsigned> map;
    resize(map,1);
    map[0] = 0;
    // Output additional Information on selected stages:
    if (!isSet(parser, "ni"))
    {
        seqan::CharString filename2, out;
        getOptionValue(out, parser, "output");
        std::cout << "Overview:\n";
        std::cout << "=========\n";
        std::cout << "Application Type: " << sizeof(void*) * 8 << " bit" << std::endl;
        getArgumentValue(filename1, parser, 0, 0);
        std::cout << "Forward-read file: " << filename1 << "\n";
        if (programParams.fileCount == 2)
        {
            getArgumentValue(filename2, parser, 0, 1);
            std::cout << "Backward-read file: " << filename2 << "\n";
        }
        else
        {
            std::cout << "Backward-read file: NONE\n";
        }
        if (isSet(parser, "output"))
        {
            std::cout << "Output-path: " << out <<"\n";
        }
        else
        {
            std::cout << "Output-path: Working Directory\n";
        }
        std:: cout << "\n"; 
        std::cout << "General Options:\n";
        std::cout << "\tReads per block: " << programParams.records * ((programParams.fileCount == 2) + 1)<< "\n";
        /*
        if (isSet(parser, "c"))
        {
            std::cout << "\tCompress output: YES" << "\n";
        }tag
        else
        {
            std::cout << "\tCompress output: NO" << "\n";
        }
        */
		if (isSet(parser, "fr") && (value(format(inputFileStreams.fileStream1)) !=
			Find<FileFormat<seqan::SeqFileIn>::Type, Fasta>::VALUE))
		{
			std::cout << "\tForce no-quality output: YES\n";
		}
		if (isSet(parser, "nq") && (value(format(inputFileStreams.fileStream1)) !=
                                    Find<FileFormat<seqan::SeqFileIn>::Type, Fasta>::VALUE))
        {
            std::cout << "\tProcess only first n reads: " << programParams.firstReads << "\n";
        }
        else if (value(format(inputFileStreams.fileStream1)) != Find<FileFormat<seqan::SeqFileIn>::Type, Fasta>::VALUE)
        {
            std::cout << "\tForce no-quality output: NO\n";
        }
        std::cout << "\tNumber of threads: " << programParams.num_threads << "\n";
        if(flexiProgram == ADAPTER_REMOVAL || flexiProgram == QUALITY_CONTROL|| flexiProgram == ALL_STEPS)
        {
            if (isSet(parser, "t"))
            {
                std::cout << "\tTag quality-trimmed or adapter-removed reads: YES\n";
            }
            else if (adapterTrimmingParams.run||qualityTrimmingParams.run)
            {
                std::cout << "\tTag quality-trimmed or adapter-removed reads: NO\n";
            }
            std::cout << "\n";
        }
    
        if(flexiProgram == FILTERING || flexiProgram == ALL_STEPS)
        {
            std::cout << "Pre-, Postprocessing and Filtering:\n";
            std::cout << "\tPre-trim 5'-end length: " << processingParams.trimLeft << "\n";
			std::cout << "\tPre-trim 3'-end length: " << processingParams.trimRight << "\n";
            std::cout << "\tExclude reads shorter than: " << processingParams.minLen << "\n";
            if (isSet(parser, "u"))
            {
                std::cout << "\tAllowed uncalled bases per sequence: " << processingParams.uncalled << "\n";
            }
            if (isSet(parser, "s"))
            {
                std::cout << "\tSubstitute for uncalled bases: " << processingParams.substitute << "\n";
            }
            if (isSet(parser, "fm") && (processingParams.finalLength == 0))
            {
                std::cout << "\tMinimum sequence length after COMPLETE workflow: " << processingParams.finalMinLength << "\n";
            }
            else if (processingParams.finalLength != 0)
            {
                    std::cout << "\tTrim sequences after COMPLETE workflow to length: " <<processingParams.finalLength<< "\n";
            }
            std::cout << "\n";   
        }
        if (demultiplexingParams.run)
        {
            std::cout << "Barcode Demultiplexing:\n";
            std::cout << "\tBarcode file: " << demultiplexingParams.barcodeFile << "\n";
            if (demultiplexingParams.runx)
            {
                std::cout << "\tMultiplex barcode file: " << demultiplexingParams.multiplexFile << "\n";
            }
            else
            {
                std::cout << "\tMultiplex barcodes file:  NO" << demultiplexingParams.multiplexFile << "\n";
            }
            if (demultiplexingParams.approximate)
            {
                std::cout << "\tApproximate matching: YES\n";
            }
            else
            {
                std::cout << "\tApproximate matching: NO\n";
            }
            if (demultiplexingParams.hardClip && !demultiplexingParams.runx)
            {
                std::cout << "\tHardClip mode: YES\n";
            }
            else
            {
                std::cout << "\tHardClip mode: NO\n";
            }
            if (demultiplexingParams.exclude)
            {
                std::cout << "\tExclude unidentified sequences: YES\n";
            }
            else
            {
                std::cout << "\tExclude unidentified sequences: NO\n";
            }
            std::cout << "\n";
        }
        if (adapterTrimmingParams.run)
        {
            std::cout << "Adapter Removal:\n";
            if (isSet(parser, "a"))
            {
                seqan::CharString adapter;
                getOptionValue(adapter, parser, "a");
                std::cout << "\tAdapter file: " << adapter << "\n";
            }
            else
            {
                std::cout << "\tAdapter file: NONE\n";
            }
            if (programParams.fileCount == 2)
            {
                if (adapterTrimmingParams.pairedNoAdapterFile)
                {
                    std::cout << "\tPaired end Mode: without adapter file\n";
                }
                else
                {
                    std::cout << "\tPaired end Mode: with adapter file\n";
                }
            }
            if (isSet(parser, "e") && !isSet(parser, "er"))
            {
                unsigned e, o;
                getOptionValue(e, parser, "e");
                getOptionValue(o, parser, "overlap");
				std::cout << "\tAllowed mismatches: " << e << "\n";
                std::cout << "\tMinimum overlap " << o << "\n";
            }
			else if (!isSet(parser, "e") && isSet(parser, "er"))
			{
				unsigned o;
				double er;
				getOptionValue(er, parser, "er");
				getOptionValue(o, parser, "overlap");
				std::cout << "\tAllowed error rate: " << er << "\n";
				std::cout << "\tMinimum overlap " << o << "\n";
			}
			else
			{
				std::cout << "\nWarning: errors and error rate can not be specified both at the same time.\n";
				return 1;
			}
			std::cout << "\n";
        }
        if (qualityTrimmingParams.run)
        {
            std::cout << "Quality Trimming:\n";
            std::cout << "\tMinumum PHRED-Quality: " << qualityTrimmingParams.cutoff << "\n";
            std::string method;
            getOptionValue(method, parser, "m");
            std::cout << "\tMethod: " << method << "\n";
            if (isSet(parser, "l"))
            {
                std::cout << "\tMinimum length after trimming: " << qualityTrimmingParams.min_length << "\n";
            }
            std::cout << "\n";
        }
    }
    // Start processing. Different functions are needed for one or two input files.
    std::cout << "\nProcessing reads...\n" << std::endl;
    GeneralStats generalStats(length(demultiplexingParams.barcodeIds) + 1);

    if (fileCount == 1)
    {
        if (!demultiplexingParams.run)
            outputStreams.addStream("", 0, useDefault);
        if(demultiplexingParams.runx)
            mainLoop(ReadMultiplex<seqan::Dna5QString>(), programParams, inputFileStreams, demultiplexingParams, processingParams, adapterTrimmingParams, qualityTrimmingParams, esaFinder, outputStreams, generalStats);
        else
            mainLoop(Read<seqan::Dna5QString>(), programParams, inputFileStreams, demultiplexingParams, processingParams, adapterTrimmingParams, qualityTrimmingParams, esaFinder, outputStreams, generalStats);
    }
     else
     {
         if (!demultiplexingParams.run)
             outputStreams.addStreams("", "", 0, useDefault);
         if (demultiplexingParams.runx)
             mainLoop(ReadMultiplexPairedEnd<seqan::Dna5QString>(), programParams, inputFileStreams, demultiplexingParams, processingParams, adapterTrimmingParams, qualityTrimmingParams, esaFinder, outputStreams, generalStats);
         else
             mainLoop(ReadPairedEnd<seqan::Dna5QString>(), programParams, inputFileStreams, demultiplexingParams, processingParams, adapterTrimmingParams, qualityTrimmingParams, esaFinder, outputStreams, generalStats);
    }
    double loop = SEQAN_PROTIMEDIFF(loopTime);
    generalStats.processTime = loop - generalStats.ioTime;

    printStatistics(programParams, generalStats, demultiplexingParams, adapterTrimmingParams, !isSet(parser, "ni"), std::cout);
    if (isSet(parser, "st"))
    {
        std::fstream statFile;
#ifdef _MSC_VER
        statFile.open(std::string(seqan::toCString(outputStreams.getBaseFilename())) + "_flexbar_statistics.txt", std::fstream::out, _SH_DENYNO);
#else
        statFile.open(std::string(seqan::toCString(outputStreams.getBaseFilename())) + "_flexbar_statistics.txt", std::fstream::out);
#endif
        printStatistics(programParams, generalStats, demultiplexingParams, adapterTrimmingParams, !isSet(parser, "ni"), statFile);
        statFile.close();
    }
    return 0;
}
