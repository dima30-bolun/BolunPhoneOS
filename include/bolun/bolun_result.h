#ifndef BOLUN_RESULT_H
#define BOLUN_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BolunResult {
    BOLUN_OK = 0,
    BOLUN_ERROR_INVALID_ARGUMENT = 1,
    BOLUN_ERROR_NOT_BOOTED = 2,
    BOLUN_ERROR_NOT_FOUND = 3,
    BOLUN_ERROR_FULL = 4,
    BOLUN_ERROR_INTERNAL = 100
} BolunResult;

const char *bolun_result_message(BolunResult result);

#ifdef __cplusplus
}
#endif

#endif
