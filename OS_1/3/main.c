#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

ucontext_t first_context, second_context;
void print_first(){
     while (1) printf("1\n");
}
void print_second(){
    while (1) printf("2\n"); 
}

const int first_time = 2;
const int second_time = 4;

void change_second();

void change_first() {
    signal(SIGALRM, change_second);
    alarm(first_time);
    swapcontext(&second_context, &first_context);
}

void change_second() {
    signal(SIGALRM, change_first);
    alarm(second_time);
    swapcontext(&first_context, &second_context);
}


int main(void){    
    first_context.uc_link          = &first_context;//uc_link указывает на контекст, который будет восстановлен при выходе из текущего контекста
    first_context.uc_stack.ss_sp = malloc(SIGSTKSZ);// uc_sigmask используется для хранения сигналов
    first_context.uc_stack.ss_size = SIGSTKSZ;
    getcontext(&first_context);

    second_context.uc_link          = &second_context;
    second_context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    second_context.uc_stack.ss_size = SIGSTKSZ;
    getcontext(&second_context);

    makecontext(&first_context, print_first, 0);  //у функции print_first нет аргументов, поэтому 3 параметр 0 
    makecontext(&second_context, print_second, 0);  


    signal(SIGALRM, change_second);
    alarm(first_time);
    setcontext(&first_context);

    
    return 0;
}



