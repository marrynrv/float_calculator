# Bit-Level Low-Level Calculator (Int & Float)

This project is a C++ implementation of low-level arithmetic and mathematical operations performed purely through **bitwise operations, bit shifts, and memory manipulation**, bypassing standard high-level arithmetic operators (`+`, `-`, `*`, `/`, `<`, `>`, etc.) where possible.

## Features

- **`IntCalculator`**: Performs operations on integers (`int`) using pure bitwise logic:
  - Addition and Subtraction via bit manipulation (XOR, AND, shifts).
  - Multiplication and Division.
  - Finding maximum values and scaling factors.
  - Integer square root using **Newton's method**.

- **`FloatCalculator`**: Implements IEEE 754 standard floating-point arithmetic from scratch:
  - Unpacking and packing `float` bits (extracting sign, exponent, and mantissa).
  - Exponent alignment, mantissa operations, and normalization for addition, subtraction, multiplication, and division.
  - Square root calculation using the **Babylonian method**.

