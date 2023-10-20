#include "call.h"
const char *HD = "HD";

int open_t(char *pathname)
{
	int inode_number;
	// write your code here.
	if (pathname[0] != '/' || strlen(pathname) == 0)
	{
		// printf("pathname should start with '/'\n");
		return -1;
	}

	if (strlen(pathname) == 1 && pathname[0] == '/')
	{
		// printf("Root directory is 0 (I'm not sure)\n");
		return 0;
	}

	char *dir_n[10];
	int i = 0;

	char *token = strtok(pathname, "/");
	while (token != NULL)
	{
		dir_n[i++] = token;
		token = strtok(NULL, "/");
	}

	// for (int j = 0; j < i; j++)
	// {
	// 	printf("dir_n[%d]:\t%s\n", j, dir_n[j]);
	// }

	for (int j = 0; j < i; j++)
	{
		int fd;
		if ((fd = open(HD, O_RDWR)) < 0)
		{
			printf("Error: open()\n");
			return -1;
		}

		inode *ip = read_inode(fd, j == 0 ? 0 : inode_number);
		if (ip == NULL)
		{
			printf("Error: read_inode()\n");
			return -1;
		}

		DIR_NODE *p_block = (DIR_NODE *)malloc(BLK_SIZE);
		int blk_num = ip->direct_blk[0];
		int currpos = lseek(fd, D_OFFSET + blk_num * BLK_SIZE, SEEK_SET);
		read(fd, p_block, BLK_SIZE);

		// printf("For %s:\n", dir_n[j]);
		int flag = 0;
		for (int file_idx = 0; file_idx < ip->sub_f_num; file_idx++)
		{
			// printf("%s \t\t %d\n", p_block[file_idx].f_name, p_block[file_idx].i_number);
			if (strcmp(p_block[file_idx].f_name, dir_n[j]) == 0)
			{
				flag = 1;
				inode_number = p_block[file_idx].i_number;
				// printf("%s \t\t %d\n", p_block[file_idx].f_name, p_block[file_idx].i_number);
				break;
			}
		}
		if (flag == 0)
		{
			// printf("Directory/file not found at %s\n", dir_n[j]);
			return -1;
		}
	}

	return inode_number;
}

int read_t(int i_number, int offset, void *buf, int count)
{
	int read_bytes;
	// write your code here.
	return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and "read_t()" are allowed to call these auxiliary functions.