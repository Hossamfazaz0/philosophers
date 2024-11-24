#include "philo.h"

void	assign_forks(t_philo *philo, pthread_mutex_t *first_fork,
		pthread_mutex_t *second_fork)
{
	if (philo->right_fork < philo->left_fork)
		first_fork = philo->right_fork;
	else
		second_fork = philo->left_fork;
}

void	eat_helper(t_philo *philo, pthread_mutex_t *first_fork,
		pthread_mutex_t *second_fork)
{
	pthread_mutex_lock(first_fork);
	print_state(philo, "has taken a fork");
	pthread_mutex_lock(second_fork);
	print_state(philo, "has taken a fork");
	print_state(philo, "is eating");
	pthread_mutex_lock(philo->data->last_meal_mutex);
	philo->last_meal_time = ft_gettime();
	pthread_mutex_unlock(philo->data->last_meal_mutex);
	ft_usleep(philo->data->time_to_eat);
	pthread_mutex_lock(philo->data->meals_mutex);
	philo->meals_eaten++;
	if (philo->data->nb_of_meals != -1
		&& philo->meals_eaten == philo->data->nb_of_meals)
		philo->data->number_philos_ate++;
	pthread_mutex_unlock(philo->data->meals_mutex);
	pthread_mutex_unlock(second_fork);
	pthread_mutex_unlock(first_fork);
	usleep(100);
}

int	eat(t_philo *philo)
{
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;

	first_fork = malloc(sizeof(pthread_mutex_t *));
	second_fork = malloc(sizeof(pthread_mutex_t *));
	if (!philo || !philo->data)
		return (1);
	if (philo->id % 2 == 0)
		usleep(500);
	pthread_mutex_lock(philo->data->stop_mutex);
	if (philo->data->stop)
	{
		pthread_mutex_unlock(philo->data->stop_mutex);
		return (1);
	}
	pthread_mutex_unlock(philo->data->stop_mutex);
	assign_forks(philo, first_fork, second_fork);
	eat_helper(philo, first_fork, second_fork);
	return (0);
}
