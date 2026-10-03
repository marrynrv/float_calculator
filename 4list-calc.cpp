#include <iostream>
#include <vector>

using namespace std;

struct IntCalculator {
    static int add(int a, int b) {
        while (b != 0) {
            int carry = (a & b) << 1;
            a = a ^ b;
            b = carry;
        }
        return a;
    }

    static int sub(int a, int b) {
        while (b != 0) {
            int borrow = (~a & b) << 1;
            a = a ^ b;
            b = borrow;
        }
        return a;
    }

    static int multiply(int a, int b) {
        int res = 0;
        while (b > 0) {
            if (b & 1) res = add(res, a);
            a <<= 1;
            b >>= 1;
        }
        return res;
    }

    static int divide(int a, int b) {
        if (b == 0) return 0; 
        int quotient = 0, remainder = 0;
        for (int i = 31; i >= 0; i = sub(i, 1)) {
            remainder = (remainder << 1) | ((a >> i) & 1);
            if (remainder >= b) {
                remainder = sub(remainder, b);
                quotient |= (1 << i);
            }
        }
        return quotient;
    }

    static int max_val(int a, int b) {
        int mask = sub(a, b) >> 31;
        return (a & ~mask) | (b & mask);
    }


    static int scale(int a, int b) {
        int maximum = max_val(a, b);
        int minimum = (a ^ b) ^ maximum; 
        return divide(maximum, minimum);
    }

    // метод Ньютона
    static int sqrt_val(int a) {
        if (a <= 0) return 0;
        int x = a;
        int y = 1;
        
        while (sub(x, y) > 1) {
            x = divide(add(x, y), 2);
            y = divide(a, x);
        }
        return x;
    }
};

struct FloatCalculator {

    struct FloatParts {
        unsigned int sign;
        int exp;
        unsigned int mant;
    };

    static FloatParts unpack(float num) {
        unsigned int bits = *(unsigned int*)&num;
        FloatParts parts;
        parts.sign = bits >> 31;
        parts.exp = (bits >> 23) & 0xFF; //0xFF - 11111111
        // Возвращаем скрытый бит 
        parts.mant = (bits & 0x7FFFFF) | 0x800000;// 1..23 нуля
        return parts;
    }

    static float pack(unsigned int sign, int exp, unsigned int mant) {
        unsigned int bits = (sign << 31) | (exp << 23) | (mant & 0x7FFFFF);
        return *(float*)&bits;
    }

    static float add(float a, float b) {
        FloatParts pA = unpack(a);
        FloatParts pB = unpack(b);
        
        // Выравниваем экспоненты перед сложением или вычитанием
        if (pA.exp > pB.exp) {
            pB.mant >>= IntCalculator::sub(pA.exp, pB.exp);
            pB.exp = pA.exp;
        } else if (pB.exp > pA.exp) {
            pA.mant >>= IntCalculator::sub(pB.exp, pA.exp);
            pA.exp = pB.exp;
        }

        unsigned int resMant = 0;
        unsigned int resSign = pA.sign;

        if (pA.sign == pB.sign) {
            resMant = IntCalculator::add(pA.mant, pB.mant);
        } else {
            if (pA.mant >= pB.mant) {
                resMant = IntCalculator::sub(pA.mant, pB.mant);
            } else {
                resMant = IntCalculator::sub(pB.mant, pA.mant);
                resSign = pB.sign;
            }
        }
        
        // Нормализуем мантиссу в случае переполнения
        if (resMant & 0x1000000) { //25 бит
            resMant >>= 1;
            pA.exp = IntCalculator::add(pA.exp, 1);
        }
        return pack(resSign, pA.exp, resMant);
    }

    static float sub(float a, float b) {
        unsigned int* bBits = (unsigned int*)&b;
        *bBits ^= 0x80000000; // 1..24 нуля
        return add(a, b);
    }

    static float multiply(float a, float b) {
        FloatParts pA = unpack(a);
        FloatParts pB = unpack(b);
        unsigned int resSign = pA.sign ^ pB.sign;
        
        // Экспоненты складываются, затем вычитается смещение (bias = 127)
        int resExp = IntCalculator::sub(IntCalculator::add(pA.exp, pB.exp), 127);
        
        // Умножаем мантиссы с использованием 64-битного типа во избежание переполнения
        unsigned long long longMant = (unsigned long long)pA.mant * pB.mant;
        unsigned int resMant = longMant >> 23; // Сдвигаем обратно к 23-битному формату
        
        // Нормализация результата
        if (resMant & 0x1000000) {
            resMant >>= 1;
            resExp = IntCalculator::add(resExp, 1);
        }
        return pack(resSign, resExp, resMant);
    }

    static float divide(float a, float b) {
        FloatParts pA = unpack(a);
        FloatParts pB = unpack(b);
        unsigned int resSign = pA.sign ^ pB.sign;
        
        // Экспоненты вычитаются, затем добавляется смещение 
        int resExp = IntCalculator::add(IntCalculator::sub(pA.exp, pB.exp), 127);

        // Масштабируем делимое вверх для сохранения точности при делении
        unsigned long long dividend = (unsigned long long)pA.mant << 23;
        unsigned int resMant = dividend / pB.mant;

        // Нормализация результата, если требуется
        if (!(resMant & 0x800000)) {
            resMant <<= 1;
            resExp = IntCalculator::sub(resExp, 1);
        }
        return pack(resSign, resExp, resMant);
    }

    // Возвращает максимум
    static float max_val(float a, float b) {
        float diff = sub(a, b);
        unsigned int diffBits = *(unsigned int*)&diff;
        unsigned int mask = diffBits >> 31; // 1 если a < b, 0 если a >= b
        
        unsigned int aBits = *(unsigned int*)&a;
        unsigned int bBits = *(unsigned int*)&b;
        
        unsigned int resBits = (aBits & ~-mask) | (bBits & -mask);
        return *(float*)&resBits;
    }

    // Сколько раз меньшее число помещается в большем
    static float scale(float a, float b) {
        float maximum = max_val(a, b);
        float minimum = sub(add(a, b), maximum); 
        return divide(maximum, minimum);
    }

    // Квадратный корень Вавилонский метод
    static float sqrt_val(float a) {
        if (a <= 0.0f) return 0.0f;
        float x = a;
        float half = 0.5f;
        for (int i = 0; i < 6; i = IntCalculator::add(i, 1)) {
            x = multiply(half, add(x, divide(a, x)));
        }
        return x;
    }
};


void printIntVector(const string& label, const vector<int>& vec) {
    cout << label << ": ";
    for (int x : vec) cout << x << " ";
    cout << endl;
}

void printFloatVector(const string& label, const vector<float>& vec) {
    cout << label << ": ";
    for (float x : vec) cout << x << "  ";
    cout << endl;
}

void processIntArrays(const vector<int>& a, const vector<int>& b) {
    vector<int> sum_res, sub_res, mul_res, div_res, max_res, scale_res, sqrt_a, sqrt_b;

    for (size_t i = 0; i < a.size(); i++) {
        sum_res.push_back(IntCalculator::add(a[i], b[i]));
        sub_res.push_back(IntCalculator::sub(a[i], b[i]));
        mul_res.push_back(IntCalculator::multiply(a[i], b[i]));
        div_res.push_back(IntCalculator::divide(a[i], b[i]));
        max_res.push_back(IntCalculator::max_val(a[i], b[i]));
        scale_res.push_back(IntCalculator::scale(a[i], b[i]));
        sqrt_a.push_back(IntCalculator::sqrt_val(a[i]));
        sqrt_b.push_back(IntCalculator::sqrt_val(b[i]));
    }

    cout << "=== INT ARRAY CALCULATOR RESULTS ===" << endl;
    printIntVector("Addition (+)     ", sum_res);
    printIntVector("Subtraction (-)  ", sub_res);
    printIntVector("Multiplication(*)", mul_res);
    printIntVector("Division (/)     ", div_res);
    printIntVector("Maximum Values   ", max_res);
    printIntVector("Scale Factor (X) ", scale_res);
    printIntVector("Square Root of A ", sqrt_a);
    printIntVector("Square Root of B ", sqrt_b);
    cout << endl;
}

void processFloatArrays(const vector<float>& a, const vector<float>& b) {
    vector<float> sum_res, sub_res, mul_res, div_res, max_res, scale_res, sqrt_a, sqrt_b;

    for (size_t i = 0; i < a.size(); i++) {
        sum_res.push_back(FloatCalculator::add(a[i], b[i]));
        sub_res.push_back(FloatCalculator::sub(a[i], b[i]));
        mul_res.push_back(FloatCalculator::multiply(a[i], b[i]));
        div_res.push_back(FloatCalculator::divide(a[i], b[i]));
        max_res.push_back(FloatCalculator::max_val(a[i], b[i]));
        scale_res.push_back(FloatCalculator::scale(a[i], b[i]));
        sqrt_a.push_back(FloatCalculator::sqrt_val(a[i]));
        sqrt_b.push_back(FloatCalculator::sqrt_val(b[i]));
    }

    cout << "=== FLOAT ARRAY CALCULATOR RESULTS ===" << endl;
    printFloatVector("Addition (+)     ", sum_res);
    printFloatVector("Subtraction (-)  ", sub_res);
    printFloatVector("Multiplication(*)", mul_res);
    printFloatVector("Division (/)     ", div_res);
    printFloatVector("Maximum Values   ", max_res);
    printFloatVector("Scale Factor (X) ", scale_res);
    printFloatVector("Square Root of A ", sqrt_a);
    printFloatVector("Square Root of B ", sqrt_b);
    cout << endl;
}


int main() {

    vector<int> intA = {16, 100, 25, 4};
    vector<int> intB = {4,  25,  9,  2};
    processIntArrays(intA, intB);


    vector<float> floatA = {9.0f,  50.0f, 0.25f, 16.0f};
    vector<float> floatB = {2.0f,  5.0f,  0.5f,  2.0f};
    processFloatArrays(floatA, floatB);

    return 0;
}
