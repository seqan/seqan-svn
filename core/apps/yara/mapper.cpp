// ==========================================================================
//                      Yara - Yet Another Read Aligner
// ==========================================================================
// Copyright (c) 2011-2014, Enrico Siragusa, FU Berlin
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
//     * Neither the name of Enrico Siragusa or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ENRICO SIRAGUSA OR THE FU BERLIN BE LIABLE
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

#define YARA_MAPPER

// ============================================================================
// Forwards
// ============================================================================

struct Options;

// ============================================================================
// Prerequisites
// ============================================================================

// ----------------------------------------------------------------------------
// SeqAn headers
// ----------------------------------------------------------------------------

#include <seqan/basic.h>
#include <seqan/sequence.h>
#include <seqan/index.h>
#include <seqan/parallel.h>

// ----------------------------------------------------------------------------
// App headers
// ----------------------------------------------------------------------------

#include "file_pair.h"
#include "file_prefetched.h"
#include "store_seqs.h"
#include "misc_timer.h"
#include "misc_tags.h"
#include "misc_types.h"
#include "misc_options.h"
#include "index_fm.h"
#include "bits_reads.h"
#include "bits_hits.h"
#include "bits_context.h"
#include "bits_matches.h"
#include "bits_seeds.h"
#include "bits_bucket.h"
#include "find_verifier.h"
#include "find_extender.h"
#include "mapper_collector.h"
#include "mapper_classifier.h"
#include "mapper_ranker.h"
#include "mapper_filter.h"
#include "mapper_extender.h"
#include "mapper_verifier.h"
#include "mapper_selector.h"
#include "mapper_aligner.h"
#include "mapper_writer.h"
#include "mapper.h"

using namespace seqan;

// ============================================================================
// Functions
// ============================================================================

// ----------------------------------------------------------------------------
// Function setupArgumentParser()
// ----------------------------------------------------------------------------

void setupArgumentParser(ArgumentParser & parser, Options const & options)
{
    setAppName(parser, "yara_mapper");
    setShortDescription(parser, "Yara Mapper");
    setCategory(parser, "Read Mapping");

    setDateAndVersion(parser);
    setDescription(parser);

    // Setup mandatory arguments.
    addUsageLine(parser, "[\\fIOPTIONS\\fP] <\\fIREFERENCE INDEX PREFIX\\fP> <\\fISE-READS FILE\\fP>");
    addUsageLine(parser, "[\\fIOPTIONS\\fP] <\\fIREFERENCE INDEX PREFIX\\fP> <\\fIPE-READS FILE 1\\fP> <\\fIPE-READS FILE 2\\fP>");

    addArgument(parser, ArgParseArgument(ArgParseArgument::INPUTPREFIX, "REFERENCE INDEX PREFIX"));
    setHelpText(parser, 0, "An indexed reference genome.");

    addArgument(parser, ArgParseArgument(ArgParseArgument::INPUT_FILE, "READS FILE", true));
    setValidValues(parser, 1, SeqFileIn::getFileFormatExtensions());
    setHelpText(parser, 1, "Either one single-end or two paired-end / mate-pairs read files.");

    addOption(parser, ArgParseOption("v", "verbose", "Displays global statistics."));
    addOption(parser, ArgParseOption("vv", "vverbose", "Displays diagnostic output per batch of reads."));

    // Setup output options.
    addSection(parser, "Output Options");

    addOption(parser, ArgParseOption("o", "output-file", "Specify an output file. \
                                     Default: use the reads filename prefix.",
                                     ArgParseOption::OUTPUTFILE));
    setValidValues(parser, "output-file", BamFileOut::getFileFormatExtensions());

    addOption(parser, ArgParseOption("os", "output-secondary", "Output suboptimal alignments as secondary alignments. \
                                                                Default: output suboptimal alignments inside XA tag."));

    addOption(parser, ArgParseOption("nh", "no-header", "Do not output SAM/BAM header. Default: output header."));

    addOption(parser, ArgParseOption("or", "output-rabema", "Output a SAM/BAM file usable as a gold standard for the \
                                                             Read Alignment BEnchMArk (RABEMA)."));

    // Setup mapping options.
    addSection(parser, "Mapping Options");

    addOption(parser, ArgParseOption("e", "error-rate", "Consider alignments within this error rate.", ArgParseOption::INTEGER));
    setMinValue(parser, "error-rate", "0");
    setMaxValue(parser, "error-rate", "10");
    setDefaultValue(parser, "error-rate", 100.0 * options.errorRate);

    addOption(parser, ArgParseOption("s", "strata-rate", "Consider suboptimal alignments within this error rate from \
                                                          the optimal one. Please, either specify a strata-rate much \
                                                          smaller than error-rate, or choose the option all.",
                                                          ArgParseOption::INTEGER));
    setMinValue(parser, "strata-rate", "0");
    setMaxValue(parser, "strata-rate", "10");
    setDefaultValue(parser, "strata-rate", 100.0 * options.strataRate);

    addOption(parser, ArgParseOption("a", "all", "Report all suboptimal alignments. Default: report alignments within \
                                                  the specified strata rate."));

    addOption(parser, ArgParseOption("q", "quick", "Be quicker by loosely mapping a few very repetitive reads."));
    hideOption(getOption(parser, "quick"));

    // Setup paired-end mapping options.
    addSection(parser, "Paired-End / Mate-Pairs Options");

    addOption(parser, ArgParseOption("ll", "library-length", "Mean template length.", ArgParseOption::INTEGER));
    setMinValue(parser, "library-length", "1");
    setDefaultValue(parser, "library-length", options.libraryLength);

    addOption(parser, ArgParseOption("le", "library-error", "Deviation from the mean template length.", ArgParseOption::INTEGER));
    setMinValue(parser, "library-error", "0");
    setDefaultValue(parser, "library-error", options.libraryError);

    addOption(parser, ArgParseOption("lo", "library-orientation", "Expected orientation of segments in the template.", ArgParseOption::STRING));
    setValidValues(parser, "library-orientation", options.libraryOrientationList);
    setDefaultValue(parser, "library-orientation", options.libraryOrientationList[options.libraryOrientation]);

//    addOption(parser, ArgParseOption("la", "anchor", "Anchor one read and verify its mate."));

    // Setup performance options.
    addSection(parser, "Performance Options");

#ifdef _OPENMP
    addOption(parser, ArgParseOption("t", "threads", "Specify the number of threads to use.", ArgParseOption::INTEGER));
    setMinValue(parser, "threads", "1");
    setMaxValue(parser, "threads", "2048");
    setDefaultValue(parser, "threads", options.threadsCount);
#endif

    addOption(parser, ArgParseOption("r", "reads-batch", "Number of reads to process in one batch.", ArgParseOption::INTEGER));
    setMinValue(parser, "reads-batch", "1000");
    setMaxValue(parser, "reads-batch", "1000000");
    setDefaultValue(parser, "reads-batch", options.readsCount);
}

// ----------------------------------------------------------------------------
// Function parseCommandLine()
// ----------------------------------------------------------------------------

ArgumentParser::ParseResult
parseCommandLine(Options & options, ArgumentParser & parser, int argc, char const ** argv)
{
    ArgumentParser::ParseResult res = parse(parser, argc, argv);

    if (res != ArgumentParser::PARSE_OK)
        return res;

    // Parse indexed genome input file.
    getArgumentValue(options.contigsIndexFile, parser, 0);

    // Parse read input files.
    switch (getArgumentValueCount(parser, 1))
    {
    case 1:
        getArgumentValue(options.readsFile.i1, parser, 1, 0);
        options.singleEnd = true;
        break;
    case 2:
        getArgumentValue(options.readsFile.i1, parser, 1, 0);
        getArgumentValue(options.readsFile.i2, parser, 1, 1);
        options.singleEnd = false;
        break;
    default:
        return ArgumentParser::PARSE_ERROR;
    }

    // Parse output file.
    getOptionValue(options.outputFile, parser, "output-file");
    if (!isSet(parser, "output-file"))
    {
        options.outputFile = trimExtension(options.readsFile.i1);
        appendValue(options.outputFile, '.');
        append(options.outputFile, "sam");
    }

    // Parse output options.
    getOptionValue(options.outputSecondary, parser, "output-secondary");
    options.outputHeader = !isSet(parser, "no-header");
    getOptionValue(options.rabema, parser, "output-rabema");

    // Parse mapping options.
    unsigned errorRate;
    if (getOptionValue(errorRate, parser, "error-rate"))
        options.errorRate = errorRate / 100.0;

    unsigned strataRate;
    getOptionValue(strataRate, parser, "strata-rate");
        options.strataRate = strataRate / 100.0;

    if (isSet(parser, "all"))
    {
        options.mappingMode = ALL;
        options.strataRate = options.errorRate;
    }

    getOptionValue(options.quick, parser, "quick");

    // Parse paired-end mapping options.
    getOptionValue(options.libraryLength, parser, "library-length");
    getOptionValue(options.libraryError, parser, "library-error");
    getOptionValue(options.libraryOrientation, parser, "library-orientation", options.libraryOrientationList);
//    getOptionValue(options.anchorOne, parser, "anchor");

#ifdef _OPENMP
    getOptionValue(options.threadsCount, parser, "threads");
#endif

    getOptionValue(options.readsCount, parser, "reads-batch");

    if (isSet(parser, "verbose")) options.verbose = 1;
    if (isSet(parser, "vverbose")) options.verbose = 2;

    // Get version.
    options.version = getVersion(parser);

    // Get command line.
    for (int i = 1; i < argc; i++)
    {
        append(options.commandLine, argv[i]);
        appendValue(options.commandLine, ' ');
    }
    eraseBack(options.commandLine);

    return seqan::ArgumentParser::PARSE_OK;
}

// ----------------------------------------------------------------------------
// Function configureMapper()
// ----------------------------------------------------------------------------

//template <typename TExecSpace, typename TThreading, typename TOutputFormat, typename TSequencing, typename TStrategy>
//void configureMapper(Options const & options, TThreading const & threading, TSequencing const & sequencing)
//{
//    if (options.anchorOne)
//        spawnMapper(options, threading, sequencing, strategy, AnchorOne());
//    else
//        spawnMapper(options, threading, sequencing, strategy, AnchorBoth());
//}

template <typename TThreading, typename TSequencing>
void configureMapper(Options const & options, TThreading const & threading, TSequencing const & sequencing)
{
    switch (options.mappingMode)
    {
    case STRATA:
        return spawnMapper(options, threading, sequencing, Strata());

    case ALL:
        return spawnMapper(options, threading, sequencing, All());

    default:
        return;
    }
}

template <typename TThreading>
void configureMapper(Options const & options, TThreading const & threading)
{
    if (options.singleEnd)
        configureMapper(options, threading, SingleEnd());
    else
        configureMapper(options, threading, PairedEnd());
}

void configureMapper(Options const & options)
{
#ifdef _OPENMP
    if (options.threadsCount > 1)
        configureMapper(options, Parallel());
    else
#endif
        configureMapper(options, Serial());
}

// ----------------------------------------------------------------------------
// Function main()
// ----------------------------------------------------------------------------

int main(int argc, char const ** argv)
{
    ArgumentParser parser;
    Options options;
    setupArgumentParser(parser, options);

    ArgumentParser::ParseResult res = parseCommandLine(options, parser, argc, argv);

    if (res != seqan::ArgumentParser::PARSE_OK)
        return res == seqan::ArgumentParser::PARSE_ERROR;

    try
    {
        configureMapper(options);
    }
    catch (BadAlloc const & /* e */)
    {
        std::cerr << "Insufficient memory." << std::endl;
        return 1;
    }
    catch (Exception const & e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
