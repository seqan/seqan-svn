#!/usr/bin/env python
"""Execute the tests for sam2matrix.

The golden test outputs are generated by the script generate_outputs.sh.

You have to give the root paths to the source and the binaries as arguments to
the program.  These are the paths to the directory that contains the 'projects'
directory.

Usage:  run_tests.py SOURCE_ROOT_PATH BINARY_ROOT_PATH
"""
import logging
import os.path
import sys

# Automagically add util/py_lib to PYTHONPATH environment variable.
path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..',
                                    '..', '..', 'util', 'py_lib'))
sys.path.insert(0, path)

import seqan.app_tests as app_tests

def main(source_base, binary_base):
    """Main entry point of the script."""

    print 'Executing test for seqan_flexbar'
    print '================================'
    print
    
    ph = app_tests.TestPathHelper(
        source_base, binary_base,
        'apps/seqan_flexbar/tests')  # tests dir

    # ============================================================
    # Auto-detect the binary path.
    # ============================================================

    path_to_program = []
    path_to_program.append(app_tests.autolocateBinary(
      binary_base, 'bin', 'sflexQC'))

    path_to_program.append(app_tests.autolocateBinary(
      binary_base, 'bin', 'sflexFilter'))
    
    path_to_program.append(app_tests.autolocateBinary(
      binary_base, 'bin', 'sflexAR'))
    
    path_to_program.append(app_tests.autolocateBinary(
      binary_base, 'bin', 'sflexDMulti'))

    # ============================================================
    # Built TestConf list.
    # ============================================================

    # Build list with TestConf objects, analoguely to how the output
    # was generated in generate_outputs.sh.
    conf_list = []

    # ============================================================
    # First Section.
    # ============================================================

    # App TestConf objects to conf_list, just like this for each
    # test you want to run.
    conf = app_tests.TestConf(
        program=path_to_program[0],
        redir_stdout=ph.outFile('out.stdout'),
        args=[ph.inFile('testsample.fq'), '-q', '20', '-o',
        ph.outFile('qc_test.fa'), '-t', '-ni'],
        to_diff=[(ph.inFile('qc_test.stdout'),
                  ph.outFile('out.stdout')),
                 (ph.inFile('gold_qc_test.fa'),
                  ph.outFile('qc_test.fa'))])
    conf_list.append(conf)

    conf = app_tests.TestConf(
        program=path_to_program[1],
        redir_stdout=ph.outFile('out.stdout'),
        args=[ph.inFile('testsample.fq'), '-tl', '3', '-tr', '4', '-ml',
        '70', '-u', '1', '-s', 'A', '-fl', '70', '-ni', '-o',
        ph.outFile('filter_test.fq')],
        to_diff=[(ph.inFile('filter_test.stdout'),
                  ph.outFile('out.stdout')),
                 (ph.inFile('gold_filter_test.fq'),
                  ph.outFile('filter_test.fq'))])
    conf_list.append(conf)

    conf = app_tests.TestConf(
        program=path_to_program[2],
        redir_stdout=ph.outFile('out.stdout'),
        args=[ph.inFile('testsample.fq'), '-a', 
        ph.inFile('adapter.fa'), 
        '-o', ph.outFile('ar_test.fq'), '-ni'],
        to_diff=[(ph.inFile('ar_test.stdout'),
                  ph.outFile('out.stdout')),
                 (ph.inFile('gold_ar_test.fq'),
                  ph.outFile('ar_test.fq'))])
    conf_list.append(conf)

    conf = app_tests.TestConf(
        program=path_to_program[3],
        redir_stdout=ph.outFile('out.stdout'),
        args=[ph.inFile('testsample_multiplex.fq'), '-b', 
        ph.inFile('barcodes.fa'), 
        '-o', ph.outFile('test_de_multi.fq'), '-ni'],
        to_diff=[(ph.inFile('gold_de_multi.stdout'),
                  ph.outFile('out.stdout')),
                 (ph.inFile('gold_de_multi_Sample-1.fq'),
                  ph.outFile('test_de_multi_Sample-1.fq')),
                 (ph.inFile('gold_de_multi_Sample26.fq'),
                  ph.outFile('test_de_multi_Sample26.fq')),
                 (ph.inFile('gold_de_multi_Sample-2.fq'),
                  ph.outFile('test_de_multi_Sample-2.fq')),
                 (ph.inFile('gold_de_multi_Sample0.fq'),
                  ph.outFile('test_de_multi_Sample0.fq')),
                 (ph.inFile('gold_de_multi_Sample1458.fq'),
                  ph.outFile('test_de_multi_Sample1458.fq')),
                 (ph.inFile('gold_de_multi_Sample37.fq'),
                  ph.outFile('test_de_multi_Sample37.fq')),
                 (ph.inFile('gold_de_multi_unidentified.fq'),
                  ph.outFile('test_de_multi_unidentified.fq'))])
    conf_list.append(conf)

    # ============================================================
    # Execute the tests.
    # ============================================================
    failures = 0
    counter = 0
    for conf in conf_list:
        res = app_tests.runTest(conf)
        # Output to the user.
        print ' '.join([path_to_program[counter]] + conf.args),
        if res:
             print 'OK'
        else:
            failures += 1
            print 'FAILED'
        counter += 1

    # Cleanup.
    ph.deleteTempDir()

    print '=============================='
    print '     total tests: %d' % len(conf_list)
    print '    failed tests: %d' % failures
    print 'successful tests: %d' % (len(conf_list) - failures)
    print '=============================='
    # Compute and return return code.
    return failures != 0


if __name__ == '__main__':
    sys.exit(app_tests.main(main))
