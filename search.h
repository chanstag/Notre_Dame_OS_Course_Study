/* search.h */

#ifndef SEARCH_H
#define	SEARCH_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#define DEBUG

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

/* Macros */

#define	    streq(s0, s1)   (strcmp((s0), (s1)) == 0)
#define     debug(M, ...) \
    fprintf(stderr, "DEBUG %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

/* Type definitions */
/** @struct Settings
 * This represents the settings struct used to filter every path searched 
 * 
 * @param  access  Access modes (-executable, -readable, -writable)
 * 
 * @param type File type (-type)
 * 
 * @param empty Empty files and directories (-empty)
 * 
 * @param name pointer to base of file name
 * 
 * @param path Path of file matches shell pattern (-path)
 * 
 * @param perm File's permission bits are exactly octal mode (-perm)
 * 
 * @param newer File was modified more recently than file (-newer)
 * 
 * @param uid Files numeric user ID is n
 * 
 * @param gid File's numeric group ID is n
 * 
 * @param print (-print)
 * 
 * @param exec_argc number of arguments for (-exec)
 *  
 * @param exec_argv arguments for (-exec)
 */
typedef struct {
    int	    access;	/* Access modes (-executable, -readable, -writable) */
    int     type;       /* File type (-type); */
    bool    empty;      /* Empty files and directories (-empty) */
    char   *name;       /* Base of file name matches shell pattern (-name) */
    char   *path;       /* Path of file matches shell pattern (-path) */
    int     perm;       /* File's permission bits are exactly octal mode (-perm) */
    time_t  newer;      /* File was modified more recently than file (-newer) */ 
    int     uid;        /* File's numeric user ID is n */
    int     gid;        /* File's numeric group ID is n */

    bool    print;      /* Print (-print) */

    int     exec_argc;  /* Number of arguments for (-exec) */
    char  **exec_argv;  /* Arguments for (-exec) */

} Settings;

/* Function prototypes */

int	        search(const char *root, const Settings *settings);
bool	    filter(const char *path, const Settings *settings);
int         execute(const char *path, const Settings *settings);

bool        is_directory_empty(const char *path);
time_t      get_mtime(const char *path);
void        initSettings(Settings *settings);

void        print_directory_contents(const char *path);
mode_t      int_to_mode(int perm);
char**      alloc_stack(size_t size);
int         expand_stack(char** stack, size_t* size);

void sig_handler(int sig);

#endif

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
