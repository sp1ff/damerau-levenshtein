// Copyright (C) 2020-2024 Michael Herstine <sp1ff@pobox.com>

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

#include "config.h"
#include "uk.hh"

#include <iostream>

namespace {

  ptrdiff_t
  algo_8(const std::string &A,
         ptrdiff_t m,
         const std::string &B,
         ptrdiff_t n,
         ptrdiff_t inf,
         ptrdiff_t k,
         ptrdiff_t p,
         size_t num_p,
         ptrdiff_t *f)
  {
    ptrdiff_t t  = f[num_p*(k+m)+(p)] + 1;    // f(k,p-1) + 1
    ptrdiff_t t1 = -inf;
    if (k+m>=1 && p >=0 ) {
      t1 = f[num_p*(k+m-1)+(p)];
    }

    ptrdiff_t t2 = -inf;
    if (k < n && p < inf + 2) {
      t2 = f[num_p*(k+m+1)+(p)] + 1;  // f(k+1,p-1) + 1
    }

    // check whether a(t)a(t+1) = b(k+t+1)b(k+t), but *only* if those
    // ranges are valid. IOW, if:
    //
    //   - t > 0
    //   - t + 1 <= m
    //   - k + t > 0
    //   - k + t + 1 <= n
    ptrdiff_t t3 = -inf;
    if (t > 0          &&
        t + 1     <= m &&
        k + t      > 0 &&
        k + t + 1 <= n) {
      if (A[t - 1] == B[k + t] && A[t] == B[k + t - 1]) {
        t3 = t + 1;
      }
    }
    // t := max(t,t1,t2,t3)
    if (t < t1) {
      t = t1;
    }
    if (t < t2) {
      t = t2;
    }
    if (t < t3) {
      t = t3;
    }
    // while a(t+1) = b(t+1+k) do t := t + 1
    while (t + 1 <= m &&
           t + k + 1 <= n &&
           A[t] == B[t + k]) t++;

    if (t > m || t + k > n) {
      t = inf;
    }

    return t;
  }
}

bool
ukkonen(const std::string &A,
        const std::string &B,
        std::size_t D,
        bool verb)
{
  using namespace std;

  ptrdiff_t m = A.length();
  ptrdiff_t n = B.length();
  ptrdiff_t inf = max(m, n); // |A,B| <= inf

  // Allocating too much space, here. This implementation won't satisfy the
  // space bounds.

  // To index into `f' in terms of (i,j), -m <= i <= n,
  // -1 <=j <= inf, do f[i+m][j+1]
  ptrdiff_t f[(m+n+1)*(inf+2)];
  for (ptrdiff_t i = 0; i < m + n + 1; ++i) {
    for (ptrdiff_t j = 0; j < inf + 2; ++j) {
      f[i*(inf+2)+j] = -inf - 1;
    }
  }

  // Initialize f: f(k,|k|-1) = |k|-1, if k < 0...
  for (ptrdiff_t k = -1; k >= -m; --k) {
    f[(k+m)*(inf+2)-k] = -k - 1;
  }
  for (ptrdiff_t k = 0; k <= n; ++k) {
    f[(k+m)*(inf+2)+k] = -1;
  }

  ptrdiff_t p = -1;
  ptrdiff_t r = p - min(m, n);
  while (f[n*(inf+2)+p+1] != m) {
    p = p + 1;
    r = r + 1;
    if (r <= 0) {
      for (ptrdiff_t k = -p; k <= p; ++k) {
        // f(k,p)
        f[(inf+2)*(k+m)+p+1] = algo_8(A, m, B, n, inf, k, p, inf+2, (ptrdiff_t*)f);
      }
    } else {
      for (ptrdiff_t k = max(-m, -p); k <= -r; ++k) {
        // f(k,p)
        f[(inf+2)*(k+m)*p+1] = algo_8(A, m, B, n, inf, k, p, inf+2, (ptrdiff_t*)f);
      }
      for (ptrdiff_t k = r; k <= max(n, p); ++k) {
        // f(k,p)
        f[(inf+2)*(k+m)+p+1] = algo_8(A, m, B, n, inf, k, p,  inf+2, (ptrdiff_t*)f);
      }
    }
  }

  size_t d = p;

  if (verb) {
    cout << "Computed distance: " << d << endl;
  }
  return d == D;
}
