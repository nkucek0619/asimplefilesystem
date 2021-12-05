// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Nicholas Kucek	 Date: 11/28/21

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Option L: List files
void listFiles() {
		
	int i, j;

	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		exit(0);
	}

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
		dir[i]=fgetc(floppy);

    // print directory
	int totalBytes = 0;
	printf("Disk directory:\n");
    for (i=0; i<512; i=i+16) {
		if (dir[i]==0) break;
		for (j=0; j<8; j++) {
			printf("%c",dir[i+j]);
		}
		if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf(".t"); else printf(".x");
		printf(" %5d bytes\n", 512*dir[i+10]);
		totalBytes = 512*(dir[i+10]+dir[i+9])-512;
	}
	printf("Total spaced used: %d", totalBytes);
	printf("\nTotal available space: %d\n", (512*511)-totalBytes);

/*
	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
*/
	fclose(floppy);
}

// Option P: Print file
void printFile(char* f) {

	int i;

	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		exit(0);
	}

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
	dir[i]=fgetc(floppy);

	int found = 0;
	i = 0;
	while(i < 16*dir[i] && found == 0) {
		char* temp;
		temp = &dir[i]; // temp is set to address of directory

		// if the temporary variable and filename are equal, and the character at location 9 is a t,
		// then the file is found
		if(strcmp(temp, f) == 0) { 
			found = 1;
			if(dir[i+8] == 't' || dir[i+8] == 'T') {
				printf("file found successfully\n");
				char buffer [12288];
				fseek(floppy, 512*dir[i+9], SEEK_SET);
				for(i=0; i<512; i++) {
					buffer[i] = fgetc(floppy);
					if(buffer[i] == 0) break;
					// print file contents to the screen
					printf("%c", buffer[i]);
				}
			}
			else {
				printf("executable file is not printable\n");
			}
		}
		i = i+16;
	}
	if(found == 0) printf("file not found or is empty\n");

	fclose(floppy);
}

// Option M: Create and store a text file
void createFile(char* f) {

	int i, j, fd = 0, ffd = 0, ffs = 0;
	
	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		exit(0);
	}

	//load the disk map from sector 256
	char map[512];
	fseek(floppy,512*256,SEEK_SET);
	for(i=0; i<512; i++)
	map[i]=fgetc(floppy);

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
	dir[i]=fgetc(floppy);

	// copy file name into c-string
	char str[512];
	for (int k = 0; k < 512; k++) { 
		if (f[k] == 0) { 
			str[k] = 0;
		} else {
			str[k] = f[k];
		}
	}

	// loop through each file entry in the directory using 16-byte iterations
	// until a free space is found
    for (i = 0; i < 512; i+=16) {
		if (ffd != 1 && dir[i] == 0) {
    		fd = i;
    		ffd = 1;
			// file already exists, or the name is invalid
    	}	else if (strcmp(&dir[i], str) == 0) { 
			printf("duplicate or invalid filename\n");
    		fd = -1;
    		break;
    	}
	}

	// if any sector in the map is set equal to 0, it is free
	for(i = 0; i < 512; i++) {
		if(map[i] == 0) {
			ffs = 1;
			break;
		}
	}

	// if there is free space in the directory and free space in the map,
	// insert the new file there
	if(fd > 0 && ffs == 1) {
		for(j = 0; j < 8; j++) {
			if(j < strlen(str)) {
				dir[fd+j] = str[j];
			} else {
				dir[fd] = 0;
		}
	}

	dir[fd+8] = 't';
	dir[fd+9] = fd;

	// use floppy to point and write to the free space in the directory for the file type
	fseek(floppy, 512*dir[fd+9], SEEK_SET);
	for(i = 0; i < strlen(str); i++) {
		fputc(str[i], floppy);
	}

	// if a sector is free, set it to -1 (0xff)
	int flag = 0;
	for(int k = 0; k < 512; k++) {
		if(map[k] == 0 && flag == 0) {
			map[k] = -1;
			dir[fd+9] = k;
			dir[fd+10] = 1;
			flag = 1;
			break;
		}
	}
	
	printf("file created successfully\n");

	//write the map and directory back to the floppy image
	fseek(floppy,512*256,SEEK_SET);
	for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
	}

	fclose(floppy);
}

// Option D: Delete file
void deleteFile(char* f) {

	int i, j, occupied = 0, fd = 0, fs = 0;
	
	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		exit(0);
	}

	//load the disk map from sector 256
	char map[512];
	fseek(floppy,512*256,SEEK_SET);
	for(i=0; i<512; i++)
	map[i]=fgetc(floppy);

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
	dir[i]=fgetc(floppy);

	// copy file name into c-string
	char str[512];
	for (int k = 0; k < 512; k++) { 
		if (f[k] == 0) { 
			str[k] = 0;
		} else {
			str[k] = f[k];
		}
	}

	// loop through each file entry in the directory using 16-byte iterations
	// until the file is found
    for (i = 0; i < 512; i+=16) {
		if (strcmp(&dir[i], str) == 0) {
    		occupied = i;
    		fd = 1;
			// file does not exist, or is an invalid name
    	}	else if (fd != 1 && dir[i] == 0) { 
			printf("file not found\n");
    		occupied = -1;
    		break;
    	}
	}

	// if any sector in the map is set equal to -1, it is occupied
	for(i = 0; i < 512; i++) {
		if(map[i] == -1) {
			fs = 1;
			break;
		}
	}

	// if there is occupied space in the directory and non-zero values in the map,
	// set each file bit to 0
		for(j = 0; j < 8; j++) {
			if(j < strlen(str)) 
				dir[occupied+j] = 0;
		}

	// use floppy to point and set each bit equal to 0 for the occupied space in the directory for the file type
	fseek(floppy, 512*dir[occupied+8], SEEK_SET);
	for(i = 0; i < strlen(str); i++) {
		fputc(0, floppy);
	}

	// set all map bits back to 0
	int flag = 0;
	for(int k = 0; k < 512; k++) {
		if(map[k] == -1 && flag == 0) {
			map[k] = 0;
			dir[occupied+8] = 0;
			dir[occupied+9] = 0;
			dir[occupied+10] = 0;
			flag = 1;
			break;
		}
	}

	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);

	fclose(floppy);
}

// function prototypes
void listFiles();
void printFile(char*);
void createFile(char*);
void deleteFile(char*);

int main(int argc, char* argv[]) {

	int i, j, size, noSecs, startPos, switchArgs;
	// char* tempString = argv[2];

	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		return 0;
	}

	//load the disk map from sector 256
	char map[512];
	fseek(floppy,512*256,SEEK_SET);
	for(i=0; i<512; i++)
	map[i]=fgetc(floppy);

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
	dir[i]=fgetc(floppy);

	// if more than two arguments are in input, call the proper function
	// commands are in the following format:
	// ./filesys -D <filename> delete the named file from the disk
    // ./filesys -L list the files on the disk
    // ./filesys -M <filename> create a text file and store it to disk
    // ./filesys -P <filename> read the named file and print it to screen

	while((switchArgs = getopt(argc, argv, "LPMD")) != -1) {
		switch(switchArgs) {
			case 'L':
				listFiles();
				return 0;
			case 'P':
			    printFile(argv[2]);
				return 0;
			case 'M':
				createFile(argv[2]);
				return 0;
			case 'D':
				deleteFile(argv[2]);
				return 0;
			default:
				break;
		}
	}

    //print disk map
	printf("Disk usage map:\n");
	printf("      0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
	printf("     --------------------------------\n");
	for (i=0; i<16; i++) {
		switch(i) {
			case 15: printf("0xF_ "); break;
			case 14: printf("0xE_ "); break;
			case 13: printf("0xD_ "); break;
			case 12: printf("0xC_ "); break;
			case 11: printf("0xB_ "); break;
			case 10: printf("0xA_ "); break;
			default: printf("0x%d_ ", i); break;
		}
		for (j=0; j<16; j++) {
			if (map[16*i+j]==-1) printf(" X"); else printf(" .");
		}
		printf("\n");
	}

    // print directory
	printf("\nDisk directory:\n");
	printf("Name    Type Start Length\n");
    for (i=0; i<512; i=i+16) {
		if (dir[i]==0) break;
		for (j=0; j<8; j++) {
			if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
		}
		if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf("text"); else printf("exec");
		printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
	}

/*
	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
*/

	fclose(floppy);
}
