#include <stdio.h>
#include <string.h>
#include "filesys.h"

int main(int argc, char **argv){
    printf("shell> start\n");

    format("CS3026 Operating Systems Assessment");    
    writedisk("virtualdiskD3_D1");

    printf("shell> end\n");
}