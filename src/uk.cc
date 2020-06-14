#include "uk.hh"

#include <iostream>
#include <vector>

namespace {

  ptrdiff_t
  algo_8(const std::string &A,
         ptrdiff_t m,
         const std::string &B,
         ptrdiff_t n,
         ptrdiff_t inf,
         ptrdiff_t k,
         ptrdiff_t p,
         const std::vector<std::vector<std::ptrdiff_t>> &f)
  {
    ptrdiff_t t  = f[k+m][p] + 1;    // f(k,p-1) + 1
    // TODO(sp1ff):
    // ptrdiff_t t1 = f[k+m-1][p];      // f(k-1,p-1)
    ptrdiff_t t1 = -inf;
    if (k+m>=1 && p >=0 ) {
      t1 = f[k+m-1][p];
    }

    // TODO(sp1ff):
    // inf = max(m,n)
    // (m + n + 1) X (inf + 2)
    // ptrdiff_t t2 = f[k+m+1][p] + 1;  // f(k+1,p-1) + 1
    ptrdiff_t t2 = -inf;
    if (k < n && p < inf + 2) {
      t2 = f[k+m+1][p] + 1;  // f(k+1,p-1) + 1
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

  // TODO(sp1ff): Allocating too much space, here. This
  // implementation won't satisfy the space bounds.

  // To index into `f' in terms of (i,j), -m <= i <= n,
  // -1 <=j <= inf, do f[i+m][j+1]
  vector<vector<ptrdiff_t>> f(m+n+1, vector<ptrdiff_t>(inf+2, -inf-1));

  // Initialize f: f(k,|k|-1) = |k|-1, if k < 0...
  for (ptrdiff_t k = -1; k >= -m; --k) {
    // f(k,|k|-1) = f(k,-k-1) = -k-1
    f[k+m][-k] = -k - 1;
  }
  for (ptrdiff_t k = 0; k <= n; ++k) {
    f[k+m][k] = -1;
  }

  ptrdiff_t p = -1;
  ptrdiff_t r = p - min(m, n);
  // ptrdiff_t r = p - min(m, n);
  // f(n-m,p)
  while (f[n][p+1] != m) {
    p = p + 1;
    r = r + 1;
    if (r <= 0) {
      for (ptrdiff_t k = -p; k <= p; ++k) {
        // f(k,p)
        f[k+m][p+1] = algo_8(A, m, B, n, inf, k, p, f);
      }
    } else {
      for (ptrdiff_t k = max(-m, -p); k <= -r; ++k) {
        // f(k,p)
        f[k+m][p+1] = algo_8(A, m, B, n, inf, k, p, f);
      }
      for (ptrdiff_t k = r; k <= max(n, p); ++k) {
        // f(k,p)
        f[k+m][p+1] = algo_8(A, m, B, n, inf, k, p, f);
      }
    }
  }

  size_t d = p;

  if (verb) {
    cout << "Computed distance: " << d << endl;
  }
  return d == D;
}
