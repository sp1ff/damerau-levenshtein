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
#include "lw.hh"

#include <iostream>

bool
lowrance_wagner(const std::string &A,
                const std::string &B,
                std::size_t known_dist,
                bool verb)
{
  using namespace std;

  // For all `i', for any character `c', DA[`c'] is the largest x <= i - 1 such
  // that A[x] = c for all `c' in A & B. Since I'm restricting strings to ASCII,
  // I'm just using a 128-element array to store the lookup table.
  size_t DA[128] = { 0 };

  size_t nA = A.length(), nB = B.length();
  // Lowrance & Wagner add an additional index -1 to both dimensions and set the
  // -1 row & -1 column to INF.
  size_t INF = nA + nB + 1;

  // This is the (in)famous matrix
  size_t H[(nA + 1)*(nB + 1)];

  for (size_t i = 0; i <= nA; ++i) {
    H[i*nB] = i;
  }
  for (size_t j = 0; j <= nB; ++j) {
    H[j] = j;
  }

  for (size_t i = 1, im1 = 0; i <= nA; ++i, ++im1) {
    size_t DB = 0;
    for (size_t j = 1; j <= nB; ++j) {
      size_t i1 = DA[B[j-1]];
      size_t j1 = DB;
      size_t d = 0;
      if (A[im1] != B[j-1]) {
        d = 1;
      } else {
        DB = j;
      }
      size_t h1 = H[im1*nB + j - 1] + d;
      size_t h2 = H[i*nB + j - 1] + 1;
      size_t h3 = H[im1*nB + j] + 1;
      size_t h4 = INF;
      if (i1 > 0 && j1 > 0) {
        h4 = H[(i1-1)*nB + j1-1] + (im1-i1) + 1 + (j-j1-1);
      }
      if (h2 < h1) h1 = h2;
      if (h3 < h1) h1 = h3;
      if (h4 < h1) h1 = h4;
      H[i*nB + j] = h1;
    }
    DA[A[im1]] = i;
  }

  if (verb) {
    cout << "computed distance is " << H[nA*nB + nB] << endl;
    for (size_t i = 0; i <= nA; ++i) {
      for (size_t j = 0; j <= nB; ++j) {
        if (j == 0) {
          cout << "|";
        }
        cout << " " << H[i*nB + j] << " |";
      }
      cout << endl;
    }
  }

  return H[nA*nB + nB] == known_dist;
}
