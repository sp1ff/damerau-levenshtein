// Copyright (C) 2020 Michael Herstine <sp1ff@pobox.com>

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

#ifndef LW_H_INCLUDED
#define LW_H_INCLUDED 1

#include <algorithm>
#include <string>
#include <tuple>

/**
 * \brief Compute the Damerau-Levenshtein distance between two strings using the
 * algorithm of Lowrance & Wagner (1975)
 *
 *
 * \param A [in] the first of the two strings whose Damerau-Levenshtein distance
 * is to be computed
 *
 * \param B [in] the second of the two strings whose Damerau-Levenshtein
 * distance is to be computed
 *
 * \param D [in] the known Damerau-Levenshtein distance between \a A & \a B
 *
 * \param verb [in] if true, produce verbose progress messages on \c stdout
 *
 * \return true if the edit distance is computed to be D, false else
 *
 *
 * This function will employ "Algorithm S" in the paper "An Extension of the
 * String-to-String Correction Problem" in the Journal of the ACM, Vol. 22,
 * No. 2., April 1975 by Roy Lowrance & Robert A. Wagner to compute the
 * Damerau-Levenshtein distance between \a A & \a B. If it is equal to \a D, it
 * will return true (or false, else).
 *
 * The algorithm runs in both time & space O(m*n) where m & n are the lengths
 * of \a A & \a B, respectively.
 *
 * This was, AFAICT, the seminal paper on this problem. In fact, it's list of
 * references contains only one entry. The bulk of the paper is concerned with
 * proving that the minimal D-L edit distance can be computed using only
 * adjacent transpositions-- the computation of the (today, well-known)
 * recurrence relation is almost an afterthought.
 *
 *
 */

bool
lowrance_wagner(const std::string &A,
                const std::string &B,
                std::size_t D,
                bool verb);

/**
 * \brief Compute Damerau-Levenshtein distance over a sequence of test cases
 * using the algorithm of Lowrance & Wagner (1975)
 *
 *
 * \param p0 [in] A forward input iterator referencing the beginning of a range
 * of test cases (on which more below)
 *
 * \param p1 [in] A forward input iterator referencing the one-past-the-end
 * position of a range of test cases
 *
 * \param inf [in] Maximum length across all strings in the test corpus; unused
 * in this case, it is provided in case a given algorithm can optimize over all
 * comparisons
 *
 * \param verb [in] If true, produce verbose status messages on \c stdout
 *
 * \return true if this implementation calculated the known D-L edit distance
 * for each case in [p0, p1), false else
 *
 *
 * A "test case" is simply a three-tuple: string "A", string "B", and the known
 * distance between them. IOW, type \a FII shall dereference to
 * std::tuple(std::string, std::string, std::size_t). I arrange for all D-L
 * algorithms in this program to take a range of test cases (instead of a single
 * test case) to enable them to take advantage any one-time initialization logic
 * or other global optimizations which they may provide.
 *
 *
 */

template <typename FII>
bool
test_lowrance_wagner(FII p0,
                     FII p1,
                     size_t /*inf*/,
                     bool verb)
{
  using namespace std;
  return all_of(p0,
                p1,
                [&](const tuple<string, string, size_t> &tc) {
                  string A, B;
                  size_t d;
                  tie(A, B, d) = tc;
                  return lowrance_wagner(A, B, d, verb);
                });
}
#endif // LW_H_INCLUDED
