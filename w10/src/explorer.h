#define NUM_COMMANDS 2

#define BUF_SIZE 1000

void build_graph();
int randint(int n);
void create_random_color(char* data);
void generate_several_lines_plot_command(char* data);
void generate_plot_command_of_one_line(char* data, int num_threads);
