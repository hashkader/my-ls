#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

typedef struct
{
    char *name;
    time_t mtime;
    long nsec;
} FileDetail;

char **parse_arguments(int argc, char *argv[], int *show_all, int *sort_time, int *dir_count);
void bubble_sort_dirs(char **dir_names, int dir_count);
int get_file_details(const char *dir_name, FileDetail **files, int show_all, int sort_time);
void free_file_details(FileDetail *files, int count);
void display_files(FileDetail *files, int count);
void bubble_sort_lex(FileDetail *files, int count);
void bubble_sort_time(FileDetail *files, int count);

int main(int argc, char *argv[])
{
    int show_all = 0;
    int sort_time = 0;
    int dir_count = 0;
    char **dir_names =
    parse_arguments(argc, argv, &show_all, &sort_time, &dir_count);

    bubble_sort_dirs(dir_names, dir_count);

    for (int i = 0; i < dir_count; i++)
    {
        char *dir_name = dir_names[i];
        if (dir_count >= 2)
        {
            printf("%s:\n", dir_name);
        }

        FileDetail *files = NULL;
        int file_count =
            get_file_details(dir_name, &files, show_all, sort_time);

        if (sort_time)
        {
            bubble_sort_time(files, file_count);
        }
        else
        {
            bubble_sort_lex(files, file_count);
        }

        display_files(files, file_count);

        if (dir_count > 1 && i < dir_count - 1)
        {
            printf("\n");
        };
        free_file_details(files, file_count);
    }

    for (int i = 0; i < dir_count; i++)
    {
        free(dir_names[i]);
    }
    free(dir_names);

    return 0;
}

char **parse_arguments(int argc, char *argv[], int *show_all, int *sort_time, int *dir_count)
{
    int opt;
    while ((opt = getopt(argc, argv, "at")) != -1)
    {
        switch (opt)
        {
            case 'a':
                *show_all = 1;
                break;
            case 't':
                *sort_time = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-at] [dir1] [dir2] ...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    *dir_count = argc - optind;

    char **dir_names = malloc(*dir_count * sizeof(char *));
    if (!dir_names)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (*dir_count <= 0)
    {
        dir_names[0] = strdup(".");
        *dir_count = 1;
        return dir_names;
    }

    for (int i = 0; i < *dir_count; i++)
    {
        dir_names[i] = strdup(argv[optind + i]);

        if (!dir_names[i])
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
    }
    return dir_names;
}

int get_file_details(const char *dir_name, FileDetail **files, int show_all,int sort_time)
{
    DIR *dir = opendir(dir_name);
    if (!dir)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!show_all && entry->d_name[0] == '.')
        {
            continue;
        }
        count++;
    }

    *files = malloc(count * sizeof(FileDetail));
    if (!*files)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    rewinddir(dir);
    int idx = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!show_all && entry->d_name[0] == '.')
        {
            continue;
        }
        (*files)[idx].name = strdup(entry->d_name);

        if (sort_time)
        {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
            struct stat info;
            if (stat(path, &info) != 0)
            {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            (*files)[idx].mtime = info.st_mtim.tv_sec;
            (*files)[idx].nsec = info.st_mtim.tv_nsec;
        }

        idx++;
    }

    closedir(dir);
    return count;
}

void display_files(FileDetail *files, int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", files[i].name);
    }
}

void free_file_details(FileDetail *files, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(files[i].name);
    }
    free(files);
}

void bubble_sort_dirs(char **dir_names, int dir_count)
{
    for (int i = 0; i < dir_count - 1; i++)
    {
        for (int j = 0; j < dir_count - i - 1; j++)
        {
            if (strcmp(dir_names[j], dir_names[j + 1]) > 0)
            {
                char *temp = dir_names[j];
                dir_names[j] = dir_names[j + 1];
                dir_names[j + 1] = temp;
            }
        }
    }
}

void bubble_sort_lex(FileDetail *files, int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (strcmp(files[j].name, files[j + 1].name) > 0)
            {
                FileDetail temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}

void bubble_sort_time(FileDetail *files, int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (files[j].mtime < files[j + 1].mtime ||
                (files[j].mtime == files[j + 1].mtime &&
                 files[j].nsec < files[j + 1].nsec))
            {
                FileDetail temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}
