/* needs to stay the first line */
asm(".code16gcc\njmp $0, $main");

/* space for additional code */

/*Typedefinitions*/
typedef int bool;                   /*boolean*/
#define true 1
#define false 0

char pass[9];
char newLine[]="\n";
char clear[]="";
bool run = true;

/*Methods*/
void printC(char ch){               /*print character to screen*/
    asm("mov $0x0e, %%ah;"          /*loading function 0eh (print character) into register AH*/
        "mov %0, %%al;"             /*saving content of %0 into register AL*/
        "int $0x10" :: "r"(ch));    /*interrupt 10h (video services and r = character to print*/
}

/*fehlerhaft, wartet nicht auf user input vor verrbeitung*/
char get_input(){                   /*Get keyboard input*/
    char c;                         /*variable to save keyboard input into*/
    asm volatile(
                 "mov %%ah, 1;"     /*loading function 01h into register AH*/
                 "int $0x16;"       /*interrupt 16h (keyboard services)*/
                 :"=r"(c)           /*saving the pressed key(ASCII code) into c*/
                 :
                 :      );
    return c;                       /*return pressed key (ASCII code)*/
}

void reboot(){                      /*reboots the system*/
    asm("int $0x19;");
}

void print_string(char msg[], int j){
    /*char msg[] = "Hello!\n";*/        /*string with welcome message*/
    int i = 0;                      /*interrupt variable for the while loop*/
    while(i <= j){                   /*repeat 8 times (welcome string has 8 characters)*/
        printC(msg[i]);             /*print character function with character to print*/
        i = i+1;                    /*interrupt variable +1*/
    }
    printC('\r');
}
/*print password in undisclosed with string and counter variable*/

void print_pass(){
    printC(46);
}

//char* hidden_pass_input(){
//    for(int i = 0; i<=8; i++){
//            char tmp = get_input();
//            pass[i]=tmp;
//            print_pass();
//            if (tmp == 13) {
//                break;
//            }
//    }
//    print_string(newLine, 2);
//    return pass;
//}

void os(){
    print_string(newLine,2);
    
    for(int i = 0; i<8; i++){
        char tmp = get_input();
        switch (tmp) {
            case 13:    print_string(newLine, 2);
                        print_string(pass, i+1);
                        break;
        }
        print_pass();
        pass[i]=tmp;
    }
    
    print_string(newLine, 2);
    if (get_input() == 13) {
        print_string(pass, 8);
    }
    print_string(newLine, 2);
}

void main(void){
    asm(
		"mov $0x007, %%ebx;"
		"mov $0xE48, %%eax; int $0x10;"
		"mov $0xE65, %%eax; int $0x10;"
		"mov $0xE6C, %%eax; int $0x10;"
		"mov $0xE6C, %%eax; int $0x10;"
		"mov $0xE6F, %%eax; int $0x10;"
		"mov $0xE21, %%eax; int $0x10;"
        ::: "eax", "ebx"
	);
    
    os();
    
    asm(
        "jmp .;"
    );
}
