#include <stdio.h>
#include <string.h>
#include "filesys.h"

int main(int argc, char **argv){
    printf("shell> start\n");

    format("CS3026 Operating Systems Assessment");    

    printf("Make /firstdir/seconddir\n");
    mymkdir("/firstdir/seconddir");

    MyFILE* fp = myfopen("/firstdir/seconddir/testfile1.txt","w");

    char* string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i=0; i< 4*BLOCKSIZE; i++){
        // printf("%d \t",i);
        myfputc(string[i% 26], fp);    
    }
    myfputc(ENDOFFILE,fp);
    myfclose(fp);

    mylistdir("/firstdir/seconddir");

    mychdir("/firstdir/seconddir");

    mylistdir("/firstdir/seconddir");

    fp = myfopen("testfile2.txt","w");

    for(int i=0; i< 4*BLOCKSIZE; i++){
        // printf("%d \t",i);
        myfputc(string[i% 26], fp);    
    }

    myfclose(fp);

    mymkdir("thirddir");

    fp = myfopen("thirddir/testfile3.txt","w");
    for(int i=0; i< 4*BLOCKSIZE; i++){
        // printf("%d \t",i);
        myfputc(string[i% 26], fp);    
    }
    myfclose(fp);

    mylistdir("");

    writedisk("virtualdiskA5_A1_a");

    printf("Remove testfile1.txt\n");
    myremove("testfile1.txt");

    printf("Remove testfile2.txt\n");
    myremove("testfile2.txt");

    writedisk("virtualdiskA5_A1_b");

    mychdir("thirddir");

    printf("Remove testfile3.txt\n");
    myremove("testfile3.txt");

    writedisk("virtualdiskA5_A1_c");

    mychdir("/firstdir/seconddir");

    printf("Remove thirddir\n");
    myrmdir("thirddir");

    mychdir("/firstdir");

    printf("Remove seconddir\n");
    myrmdir("seconddir");

    mychdir("..");

    printf("Remove firstdir\n");
    myrmdir("firstdir");

    writedisk("virtualdiskA5_A1_d");

    printf("shell>end\n");
}