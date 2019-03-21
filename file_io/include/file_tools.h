#pragma once

#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

const int FR_NOT_FOUND = -1; //!< FileNotFound error num
const int FR_READ_ERR = -2; //!< Error while reading num
const int FR_EMPTY_FILE = -3; //!< Empty file error num



/**
 * @brief      Count lines in string
 *
 * @param[in]  buff  String
 *
 * @return     Lines count.
 */
size_t get_lines_count(const char* buff);


/**
 * @brief      Makes a pointer arr.
 *
 * @param[in]  buffer  File buffer
 * @param[in]  ptr_arr Ptr array
 *
 * @return     line count
 */
size_t make_ptr_arr(char* buffer, char*** ptr_arr, size_t size);


/*
  _____ ___ _     _____   ________  _   _ _____
|  ___|_ _| |   | ____| |__  / _ \| \ | | ____|
| |_   | || |   |  _|     / / | | |  \| |  _|
|  _|  | || |___| |___   / /| |_| | |\  | |___
|_|   |___|_____|_____| /____\___/|_| \_|_____|

 */

/**
 * @brief      Open file in given mode
 *
 * @param      file  The file
 * @param[in]  path  The path
 * @param[in]  mode  The mode
 *
 * @return     Status code, 0 if OK. 
 * 
 */
int ft_open(FILE** file, const char* path, const char* mode);

/**
 * @brief      Read len bytes from file
 *
 * @param      file  The file
 * @param      buff  The buffer
 * @param[in]  len   The length
 *
 * @return     Status code, 0 if OK. 
 */
int ft_read(FILE* file, char** buff, size_t len);

/**
 * @brief      Write len bytes to file, ended with sep char
 *
 * @param      file  The file
 * @param[in]  buff  The buffer
 * @param[in]  len   The length
 * @param[in]  sep   The separator
 *
 * @return     Status code, 0 if OK. 
 * 
 * Default sep char in newline
 */
int ft_write(FILE* file, const char* buff, size_t len, char sep);

int ft_write_offset(FILE * file, int size);

/**
 * @brief      Return size of file in bytes
 *
 * @param      file  The file
 *
 * @return     Size of file in bytes
 */
size_t ft_size(FILE* file);

/**
 * @brief      Close file object
 *
 * @param      file  The file
 *
 * @return     Status code, 0 if OK. 
 */
int ft_close(FILE* file);