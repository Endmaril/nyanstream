#ifndef __NYAN_COMMON__
#define __NYAN_COMMON__

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define NYAN_BUFFER_SIZE 8192
#define NYAN_MESSAGE_SIZE 1337
#define NYAN_MAGIC Ox6723

#define NYAN_TYPE_FORMAT Ox00
#define NYAN_TYPE_ACK Ox01
#define NYAN_TYPE_WAVE Ox02
#define NYAN_TYPE_SPEEX Ox03
#define NYAN_TYPE_JPG Ox04

#endif
