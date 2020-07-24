#ifndef TLS_CLIENT
#define TLS_CLIENT

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t data[8 * 1024];
    int32_t size;
} read_buff_t;

int tls_client(const uint8_t *data, uint32_t size, const char *server_name, read_buff_t *read_buf);

#ifdef	__cplusplus
}
#endif

#endif // TLS_CLIENT
