#include "philo.h"

int	ft_atoi(const char *str)
{
	long int	result;
	long int	result2;
	int			sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		result2 = result;
		result = result * 10 + (*str++ - '0');
		if (result < result2 && sign == -1)
			return (0);
		if (result < result2 && sign == 1)
			return (-1);
	}
	return (sign * result);
}
// void print_state(t_philo *philo,  char *state)
// {
//     pthread_mutex_lock(philo->data->print_mutex);
//     pthread_mutex_lock(philo->data->stop_mutex);
//     if (!philo->data->stop)
//     {
//         printf("%ld %d %s\n",
//             ft_gettime() - philo->data->time_to_start,
//             philo->id,
//             state);
//     }
//     pthread_mutex_unlock(philo->data->stop_mutex);
//     pthread_mutex_unlock(philo->data->print_mutex);
// }
size_t	ft_gettime(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) != 0)
	{
		write(2, "Error: gettimeofday failed\n", 26);
		return (0);
	}
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void	ft_usleep(long time)
{
	long	start;

	start = ft_gettime();
	while ((ft_gettime() - start) < time)
	{
		usleep(100);
	}
}
