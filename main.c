#include "philo.h"
int main(int ac , char **av)
{
    t_data	*data;
	t_philo	*philo;
	data = malloc(sizeof(t_data));
	// data = malloc(sizeof(t_data));
	// philo = malloc(sizeof(t_philo));
	init_data(data, ac, av);
	philo = malloc(sizeof(t_philo ) * data->nb_of_philo);
	init_mutexes(data);
	init_philosophers(data, philo);
	start_simulation(data, philo);
}