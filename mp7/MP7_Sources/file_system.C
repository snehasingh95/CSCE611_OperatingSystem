/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define FREE_BLK_REP 'F'
#define USED_BLK_REP 'U'
#define INODE_BLK_NUMBER 1
#define BITMAP_BLK_NUMBER 0

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */
   
Inode::Inode(){
	fs = NULL;
	long id = -1;
	//unsigned int blk_number;
	bool inode_is_free = true;
	int fle_size = 0;
}
/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
	Console::puts("In file system constructor.\n");
	
	disk = NULL;
	size = 0;
	free_blk_cnt = SimpleDisk::BLOCK_SIZE / sizeof(unsigned char);
	unsigned int inode_cntr = 0;
	
	unsigned char *free_blocks = new unsigned char[free_blk_cnt];
	inodes = new Inode[MAX_INODES];
	
	Console::puts("FileSystem constructor initialized.\n");
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    assert(false);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
	Console::puts("mounting file system from disk\n");

	/* Here you read the inode list and the free list into memory */

	disk = _disk;
	unsigned char* tmp_inode_ref;

	_disk->read(BITMAP_BLK_NUMBER, free_blocks);
	_disk->read(INODE_BLK_NUMBER, tmp_inode_ref);

	inodes = (Inode *) tmp_inode_ref;

	// finding a free inode
	inode_cntr = 0;
	unsigned int i = 0;
	while(i<MAX_INODES){
		if(!inodes[i].inode_is_free){
			inode_cntr++;
		}
		i++;
	}

	free_blk_cnt = SimpleDisk::BLOCK_SIZE / sizeof(unsigned char);

	Console::puts("Mounted file system from disk\n");

	return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
	Console::puts("formatting disk\n");
	/* Here you populate the disk with an initialized (probably empty) inode list
	and a free list. Make sure that blocks used for the inodes and for the free list
	are marked as used, otherwise they may get overwritten. */

	unsigned int n_free_blks  = _size/SimpleDisk::BLOCK_SIZE;
	unsigned char* free_blks_arr = new unsigned char[n_free_blks];
	
	// reserved blocks
	free_blks_arr[0] = free_blks_arr[1] = USED_BLK_REP;
	
	//freeing blocks
	unsigned int i = 2;
	while(i<n_free_blks){
		free_blks_arr[i] = USED_BLK_REP;
		i++;
	}
	
	_disk->write(BITMAP_BLK_NUMBER, free_blks_arr);
	
	Inode* tmp_inodes_ref = new Inode[MAX_INODES];
	unsigned char* tmp_inode_ref = (unsigned char*) tmp_inodes_ref;
	_disk->write(INODE_BLK_NUMBER,tmp_inode_ref);
	
	Console::puts("formatted disk\n");
	return true;
}

Inode * FileSystem::LookupFile(int _file_id) {
	Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
	/* Here you go through the inode list to find the file. */
	unsigned int i;
	for(i=0;i<inode_cntr;i++){
		if(inodes[i].id== _file_id){
			return &inodes[i];
		}	
	}
	Console::puts("File with id = "); Console::puti(_file_id); Console::puts(" not found\n");
	return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
	Console::puts("creating file with id: "); Console::puti(_file_id); Console::puts("\n");
	/* Here you check if the file exists already. If so, throw an error.
	Then get yourself a free inode and initialize all the data needed for the
	new file. After this function there will be a new file on disk. */

	unsigned int i;
	for(i=0;i<inode_cntr;i++){
		if(inodes[i].id== _file_id){
			Console::puts("Error: File with id: ");Console::puti(_file_id); Console::puts(" already exists.\n");
			assert(false);
		}	
	}
	
	
	unsigned int free_inode_indx = -1;
	for(i=0;i<MAX_INODES;i++){
		if(inodes[i].inode_is_free){
			free_inode_indx = i;
			break;
		}	
	}
	
	unsigned int free_blk_indx = -1;
	for(i=0;i<free_blk_cnt;i++){
		if(free_blocks[i] == FREE_BLK_REP){
			free_blk_indx = i;
			break;
		}	
	}
	
	assert((free_inode_indx!=-1) && (free_blk_indx!=-1));
	inodes[free_inode_indx].inode_is_free = false;
	inodes[free_inode_indx].fs = this;
	inodes[free_inode_indx].id = _file_id;
	inodes[free_inode_indx].blk_number = free_blk_indx;
	free_blocks[free_inode_indx] = USED_BLK_REP;
	
	Console::puts("File created successfully");
	
	return true;
}

bool FileSystem::DeleteFile(int _file_id) {
	Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
	/* First, check if the file exists. If not, throw an error. 
	Then free all blocks that belong to the file and delete/invalidate 
	(depending on your implementation of the inode list) the inode. */
	
	bool file_exists = false;
	unsigned int indx=0;
	
	while(indx<MAX_INODES){
		if(inodes[indx].id ==_file_id){
			file_exists = true;
			break; 
		}
		indx++;
	}
	
	assert(file_exists);
	
	int blk_number = inodes[indx].blk_number;
	free_blocks[blk_number] = FREE_BLK_REP;
	
	inodes[indx].inode_is_free = true;
	inodes[indx].fle_size = 0;
	
	Console::puts("File created successfully");
	return true;
}
