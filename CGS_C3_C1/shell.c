#include <stdio.h>
#include <string.h>
#include "filesys.h"

int main(int argc, char **argv){
    printf("shell> start\n");

    format("CS3026 Operating Systems Assessment");    

    MyFILE* fp = myfopen("testfile.txt","w");

    printf(">Write to file\n");

    char* string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i=0; i< 4*BLOCKSIZE; i++){
        // printf("%d \t",i);
        myfputc(string[i% 26], fp);    
    }
    myfputc(ENDOFFILE,fp);

    printf("Close file.\n");
    myfclose(fp);

    printf("Write to disk\n");
    writedisk("virtualdiskC3_C1");

    printf("Open file for reading.\n");
    fp = myfopen("testfile.txt","r");

    printf("Start reading.\n");
    char c;
    while((c=myfgetc(fp))!=ENDOFFILE){
        printf("%c",c);
    }
    printf("\nEnd reading\n");

    printf("Close file.\n");
    myfclose(fp);

    printf("shell> end\n");
}


