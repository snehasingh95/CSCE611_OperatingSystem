/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
	Console::puts("Opening file.\n");
	fle_system = _fs;
	fle_identifier = _id;
	curr_pos = 0;
	
	bool fle_blk_found = false;
	unsigned int i=0;
	while(i<fle_system->MAX_INODES){
		if(fle_system->inodes[i].id==fle_identifier){
			inode_indx = i;
			blk_no = fle_system->inodes[i].block_no;
			fle_size = fle_system->inodes[i].fle_size;
			fle_blk_found = true;
			break;
		}
		i++;
	}
	
	assert(fle_blk_found);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    assert(false);
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    assert(false);
}

void File::Reset() {
    Console::puts("resetting file\n");
    assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    assert(false);
}
