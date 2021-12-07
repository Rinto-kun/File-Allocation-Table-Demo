### Introduction
Name: Matey Krastev | Student ID: 51987311

### How-to use
In the terminal, run the following command: "make all".
Then, in the terminal, run the following command to see the results: "./shell".
You may also inspect the provided virtualdisk_* files and traces.

### Global remarks and instructions
A default execution procedure is provided in the shell.c file of all submissions, however, you can change it to suit your own preferences. Wherever the variable "path" is seen, it can be either relative or absolute, that is, it can either work within the current folder or navigate from the root folder to the desired folder.

. and .. are implemented to the best of my ability.

### void format(char * )
Format takes one input and that is the name of the drive with optional text that could be added.
It sets the FAT up as well as the virtualDisk, and the root folder, and defines all the necessary variables like rootDirIndex.
Note: It is called root folder but this is only for ease of reading in hex format.

### MyFILE* myfopen(char* path, char* mode)
This function takes an absolute or relative path and creates a file descriptor object that may be utilized by the user along with myfputc() and myfgetc() to write and read from, respectively.
The file must be closed with myfclose() in order to free the program memory and avoid issues.

Notes: This function went through a few iterations, but should be working properly currently.

### void myfputc(int character, MyFILE *file)
Takes a MyFILE pointer and a character, and performs a write operation while at the same advancing the pointer to the next free Byte in the virtual memory. 
If the pointer goes over the block, then a new block will be provided and the connection to the previous one kept by means of blockchaining. The pointer is also reset in this operation.

### int myfgetc(MyFILE *file)
Returns a single character from the buffer and advances the pointer. It behaves similarly to myfputc whereas it will also go to the next block in the blockchain when the pointer exceeds the size of a single block.

### void myfclose(MyFILE *file)
Frees up the memory occupied by the file object and writes the block to the disk. 

### void mymkdir(char *path)
Creates a new folder with the specified path. If any folders in the path do not exist within the virtual memory, it will create them too and assign the appropriate structure.

### char** mylistdir(char *path)
Returns a list of the contents of the folder specified within the path. The path may be relative or absolute, which is the case in the entire program.

### void mychdir(char *path)
Changes the global variable currentDirIndex by navigating through the path to reach the required destination folder. If a folder within the path is not found, it will raise an error.

### void myrmdir(char *path)
Removes a single directory from the virtual disk as long as it is empty at the time of removal. Otherwise, it will raise an error.

### void myremove(char *path)
Removes a single file.

### Requirements and considerations
The software was tested in the Codio environment, as well as locally on a Windows machine using MINGW64.
The program was compiled using gcc. Please make sure you have at least one of these.