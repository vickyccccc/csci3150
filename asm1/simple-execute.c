#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int shell_execute(char **args, int argc)
{
	int pipeCount = 0;
	int pipeAt[argc / 2];

	for (int i = 0; i < argc; i++)
	{
		if (args[i] == NULL)
		{
			pipeAt[pipeCount] = i;
		}
		else if (strcmp(args[i], "|") == 0)
		{
			args[i] = NULL;
			pipeAt[pipeCount++] = i;
		}
		// printf("args[%d]: %s\n", i, args[i]);
	}

	char ***pipe_n;

	if ((pipe_n = malloc((pipeCount + 1) * sizeof(char **))) == NULL)
	{
		printf("malloc() error for pipe_n\n");
		exit(-1);
	}

	for (int i = 0; i < pipeCount + 1; i++)
	{
		if ((pipe_n[i] = malloc(30 * sizeof(char *))) == NULL)
		{
			printf("malloc() error for cmd_args\n");
			exit(-1);
		}
	}

	for (int i = 0, j = 0; i < argc; i++, j++)
	{
		if (pipeCount == 3 && i > pipeAt[2] && i <= pipeAt[3])
		{
			pipe_n[3][j] = args[i];
			// printf("pipe3[%d]: %s\n", j, pipe_n[3][j]);
		}
		else if (pipeCount >= 2 && i > pipeAt[1] && i <= pipeAt[2])
		{
			pipe_n[2][j] = args[i];
			// printf("pipe2[%d]: %s\n", j, pipe_n[2][j]);
		}
		else if (pipeCount >= 1 && i > pipeAt[0] && i <= pipeAt[1])
		{
			pipe_n[1][j] = args[i];
			// printf("pipe1[%d]: %s\n", j, pipe_n[1][j]);
		}
		else if (pipeCount >= 0 && i <= pipeAt[0])
		{
			pipe_n[0][j] = args[i];
			// printf("pipe0[%d]: %s\n", j, pipe_n[0][j]);
		}
		else
		{
			printf("err\n");
			break;
		}

		if (args[i] == NULL)
		{
			// printf("\n");
			j = -1;
		}
	}

	// int child_pid, wait_return, status;
	int child_pid[pipeCount + 1], wait_return, status[pipeCount + 1];

	int fd[pipeCount][2];

	if (strcmp(args[0], "EXIT") == 0)
		return -1;

	for (int i = 0; i <= pipeCount; i++)
	{
		if (pipeCount != 0)
		{
			if (pipe(fd[i]) < 0)
			{
				printf("pipe() error\n");
				exit(-1);
			}
		}

		if ((child_pid[i] = fork()) < 0)
		{
			printf("fork() error \n");
		}
		else if (child_pid[i] == 0)
		{
			if (i > 0)
			{
				// printf("read from previous pipe	%d\n", i);
				close(0);
				dup(fd[i - 1][0]);
				// printf("previous: dup2() done	c%d\n", i);
			}

			if (i < pipeCount)
			{
				// printf("write to next pipe	%d\n", i);
				close(1);
				dup(fd[i][1]);
				// printf("next: dup2 done		c%d\n", i);
			}

			for (int j = 0; j < i && pipeCount != 0; j++)
			{
				close(fd[j][0]);
				close(fd[j][1]);
				// printf("close unused fd		c%d\n", i);
			}

			// printf("ready to exe cmd%d...\n", i);

			if (execvp(pipe_n[i][0], pipe_n[i]) < 0)
			{
				printf("execvp() error \n");
				exit(-1);
			}
		}
	}

	if (pipeCount != 0)
	{
		for (int j = 0; j < pipeCount; j++)
		{
			close(fd[j][0]);
			close(fd[j][1]);
			// printf("close unused fd		p%d\n", i);
		}
	}

	for (int i = 0; i <= pipeCount; i++)
	{
		if ((wait_return = wait(&child_pid[i])) < 0)
		{
			printf("wait() error [%d]\n", i);
			break;
		}
		else
		{
			// printf("cmd%d done\n", i);
		}
	}

	return 0;
}