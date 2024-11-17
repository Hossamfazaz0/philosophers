#include "philo.h"

static size_t ft_gettime(void)
{
    struct timeval time;
    
    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void ft_usleep(long ms)
{
    long start;
    
    start = ft_gettime();
    while (ft_gettime() - start < ms)
        usleep(500);
}

int init_data(t_data *data, int ac, char **av)
{
    if (ac < 5 || ac > 6)
        return (0);
    
    data->nb_of_philo = atoi(av[1]);
    data->time_to_die = atoi(av[2]);
    data->time_to_eat = atoi(av[3]);
    data->time_to_sleep = atoi(av[4]);
    data->nb_of_meals = (ac == 6 ? atoi(av[5]) : -1);
    data->number_philos_ate = 0;  // Added missing initialization
    data->forks = NULL;
    data->philosophers = NULL;
    data->meals_mutex = NULL;
    data->print_mutex = NULL;
    data->stop = 0;
    data->stop_mutex = NULL;  // Changed from 0 to NULL
    data->time_to_start = ft_gettime();
    
    if (data->nb_of_philo <= 0 || data->time_to_die <= 0 || 
        data->time_to_eat <= 0 || data->time_to_sleep <= 0 || 
        (ac == 6 && data->nb_of_meals <= 0))
        return (0);
    
    return (1);
}

int init_mutexes(t_data *data)
{
    int i;
    
    data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_of_philo);
    if (!data->forks)
        return (0);
        
    data->last_meal_mutex = malloc(sizeof(pthread_mutex_t));
    data->meals_mutex = malloc(sizeof(pthread_mutex_t));
    data->stop_mutex = malloc(sizeof(pthread_mutex_t));
    data->print_mutex = malloc(sizeof(pthread_mutex_t));
    data->philosophers = malloc(sizeof(pthread_t) * data->nb_of_philo);
    
    if (!data->last_meal_mutex || !data->meals_mutex || !data->stop_mutex || 
        !data->print_mutex || !data->philosophers)
        return (0);
    
    i = 0;
    while (i < data->nb_of_philo)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            return (0);
        i++;
    }
    
    if (pthread_mutex_init(data->print_mutex, NULL) != 0 ||
        pthread_mutex_init(data->stop_mutex, NULL) != 0 ||
        pthread_mutex_init(data->meals_mutex, NULL) != 0 ||
        pthread_mutex_init(data->last_meal_mutex, NULL) != 0)
        return (0);
        
    return (1);
}

t_philo *init_philos(t_data *data, t_philo *philos)
{
    int i;
    
    philos = malloc(sizeof(t_philo) * data->nb_of_philo);
    if (!philos)
        return (0);
        
    i = 0;
    while (i < data->nb_of_philo)
    {
        philos[i].id = i + 1;
        philos[i].data = data;
        if (i % 2 == 0)
        {
            philos[i].left_fork = &data->forks[i];
            philos[i].right_fork = &data->forks[(i + 1) % data->nb_of_philo];
        }
        else
        {
            philos[i].right_fork = &data->forks[i];
            philos[i].left_fork = &data->forks[(i + 1) % data->nb_of_philo];
        }
        philos[i].last_meal_time = ft_gettime();
        philos[i].meals_eaten = 0;
        i++;
    }
    return (1);
}

int check_death(t_philo *philo)
{
    long time;
    
    pthread_mutex_lock(philo->data->stop_mutex);
    if (philo->data->stop)
    {
        pthread_mutex_unlock(philo->data->stop_mutex);
        return (1);
    }
    
    pthread_mutex_lock(philo->data->last_meal_mutex);
    time = ft_gettime();
    if (time - philo->last_meal_time > philo->data->time_to_die)
    {
        pthread_mutex_lock(philo->data->print_mutex);
        printf("%ld %d died\n", 
            ft_gettime() - philo->data->time_to_start, 
            philo->id);
        philo->data->stop = 1;
        pthread_mutex_unlock(philo->data->print_mutex);
        pthread_mutex_unlock(philo->data->last_meal_mutex);
        pthread_mutex_unlock(philo->data->stop_mutex);
        return (1);
    }
    pthread_mutex_unlock(philo->data->last_meal_mutex);
    pthread_mutex_unlock(philo->data->stop_mutex);
    return (0);
}

void print_status(t_philo *philo, char *status)
{
    pthread_mutex_lock(philo->data->print_mutex);
    pthread_mutex_lock(philo->data->stop_mutex);
    if (!philo->data->stop)
        printf("%ld %d %s\n", 
            ft_gettime() - philo->data->time_to_start, 
            philo->id, 
            status);
    pthread_mutex_unlock(philo->data->stop_mutex);
    pthread_mutex_unlock(philo->data->print_mutex);
}

int eat(t_philo *philo)
{
    pthread_mutex_lock(philo->left_fork);
    print_status(philo, "has taken a fork");
    if (philo->data->nb_of_philo == 1)
    {
        pthread_mutex_unlock(philo->left_fork);
        return (1);
    }
    
    pthread_mutex_lock(philo->right_fork);
    print_status(philo, "has taken a fork");
    print_status(philo, "is eating");
    
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
    return (0);
}

static void *philo_routine(void *philos_i)
{
    t_philo *philo;
    
    philo = (t_philo *)philos_i;
    pthread_mutex_lock(philo->data->last_meal_mutex);
    philo->last_meal_time = ft_gettime();
    pthread_mutex_unlock(philo->data->last_meal_mutex);
    
    if (philo->id % 2 == 0)
        ft_usleep(philo->data->time_to_eat / 2);
        
    while (1)
    {
        pthread_mutex_lock(philo->data->stop_mutex);
        if (philo->data->stop)
        {
            pthread_mutex_unlock(philo->data->stop_mutex);
            break;
        }
        pthread_mutex_unlock(philo->data->stop_mutex);
        
        if (eat(philo))
            break;
        print_status(philo, "is sleeping");
        ft_usleep(philo->data->time_to_sleep);
        print_status(philo, "is thinking");
    }
    return (NULL);
}

static void *monitor_routine(void *philos)
{
    t_philo *philo;
    int i;
    
    philo = (t_philo *)philos;
    while (1)
    {
        i = 0;
        while (i < philo->data->nb_of_philo)
        {
            if (check_death(&philo[i]))
                return (NULL);
            i++;
        }
        
        pthread_mutex_lock(philo->data->meals_mutex);
        if (philo->data->nb_of_meals != -1 && 
            philo->data->number_philos_ate == philo->data->nb_of_philo)
        {
            pthread_mutex_lock(philo->data->stop_mutex);
            philo->data->stop = 1;
            pthread_mutex_unlock(philo->data->stop_mutex);
            pthread_mutex_unlock(philo->data->meals_mutex);
            return (NULL);
        }
        pthread_mutex_unlock(philo->data->meals_mutex);
        usleep(1000);
    }
}

int start_simulation(t_data *data, t_philo *philos)
{
    int i;
    
    i = 0;
    while (i < data->nb_of_philo)
    {
        if (pthread_create(&data->philosophers[i], NULL, philo_routine, &philos[i]) != 0)
            return (0);
        i++;
    }
    
    if (pthread_create(&data->monitor, NULL, monitor_routine, philos) != 0)
        return (0);
        
    i = 0;
    while (i < data->nb_of_philo)
    {
        if (pthread_join(data->philosophers[i], NULL) != 0)
            return (0);
        i++;
    }
    
    if (pthread_join(data->monitor, NULL) != 0)
        return (0);
        
    return (1);
}

int main(int ac, char **av)
{
    t_data *data;
    t_philo *philos;
    
    data = malloc(sizeof(t_data));

    
    init_data(data, ac, av) ;
    philos = init_philos(data);
    
    
   
    
    // Add cleanup function here
    return (0);
}