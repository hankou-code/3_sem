#define MIN_SEGMENTS 1000000
#define MAX_SEGMENTS 10000000
#define SEGMENT_MULTIPLIER 2
#define SEGMENT_ADD_STEP 1000000

#define FLAG_MULTIPLY 1
#define FLAG_ACCUMULATE 2

#define MIN_THREADS 4
#define MAX_THREADS 8

// explain why I use 0666:
// https://stackoverflow.com/questions/2245193/why-does-open-create-my-file-with-the-wrong-permissions
#define RWX_PERMISSION 0666