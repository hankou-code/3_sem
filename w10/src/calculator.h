#define BEGINNING_INTERVAL 0
#define ENDING_INTERVAL 2

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define BAD_MESSAGE         -13
#define SUCCESS               0

#define USEC_IN_SEC 1000000
#define NUM_SAMPLES_TO_AVG 10

typedef struct {
  double start_interval;
  double end_interval;
  double step;
  double sum;
  int num;
} Data;

double func(double x);

void filling_info_about_intervals(long num_segments, int num_threads, Data* info_about_intervals);

void* sum_at_subinterval(void* args);

double integrate_func(long num_segments, int num_threads);

double find_avg_time_and_avg_res(long num_segments, long num_threads, double* avg_res);

