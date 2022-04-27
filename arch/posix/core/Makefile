#
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2022 Friedt Professional Engineering Services, Inc
#
# SPDX-License-Identifier: Apache-2.0
#

PREFIX ?= .
SRC_DIR ?= $(PREFIX)
OBJ_DIR ?= $(PREFIX)/obj
LIB_DIR ?= $(PREFIX)/lib

all:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/cpuhalt.c -o $(OBJ_DIR)/cpuhalt.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/fatal.c -o $(OBJ_DIR)/fatal.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/posix_arch.c -o $(OBJ_DIR)/posix_arch.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/posix_core.c -o $(OBJ_DIR)/posix_core.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/irq.c -o $(OBJ_DIR)/irq.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/swap.c -o $(OBJ_DIR)/swap.o
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SRC_DIR)/thread.c -o $(OBJ_DIR)/thread.o
	$(AR) -rcs $(LIB_DIR)/libposix_core.a \
		$(OBJ_DIR)/cpuhalt.o \
		$(OBJ_DIR)/fatal.o \
		$(OBJ_DIR)/fatal.o \
		$(OBJ_DIR)/irq.o \
		$(OBJ_DIR)/posix_arch.o \
		$(OBJ_DIR)/posix_core.o \
		$(OBJ_DIR)/swap.o \
		$(OBJ_DIR)/thread.o

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)
