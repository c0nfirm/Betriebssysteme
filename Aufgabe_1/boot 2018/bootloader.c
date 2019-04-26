#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif


typedef int bool;
#define true 1
#define false 0

/* needs to stay the first line */
asm("jmp $0, $main");

/* space for additional code */

/*print character function with char variable*/
void printC(char ch){
    asm("mov $0x0e, %%ah;"          /*loading function 0eh (print character) into register AH*/
        "mov %0, %%al;"             /*saving content of %0 into register AL*/
        "int $0x10" :: "r"(ch));    /*interrupt 10h (video services and r = character to print*/
}
/*get keyboard input function which returns a character*/
char get_input(){
    char c;                         /*variable to save keyboard input into*/
    asm volatile(
                 "mov %%ah, 1;"     /*loading function 01h into register AH*/
                 "int $0x16;"       /*interrupt 16h (keyboard services)*/
                 :"=r"(c)           /*saving the pressed key(ASCII code) into c*/
                 :
                 :      );
    return c;                       /*return pressed key (ASCII code)*/
}
/*print welcome message function */
void hello(){
    char msg[] = "Hello!\n";        /*string with welcome message*/
    int i = 0;                      /*interrupt variable for the while loop*/
    while(i < 8){                   /*repeat 8 times (welcome string has 8 characters)*/
        printC(msg[i]);             /*print character function with character to print*/
        i = i+1;                    /*interrupt variable +1*/
    }
    printC('\r');                   /*print return to reach next line*/
}
/*bios reboot function INT 16h*/
void reboot(){
    asm("int $0x19;");
}


/*print password in undisclosed with string and counter variable*/
void print_pass(int tmp, char txt[]){
    int i = 0;                      /*interrupt variable for while loop*/
    while(i < tmp){                 /*repeat as long at the password string is*/
        printC(txt[i]);             /*print string on index i*/
        i = i+1;                    /*interrupt variable +1*/
    }
    printC('\n');                   /*print end of line*/
    printC('\r');                   /*beginning of next line*/
}

bool start = false;
void os(){
    char pass[] = " ";
    char input = ' ';
    int j = 0;
    int i = 0;

    if(start == false){
        hello();
        start = true;
    }
	for(int i = 0; i < 8; i++){
            input = get_input();
            pass[j] = input;
            printC('.');

            if (pass[j] == '\r'){
                    i = 8;
            }
            j = j+1;
    }

    printC('\r');
    int i = 0;                      /*interrupt variable for while loop*/
    while(i < j){                 /*repeat as long at the password string is*/
        printC(txt[i]);             /*print string on index i*/
        i = i+1;                    /*interrupt variable +1*/
    }
    printC('\n');                   /*print end of line*/
    printC('\r');

    i = 0;
}




void main(void){
    while(1){
            os();
    }
}
