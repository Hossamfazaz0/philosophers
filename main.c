#include "philo.h"

static int	check_args(int ac, char **av)
{
	int	i;
	int	j;

	if (ac != 5 && ac != 6)
	{
		printf("Error: Wrong number of arguments\n");
		return (0);
	}
	i = 1;
	while (i < ac)
	{
		j = 0;
		if (!av[i][0])
		{
			printf("Error: Empty argument\n");
			return (0);
		}
		while (av[i][j])
		{
			if (av[i][j] == '+' && j == 0)
				j++;
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Error: Arguments must be positive numbers\n");
				return (0);
			}
			j++;
		}
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
	if(av[5] && ft_atoi(av[5]) <= 0)
	{
		printf("number of meals should be greater than 0");
		return 0;
	}
	return (1);
}

void	print_usage(void)
{
	printf("\nUsage: ./philo <number_of_philosophers> <time_to_die> <time_to_eat> ");
	printf("<time_to_sleep> [number_of_times_each_philosopher_must_eat]\n\n");
	printf("Arguments:\n");
	printf("- number_of_philosophers : Number of philosophers and forks [1-200]\n");
	printf("- time_to_die          : Time in ms before a philosopher dies without eating\n");
	printf("- time_to_eat          : Time in ms it takes to eat using two forks\n");
	printf("- time_to_sleep        : Time in ms that a philosopher spends sleeping\n");
	printf("- [times_must_eat]     : Optional\
	- Number of times each philosopher must eat\n");
	printf("\nAll time values must be positive integers in milliseconds\n");
}

int	main(int ac, char **av)
{
	t_data *data;
	t_philo *philo;
	data = malloc(sizeof(t_data));
	if (!check_args(ac, av) || !check_values(av))
	{
		print_usage();
		return (1);
	}
	init_data(data, ac, av);
	philo = malloc(sizeof(t_philo) * data->nb_of_philo);
	init_mutexes(data);
	init_philosophers(data, philo);
	start_simulation(data, philo);
}