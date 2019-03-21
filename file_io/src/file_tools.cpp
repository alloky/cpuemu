#pragma once
#include <assert.h>

#include "file_tools.h"
#include "logging.h"

#ifdef WIN
    #include <io.h>
    #define fr_stat _stat
    #define fr_open _open
    #define fr_read _read
    #define fr_write _write
    #define fr_close _close
    #define FR_RDONLY _O_RDONLY 
    #define FR_RULES 0777
    #define FR_WRTCREAT (_O_WRONLY | _O_CREAT)
#else
    #include <unistd.h>
    #define fr_stat stat
    #define fr_open open
    #define fr_read read
    #define fr_write write
    #define fr_close close 
    #define FR_RDONLY O_RDONLY
    #define FR_RULES 0777
    #define FR_WRTCREAT (O_WRONLY | O_CREAT)
#endif



void _mark_ptrs(char* buffer, char** string_arr);
int _process_error(const char* filename);

int get_file_size(const char* filename){
    
    struct fr_stat file_info;
    int status = fr_stat(filename, &file_info);
    
    int code = errno;

    if(status == 0){
        return file_info.st_size;
    }
    
    return _process_error(filename);
}


/*

 _____ ___ _     _____   ________  _   _ _____
|  ___|_ _| |   | ____| |__  / _ \| \ | | ____|
| |_   | || |   |  _|     / / | | |  \| |  _|
|  _|  | || |___| |___   / /| |_| | |\  | |___
|_|   |___|_____|_____| /____\___/|_| \_|_____|


*/

int ft_open(FILE** file, const char* path, const char* mode){
    *file = fopen(path, mode);
    if(*file == NULL){
        LEV_LOG(LL_ERR, "While opening " << path << " error occured");
        return _process_error(""); 
    }
    LEV_LOG(LL_INFO, "File " << path << " opened.");
    return 0; 
}

int ft_read(FILE* file, char** buff, size_t len){
    *buff = new char[len + 1];
    int num_bytes = fread(*buff, sizeof(char), len, file);
    if(num_bytes != len && !feof(file)){
        return _process_error("While reading from file error occured"); 
    }
    (*buff)[len] = '\0';
    return 0; 
}

int ft_write(FILE* file, const char* buff, size_t len, char sep='\n'){
    int num_bytes = fwrite(buff, sizeof(char), len, file);
    if(num_bytes != len){
        return _process_error("While writing to file error occured"); 
    }
    if(sep != '\0'){
        num_bytes = fwrite(&sep, sizeof(char), 1, file);
        if(num_bytes != 1){
            return _process_error("While writing to file error occured"); 
        }   
    }
    return 0; 
}

int ft_write_offset(FILE* file, int size) {
	fseek(file, size, SEEK_CUR);
	char tmp = EOF;
	fwrite(&tmp, sizeof(char), 1, file);
	fseek(file, -1, SEEK_CUR);
	return 0;
}

size_t ft_size(FILE* file){
	long prev = ftell(file);
    fseek(file , 0 , SEEK_END);
    long lSize = ftell(file) - prev;
    fseek(file , prev , SEEK_SET);
    return (size_t)lSize;
}

int ft_close(FILE* file){
    int status = fclose(file);
    if(status != 0){
        return _process_error("While while closing file error occured"); 
    }
    LEV_LOG(LL_INFO, "File closed.");
    return 0;
}



/*
    _____ _______ _____    _______ ____   ____  _       _____
   |  __ \__   __|  __ \  |__   __/ __ \ / __ \| |     / ____|
   | |__) | | |  | |__) |    | | | |  | | |  | | |    | (___
   |  ___/  | |  |  _  /     | | | |  | | |  | | |     \___ \
   | |      | |  | | \ \     | | | |__| | |__| | |____ ____) |
   |_|      |_|  |_|  \_\    |_|  \____/ \____/|______|_____/
   
 */

size_t make_ptr_arr( char* buffer, char*** ptr_arr, size_t size){
    size_t s_count = get_lines_count(buffer);
	buffer[size - s_count] = '\0';
    *ptr_arr = new char*[s_count];
    assert(ptr_arr != NULL); 
    _mark_ptrs(buffer, *ptr_arr); 
    return s_count;
}

void _mark_ptrs(char* buffer, char** string_arr){
    char* cur = buffer;
    size_t i = 1;
    string_arr[0] = buffer;
    while(*cur != '\0'){
        if(*cur == '\n'){
            string_arr[i] = cur + 1;
            *cur = '\0';
            ++i;
        }
        ++cur;
    }
}

size_t get_lines_count(const char* buff){
    size_t ind = 0;
    size_t count = 0;    


    assert(buff != NULL);

    while(buff[ind] != '\0'){
        if(buff[ind] == '\n'){
            ++count;
        }
        ++ind;
    }
    
    return count + 1; 
}

int _process_error(const char* msg){
    int code = errno;
    
    LEV_LOG(LL_ERR, msg);
    LEV_LOG(LL_ERR, strerror(code));

    if(code == ENOENT){
        return FR_NOT_FOUND;
    }

    return FR_READ_ERR;
}
