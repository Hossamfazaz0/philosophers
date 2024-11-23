#include "philo.h"
int eat(t_philo *philo)
{
    if (!philo || !philo->data)
        return 1;

    pthread_mutex_lock(philo->data->stop_mutex);
    if (philo->data->stop)
    {
        pthread_mutex_unlock(philo->data->stop_mutex);
        return 1;
    }
    pthread_mutex_unlock(philo->data->stop_mutex);

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
    if (philo->data->nb_of_meals != -1 && 
        philo->meals_eaten == philo->data->nb_of_meals)
        philo->data->number_philos_ate++;
    pthread_mutex_unlock(philo->data->meals_mutex);

    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);

    return 0;
}

void *philosopher_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;

    if (!philo || !philo->data)
        return NULL;

    if (philo->data->nb_of_philo == 1)
    {
        print_state(philo, "has taken a fork");
        ft_usleep(philo->data->time_to_die);
        return NULL;
    }

    pthread_mutex_lock(philo->data->last_meal_mutex);
    philo->last_meal_time = ft_gettime();
    pthread_mutex_unlock(philo->data->last_meal_mutex);

    if (philo->id % 2 == 0)
        ft_usleep(philo->data->time_to_eat / 2);

    while (!should_stop(philo))
    {
        if (eat(philo))
            break;
        print_state(philo, "is sleeping");
        ft_usleep(philo->data->time_to_sleep);
        print_state(philo, "is thinking");
    }
    return NULL;
}

int should_stop(t_philo *philo)
{
    int stop;

    pthread_mutex_lock(philo->data->stop_mutex);
    stop = philo->data->stop;
    pthread_mutex_unlock(philo->data->stop_mutex);
    return (stop);
}

void check_philo_death(t_philo *philo, int i)
{
    long current_time;
    long last_meal_time;

    pthread_mutex_lock(philo->data->last_meal_mutex);
    current_time = ft_gettime();
    last_meal_time = philo[i].last_meal_time;
    pthread_mutex_unlock(philo->data->last_meal_mutex);

    if (current_time - last_meal_time > philo->data->time_to_die)
    {
        pthread_mutex_lock(philo->data->stop_mutex);
        if (!philo->data->stop)
        {
            philo->data->stop = 1;
            printf("%ld %d died\n", 
                current_time - philo->data->time_to_start,
                philo[i].id);
        }
        pthread_mutex_unlock(philo->data->stop_mutex);
    }
}

void check_meals_complete(t_philo *philo)
{
    pthread_mutex_lock(philo->data->meals_mutex);
    if (philo->data->nb_of_meals != -1 && 
        philo->data->number_philos_ate >= philo->data->nb_of_philo)
    {
        pthread_mutex_lock(philo->data->stop_mutex);
        philo->data->stop = 1;
        pthread_mutex_unlock(philo->data->stop_mutex);
    }
    pthread_mutex_unlock(philo->data->meals_mutex);
}

void *monitor_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    int i;

    if (!philo || !philo->data)
        return NULL;

    while (!should_stop(philo))
    {
        i = 0;
        while (i < philo->data->nb_of_philo && !should_stop(philo))
        {
            check_philo_death(philo, i);
            if (should_stop(philo))
                break;
            check_meals_complete(philo);
            i++;
        }
        usleep(100);
    }
    return NULL;
}

void print_state(t_philo *philo, char *state)
{
    pthread_mutex_lock(philo->data->stop_mutex);
    if (!philo->data->stop)
    {
        pthread_mutex_lock(philo->data->print_mutex);
        printf("%ld %d %s\n", 
            ft_gettime() - philo->data->time_to_start,
            philo->id, 
            state);
        pthread_mutex_unlock(philo->data->print_mutex);
    }
    pthread_mutex_unlock(philo->data->stop_mutex);
}