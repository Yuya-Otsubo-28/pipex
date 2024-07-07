/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yuotsubo <yuotsubo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 22:17:45 by yuotsubo          #+#    #+#             */
/*   Updated: 2024/07/07 14:13:01 by yuotsubo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../pipex.h"
#include "libft.h"

// [!] need to modify
// - close fds before program finish. ✅
// - adjust error message.

// __attribute__((destructor))
// static void destructor() {
//     system("leaks -q pipex");
// }

static void	free_data(t_data *data)
{
	if (!data)
		return ;
	if (data->cmds)
		free_cmds(data->cmds);
	if (data->cmd_paths)
		free_paths(data->cmd_paths, 2);
	if (data)
		free(data);
}

static int	err_return(t_data *data, int err_num)
{
	if (data)
		free_data(data);
	if (err_num == ERR_INPUT)
		ft_putendl_fd("ERROR: input", STDERR_FILENO);
	if (err_num == ERR_CMD_FIND)
		ft_putendl_fd("ERROR: command not found", STDERR_FILENO);
	return (EXIT_FAILURE);
}

int	main(int argc, char *argv[])
{
	t_data	*data;
	int		cmds_res;

	data = make_struct(argc, argv);
	if (!data)
		return (err_return(NULL, ERR_INPUT));
	if (!search_paths(data))
		return (err_return(data, ERR_CMD_FIND));
	cmds_res = do_cmds(data, argv);
	if (cmds_res < 0)
		return (err_return(data, cmds_res));
	free_data(data);
	// while (1){};
	return (0);
}
