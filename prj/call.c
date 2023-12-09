#include "call.h"
const char *HD = "HD";

int open_t(char *pathname)
{
	int inode_number;
	// write your code here.
	char *string = malloc(strlen(pathname) + 1);
	strcpy(string, pathname);
	char *token = strtok(string, "/");
	if (token == NULL)
		return 0;
	else
		inode_number = 0;

	while (token != NULL)
	{
		int fd;
		if ((fd = open(HD, O_RDWR)) < 0)
		{
			printf("Error: open()\n");
			return -1;
		}

		inode *ip = read_inode(fd, inode_number);
		if (ip == NULL)
		{
			printf("Error: read_inode()\n");
			return -1;
		}

		DIR_NODE *p_block = (DIR_NODE *)malloc(BLK_SIZE);
		int blk_num = ip->direct_blk[0];
		lseek(fd, D_OFFSET + blk_num * BLK_SIZE, SEEK_SET);
		read(fd, p_block, BLK_SIZE);

		int flag = 0;
		for (int file_idx = 0; file_idx < ip->sub_f_num; file_idx++)
		{
			if (strcmp(p_block[file_idx].f_name, token) == 0)
			{
				flag = 1;
				inode_number = p_block[file_idx].i_number;
				break;
			}
		}
		if (flag == 0)
		{
			printf("Directory/file not found \n");
			return -1;
		}
		close(fd);
		free(p_block);
		token = strtok(NULL, "/");
	}

	return inode_number;
}

int read_t(int i_number, int offset, void *buf, int count)
{
	int read_bytes;
	// write your code here.
	read_bytes = 0;

	int fd;
	if ((fd = open(HD, O_RDWR)) < 0)
		return -1;

	inode *ip = read_inode(fd, i_number);
	if (ip == NULL || ip->f_type == DIR || offset < 0)
		return -1;

	if (offset > ip->f_size)
		return 0;

	int offsetBLK = offset / BLK_SIZE;

	if (ip->f_size <= offset + count)
		count = ip->f_size - offset;

	int tempOffset = offset, tempCount = count, tempRead = 0;

	for (int x = offsetBLK; tempCount != 0; x++)
	{
		if (x < 2)
			lseek(fd, D_OFFSET + ip->direct_blk[x] * BLK_SIZE + (tempOffset % BLK_SIZE), SEEK_SET);
		else
		{
			int indirect[BLK_SIZE / sizeof(int)];
			lseek(fd, D_OFFSET + ip->indirect_blk * BLK_SIZE, SEEK_SET);
			read(fd, indirect, BLK_SIZE);
			lseek(fd, D_OFFSET + indirect[x - 2] * BLK_SIZE + (tempOffset % BLK_SIZE), SEEK_SET);
		}
		tempRead = BLK_SIZE - (tempOffset % BLK_SIZE);
		if (tempCount <= tempRead)
			tempRead = tempCount;

		read_bytes += read(fd, buf + read_bytes, tempRead);
		tempCount -= tempRead;
		tempOffset += tempRead;
	}

	free(ip);
	close(fd);
	return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and "read_t()" are allowed to call these auxiliary functions.

inode *read_inode(int fd, int i_number)
{
	inode *ip = malloc(sizeof(inode));
	int currpos = lseek(fd, I_OFFSET + i_number * sizeof(inode), SEEK_SET);
	if (currpos < 0)
	{
		printf("Error: lseek()\n");
		return NULL;
	}

	// read inode from disk
	int ret = read(fd, ip, sizeof(inode));
	if (ret != sizeof(inode))
	{
		printf("Error: read()\n");
		return NULL;
	}
	return ip;
}