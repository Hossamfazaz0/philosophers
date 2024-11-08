#include "philo.h"
size_t	ft_gettime(void)
{
	t_time	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}
void init_data(t_data *data , int ac, char **av)
{
    data->nb_of_philo = ft_atoi(av[1]);
    data->time_to_die = ft_atoi(av[2]);
    data->time_to_eat = ft_atoi(av[3]);
    data->time_to_sleep = ft_atoi(av[4]);
    data->nb_of_meals = (ac == 6) ? ft_atoi(av[5]) : -1;
    data->time_to_start = 0;
    data->stop = 0;
    data->number_philos_ate = 0;

}
void init_forks(t_data *data)
{
    int i;
    data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_of_philo);
    i = 0;
    while (i < data->nb_of_philo)
    {
        pthread_mutex_init(&data->forks[i], NULL);
        i++;
    }
    data->m1 = malloc(sizeof(pthread_mutex_t));
    data->m2 = malloc(sizeof(pthread_mutex_t));
    data->m_last_time = malloc(sizeof(pthread_mutex_t));
    data->meals_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(data->m1, NULL);
    pthread_mutex_init(data->m2, NULL);
    pthread_mutex_init(data->m_last_time, NULL);
    pthread_mutex_init(data->meals_mutex, NULL);

}
void init_philos(t_data *data)
{
    int i;
    t_philo *philos;
    i = 0;
    while(i <data->nb_of_philo)
    {

        philos = malloc(sizeof(t_philo));
        philos->philo_id = i + 1;
        philos[i].data = data;
        if(philos->philo_id % 2 == 0)
        {
            philos->left_fork = &data->forks[i];
            philos->right_fork = &data->forks[(i + 1) % data->nb_of_philo];
        }
        else
        {
            philos->left_fork = &data->forks[(i + 1) % data->nb_of_philo];
            philos->right_fork = &data->forks[i];
        }

        i++;
    }
    
}
void ft_print(t_philo *philo, char *str)
{
    pthread_mutex_lock(philo->data->m1);
    printf("%ld %d %s\n", ft_gettime() - philo->data->time_to_start, philo->philo_id, str);
    pthread_mutex_unlock(philo->data->m1);
}
int	check_meals(t_philo *philo)
{
	int nb_of_meals;

	pthread_mutex_lock(philo->data->meals_mutex);
	nb_of_meals = philo->data->nb_of_meals;
	if (philo->number_of_meals == nb_of_meals)
	{
		philo->data->number_philos_ate++;
		pthread_mutex_unlock(philo->data->meals_mutex);
		return (1);
	}
	pthread_mutex_unlock(philo->data->meals_mutex);
	return (0);
}



int ft_eat(t_philo *philo)
{
    pthread_mutex_lock(philo->data->m1);
    if (philo->data->stop) {
        pthread_mutex_unlock(philo->data->m1);
        return (1);
    }
    pthread_mutex_unlock(philo->data->m1);
    
    pthread_mutex_lock(philo->left_fork);
    ft_print(philo, "has taken left fork");

    pthread_mutex_lock(philo->right_fork);
    ft_print(philo, "has taken right fork");

    pthread_mutex_lock(philo->data->m_last_time);
    philo->last_time_eat = ft_gettime();
    if (philo->number_of_meals < philo->data->nb_of_meals)
        philo->number_of_meals++;
    pthread_mutex_unlock(philo->data->m_last_time);

    ft_usleep(philo->data->time_to_eat);

    if (check_meals(philo))
    {
        pthread_mutex_unlock(philo->left_fork);
        pthread_mutex_unlock(philo->right_fork);
        return (1);
    }
    return (0);
}


int ft_sleep(t_philo *philo)
{
    ft_print(philo, "is sleeping");
    usleep(philo->data->time_to_sleep * 1000);
    return (0);
}
int ft_think(t_philo *philo)
{
    ft_print(philo, "is thinking");
    return (0);
}


void *routine(void *philos)
{
    t_philo *philo;

    philo = (t_philo *)philos;
    if (philo->philo_id % 2 == 0)
        usleep(250);
    while (!philo->data->stop)
    {
        if (ft_eat(philo) || ft_sleep(philo) || ft_think(philo))
            break ;
        usleep(400);
    }
    return (NULL);
}

int main(int ac , char **av)
{
    t_data *data;
    t_philo *philos;
    int i = 0;
    data = malloc(sizeof(t_data));
    pthread_mutex_t *forks;
    init_data(data,ac,av);
    data->threads = malloc(sizeof(pthread_t) * data->nb_of_philo);
    init_forks(data);
    init_philos(data);
    data->time_to_start = ft_gettime();
    while(i < data->nb_of_philo)
    {
        pthread_create(&data->threads[i], NULL, &routine, &philos[i]);
        i++;
    }

}