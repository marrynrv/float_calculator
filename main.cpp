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
int main(){
    cout<<add(455,288)<<endl;
    return 0;
}