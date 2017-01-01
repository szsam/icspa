#include "common.h"
#include <sys/ioctl.h>
#include <string.h>

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

typedef struct {
	bool opened;
	uint32_t offset;
} Fstate;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] __attribute__((used)) = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_ioctl(int fd, uint32_t request, void *p) {
	assert(request == TCGETS);
	return (fd >= 0 && fd <= 2 ? 0 : -1);
}

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

Fstate fstates[NR_FILES + 3] =
{
	{ true, 0},		// stdin
	{ true, 0},		// stdout
	{ true, 0}		// stderr	
};

/* Given a pathname for a file, open() returns a file descriptor, a small,
 * nonnegative integer for use in subsequent system  calls */
int fs_open(const char *pathname, int flags)    /* 在我们的实现中可以忽略flags */
{
	int i;
	for (i = 0; i < NR_FILES; i++) {
		if (!strcmp(file_table[i].name, pathname)) {
			fstates[i + 3].opened = true;
			fstates[i + 3].offset = 0;
			return i + 3;
		}
	}
	assert(0);
	return -1;
}

/* read()  attempts to read up to count bytes from file descriptor fd into
 * the buffer starting at buf. */
int fs_read(int fd, void *buf, int len) {
	// ignore stdin, stdout, stderr
	assert(fd >= 3);

	assert(fstates[fd].opened);
	if (fstates[fd].offset + len <= file_table[fd -3].size) {
		ide_read(buf, 
				file_table[fd -3].disk_offset + fstates[fd].offset, 
				len);
		fstates[fd].offset += len;
		return len;
	}
	else {
		int len1 = file_table[fd -3].size - fstates[fd].offset;
		ide_read(buf, 
				file_table[fd -3].disk_offset + fstates[fd].offset, 
				len1); 
		fstates[fd].offset = file_table[fd -3].size;
		return len1;
	}
}

int fs_write(int fd, const void *buf, int len) {
	assert(fd >= 3);

	assert(fstates[fd].opened);
	if (fstates[fd].offset + len <= file_table[fd -3].size) {
		ide_write(buf, 
				file_table[fd -3].disk_offset + fstates[fd].offset, 
				len);
		fstates[fd].offset += len;
		return len;
	}
	else {
		int len1 = file_table[fd -3].size - fstates[fd].offset;
		ide_write(buf, 
				file_table[fd -3].disk_offset + fstates[fd].offset, 
				len1); 
		fstates[fd].offset = file_table[fd -3].size;
		return len1;
	}
}

int fs_lseek(int fd, int offset, int whence) {
	assert(fstates[fd].opened);

	uint32_t new_offset = 0;
	switch (whence) {
		case SEEK_SET: 
			new_offset = offset; break;
		case SEEK_CUR: 
			new_offset = fstates[fd].offset + offset; break;
		case SEEK_END: 
			new_offset = file_table[fd - 3].size + offset; 
			break;
		default:
			assert(0);
	}
	fstates[fd].offset = new_offset;
	return new_offset;
}

int fs_close(int fd) {
	fstates[fd].opened = false;
	return 0;
}
