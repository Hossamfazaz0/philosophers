#include "philo.h"

int eat(t_philo *philo)
{
    if (!philo || !philo->data)
        return 1;

    pthread_mutex_lock(philo->data->last_meal_mutex);
    pthread_mutex_lock(philo->data->stop_mutex);

    long current_time = ft_gettime();
    if (philo->data->stop || 
        current_time - philo->last_meal_time > philo->data->time_to_die)
    {
        pthread_mutex_unlock(philo->data->stop_mutex);
        pthread_mutex_unlock(philo->data->last_meal_mutex);
        return 1;
    }

    pthread_mutex_unlock(philo->data->stop_mutex);
    pthread_mutex_unlock(philo->data->last_meal_mutex);

    pthread_mutex_lock(philo->left_fork);
    print_state(philo, "has taken a fork");
    pthread_mutex_lock(philo->right_fork);
    print_state(philo, "has taken a fork");
    print_state(philo, "is eating");

    pthread_mutex_lock(philo->data->last_meal_mutex);
    philo->last_meal_time = ft_gettime();
    pthread_mutex_unlock(philo->data->last_meal_mutex);

    ft_usleep(philo->data->time_to_eat);

    pthread_mutex_lock(philo->data->meals_mutex);
    philo->meals_eaten++;
    if (philo->meals_eaten == philo->data->nb_of_meals)
        philo->data->number_philos_ate++;
    pthread_mutex_unlock(philo->data->meals_mutex);

    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);

    return 0;
}

void *philosopher_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    pthread_mutex_lock(philo->data->last_meal_mutex);
    philo->last_meal_time = ft_gettime();
    pthread_mutex_unlock(philo->data->last_meal_mutex);
    if (philo->id % 2 == 0)
        ft_usleep(philo->data->time_to_eat / 2);
    while (1)
    {
        if (eat(philo))
            break;
        pthread_mutex_lock(philo->data->stop_mutex);
        if (philo->data->stop)
        {
            pthread_mutex_unlock(philo->data->stop_mutex);
            break;
        }
        pthread_mutex_unlock(philo->data->stop_mutex);
        print_state(philo, "is sleeping");
        ft_usleep(philo->data->time_to_sleep);
        pthread_mutex_lock(philo->data->stop_mutex);
        if (philo->data->stop)
        {
            pthread_mutex_unlock(philo->data->stop_mutex);
            break;
        }
        pthread_mutex_unlock(philo->data->stop_mutex);
        print_state(philo, "is thinking");
    }
    return NULL;
}


int should_stop(t_philo *philo)
{
    if (!philo || !philo->data)
        return 1;

    pthread_mutex_lock(philo->data->stop_mutex);
    int stop = philo->data->stop;
    pthread_mutex_unlock(philo->data->stop_mutex);

    return stop;
}

int check_death(t_philo *philo)
{
    if (!philo || !philo->data)
        return 0;

    int is_dead = 0;

    pthread_mutex_lock(philo->data->last_meal_mutex);
    pthread_mutex_lock(philo->data->stop_mutex);

    long current_time = ft_gettime();
    if (!philo->data->stop && 
        current_time - philo->last_meal_time > philo->data->time_to_die)
    {
        pthread_mutex_lock(philo->data->print_mutex);
        printf("%ld %d %s\n", 
            ft_gettime() - philo->data->time_to_start,
            philo->id, "died");
        pthread_mutex_unlock(philo->data->print_mutex);

        philo->data->stop = 1;
        is_dead = 1;
    }

    pthread_mutex_unlock(philo->data->stop_mutex);
    pthread_mutex_unlock(philo->data->last_meal_mutex);

    return is_dead;
}

void *monitor_routine(void *arg)
{
    t_data *philo = (t_data *)arg;
    while (1)
    {
        pthread_mutex_lock(&data->stop_mutex);
        if (data->stop)
        {
            pthread_mutex_unlock(&data->stop_mutex);
            break;
        }
        pthread_mutex_unlock(&data->stop_mutex);

        for (int i = 0; i < data->nb_of_philo; i++)
        {
            pthread_mutex_lock(&data->last_meal_mutex);
            if (ft_gettime() - data->philo[i].last_meal_time > data->time_to_die)
            {
                pthread_mutex_lock(&data->stop_mutex);
                data->stop = 1;
                printf("%ld %d died\n", ft_gettime() - data->time_to_start, data->philo[i].id);
                pthread_mutex_unlock(&data->stop_mutex);
                pthread_mutex_unlock(&data->last_meal_mutex);
                return NULL;
            }
            pthread_mutex_unlock(&data->last_meal_mutex);
        }
    }
    return NULL;
}

