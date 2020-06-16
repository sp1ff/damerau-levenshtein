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

#include "br.hh"

#include <iostream>

namespace {

  ptrdiff_t
  f(ptrdiff_t k,
    ptrdiff_t p,
    const std::string &A,
    const std::string &B,
    size_t m,
    size_t n,
    size_t max_k,
    size_t max_p,
#   ifdef HAVE_C_VARARRAYS
    std::ptrdiff_t *fkp,
#   else
    std::vector<std::vector<ptrdiff_t>> &fkp,
#   endif
    ptrdiff_t zero_k,
    size_t inf) {

    ptrdiff_t t = -inf;
    if (p >= 0) {
#     ifdef HAVE_C_VARARRAYS
      t = fkp[(k + zero_k)*max_p+p] + 1;
#     else
      t = fkp[k + zero_k][p] + 1;
#     endif
    }
    ptrdiff_t t2 = t;
    if (t > 0 && t < m && k+t-1 >= 0 && k + t < n) {
      // t-1 >=0
      // t < m
      // k+t-1 >= 0
      // k+t < n
      if (A[t-1] == B[k+t] && A[t] == B[k+t-1]) {
        t2 = t + 1;
      }
    }
    ptrdiff_t ta = -inf;
    if (p >= 0) {
#     ifdef HAVE_C_VARARRAYS
      ta = fkp[(k - 1 + zero_k)*max_p+p];
#     else
      ta = fkp[k - 1 + zero_k][p];
#     endif
    }
    ptrdiff_t tb = -inf;
    if (p >= 0) {
#     ifdef HAVE_C_VARARRAYS
      tb = fkp[(k + 1 + zero_k)*max_p+p] + 1;
#     else
      tb = fkp[k + 1 + zero_k][p] + 1;
#     endif
    }
    if (ta > t) t = ta;
    if (tb > t) t = tb;
    if (t2 > t) t = t2;
    while (t < (ptrdiff_t)std::min(m, n - k) && A[t] == B[t+k]) ++t;
    if (k + zero_k >= 0 && p > -2) {
#     ifdef HAVE_C_VARARRAYS
      fkp[(k + zero_k)*max_p+p+1] = t;
#     else
      fkp[k + zero_k][p+1] = t;
#     endif
    }
    return t;
  }

}

bool
berghel_roach(const std::string &A,
              const std::string &B,
              size_t max_k,
              size_t max_p,
#             ifdef HAVE_C_VARARRAYS
              std::ptrdiff_t *fkp,
#             else
              std::vector<std::vector<ptrdiff_t>> &fkp,
#             endif // HAVE_C_VARARRAYS
              ptrdiff_t zero_k,
              size_t inf,
              std::size_t D,
              bool verb)
{
  using namespace std;

  size_t m = A.length();
  size_t n = B.length();
  // The minmal p will be at the end of diagonal k
  ptrdiff_t k = n - m;
  ptrdiff_t p = k;

  if (verb) {
    cout << "Comparing '" << A << "' (" << m << ") to '" << B <<
      "' (" << n << ") starting k at " << k << endl;
    cout << "FKP:\n";
    for (size_t k = 0; k < max_k; ++k) {
      for (size_t p = 0; p < max_p; ++p) {
        if (p != 0) cout << ", ";
#       ifdef HAVE_C_VARARRAYS
        cout << fkp[k*max_p+p];
#       else
        cout << fkp[k][p];
#       endif
      }
      cout << endl;
    }
  }

  do {
    ptrdiff_t inc = p;
    for (ptrdiff_t temp_p = 0; temp_p < p; ++temp_p) {
      ptrdiff_t x = n - m - inc;
      if (abs(x) <= temp_p) {
        f(x, temp_p, A, B, m, n, max_k, max_p, fkp, zero_k, inf);
      }
      x = n - m + inc;
      if (abs(x) <= temp_p) {
        f(x, temp_p, A, B, m, n, max_k, max_p, fkp, zero_k, inf);
      }
      --inc;
    }
    f(n - m, p, A, B, m, n, max_k, max_p, fkp, zero_k, inf);
    ++p;
#   ifdef HAVE_C_VARARRAYS
    } while (fkp[(n - m + zero_k)*max_p+p] != m);
#   else
    } while (fkp[n - m + zero_k][p] != m);
#   endif // HAVE_C_VALARRAYS

  size_t s = p - 1;

  if (verb) {
    cout << "Computed distance: " << s << endl;
  }
  return s == D;

}
