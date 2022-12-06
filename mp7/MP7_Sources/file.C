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
			blk_number = fle_system->inodes[i].blk_number;
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

	fle_system->disk->write(blk_number, block_cache);
	fle_system->inodes[inode_indx].fle_size = fle_size;

	Inode* tmp_inodes_ref = fle_system->inodes;
	unsigned char* tmp_inode_ref = (unsigned char*) tmp_inodes_ref;
	fle_system->disk->write(1,tmp_inode_ref);
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
	Console::puts("reading from file\n");
	int char_cnt = 0;
	for(int indx = curr_pos; indx<fle_size;indx++){
		if(char_cnt==_n)
			break;
		_buf[char_cnt++] = block_cache[indx];
	}

	Console::puts("reading from file complete\n");
	return char_cnt;
}

int File::Write(unsigned int _n, const char *_buf) {
	Console::puts("writing to file\n");
	int char_cnt = 0;
	int end_indx = curr_pos+_n;
	while(curr_pos<end_indx){
		if(curr_pos==SimpleDisk::BLOCK_SIZE){
			Console::puts("EOF reached while writing\n");
			break;
		}
		block_cache[curr_pos++]=_buf[char_cnt++];
		fle_size++;
	}

	Console::puts("writing to file complete\n");
	return char_cnt;
}

void File::Reset() {
    Console::puts("resetting file\n");
    curr_pos=0;
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    return (curr_pos<fle_size);
}
