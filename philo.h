#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct timeval	t_time;

typedef struct s_data
{
	long			nb_of_philo;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	long			nb_of_meals;
	long			time_to_start;
	int				stop;
	pthread_mutex_t	*m1;
	pthread_mutex_t	*m2;
	pthread_mutex_t	*m_last_time;
	pthread_mutex_t	*meals_mutex;
	int				number_philos_ate;
	pthread_t		*threads;
	pthread_t		monitor;
	pthread_mutex_t *forks;
}					t_data;

typedef struct s_philo
{
	int				philo_id;
	int				number_of_meals;
	long			last_time_eat;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_data		*data;
}					t_philo;

#endif