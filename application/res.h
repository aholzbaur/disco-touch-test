#ifndef RES_H
#define RES_H

enum RES_t {
  RES_OK = 0,
  RES_ERROR = 1,
  RES_TIMEOUT = 2,
  RES_NOT_FOUND = 3,
};
typedef enum RES_t RES;

#endif