/* filesys.h
 * 
 * describes FAT structures
 * http://www.c-jump.com/CIS24/Slides/FAT/lecture.html#F01_0020_fat
 * http://www.tavi.co.uk/phobos/fat.html
 */

#ifndef FILESYS_H
#define FILESYS_H

#include <time.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MAXBLOCKS     1024
#define BLOCKSIZE     1024
#define FATENTRYCOUNT (BLOCKSIZE / sizeof(fatentry_t))
#define DIRENTRYCOUNT ((BLOCKSIZE - (2* sizeof(int)) ) / sizeof(direntry_t))
#define MAXNAME       26
#define MAXPATHLENGTH 1024

#define UNUSED        -1
#define ENDOFCHAIN     0

#ifndef ENDOFFILE
#define ENDOFFILE     -1
#endif


typedef unsigned char Byte ;

/* create a type fatentry_t, we set this currently to short (16-bit)
 */
typedef short fatentry_t ;



//const int   fatentrycount = (blocksize / sizeof(fatentry_t)) ;

// a FAT block is a list of 16-bit entries that form a chain of disk addresses
typedef fatentry_t fatblock_t [ FATENTRYCOUNT ] ;


/* create a type direntry_t
*/

typedef struct direntry_t {
   int         entrylength ;   // records length of this entry (can be used with names of variables length)
   Byte        isdir ;
   Byte        used ;
   time_t      modtime ;
   int         filelength ;
   fatentry_t  firstblock ;
   char   name [MAXNAME] ;
} direntry_t ;


//const int   direntrycount = (blocksize - (2*sizeof(int)) ) / sizeof(direntry_t) ;

// a directory block is an array of directory entries
typedef struct dirblock_t {
   int isdir ;
   int nextEntry ;
   direntry_t entrylist [ DIRENTRYCOUNT ] ;
} dirblock_t ;


// a data block holds the actual data of a filelength, it is an array of 8-bit (byte) elements
typedef Byte datablock_t [ BLOCKSIZE ] ;


// a diskblock can be either a directory block, a FAT block or actual data
typedef union block {
   datablock_t data ;
   dirblock_t  dir  ;
   fatblock_t  fat  ;
} diskblock_t ;

// finally, this is the disk: a list of diskblocks
// the disk is declared as extern, as it is shared in the program
// it has to be defined in the main program filelength
extern diskblock_t virtualDisk [ MAXBLOCKS ] ;


// when a file is opened on this disk, a file handle has to be
// created in the opening program
typedef struct filedescriptor {
   char        mode[3] ;
   fatentry_t  blockno ;           // block no
   int         pos     ;           // byte within a block
   diskblock_t buffer  ;
} MyFILE ;



void format() ;
void writedisk ( const char * filename ) ;
void readdisk ( const char * filename ) ;

// Prints the contents of the disk
void printdisk();

// Opens a file on the virtual disk and manages a buffer for it of size BLOCKSIZE.
// mode may be either "r" for readonly or "w" for read/write/append (default "w") 
MyFILE* myfopen(char* filename,const char* mode);

// Writes a byte to the file. Depending on the write policy, either writes the disk block containing the 
// written byte to disk, or waits until block is full  
void myfputc(int b, MyFILE* stream);

// Returns the next byte of the open file, or EOF (EOF == -1) 
int myfgetc(MyFILE* stream);

// Closes the file, writes out any blocks not written to disk 
void myfclose(MyFILE* stream);

// Prints the block at virtualDisk[pos] for debugging purposes.
void printBlock(int);

// Make a directory via the path specified.
// Path could be absolute or relative.
void mymkdir(char* path);

// Lists the contents of a directory including files and subfolders (I assume).
char ** mylistdir (char * path);

int mychdir(char* path);

void myremove(char* path);

// Removes a directory via a path. The directory must be empty before that.
void myrmdir(char* path);

#endif

/*
#define NUM_TYPES (sizeof types / sizeof types[0])
static* int types[] = { 
    1,
    2, 
    3, 
    4 };
*/