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

#ifndef BR_HH_INCLUDED
#define BR_HH_INCLUDED 1

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

/**
 * \brief Compute the Damerau-Levenshtein distance between two strings using
 * the algorithm of Berghel & Roach (1996)
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
 * This function will employ Algorithm "BR" as defined in section 3 of "An
 * Extension of Ukknonen's Enhanced Dynamic Programming ASM Algorithm" by Hal
 * Berghel and David Roach the the ACM Transactions on Information Systems,
 * Vol. 14 No. 1 January 1996, pp94-106.
 *
 * Berghel's & Roach's improvement is a tighter bound on the range of f(k,p)
 * that need to be computed for any given comparison. If s is the edit
 * distance from \a A to \a B, and m & n their resp. string lenghts, and
 * WLOG n >= m, define p:
 *
 \code

    1       1
   --- - -------
   2*s   2*(n-m)

 \endcode
 *
 * The worst-case running time of this algorithms is O(n*p) and the expected
 * running time is O(n+(p*s)).
 *
 * After reading this paper, I spent some time searching the literature for
 * references to it (looking for further improvements). I didn't find any
 * directly-related improvements. Later references either cited it as the
 * best known to the author(s)' for computing Damerau-Levenshtein distance,
 * or as a point of interest in developing string metrics.
 *
 *
 */

bool
berghel_roach(const std::string &A,
              const std::string &B,
              size_t max_k,
              size_t max_p,
              std::ptrdiff_t *fkp,
              ptrdiff_t zero_k,
              size_t inf,
              std::size_t D,
              bool verb);

/**
 * \brief Compute Damerau-Levenshtein distance over a sequence of test cases
 * using the algorithm of Berghel & Roach (1996)
 *
 *
 * \param p0 [in] A forward input iterator referencing the beginning of a range
 * of test cases (on which more below)
 *
 * \param p1 [in] A forward input iterator referencing the one-past-the-end
 * position of a rnage of test cases
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
test_berghel_roach(FII p0,
                   FII p1,
                   size_t inf,
                   bool verb)
{
  using namespace std;

  // We can build FKP here & re-use it for each individual comparison. FKP is a
  // two- dimensional array consisting of `max_k' rows & `max_p' columns. In the
  // exposition, the indicies run over -m to n (all diagonals) and from -1 to
  // the maxium p (max(m,n) = `inf') respectivey.
  size_t max_k = inf + inf + 1;
  size_t max_p = inf + 2;
  // That means that to lookup the value for f(k,p), we need to index as:
  // FKP[k + zero_k][p+1] (i.e. the exposition is incorrect in this regard).
  ptrdiff_t zero_k = inf;

  // ptrdiff_t FKP[max_k][max_p];
  ptrdiff_t FKP[max_k*max_p];
  for (ptrdiff_t i = 0; i < max_k; ++i) {
    for (ptrdiff_t j = 0; j < max_p; ++j) {
      FKP[i*max_p+j] = -inf;
    }
  }
  for (ptrdiff_t k = - zero_k; k <= zero_k; ++k) {
    ptrdiff_t abs_k = k;
    if (k < 0) abs_k = -k;
    for (ptrdiff_t p = -1; p <= (ptrdiff_t)inf; ++p) {
      if (p == abs_k - 1) {
        if (k < 0) {
          FKP[(k + zero_k)*max_p + p+1] = abs_k - 1;
        } else {
          FKP[(k + zero_k)*max_p + p+1] = -1;
        }
      }
    }
  }

  return all_of(p0,
                p1,
                [&](const tuple<string, string, size_t> &tc) {
                  string A, B;
                  size_t d;
                  tie(A, B, d) = tc;
                  return berghel_roach(A, B, max_k, max_p, (ptrdiff_t*)FKP, zero_k, inf, d, verb);
                });
}
#endif // BR_HH_INCLUDED
