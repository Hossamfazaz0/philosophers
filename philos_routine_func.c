// #include "philo.h"
// int eat(t_philo *philo)
// {
//     pthread_mutex_lock(philo->data->print_mutex);      
//     pthread_mutex_lock(philo->data->stop_mutex);       
//     pthread_mutex_lock(philo->data->last_meal_mutex);  
//     long current_time = ft_gettime();
//     if (philo->data->stop || 
//         current_time - philo->last_meal_time > philo->data->time_to_die)
//     {
//         pthread_mutex_unlock(philo->data->last_meal_mutex);
//         pthread_mutex_unlock(philo->data->stop_mutex);
//         pthread_mutex_unlock(philo->data->print_mutex);
//         return 1;
//     }
    
//     pthread_mutex_unlock(philo->data->last_meal_mutex);
//     pthread_mutex_unlock(philo->data->stop_mutex);
//     pthread_mutex_unlock(philo->data->print_mutex);
//     pthread_mutex_lock(philo->left_fork);
//     print_state(philo,"taken a fork");
//     pthread_mutex_lock(philo->right_fork);
//     print_state(philo,"taken a fork");
//     print_state(philo,"eating");
//     pthread_mutex_lock(philo->data->last_meal_mutex);
//     philo->last_meal_time = ft_gettime();
//     pthread_mutex_unlock(philo->data->last_meal_mutex);
//     ft_usleep(philo->data->time_to_eat);
//     pthread_mutex_lock(philo->data->meals_mutex);
//     philo->meals_eaten++;
//     if(philo->meals_eaten == philo->data->nb_of_meals)
//         philo->data->number_philos_ate++;
//     pthread_mutex_unlock(philo->data->meals_mutex);
//     pthread_mutex_unlock(philo->right_fork);
//     pthread_mutex_unlock(philo->left_fork);
//     return 0;
// }