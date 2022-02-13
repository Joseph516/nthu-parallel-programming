#pragma once

#include <fstream>
#include <iterator>
#include <vector>

#include "comp.h"

#define kMaxIter 256

int calPixel(const Complex& c) {
  int cnt = 0;
  Complex z(0, 0);
  float tmp;
  while (z.getModSquare() < 4 && cnt != kMaxIter) {
    tmp = z.real_;
    z.real_ = z.real_ * z.real_ - z.image_ * z.image_ + c.real_;
    z.image_ = 2 * tmp * z.image_ + c.image_;
    cnt++;
  }
  return cnt;
}

template <typename T>
void vecToBinaryFile(const std::vector<T>& vec, std::string filename) {
  if (!vec.empty()) {
    std::ofstream fout(filename, std::ios::out | std::ios::binary);
    fout.write(reinterpret_cast<char*>(&vec[0]), vec.size() * sizeof(T));
    fout.close();
  }
}

template <typename T>
void vecToFile(const std::vector<T>& vec, std::string filename,
               std::string sep = " ") {
  if (!vec.empty()) {
    std::ofstream fout(filename, std::ios::out);
    for (const auto& v : vec) {
      fout << v << sep;
    };
    fout.close();
  }
}

template <typename T>
void twoDVecToFile(const std::vector<std::vector<T>>& vec,
                   std::string filename) {
  if (!vec.empty()) {
    std::ofstream fout(filename, std::ios::out);
    for (const auto& v1 : vec) {
      for (const auto& v2 : v1) {
        fout << v2 << " ";
      }
      fout << "\n";
    };
    fout.close();
  }
}
