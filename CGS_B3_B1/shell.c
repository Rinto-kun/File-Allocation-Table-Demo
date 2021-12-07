#include <stdio.h>
#include <string.h>
#include "filesys.h"

int main(int argc, char **argv){
    printf("shell> start\n");

    format("CS3026 Operating Systems Assessment");    

    printf("Make /myfirstdir/myseconddir/mythirddir\n");
    mymkdir("/myfirstdir/myseconddir/mythirddir");

    mylistdir("/myfirstdir/myseconddir");

    writedisk("virtualdiskB3_B1_a");

    printf("Open file for writing.\n");
    MyFILE* fp = myfopen("/myfirstdir/myseconddir/testfile.txt","w");

    char* string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i=0; i< 4*BLOCKSIZE; i++){
        // printf("%d \t",i);
        myfputc(string[i% 26], fp);    
    }
    myfputc(ENDOFFILE,fp);

    printf("Close file.\n");
    myfclose(fp);

    mylistdir("/myfirstdir/myseconddir");

    writedisk("virtualdiskB3_B1_b");

    printf("shell>end\n");
}