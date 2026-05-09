#include <iostream>
#include <vector>
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

void arrayCalc(vector<int>& a, vector<int>& b){
    vector<int> sum = {};
    vector<int> substracted = {};
    vector<int> multiplied = {};
    vector<int> divided = {};

    int size = a.size();
    int* ptr1 = &a[0];
    int* ptr2 = &b[0];

    for(int i = 0; i < size; i = add(i, 1)){
        sum.push_back(add(*ptr1, *ptr2));
        substracted.push_back(sub(*ptr1, *ptr2));
        multiplied.push_back(multiply(*ptr1, *ptr2));
        divided.push_back(divide(*ptr1, *ptr2));
        ptr1++;
        ptr2++;
    }
    for (int x : sum) {
        cout << x << " ";
    }
    cout<<endl;
    for (int x : substracted) {
        cout << x << " ";
    }
    cout<<endl;
    for (int x : multiplied) {
        cout << x << " ";
    }
    cout<<endl;
    for (int x : divided) {
        cout << x << " ";
    }
    cout<<endl;
}
void comparison(vector<int>& a, vector<int>& b, vector<int>& max_arr, vector<int>& min_arr){
    int* ptr1 = &a[0];
    int* ptr2 = &b[0];
    int size = a.size();

    for(int i = 0; i < size; i = add(i, 1)){
        int diff = sub(*ptr1, *ptr2);
        int mask = diff >> 31;
        int max_val = (*ptr1 & ~mask) | (*ptr2 & mask);
        int min_val = (*ptr1 & mask) | (*ptr2 & ~mask);

        max_arr.push_back(max_val);
        min_arr.push_back(min_val);
        
        ptr1++;
        ptr2++;
    }
    for (int x : max_arr) {
        cout << x << " ";
    }
    cout << endl;
}
int getScaleVal(int max_val, int min_val) {
    int shift_count = 0;
    int temp = max_val;

    for (int i = 0; i < 31; i = add(i, 1)) {
        int next_temp = sub(temp, min_val);
        
        //  next_temp >= 0, знаковый бит равен 0 
        int keep_counting = ((next_temp >> 31) & 1) ^ 1;

        shift_count = add(shift_count, keep_counting);

        temp = (next_temp & -keep_counting) | (temp & ~-keep_counting);
    }

    return shift_count;
}

void scale(vector<int>& max_arr, vector<int>& min_arr) {
    int size = max_arr.size();
    int* ptr_max = &max_arr[0];
    int* ptr_min = &min_arr[0];

    cout << "Scale: ";

    for (int i = 0; i < size; i = add(i, 1)) {
        int times = getScaleVal(*ptr_max, *ptr_min);
        cout << times << "x ";

        ptr_max++;
        ptr_min++;
    }
    cout << endl;
}

int main(){
    vector<int> a = {6,36,24,18};
    vector<int> b = {5,7,3,8};
    arrayCalc(a,b);
    vector<int> max_vals = {};
    vector<int> min_vals = {};

    comparison(a, b, max_vals, min_vals);

    scale(max_vals, min_vals);
    return 0;
}