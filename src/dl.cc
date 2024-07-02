// Copyright (C) 2020-2021 Michael Herstine <sp1ff@pobox.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at
// your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see https://www.gnu.org/licenses/.

#include <config.h>

#include <stdlib.h>
#include <getopt.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "lw.hh"
#include "uk.hh"
#include "br.hh"

enum class algorithm { lw, uk, br };

/// A test case for computing the edit distance between two strings: a three
/// tuple consisting of string "A", string "B", and the known edit distance
/// between them
typedef std::tuple<std::string, std::string, size_t> test_case;

/**
 * \brief Read a collection of test cases from file
 *
 *
 * \param pth [in] a path, absolute or relative to the present working
 * directory, to an ASCII text file containing one or more test cases (see below
 * for expected format)
 *
 * \param pout [in,out] A forward output iterator two which each test case read
 * from \a pth shall be copied
 *
 * \post In each triple, the first string shall be less than or equal to the
 * second in length
 *
 *
 * In order to focus on the underlying algorithms for computing
 * Damerau-Levenshtein distance, I've kept the format of test cases, and the
 * files in which they may be recorded, as simple as possible:
 *
 * - ASCII text only
 * - each line shall consist of three tab-delimited fields:
 *   1. string A
 *   2. string B
 *   3. the known edit distance between them, expressed in base 10
 *
 * Lines with a '#' character in the first column will be considered
 * comments & discarded.
 *
 * For instance:
 *
 \code
 # Test file:
 a\tb\t1
 kitten\tsitting\t3
 \endcode
 *
 *
 * Nb this function will swap the two strings, if necessary, so that the second
 * is at least as long as the first. This is to ensure that the precondition
 * imposed by berghel_roach is met "up-front" and so we don't have to clutter
 * the implementation, for now. I may come back & just update the implementation
 * but for now I just want address issues #3 & #5.
 *
 *
 */

template <typename FOI> // Forward Output Iterator
void
read_corpus(const std::filesystem::path &pth, FOI pout)
{
  using namespace std;

  ifstream in(pth);
  size_t lineno = 1;
  for (string line; getline(in, line); ++lineno) {
    if (line.empty() || line[0] == '#') continue;
    string::size_type idx0 = line.find('\t');
    if (string::npos == idx0) {
      stringstream stm;
      stm << "parse error in `" << pth << "', line " << lineno;
      throw std::runtime_error(stm.str());
    }
    string::size_type idx1 = line.find('\t', idx0 + 1);
    if (string::npos == idx1) {
      stringstream stm;
      stm << "parse error in `" << pth << "', line " << lineno;
      throw std::runtime_error(stm.str());
    }

    if (idx0 < idx1 - idx0 - 1) {
        *pout++ = make_tuple(line.substr(0, idx0),
                             line.substr(idx0 + 1, idx1 - idx0 - 1),
                             (size_t)stoi(line.substr(idx1 + 1)));
    } else {
        *pout++ = make_tuple(line.substr(idx0 + 1, idx1 - idx0 - 1),
                             line.substr(0, idx0),
                             (size_t)stoi(line.substr(idx1 + 1)));
    }
  }

}

/**
 * \brief Program logic for the `dl' driver program; read one or more corpuses
 * (corpii?) of test data; run a selected algorithm over them, report
 * correctness & possibly timing
 *
 *
 * \param algo [in] The algorithm to be exercised
 *
 * \param verbose [in] If true, produce verbose status messages on stdout; this
 * is intended for debugging & trouble-shooting; turning this feature on with a
 * large corpus of test data will likely be unhelpful
 *
 * \param randomize [in] If true, randomize the order of traversal through the
 * corpus of test data; specify this on successive runs to defeat any sort of
 * branch prediction, cache warming or other black magic going on that could
 * affect test timings
 *
 * \param num_loops [in] The number of times to go over the corpus; to get
 * reliable timings over a small set of test cases, you might want to set this
 * to a large number & set \a randomize to true. The test cases will be shuffled
 * before each iteration, and the time spent shuffling will not be included in
 * the reported timings
 *
 * \param print_timings [in] If true, print the time, in milliseconds on the
 * system clock, spent going over the test cases, exclusive of any shuffling
 * that may have been done. If you have a large test corpus, you may instead
 * just run this with \a num_loops = 1, \a randomize false, and just run the
 * program under `time'
 *
 * \param pcorp0 [in] A forward input iterator pointing to the beginning of a
 * range of paths naming ASCII text files containing test cases (see read_corpus
 * for details on their format)
 *
 * \param pcorp1 [in] A forward input iterator pointing to the one-past-the-end
 * position in a range of paths naming test case files
 *
 *
 */

template <typename FII> // Forward Input Iterator
bool
dl(algorithm algo,
   bool      verbose,
   bool      randomize,
   size_t    num_loops,
   bool      print_timings,
   FII       pcorp0,
   FII       pcorp1)
{
  using namespace std;

  typedef chrono::steady_clock::duration duration;
  typedef chrono::milliseconds ms;

  using chrono::duration_cast;

  vector<test_case> C;
  for_each(pcorp0, pcorp1, [&](char *p) { read_corpus(p, back_inserter(C)); });

  size_t inf = 0;
  for_each(C.begin(),
           C.end(),
           [&](const test_case &tc) {
             size_t m = get<0>(tc).length();
             if (m > inf) inf = m;
             m = get<1>(tc).length();
             if (m > inf) inf = m;
           });

  if (verbose) {
    cout << "Testing over a corpus of " << C.size() << " string pairs " <<
      "(max length " << inf << ")." << endl;
  }

  chrono::steady_clock clock;
  duration total_time;

  random_device rd;
  mt19937 g(rd());

  bool ok = true;
  for (int i = 0; i < num_loops; ++i) {

    if (randomize) {
      shuffle(C.begin(), C.end(), g);
    }

    auto then = clock.now();

    if (algorithm::lw == algo) {
      if (!test_lowrance_wagner(C.begin(), C.end(), inf, verbose)) {
        ok = false;
      }
    } else if (algorithm::uk == algo) {
      if (!test_ukkonen(C.begin(), C.end(), inf, verbose)) {
        ok = false;
      }
    }
    else {
      if (!test_berghel_roach(C.begin(), C.end(), inf, verbose)) {
        ok = false;
      }
    }

    auto now = clock.now();
    total_time += now - then;
  }

  if (print_timings) {
    ms msecs = duration_cast<ms>(total_time);
    cout << "processing took " << msecs.count() << "ms" << endl;
  }

  return ok;
}

const char * const USAGE = R"use(`%s' -- exercise a few ways of computing the Damerau-Levenshtein distance

The Damerau-Levenshtein distance between two strings is the minimum number of
insertions, deletions, single-character substitutions or transpositions
required to transform the first string to the second. This program is a test
harness that will compute this quantity for a corpus of strings in one of
three ways:

    1. that of Lowrance & Wagner in "An Extension of the String-to-String
       Correction Problem" (1975)
    2. that of Ukkonen in "Algorithms for Approximate String Matching" (1985)
    3. that of Berghel & Roach in "An Extension of Ukkonen's Enhanced Dynamic
       Programming ASM Algoirthm" (1996)

Berghel & Roach remains, as far as I am aware, the optimal solution to this
problem (later work has focused on defining different, easier-to-compute
distance metrics).

Usage: dl [OPTION...] CORPUS...

where OPTION is one of:

         -h, --help: display this usage message & exit with status zero
      -V, --version: display this program's version & exit with status zero
-a A, --algorithm=A: select the algorithm to apply to CORPUS...
                     A may be one of the following:

                     lw: Lowrance & Wagner
                     uk: Ukkonen
                     br: Berghel & Roach (default)

-n N, --num-loops=N: run the corpus N times (for benchmarking purposes)
    -r, --randomize: run the corpus in random order
-t, --print-timings: print timings on the command line; one could invoke this
                     program using `time' for benchmarking purposes, but this
                     option will exclude time spent reading & parsing the
                     input files
      -v, --verbose: provide verbose output on the computation; this is
                     intended for debugging purposes; supplying this option
                     with a large corpus of input strings will likely
                     be un-helpful

and CORPUS is a path (absolute or relative to the present working directory)
of a plain text file containing one or more lines of three tab-delimited
fields:

    A,B,D

where A & B are two strings to be compared & D is their (previously computed)
Damerau-Levenshtein distance. This program will exit with status zero if it
computes the same value as D for all (A,B), and 1 if any differ.

Please see %s for more discussion & links to these papers.
)use";

int
main(int argc, char **argv)
{
  static struct option long_options[] = {
    {"algorithm",     required_argument, 0, 'a'},
    {"help",          no_argument,       0, 'h'},
    {"num-loops",     required_argument, 0, 'n'},
    {"print-timings", no_argument,       0, 't'},
    {"randomize",     no_argument,       0, 'r'},
    {"verbose",       no_argument,       0, 'v'},
    {"version",       no_argument,       0, 'V'},
    {0, 0, 0, 0}
  };

  auto algo = algorithm::br;
  bool print_timings = false, randomize = false, verbose = false;
  size_t num_loops = 1;
  int c, option_index = 0;
  while (1) {
    c = getopt_long(argc, argv, "a:hn:prtvV", long_options, &option_index);
    if (-1 == c) break;
    switch (c) {
    case 'a':
      if (0 == strcmp(optarg, "lw")) {
        algo = algorithm::lw;
      } else if (0 == strcmp(optarg, "uk")) {
        algo = algorithm::uk;
      } else if (0 != strcmp(optarg, "br")) {
        fprintf(stderr, "unknown algorithm `%s' -- try `dl --help'", optarg);
        exit(2);
      }
      break;
    case 'h':
      printf(USAGE, argv[0], PACKAGE_URL);
      exit(0);
    case 'n':
      num_loops = atoi(optarg);
      if (0 == num_loops) {
        fprintf(stderr, "can't interpret `%s' as # of loops-- try `dl --help'",
                optarg);
        exit(2);
      }
      break;
    case 'p':
      print_timings = true;
      break;
    case 'r':
      randomize = true;
      break;
    case 't':
      print_timings = true;
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      printf("`%s' %s\n", argv[0], PACKAGE_VERSION);
      exit(0);
    case '?':
      /* getopt_long has already printed an error message */
      exit(2);
    }
  }

  if (optind == argc) {
    fprintf(stderr, "you didn't specify a corpus-- try `dl--help'\n");
    exit(2);
  }

  if (verbose) {
    printf("algo is %d\n", (int) algo);
    printf("verbose is %d\n", verbose ? 1 : 0);
    printf("num-loops is %lu\n", num_loops);
    printf("randomize is %d\n", randomize ? 1 : 0);
    printf("print-timings is %d\n", print_timings ? 1 : 0);
    for (int i = optind; i < argc; ++i) {
      printf("corpus: %s\n", argv[i]);
    }
  }

  int status = EXIT_SUCCESS;
  try {
    if (!dl(algo, verbose, randomize, num_loops, print_timings, argv + optind,
            argv + argc)) {
      status = EXIT_FAILURE;
    }
  } catch (const std::exception &ex) {
    fprintf(stderr, "%s\n", ex.what());
    status = 127;
  }

  return status;
}
