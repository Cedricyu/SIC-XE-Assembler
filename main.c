#include <stdio.h>
int pass_one();
int pass_two();
void print_object_list();

int main(){
    pass_one();
    rewind(stdin);
    pass_two();
    print_object_list();
}