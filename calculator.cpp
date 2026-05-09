#include <iostream>
using namespace std;

int add(int a, int b){
    while (b != 0){
        int result = a^b; //XOR
        int carry = (a&b) << 1;//AND
        a = result;
        b = carry;
    }
    return a;
}
int sub(int a,int b){
    while (b != 0){
        int result = a^b; //XOR
        int borrow = (~a&b) << 1;//invert AND
        a = result;
        b = borrow;
    }
    return a;
}
int multiply(int a,int b){
    int res = 0;
    while (b > 0){
       if (b & 1) {
            res = add(res, a); 
        }
        a <<= 1;
        b >>= 1;
    }
    return res;
}
int divide(int a, int b) {
    int quotient = 0;
    int remainder = 0;

    for (int i = 31; i >= 0; i = sub(i, 1)) {
        remainder = (remainder << 1) | ((a >> i) & 1);

        if (remainder >= b) {
            remainder = sub(remainder, b);
            quotient |= (1 << i); 
        }
    }
    return quotient;
}

int main(){
    cout<<add(23,9988)<<endl;
    cout<<sub(2500,693)<<endl;
    cout<<multiply(25,20)<<endl;
    cout<<divide(25,6)<<endl;
    return 0;
}