/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfazaz <hfazaz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 02:01:35 by hfazaz            #+#    #+#             */
/*   Updated: 2024/11/24 23:30:14 by hfazaz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	is_valid_number(char *str)
{
	int	j;

	j = 0;
	if (!str[0])
	{
		printf("Error: Empty argument\n");
		return (0);
	}
	while (str[j])
	{
		if (str[j] == '+' && j == 0)
			j++;
		if (str[j] < '0' || str[j] > '9')
		{
			printf("Error: Arguments must be positive numbers\n");
			return (0);
		}
		j++;
	}
	return (1);
}

static int	check_args(int ac, char **av)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("Error: Wrong number of arguments\n");
		return (0);
	}
	i = 1;
	while (i < ac)
	{
		if (!is_valid_number(av[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	check_values(char **av)
{
	int	num;

	num = ft_atoi(av[1]);
	if (num > 200)
	{
		printf("Error: Maximum number of philosophers is 200\n");
		return (0);
	}
	if (num <= 0)
	{
		printf("Error: Number of philosophers must be positive\n");
		return (0);
	}
	if (ft_atoi(av[2]) <= 0 || ft_atoi(av[3]) <= 0 || ft_atoi(av[4]) <= 0)
	{
		printf("Error: Time values must be positive\n");
		return (0);
	}
	if (av[5] && ft_atoi(av[5]) <= 0)
	{
		printf("number of meals should be greater than 0");
		return (0);
	}
	return (1);
}

void	print_usage(void)
{
	printf("\nUsage: ./philo <number_of_philosophers>\
	<time_to_die> <time_to_eat> ");
	printf("<time_to_sleep> [number_of_times_each_philosopher_must_eat]\n\n");
	printf("Arguments:\n");
	printf("- number_of_philosophers : \
	Number of philosophers and forks [1-200]\n");
	printf("- time_to_die          : \
	Time in ms before a philosopher dies without eating\n");
	printf("- time_to_eat          : \
	Time in ms it takes to eat using two forks\n");
	printf("- time_to_sleep        : \
	Time in ms that a philosopher spends sleeping\n");
	printf("- [times_must_eat]     : Optional\
	- Number of times each philosopher must eat\n");
	printf("\nAll time values must be positive integers in milliseconds\n");
}

int	main(int ac, char **av)
{
	t_data	*data;
	t_philo	*philo;

	data = malloc(sizeof(t_data));
	if (!data)
		return (1);
	if (!check_args(ac, av) || !check_values(av))
	{
		print_usage();
		free(data);
		return (1);
	}
	init_data(data, ac, av);
	philo = malloc(sizeof(t_philo) * data->nb_of_philo);
	if (!philo)
	{
		free(data);
		return (1);
	}
	start(data, philo);
	return (0);
}
