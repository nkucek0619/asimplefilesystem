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

// Option L: List files
void listFiles() {
		
	int i, j;

	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		return;
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
void printFile(FILE* floppy, char sector, char size) {

	char* buffer[12288];
	int temp = 0, count = 0;

	int fileFound = fseek(floppy, sector, SEEK_SET);

	//printf("%c", fileFound);
	//printf("\n");

	/*if(fileFound!=0) {
		printf("file not found\n");
		return;
	}
	else if (fileFound==0) {
		printf("executable file is not printable");
		return;
	}*/

	while((temp = fgetc(floppy)) != EOF) {
		// display read character
		printf("%c", temp);
	}

	while(buffer[count] != '0') {
		printf("%s", buffer[count]);
		if(count<0 && count%30==0) printf("\n");
		count++;
	}
	return;
}

// Option M: Create and store a text file
void createFile() {

}

// Option D: Delete file
void deleteFile() {

}

// function prototypes
void listFiles();
void printFile(FILE*, char, char);
void createFile();
void deleteFile();

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
	// ./filesys -D filename delete the named file from the disk
    // ./filesys -L list the files on the disk
    // ./filesys -M filename create a text file and store it to disk
    // ./filesys -P filename read the named file and print it to screen

	while((switchArgs = getopt(argc, argv, "LPMD")) != -1) {
		switch(switchArgs) {
			case 'L':
				listFiles();
				return 0;
			case 'P':
			    printFile(floppy, noSecs, size);
				return 0;
			case 'M':
				return 0;
			case 'D':
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
