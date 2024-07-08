/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   do_cmds.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yuotsubo <yuotsubo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 17:24:45 by yuotsubo          #+#    #+#             */
/*   Updated: 2024/07/08 20:57:14 by yuotsubo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../pipex.h"
#include "libft.h"

// [!] need to modify
// - close pipe_fsd when error occurred.

static int	err_return(int err_num, int *fd_1, int *fd_2)
{
	if (err_num)
		ft_printf("%s\n", strerror(errno));
	if (fd_1)
		close(*fd_1);
	if (fd_2)
		close(*fd_2);
	return (err_num);
}

static void	first_cmd_prc(t_data *data, char **argv, int *pipe_fds, char **environ)
{
	int	infile_fd;

	close(pipe_fds[0]);
	if (dup2(pipe_fds[1], 1) < 0)
		exit(err_return(errno, NULL, &pipe_fds[1]));
	infile_fd = open(argv[1], O_RDONLY);
	if (infile_fd < 0)
		exit(err_return(0, NULL, &pipe_fds[1]));
	if (dup2(infile_fd, 0) < 0)
		exit(err_return(errno, &infile_fd, &pipe_fds[1]));
	if (!(data->cmd_paths[0]))
		exit(err_return(0, &infile_fd, &pipe_fds[1]));
	if (execve(data->cmd_paths[0], data->cmds[0], environ) < 0)
		exit(err_return(errno, &infile_fd, &pipe_fds[1]));
}

static void	second_cmd_prc(t_data *data, char **argv, char **environ)
{
	int	pipe_fds[2];
	int	child_fd;
	int	outfile_fd;

	if (pipe(pipe_fds) < 0)
		exit(err_return(errno, NULL, NULL));
	if ((child_fd = fork()) < 0)
		exit(err_return(errno, &pipe_fds[0], &pipe_fds[1]));
	if (child_fd > 0)
	{
		outfile_fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (outfile_fd < 0)
			exit(err_return(0, &pipe_fds[0], &pipe_fds[1]));
		wait(NULL);
		close(pipe_fds[1]);
		if (dup2(pipe_fds[0], 0) < 0)
			exit(err_return(errno, &pipe_fds[0], &outfile_fd));
		if (dup2(outfile_fd, 1) < 0)
			exit(err_return(errno, &pipe_fds[0], &outfile_fd));
		if (!(data->cmd_paths[1]))
			exit(err_return(0, &pipe_fds[0], &outfile_fd));
		if (execve(data->cmd_paths[1], data->cmds[1], environ) < 0)
			exit(err_return(errno, &pipe_fds[0], &outfile_fd));
	}
	else
		first_cmd_prc(data, argv, pipe_fds, environ);
}

int	do_cmds(t_data *data, char **argv)
{
	int			child_fd;
	extern char	**environ;

	if ((child_fd = fork()) < 0)
		return (err_return(errno, NULL, NULL));
	if (child_fd > 0)
		wait(NULL);
	else
		second_cmd_prc(data, argv, environ);
	return (EXIT_SUCCESS);
}