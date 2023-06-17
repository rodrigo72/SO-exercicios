typedef enum request_type {
    NEW,
    STRING
} REQUEST_TYPE;

typedef struct header {
    size_t size;
    REQUEST_TYPE type;
    int pid;
} HEADER;

typedef struct header_private {
    size_t size;
    REQUEST_TYPE type;
} HEADER_PRIVATE;