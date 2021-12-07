
/* filesys.c
 * 
 * provides interface to virtual disk
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "filesys.h"


diskblock_t   virtualDisk[MAXBLOCKS] ;      // define our in-memory virtual disk, with MAXBLOCKS(1024) blocks
fatentry_t    FAT[MAXBLOCKS] ;              // define a file allocation table with MAXBLOCKS(1024) 2-byte (short) entries
fatentry_t    rootDirIndex    = 0 ;         // rootDir will be set by format
direntry_t*   currentDir      = NULL ;
fatentry_t    currentDirIndex = 0 ;
fatentry_t    tempDirIndex = 0;     //  Holder for the currentDirIndex in case you are modifying folders along the way. Always reset the value of currentDirIndex at the end of such a function's execution.

/* writedisk : writes virtual disk out to physical disk
 * 
 * in: file name of stored virtual disk
 */
void writedisk ( const char * filename )
{
    // printf("%s",virtualDisk);
    FILE * dest = fopen( filename, "w" ) ;
    if ( fwrite ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
        fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
    // write( dest, virtualDisk, sizeof(virtualDisk) ) ;
    fclose(dest) ;
}

void readdisk ( const char * filename )
{
    FILE * dest = fopen( filename, "r" ) ;
    if ( fread ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
        fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
    //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
    fclose(dest) ;
}

void printdisk(){
    printf("%s",virtualDisk);
}


/* the basic interface to the virtual disk
 * this moves memory around
 */
void writeblock ( diskblock_t * block, int block_address )
{
    //printf ( "writeblock> block %d = %s\n", block_address, block->data ) ;
    memmove ( virtualDisk[block_address].data, block->data, BLOCKSIZE ) ;
    //printf ( "writeblock> virtualdisk[%d] = %s / %d\n", block_address, virtualDisk[block_address].data, (int)virtualDisk[block_address].data ) ;
}


/* read and write FAT
 * 
 * please note: a FAT entry is a short, this is a 16-bit word, or 2 bytes
 *                  our blocksize for the virtual disk is 1024, therefore
 *                  we can store 512 FAT entries in one block
 * 
 *                  how many disk blocks do we need to store the complete FAT:
 *                  - our virtual disk has MAXBLOCKS blocks, which is currently 1024
 *                     each block is 1024 bytes long
 *                  - our FAT has MAXBLOCKS entries, which is currently 1024
 *                     each FAT entry is a fatentry_t, which is currently 2 bytes
 *                  - we need (MAXBLOCKS /(BLOCKSIZE / sizeof(fatentry_t))) blocks to store the
 *                     FAT
 *                  - each block can hold (BLOCKSIZE / sizeof(fatentry_t)) fat entries
 */


diskblock_t emptyBlock(){
    diskblock_t block;
    for(int i=0; i<BLOCKSIZE; i++) block.data[i] = '\0';
    return block;
}

/*  copies the content of the FAT into one or more blocks, 
    then write these blocks to the virtual disk*/
void copyFAT(){
    diskblock_t block = emptyBlock();
    int i,j=MAXBLOCKS/2;
    for(i=0; i<MAXBLOCKS/2; i++)
        block.fat[i] = FAT[i];
    writeblock(&block,1);
    for(i=MAXBLOCKS/2; i<MAXBLOCKS;i++)
        block.fat[i-j] = FAT[i];
    writeblock(&block,2);
}

void format(char* driveName)
{
    diskblock_t block ;
    direntry_t  rootDir ;
    int         pos             = 0 ;
    int         fatentry        = 0 ;
    int         fatblocksneeded =  (MAXBLOCKS / FATENTRYCOUNT ) ;

    /* prepare block 0 : fill it with '\0',
    * use strcpy() to copy some text to it for test purposes
	* write block 0 to virtual disk*/

    block = emptyBlock();

    strcpy(block.data,driveName);

    writeblock(&block,0);


	/* prepare FAT table
	 * write FAT blocks to virtual disk
	*/

    // First, reset the FAT, make all entries unused
    for(int i=0; i<MAXBLOCKS; i++) FAT[i]=UNUSED;

    // FAT[0] is unused for some reason, it's the start ig. FAT 1 and 2 correspond to the FAT itself.
    // FAT[1] points to FAT[2] and FAT[2] points to FAT[3] which is EOC aka "Stop here"

    // Never mind the above, maybe I was just confused.
    FAT[0] = ENDOFCHAIN;
    FAT[1] = 2;
    FAT[2] = ENDOFCHAIN;

    // FAT[2] = 3;
    // FAT[3] = ENDOFCHAIN;

    copyFAT();

	 /* prepare root directory
	  * write root directory block to virtual disk
	  */

    block = emptyBlock();
    block.dir.isdir = TRUE;
    block.dir.nextEntry = 0;
    strcpy(block.dir.entrylist[0].name,"root");
    block.dir.entrylist[0].used = TRUE;
    block.dir.entrylist[0].firstblock = 3;

    writeblock(&block,3);
    FAT[3] = ENDOFCHAIN;
    copyFAT();

    rootDirIndex=3;
    currentDirIndex=3;
}


/* use this for testing */
void printBlock ( int blockIndex )
{
    printf ( "virtualdisk[%d] = %s\n", blockIndex, virtualDisk[blockIndex].data ) ;
}


// Navigates to a file by changing the currentDirIndex.
// Returns the filename that is provided from a path.
char* navigateToFile(char* filename){
    char* str = malloc(MAXPATHLENGTH);
    int count=0, i=0;
    char* token;

    char** dirs = malloc(MAXPATHLENGTH);

    // Count the number of folders to be opened.
    strcpy(str,filename);
    for (char *rest = NULL, *token = strtok_r(str, "/", &rest); 
        token != NULL; 
        token = strtok_r(NULL, "/", &rest),count++) {
            dirs[count] = malloc(MAXNAME);
            strcpy(dirs[count],token);
    }


    // Refresh str. This is needed.
    free(str);
    str = malloc(MAXPATHLENGTH);
    strcpy(str,filename);

    // If absolute path
    if(strchr(str,'/')==str) currentDirIndex = rootDirIndex;
    for(i=0;i<count;i++){
        // printf("%d      %s\n",currentDirIndex,dirs[i]);
        if(i<count-1){
            // Handle error case while simulateneously navigating to the folder.
            if(mychdir(dirs[i])==-1) {
                exit(9);
            }
        }
        else{
            filename = dirs[i];
        }
    }
    free(dirs);
    free(str);
    return filename;
};

MyFILE* myfopen(char* filename, const char* mode){
    tempDirIndex = currentDirIndex;

    printf("myfopen> start (args: %s)\n",filename);
    if(strcmp(mode,"r")!=0 && strcmp(mode,"w")!=0){
        printf("File mode is incorrect");
        return FALSE;
    }    
    // If the file contains a path to a folder.
    filename = navigateToFile(filename);

    // Check if file exists before
    int fileExists = FALSE;
    int pos;

    int freeListPos = 0;

    // Position of block in the virtualDisk.
    diskblock_t block = virtualDisk[currentDirIndex];


    // Check for file existence in dir block.
    for(int i=2; i<DIRENTRYCOUNT;i++){
        // This file is empty, don't check next.
        if(block.dir.entrylist[i].used==FALSE) continue;

        //Check whether the name matches and file exists.
        if(strcmp(block.dir.entrylist[i].name,filename)==0){
            pos = i;
            fileExists = TRUE;
            break;
        }
    }

    MyFILE * file = malloc(sizeof(MyFILE));

    // If file already exists, return the reference to it.
    if (fileExists) {
        file->blockno = block.dir.entrylist[pos].firstblock;
        file->buffer = virtualDisk[block.dir.entrylist[pos].firstblock];
    }
    else{
        // The file does not exist but you selected readmode? Can't do that.
        // Read mode / False is returned
        if (strcmp("r",mode)==0){
            printf("Incorrect mode selected. The file does not exist.");
            return NULL;
        }

        
        // Write mode selected, find a free position in the directory.
        for(int i=2; i<DIRENTRYCOUNT;i++){
            if(!block.dir.entrylist[i].used){
                freeListPos = i;
                break;
            }
        }

        // Look for a free FAT entry, start from index 4 (right after rootdir).
        for(pos=4; pos<MAXBLOCKS;pos++){
            if(FAT[pos]==UNUSED){
                FAT[pos]=ENDOFCHAIN;
                break;
            }
        }
        copyFAT();

        // Copy the name of the file in the directory block.
        strcpy(block.dir.entrylist[freeListPos].name,filename);

        // Position of new file in the directory (virtualDisk)
        block.dir.entrylist[freeListPos].firstblock = pos;
        block.dir.entrylist[freeListPos].used = TRUE;

        writeblock(&block,currentDirIndex);

        file->blockno = pos;
        file->buffer = virtualDisk[pos];
    }

    // File to return.

    // Copy the mode and reset the position.
    strcpy(file->mode,mode);
    file->pos = 0;

    copyFAT();

    printf("myfopen> end\n");

    currentDirIndex = tempDirIndex;
    return file;
}


void myfputc(int b, MyFILE* stream){
    if(stream == NULL || strcmp(stream->mode,"r")==0){
        printf("Incorrect mode selected.\n");
        return;
    }
    
    diskblock_t* buffer = &stream->buffer;

    buffer->data[stream->pos] = b;
    stream->pos++;

    if(stream->pos >= BLOCKSIZE){
        writeblock(buffer,stream->blockno);

        // Extend the chain.
        int start_pos = stream->blockno;
        for(int pos=start_pos+1; pos<MAXBLOCKS;pos++){
            if(FAT[pos]==UNUSED){
                FAT[start_pos] = pos;
                FAT[pos]=ENDOFCHAIN;

                stream->buffer = virtualDisk[pos];
                stream->blockno = pos;
                stream->pos = 0;
                break;
            }
        }

        copyFAT();
    }
}

// Early version of how I imagine it would work.
int myfgetc(MyFILE* stream){
    if(strcmp(stream->mode,"w")==0){
        printf("Incorrect mode selected for reading from file.\n");
        return 0;
    }
    diskblock_t* buffer = &stream->buffer;
    int ret = buffer->data[stream->pos];
    stream->pos++;
    if(stream->pos >= BLOCKSIZE){
        stream->buffer = virtualDisk[stream->blockno];
        stream->blockno = FAT[stream->blockno];
        stream->pos = 0;
    }
    return ret;
}

void myfclose(MyFILE* stream){
    // Closes the file, write out any blocks not written to the disk yet.
    writeblock(&stream->buffer,stream->blockno);
    free(stream);
}

direntry_t setDirEntry(direntry_t dir, int entrylength, fatentry_t firstblock, int isdir, Byte used, char* name){
    dir.entrylength = entrylength;
    dir.firstblock = firstblock;
    dir.isdir = isdir;
    dir.used = used;
    dir.filelength = entrylength;
    dir.modtime = 0;
    if(strcmp(name,"\0")==0) memset(dir.name,0,strlen(dir.name));
    else strcpy(dir.name,name);
    return dir;
}

// Sets the dir up in a quite unconventional way I guess.
// Returns the index of the dir being modified.
fatentry_t dirMaker(char* name){
    fatentry_t i;
    for( i=3; i<MAXBLOCKS;i++){
        if(FAT[i]==UNUSED) {
            FAT[i] = ENDOFCHAIN;
            copyFAT();
            break;
        }
    }
    diskblock_t dir = virtualDisk[i];

    dir.dir.isdir = TRUE;

    FAT[currentDirIndex] = i;

    dir.dir.entrylist[0] = setDirEntry(dir.dir.entrylist[0], 1,i,TRUE,TRUE,".");
    dir.dir.entrylist[1] = setDirEntry(dir.dir.entrylist[1], 2,currentDirIndex,TRUE,TRUE,"..");

    // Save the directory. It's probably a bit overcomplicated but it should work.

    writeblock(&dir,i);

    return i;
}

void mymkdir(char* path){

    tempDirIndex = currentDirIndex;

    // Prepare the variables to be used for the tokenizing.
    char* str = malloc(strlen(path)+1);
    strcpy(str,path);
    
    int immediateDirIndex, code;
    diskblock_t curBlock;
    if(path[0]=='/'){ immediateDirIndex = rootDirIndex;}
    else immediateDirIndex = currentDirIndex;
    // How it works: Loop through the path creating more folders along the way and updating the index.
    for (char *rest = NULL, *token = strtok_r(str, "/", &rest); token != NULL; token = strtok_r(NULL, "/", &rest)) {

        // Try to navigate
        // code = mychdir(token);

        // if(code==-1){
            // The folder doesn't exist. We have to create it from here.
            curBlock = virtualDisk[immediateDirIndex];
            for(int i=2; i<DIRENTRYCOUNT;i++){
                if(curBlock.dir.entrylist[i].used==FALSE){
                    immediateDirIndex = dirMaker(token);
                    curBlock.dir.entrylist[i] = setDirEntry(
                        curBlock.dir.entrylist[i],
                        strlen(token),
                        immediateDirIndex,
                        TRUE,
                        TRUE,
                        token
                    );

                    writeblock(&curBlock,currentDirIndex);
                    currentDirIndex = immediateDirIndex;
                    break;
                }
                // Check if the folder names are the same
                else if(strcmp(curBlock.dir.entrylist[i].name,token)==0){
                    immediateDirIndex = curBlock.dir.entrylist[i].firstblock;
                    currentDirIndex = immediateDirIndex;
                    break;
                }
                currentDirIndex = immediateDirIndex;
            }


        // }


        // Possible TODO:    Handle long chains of path i.e. make the dir block have a chain similar to the file one.
        // curBlock = virtualDisk[immediateDirIndex];
        // for(int i=2; i<DIRENTRYCOUNT;i++){
        //     if(curBlock.dir.entrylist[i].used==FALSE){
                
        //         curBlock.dir.isdir = TRUE;
        //         immediateDirIndex = dirMaker(token);   
        //         curBlock.dir.entrylist[i] = setDirEntry(
        //             curBlock.dir.entrylist[i],
        //             strlen(token),
        //             immediateDirIndex,
        //             TRUE,
        //             TRUE,
        //             token
        //         );

        //         writeblock(&curBlock,currentDirIndex);
        //         break;
        //     }
        // }
        currentDirIndex = immediateDirIndex;
    }

    // In the end, reset the currentDirIndex.
    currentDirIndex = tempDirIndex;
    free(str);
}

// Prints the directory and also returns it.
char ** mylistdir (char * path){
    // If it can't open, don't open it.
    tempDirIndex = currentDirIndex;
    if(mychdir(path)==-1) {
        printf("Directory can't be found.\n");
        return NULL;
    }

    char** res = malloc(DIRENTRYCOUNT*MAXNAME*sizeof(char));
    int i = 0;
    diskblock_t block = virtualDisk[currentDirIndex];


    res[0] = malloc(MAXNAME);
    strcpy(res[0],block.dir.entrylist[0].name);
    printf("Directory entries in path %s:\n",path);
    
    for(i=2;i<DIRENTRYCOUNT && strlen(block.dir.entrylist[i].name)>1;i++){
        res[i-1] = malloc(MAXNAME);
        strcpy(res[i-1],block.dir.entrylist[i].name);
        printf("%d\t  %s \n",i-1,res[i-1]);
    }

    currentDirIndex = tempDirIndex;
    free(res);
    return res;
}

// Changes the current directory without creating new folders. 
// If an unspecified folder is mentioned, the program will exit.
// For now the program only works with absolute path.
int mychdir(char* path){
    char* str = malloc(sizeof(path)+1);
    strcpy(str,path);
    diskblock_t curBlock;
    int immediateDirIndex;
    
    if(strchr(str,'/')==str) immediateDirIndex = rootDirIndex;
    else immediateDirIndex = currentDirIndex;

    free(str);
    str = malloc(sizeof(path)+1);
    strcpy(str,path);

    for (char *rest = NULL, *token = strtok_r(str, "/", &rest); token != NULL; token = strtok_r(NULL, "/", &rest)) {
        Byte flag = FALSE;
        curBlock = virtualDisk[immediateDirIndex];
        for(int i=0; i<DIRENTRYCOUNT;i++){
            if(curBlock.dir.entrylist[i].used==TRUE && strcmp(curBlock.dir.entrylist[i].name,token)==0){
                immediateDirIndex = curBlock.dir.entrylist[i].firstblock;
                flag = TRUE;
                break;
            }
        }
        // Flag here checks whether the folder has actually been found.
        if(flag==FALSE) {
            printf("An error has occurred. Folder %s does not exist at index %d.\n",token,immediateDirIndex);
            return -1;
        }
    }
    free(str);
    currentDirIndex = immediateDirIndex;
    return immediateDirIndex;
}


// Removes a file.
void myremove(char* path){
    tempDirIndex = currentDirIndex;
    char* filename = navigateToFile(path);

    diskblock_t block = virtualDisk[currentDirIndex];
    fatentry_t pos;
    fatentry_t prevPos;

    // Find the dir entry of the file and delete it first.
    for(int i=0; i<DIRENTRYCOUNT;i++){
        if(strcmp(filename,block.dir.entrylist[i].name)==0){
            pos = block.dir.entrylist[i].firstblock;
            block.dir.entrylist[i] = setDirEntry(block.dir.entrylist[i],0,0,FALSE,FALSE,"\0");
            writeblock(&block,currentDirIndex);
            break;
        }
    }
    
    // Go through the blockchain and delete the contents of the file.
    do{
        // printf("FAT[%d] = %d\n",pos,FAT[pos]);

        // Empty the contents of the block and write it to memory.
        block = virtualDisk[pos];
        memset(block.data,0,BLOCKSIZE);
        writeblock(&block, pos);

        // Advance the blockchain.
        prevPos = pos;
        pos = FAT[pos];
        FAT[prevPos]=UNUSED;
    }
    while(FAT[pos]!=ENDOFCHAIN);

    // Finally, copy the FAT table.
    copyFAT();

    currentDirIndex = tempDirIndex;
}

void myrmdir(char* path){
    tempDirIndex = currentDirIndex;

    mychdir(path);

    diskblock_t block = virtualDisk[currentDirIndex];

    for(int i=2; i<DIRENTRYCOUNT;i++){
        if(strcmp(block.dir.entrylist[i].name,"\0")!=0){
            printf("The directory %s is not empty. Please, empty it first before removing.\n",path);
            return;
        }
    }

    fatentry_t parentPos = block.dir.entrylist[1].firstblock;
    block = virtualDisk[parentPos];
    // Atttempted try at fixing this.
    for(int i=2; i<DIRENTRYCOUNT;i++){
        if(block.dir.entrylist[i].firstblock==currentDirIndex){
            block.dir.entrylist[i] = setDirEntry(block.dir.entrylist[i],0,0,FALSE,FALSE,"\0");
            writeblock(&block,parentPos);
            break;
        }
    }


    fatentry_t prevPos, pos = currentDirIndex;
    do
    {
        printf("FAT[%d] = %d\n",pos,FAT[pos]);

        // Empty the contents of the block and write it to memory.
        block = virtualDisk[pos];
        memset(block.data,0,BLOCKSIZE);
        writeblock(&block, pos);

        // Advance the blockchain.
        prevPos = pos;
        pos = FAT[pos];
        FAT[prevPos]=UNUSED;
    }
    while(FAT[pos]!=ENDOFCHAIN);

    copyFAT();
    currentDirIndex = tempDirIndex;
}