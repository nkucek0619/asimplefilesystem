# A Simple File System
## O.S. Lab 3: A Simple File System
A floppy disc image file is an exact sector-by-sector copy of the floppy disc. It can be used to create a backup of the floppy disc. In virtual machines, a floppy disc image file can be used to transfers files to or from virtual machine systems.
## Overview of the Lab
The purpose of this lab is to turn filesys.c into a simple tool for manipulating the files stored on the 
provided disk. It will accept options from the Linux command line and alter the disk accordingly. In short, 
once compiled (gcc -o filesys filesys.c) these four commands will be implemented:\
*You must make the commands flags or else they will not work for this program.*\
• ./filesys -D filename delete the named file from the disk\
• ./filesys -L list the files on the disk\
• ./filesys -M filename create a text file and store it to disk\
• ./filesys -P filename read the named file and print it to screen\
Any other option yields an error message. Let us review each of these in turn.