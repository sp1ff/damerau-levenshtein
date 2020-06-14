#ifndef UK_HH_INCLUDED
#define UK_HH_INCLUDED 1

#include <algorithm>
#include <string>
#include <tuple>

/**
 * \brief Compute the Damerau-Levenshtein distance between two strings using
 * the algorithm of Ukkonen (1985)
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
 * This function will employ Algorithm (11), which builds Algorithm (8), as
 * modified in section 4 of the paper "Algorithms for Approximate String
 * Matching" by Esko Ukkonen in the journal Information & Control Vol. 64, pp
 * 100-118, 1985.
 *
 * Ukknonen builds on the approach of Lowrance & Wagner in a few ways:
 *
 * - he proves that one need not compute the complete recurrence relation; the
 *   optimal path will be in a band around the main diagonal
 *
 * - he framed his algorithm in such a way that it can terminate early if the
 *   edit distance is found to be outside a particular threshold (a common
 *   use case)
 *
 * - he went on to realize time & space savings by computing an ancillary
 *   function, rather than the primary recurrence relation
 *
 * The algorithm runs in time O(s*min(m,n)) and space O(s*min(s,m,n)) where s is
 * the edit distance & m and n are the string lengths of \a A & \a B,
 * respectively.
 *
 *
 */

bool
ukkonen(const std::string &A,
        const std::string &B,
        std::size_t D,
        bool verb);

/**
 * \brief Compute Damerau-Levenshtein distance over a sequence of test cases
 * using the algorithm of Ukkonen (1985)
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
bool test_ukkonen(FII p0,
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
                  return ukkonen(A, B, d, verb);
                });
}
#endif // UK_HH_INCLUDED
