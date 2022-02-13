#pragma once

// 复数定义
typedef struct Complex {
  float real_;
  float image_;

  Complex() : real_(0), image_(0){};
  Complex(float real, float image) : real_(real), image_(image){};

  float getModSquare();
} Complex;
