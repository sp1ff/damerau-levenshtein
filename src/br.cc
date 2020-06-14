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
    std::vector<std::vector<ptrdiff_t>> &fkp,
    ptrdiff_t zero_k,
    size_t inf) {

    ptrdiff_t t = -inf;
    if (p >= 0) {
      t = fkp[k + zero_k][p] + 1;
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
      ta = fkp[k - 1 + zero_k][p];
    }
    ptrdiff_t tb = -inf;
    if (p >= 0) {
      tb = fkp[k + 1 + zero_k][p] + 1;
    }
    if (ta > t) t = ta;
    if (tb > t) t = tb;
    if (t2 > t) t = t2;
    while (t < (ptrdiff_t)std::min(m, n - k) && A[t] == B[t+k]) ++t;
    if (k + zero_k >= 0 && p > -2) {
      fkp[k + zero_k][p+1] = t;
    }
    return t;
  }

}

bool
berghel_roach(const std::string &A,
              const std::string &B,
              size_t max_k,
              size_t max_p,
              std::vector<std::vector<ptrdiff_t>> &fkp,
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
        cout << fkp[k][p];
      }
      cout << endl;
    }
  }

  do {
    ptrdiff_t inc = p;
    // TODO(sp1ff): if `p-1' is less than zero, do we have to count down?
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
    } while (fkp[n - m + zero_k][p] != m);

  size_t s = p - 1;

  if (verb) {
    cout << "Computed distance: " << s << endl;
  }
  return s == D;

}
