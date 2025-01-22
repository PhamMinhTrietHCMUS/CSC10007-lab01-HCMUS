#include "../kernel/types.h" // for uint, ushort, etc.
#include "../kernel/stat.h" // for struct stat
#include "../kernel/fs.h" // for struct dirent, DIRSIZ, etc.
#include "../user/user.h" // for system calls like open, read, etc.

void find(char *path, char *name) 
{
    char buf[512], *p;  // buffer to store the current path and pointer to traverse it
    int fd;             // file descriptor
    struct dirent de;   // directory entry
    struct stat st;     // file status

    if ((fd = open(path, 0)) < 0)  // openning the directory with read-only mode (0) if it fails the fd will be < 0
    {
        fprintf(2, "find: cannot open %s\n", path); 
        return;
    }

    if (fstat(fd, &st) < 0)  // get the status of the file 
    {
        fprintf(2, "find: cannot stat %s\n", path); 
        close(fd);
        return;
    }

    if (st.type != T_DIR)  // check whether the file is not a directory
    {
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) // check for buffer overflow
    {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path); 
    p = buf + strlen(buf);  // move the pointer to the end of the buffer
    *p++ = '/'; // add a '/' to the end of the buffer

    while (read(fd, &de, sizeof(de)) == sizeof(de)) // read the directory entry
    {
        if (de.inum == 0) // skip if empty
            continue;
        
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) //skip "." and ".."
            continue;
        
        memmove(p, de.name, DIRSIZ); // copy the name of the directory entry to the buffer (memmove is used to copy files without overlapping memory)
        p[DIRSIZ] = 0; // null-terminate the string

        if (strcmp(de.name, name) == 0)  // check if the name of the directory entry is the same as the name we are looking for
            printf("%s\n", buf);
        
        find(buf, name);
    }
    close(fd);
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        printf("Usage: find <path> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}