/* philo.h */
#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>
# include <stdbool.h>

# define SUCCESS 0
# define ERROR 1

typedef struct timeval t_time;

typedef enum e_state {
    THINKING,
    EATING,
    SLEEPING,
    FORK_TAKEN,
    DIED
} t_state;

typedef struct s_data {
    long            nb_of_philo;
    long            time_to_die;
    long            time_to_eat;
    long            time_to_sleep;
    long            nb_of_meals;
    long            time_to_start;
    int             stop;
    pthread_mutex_t *print_mutex;
    pthread_mutex_t *stop_mutex;
    pthread_mutex_t *last_meal_mutex;
    pthread_mutex_t *meals_mutex;
    int             number_philos_ate;
    pthread_t       *philosophers;
    pthread_t       monitor;
    pthread_mutex_t *forks;
} t_data;

typedef struct s_philo {
    int             id;
    int             meals_eaten;
    long            last_meal_time;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    t_data         *data;
} t_philo;

/* Utils */
int     ft_atoi(const char *str);
size_t  ft_gettime(void);
void    ft_usleep(long ms);
void    clean_exit(t_data *data, t_philo *philos);
void    print_state(t_philo *philo, const char *state);

/* Init */
int     init_data(t_data *data, int ac, char **av);
int     init_mutexes(t_data *data);
int     init_philosophers(t_data *data, t_philo **philos);

/* Core */
void    *philosopher_routine(void *arg);
void    *monitor_routine(void *arg);
int     start_simulation(t_data *data, t_philo *philos);

#endif

/* main.c */
#include "philo.h"


/* init.c */
#include "philo.h"

int init_data(t_data *data, int ac, char **av)
{
    data->nb_of_philo = ft_atoi(av[1]);
    data->time_to_die = ft_atoi(av[2]);
    data->time_to_eat = ft_atoi(av[3]);
    data->time_to_sleep = ft_atoi(av[4]);
    data->nb_of_meals = (ac == 6) ? ft_atoi(av[5]) : -1;
    
    if (data->nb_of_philo <= 0 || data->time_to_die <= 0 ||
        data->time_to_eat <= 0 || data->time_to_sleep <= 0 ||
        (ac == 6 && data->nb_of_meals <= 0))
        return (ERROR);
    
    data->time_to_start = ft_gettime();
    data->stop = 0;
    data->number_philos_ate = 0;
    data->philosophers = NULL;
    data->forks = NULL;
    data->print_mutex = NULL;
    data->stop_mutex = NULL;
    data->last_meal_mutex = NULL;
    data->meals_mutex = NULL;
    
    return (SUCCESS);
}

int init_mutexes(t_data *data)
{
    int i;
    
    data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_of_philo);
    data->print_mutex = malloc(sizeof(pthread_mutex_t));
    data->stop_mutex = malloc(sizeof(pthread_mutex_t));
    data->last_meal_mutex = malloc(sizeof(pthread_mutex_t));
    data->meals_mutex = malloc(sizeof(pthread_mutex_t));
    data->philosophers = malloc(sizeof(pthread_t) * data->nb_of_philo);
    
    if (!data->forks || !data->print_mutex || !data->stop_mutex ||
        !data->last_meal_mutex || !data->meals_mutex || !data->philosophers)
        return (ERROR);
    
    for (i = 0; i < data->nb_of_philo; i++)
        if (pthread_mutex_init(&data->forks[i], NULL) != SUCCESS)
            return (ERROR);
    
    if (pthread_mutex_init(data->print_mutex, NULL) != SUCCESS ||
        pthread_mutex_init(data->stop_mutex, NULL) != SUCCESS ||
        pthread_mutex_init(data->last_meal_mutex, NULL) != SUCCESS ||
        pthread_mutex_init(data->meals_mutex, NULL) != SUCCESS)
        return (ERROR);
    
    return (SUCCESS);
}

int init_philosophers(t_data *data, t_philo **philos)
{
    int i;
    
    *philos = malloc(sizeof(t_philo) * data->nb_of_philo);
    if (!*philos)
        return (ERROR);
    
    for (i = 0; i < data->nb_of_philo; i++)
    {
        (*philos)[i].id = i + 1;
        (*philos)[i].meals_eaten = 0;
        (*philos)[i].last_meal_time = ft_gettime();
        (*philos)[i].data = data;
        
        // Prevent deadlock by having odd numbered philosophers
        // take forks in opposite order
        if (i % 2 == 0)
        {
            (*philos)[i].left_fork = &data->forks[i];
            (*philos)[i].right_fork = &data->forks[(i + 1) % data->nb_of_philo];
        }
        else
        {
            (*philos)[i].right_fork = &data->forks[i];
            (*philos)[i].left_fork = &data->forks[(i + 1) % data->nb_of_philo];
        }
    }
    
    return (SUCCESS);
}

/* utils.c */
#include "philo.h"

size_t ft_gettime(void)
{
    t_time time;
    
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

int ft_atoi(const char *str)
{
    long result;
    int sign;
    
    result = 0;
    sign = 1;
    
    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
    
    if (*str == '-')
        sign = -1;
    if (*str == '-' || *str == '+')
        str++;
    
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        if (result * sign > INT_MAX || result * sign < INT_MIN)
            return (sign == 1 ? -1 : 0);
        str++;
    }
    
    return (result * sign);
}

void print_state(t_philo *philo, const char *state)
{
    pthread_mutex_lock(philo->data->stop_mutex);     // Lock stop first
    if (!philo->data->stop)
    {
        pthread_mutex_lock(philo->data->print_mutex); // Then print
        printf("%ld %d %s\n", 
            ft_gettime() - philo->data->time_to_start,
            philo->id,
            state);
        pthread_mutex_unlock(philo->data->print_mutex);
    }
    pthread_mutex_unlock(philo->data->stop_mutex);
}

void clean_exit(t_data *data, t_philo *philos)
{
    int i;
    
    if (data->forks)
    {
        for (i = 0; i < data->nb_of_philo; i++)
            pthread_mutex_destroy(&data->forks[i]);
        free(data->forks);
    }
    
    if (data->print_mutex)
    {
        pthread_mutex_destroy(data->print_mutex);
        free(data->print_mutex);
    }
    if (data->stop_mutex)
    {
        pthread_mutex_destroy(data->stop_mutex);
        free(data->stop_mutex);
    }
    if (data->last_meal_mutex)
    {
        pthread_mutex_destroy(data->last_meal_mutex);
        free(data->last_meal_mutex);
    }
    if (data->meals_mutex)
    {
        pthread_mutex_destroy(data->meals_mutex);
        free(data->meals_mutex);
    }
    
    if (data->philosophers)
        free(data->philosophers);
    if (philos)
        free(philos);
    free(data);
}

/* core.c */
#include "philo.h"

static int should_stop(t_data *data)
{
    int stop;
    
    pthread_mutex_lock(data->stop_mutex);
    stop = data->stop;
    pthread_mutex_unlock(data->stop_mutex);
    return (stop);
}

static int check_death(t_philo *philo)
{
    long current_time;
    
    pthread_mutex_lock(philo->data->stop_mutex);      // Lock stop first
    if (philo->data->stop)
    {
        pthread_mutex_unlock(philo->data->stop_mutex);
        return (0);
    }
    
    pthread_mutex_lock(philo->data->last_meal_mutex); // Then last_meal
    current_time = ft_gettime();
    if (current_time - philo->last_meal_time > philo->data->time_to_die)
    {
        pthread_mutex_lock(philo->data->print_mutex);  // Then print if needed
        printf("%ld %d %s\n", 
            ft_gettime() - philo->data->time_to_start,
            philo->id,
            "died");
        pthread_mutex_unlock(philo->data->print_mutex);
            
        philo->data->stop = 1;
        pthread_mutex_unlock(philo->data->last_meal_mutex);
        pthread_mutex_unlock(philo->data->stop_mutex);
        return (1);
    }
    
    pthread_mutex_unlock(philo->data->last_meal_mutex);
    pthread_mutex_unlock(philo->data->stop_mutex);
    return (0);
}

static int eat(t_philo *philo)
{
    pthread_mutex_lock(philo->left_fork);
    print_state(philo, "has taken a fork");
    if (philo->data->nb_of_philo == 1)
    {
        pthread_mutex_unlock(philo->left_fork);
        return (1);
    }
    
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
    
    return (0);
}

void *philosopher_routine(void *arg)
{
    t_philo *philo;
    
    philo = (t_philo *)arg;
    if (philo->id % 2 == 0)
        ft_usleep(philo->data->time_to_eat / 2);
    
    while (!should_stop(philo->data))
    {
        if (eat(philo))
            break;
        
        print_state(philo, "is sleeping");
        ft_usleep(philo->data->time_to_sleep);
        
        print_state(philo, "is thinking");
    }
    return (NULL);
}

void *monitor_routine(void *arg)
{
    t_philo *philos;
    int i;
    
    philos = (t_philo *)arg;
    while (!should_stop(philos->data))
    {
        i = 0;
        while (i < philos->data->nb_of_philo && !should_stop(philos->data))
        {
            if (check_death(&philos[i]))
                return (NULL);
                
            pthread_mutex_lock(philos->data->meals_mutex);
            if (philos->data->nb_of_meals != -1 && 
                philos->data->number_philos_ate == philos->data->nb_of_philo)
            {
                pthread_mutex_lock(philos->data->stop_mutex);
                philos->data->stop = 1;
                pthread_mutex_unlock(philos->data->stop_mutex);
                pthread_mutex_unlock(philos->data->meals_mutex);
                return (NULL);
            }
            pthread_mutex_unlock(philos->data->meals_mutex);
            i++;
        }
        ft_usleep(1);
    }
    return (NULL);
}

/* core.c continued... */

int start_simulation(t_data *data, t_philo *philos)
{
    int i;
    
    data->time_to_start = ft_gettime();
    for (i = 0; i < data->nb_of_philo; i++)
    {
        if (pthread_create(&data->philosophers[i], NULL,
            philosopher_routine, &philos[i]) != SUCCESS)
            return (ERROR);
    }
    
    if (pthread_create(&data->monitor, NULL,
        monitor_routine, philos) != SUCCESS)
        return (ERROR);
    
    for (i = 0; i < data->nb_of_philo; i++)
        if (pthread_join(data->philosophers[i], NULL) != SUCCESS)
            return (ERROR);
            
    if (pthread_join(data->monitor, NULL) != SUCCESS)
        return (ERROR);
    
    return (SUCCESS);
}

/* error_handling.c */
#include "philo.h"

int check_input_values(t_data *data)
{
    if (data->nb_of_philo < 1 || data->nb_of_philo > 200)
    {
        printf("Error: Invalid number of philosophers (1-200)\n");
        return (ERROR);
    }
    
    if (data->time_to_die < 60)
    {
        printf("Error: Time to die must be at least 60ms\n");
        return (ERROR);
    }
    
    if (data->time_to_eat < 60)
    {
        printf("Error: Time to eat must be at least 60ms\n");
        return (ERROR);
    }
    
    if (data->time_to_sleep < 60)
    {
        printf("Error: Time to sleep must be at least 60ms\n");
        return (ERROR);
    }
    
    if (data->nb_of_meals != -1 && data->nb_of_meals < 1)
    {
        printf("Error: Number of meals must be positive\n");
        return (ERROR);
    }
    
    return (SUCCESS);
}

void handle_mutex_error(t_data *data, t_philo *philos)
{
    printf("Error: Failed to initialize mutex\n");
    clean_exit(data, philos);
    exit(ERROR);
}

void handle_thread_error(t_data *data, t_philo *philos)
{
    printf("Error: Failed to create thread\n");
    pthread_mutex_lock(data->stop_mutex);
    data->stop = 1;
    pthread_mutex_unlock(data->stop_mutex);
    clean_exit(data, philos);
    exit(ERROR);
}

/* Additional utility functions */
void check_simulation_end(t_data *data, t_philo *philos)
{
    pthread_mutex_lock(data->meals_mutex);
    if (data->nb_of_meals != -1 && 
        data->number_philos_ate == data->nb_of_philo)
    {
        pthread_mutex_lock(data->print_mutex);
        printf("All philosophers have eaten %ld times\n", data->nb_of_meals);
        pthread_mutex_unlock(data->print_mutex);
        
        pthread_mutex_lock(data->stop_mutex);
        data->stop = 1;
        pthread_mutex_unlock(data->stop_mutex);
    }
    pthread_mutex_unlock(data->meals_mutex);
}

int check_philosopher_alive(t_philo *philo)
{
    long current_time;
    int alive;
    
    pthread_mutex_lock(philo->data->last_meal_mutex);
    current_time = ft_gettime();
    alive = (current_time - philo->last_meal_time <= philo->data->time_to_die);
    pthread_mutex_unlock(philo->data->last_meal_mutex);
    
    return alive;
}

/* Improved main function with better error handling */
int main(int ac, char **av)
{
    t_data *data;
    t_philo *philos;
    
    if (ac != 5 && ac != 6)
    {
        printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat ");
        printf("time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
        return (ERROR);
    }
    
    data = malloc(sizeof(t_data));
    if (!data)
    {
        printf("Error: Memory allocation failed\n");
        return (ERROR);
    }
    
    if (init_data(data, ac, av) != SUCCESS)
    {
        printf("Error: Invalid input values\n");
        free(data);
        return (ERROR);
    }
    
    if (check_input_values(data) != SUCCESS)
    {
        free(data);
        return (ERROR);
    }
    
    if (init_mutexes(data) != SUCCESS)
        handle_mutex_error(data, NULL);
    
    if (init_philosophers(data, &philos) != SUCCESS)
    {
        printf("Error: Failed to initialize philosophers\n");
        clean_exit(data, NULL);
        return (ERROR);
    }
    
    if (start_simulation(data, philos) != SUCCESS)
        handle_thread_error(data, philos);
    
    clean_exit(data, philos);
    return (SUCCESS);
}