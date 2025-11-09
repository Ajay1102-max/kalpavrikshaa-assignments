#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BLOCK_SIZE 64
#define NUM_BLOCKS 64
#define MAX_NAME 48
#define MAX_BLOCK_PTRS 16
#define LINE_BUF 512

typedef struct FreeBlock {
    int index;
    struct FreeBlock *prev;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME];
    bool isDirectory;
    int blockPointers[MAX_BLOCK_PTRS];
    int blockCount;
    int sizeBytes;
    struct FileNode *parent;
    struct FileNode *child;   /* circular list of children */
    struct FileNode *next;    /* sibling in circular list */
} FileNode;

static unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
static FreeBlock *freeHead = NULL;
static FileNode *root = NULL;
static FileNode *cwd = NULL;

/* initialize free block list */
static void init_free_list(void) {
    freeHead = NULL;
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        FreeBlock *node = malloc(sizeof(FreeBlock));
        node->index = i;
        node->prev = NULL;
        node->next = freeHead;
        if (freeHead != NULL) {
            freeHead->prev = node;
        }
        freeHead = node;
    }
}

/* allocate one free block (return index) */
static int allocate_block(void) {
    if (freeHead == NULL) {
        return -1;
    }
    FreeBlock *node = freeHead;
    int idx = node->index;
    freeHead = node->next;
    if (freeHead != NULL) {
        freeHead->prev = NULL;
    }
    free(node);
    memset(virtualDisk[idx], 0, BLOCK_SIZE);
    return idx;
}

/* add a block back to free list (head insertion) */
static void free_block_at_index(int idx) {
    if (idx < 0 || idx >= NUM_BLOCKS) {
        return;
    }
    FreeBlock *node = malloc(sizeof(FreeBlock));
    node->index = idx;
    node->prev = NULL;
    node->next = freeHead;
    if (freeHead != NULL) {
        freeHead->prev = node;
    }
    freeHead = node;
}

/* free array of blocks */
static void free_blocks_array(int *blocks, int count) {
    for (int i = 0; i < count; ++i) {
        if (blocks[i] >= 0 && blocks[i] < NUM_BLOCKS) {
            free_block_at_index(blocks[i]);
            blocks[i] = -1;
        }
    }
}

/* create a new file/directory node */
static FileNode* make_node(const char *name, bool isDir) {
    FileNode *n = malloc(sizeof(FileNode));
    strncpy(n->name, name, MAX_NAME - 1);
    n->name[MAX_NAME - 1] = '\0';
    n->isDirectory = isDir;
    n->parent = NULL;
    n->child = NULL;
    n->next = NULL;
    n->blockCount = 0;
    n->sizeBytes = 0;
    for (int i = 0; i < MAX_BLOCK_PTRS; ++i) {
        n->blockPointers[i] = -1;
    }
    return n;
}

/* add child (circular list) */
static void add_child(FileNode *dir, FileNode *node) {
    if (dir->child == NULL) {
        dir->child = node;
        node->next = node;
    } else {
        FileNode *first = dir->child;
        FileNode *last = first;
        while (last->next != first) {
            last = last->next;
        }
        last->next = node;
        node->next = first;
    }
    node->parent = dir;
}

/* remove child by pointer */
static bool remove_child(FileNode *dir, FileNode *node) {
    if (dir->child == NULL || node == NULL) {
        return false;
    }
    FileNode *first = dir->child;
    FileNode *cur = first;
    if (cur->next == cur && cur == node) {
        dir->child = NULL;
        node->next = NULL;
        node->parent = NULL;
        return true;
    }
    FileNode *prev = NULL;
    do {
        if (cur == node) {
            FileNode *p = cur;
            while (p->next != cur) {
                p = p->next;
            }
            p->next = cur->next;
            if (dir->child == cur) {
                dir->child = cur->next;
            }
            node->next = NULL;
            node->parent = NULL;
            return true;
        }
        cur = cur->next;
    } while (cur != first);
    return false;
}

/* find child by name */
static FileNode* find_child_by_name(FileNode *dir, const char *name) {
    if (dir == NULL || name == NULL) {
        return NULL;
    }
    if (dir->child == NULL) {
        return NULL;
    }
    FileNode *first = dir->child;
    FileNode *cur = first;
    do {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    } while (cur != first);
    return NULL;
}

/* mkdir */
static void command_mkdir(const char *name) {
    if (find_child_by_name(cwd, name) != NULL) {
        printf("Directory '%s' already exists.\n", name);
        return;
    }
    FileNode *d = make_node(name, true);
    add_child(cwd, d);
    printf("Directory '%s' created successfully.\n", name);
}

/* create file */
static void command_create(const char *name) {
    if (find_child_by_name(cwd, name) != NULL) {
        printf("File or directory '%s' already exists.\n", name);
        return;
    }
    FileNode *f = make_node(name, false);
    add_child(cwd, f);
    printf("File '%s' created successfully.\n", name);
}

/* list contents */
static void command_ls(void) {
    if (cwd->child == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *first = cwd->child;
    FileNode *cur = first;
    do {
        if (cur->isDirectory) {
            printf("[D] %s\n", cur->name);
        } else {
            printf("[F] %s (size=%d bytes)\n", cur->name, cur->sizeBytes);
        }
        cur = cur->next;
    } while (cur != first);
}

/* build and print path */
static void command_pwd(void) {
    char path[1024] = "";
    FileNode *cur = cwd;
    while (cur != NULL && cur->parent != NULL) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "/%s%s", cur->name, path);
        strncpy(path, buffer, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        cur = cur->parent;
    }
    if (strlen(path) == 0) {
        printf("/\n");
    } else {
        printf("%s\n", path);
    }
}

/* change directory */
static void command_cd(const char *arg) {
    if (arg == NULL) {
        printf("Usage: cd <dir>\n");
        return;
    }
    if (strcmp(arg, "/") == 0) {
        cwd = root;
        printf("Moved to /\n");
        return;
    }
    if (strcmp(arg, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
            printf("Moved to ");
            command_pwd();
        } else {
            printf("Already at root.\n");
        }
        return;
    }
    FileNode *target = find_child_by_name(cwd, arg);
    if (target == NULL) {
        printf("Directory '%s' not found.\n", arg);
        return;
    }
    if (!target->isDirectory) {
        printf("'%s' is not a directory.\n", arg);
        return;
    }
    cwd = target;
    printf("Moved to ");
    command_pwd();
}

/* disk usage */
static void command_df(void) {
    int freeCount = 0;
    FreeBlock *cur = freeHead;
    while (cur != NULL) {
        freeCount++;
        cur = cur->next;
    }
    int used = NUM_BLOCKS - freeCount;
    printf("Total blocks: %d, Used: %d, Free: %d\n", NUM_BLOCKS, used, freeCount);
}

/* write data to file (overwrite) */
static void command_write(const char *name, const unsigned char *data, int len) {
    FileNode *f = find_child_by_name(cwd, name);
    if (f == NULL) {
        printf("File '%s' not found.\n", name);
        return;
    }
    if (f->isDirectory) {
        printf("'%s' is a directory.\n", name);
        return;
    }
    if (f->blockCount > 0) {
        free_blocks_array(f->blockPointers, f->blockCount);
        f->blockCount = 0;
        f->sizeBytes = 0;
        for (int i = 0; i < MAX_BLOCK_PTRS; ++i) {
            f->blockPointers[i] = -1;
        }
    }
    int offset = 0;
    int blocksNeeded = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocksNeeded > MAX_BLOCK_PTRS) {
        printf("File too large: max %d blocks allowed.\n", MAX_BLOCK_PTRS);
        return;
    }
    for (int b = 0; b < blocksNeeded; ++b) {
        int idx = allocate_block();
        if (idx < 0) {
            free_blocks_array(f->blockPointers, f->blockCount);
            f->blockCount = 0;
            f->sizeBytes = 0;
            for (int i = 0; i < MAX_BLOCK_PTRS; ++i) {
                f->blockPointers[i] = -1;
            }
            printf("Not enough free blocks for write.\n");
            return;
        }
        f->blockPointers[f->blockCount++] = idx;
        int toWrite = (len - offset) < BLOCK_SIZE ? (len - offset) : BLOCK_SIZE;
        memcpy(virtualDisk[idx], data + offset, toWrite);
        if (toWrite < BLOCK_SIZE) {
            memset(virtualDisk[idx] + toWrite, 0, BLOCK_SIZE - toWrite);
        }
        offset += toWrite;
    }
    f->sizeBytes = len;
    printf("Data written successfully (size=%d bytes).\n", len);
}

/* read file content */
static void command_read(const char *name) {
    FileNode *f = find_child_by_name(cwd, name);
    if (f == NULL) {
        printf("File '%s' not found.\n", name);
        return;
    }
    if (f->isDirectory) {
        printf("'%s' is a directory.\n", name);
        return;
    }
    if (f->blockCount == 0 || f->sizeBytes == 0) {
        printf("(empty)\n");
        return;
    }
    int remaining = f->sizeBytes;
    for (int i = 0; i < f->blockCount && remaining > 0; ++i) {
        int idx = f->blockPointers[i];
        int toPrint = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtualDisk[idx], 1, toPrint, stdout);
        remaining -= toPrint;
    }
    printf("\n");
}

/* delete file */
static void command_delete(const char *name) {
    FileNode *f = find_child_by_name(cwd, name);
    if (f == NULL) {
        printf("File '%s' not found.\n", name);
        return;
    }
    if (f->isDirectory) {
        printf("'%s' is a directory. Use rmdir to remove directories.\n", name);
        return;
    }
    if (f->blockCount > 0) {
        free_blocks_array(f->blockPointers, f->blockCount);
    }
    remove_child(cwd, f);
    free(f);
    printf("File deleted successfully.\n");
}

/* remove directory if empty */
static void command_rmdir(const char *name) {
    FileNode *d = find_child_by_name(cwd, name);
    if (d == NULL) {
        printf("Directory '%s' not found.\n", name);
        return;
    }
    if (!d->isDirectory) {
        printf("'%s' is not a directory.\n", name);
        return;
    }
    if (d->child != NULL) {
        printf("Directory '%s' is not empty.\n", name);
        return;
    }
    remove_child(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

/* cleanup nodes recursively */
static void cleanup_node_recursive(FileNode *n) {
    if (n == NULL) {
        return;
    }
    if (n->isDirectory && n->child != NULL) {
        FileNode *first = n->child;
        FileNode *cur = first->next;
        while (cur != first) {
            FileNode *next = cur->next;
            cleanup_node_recursive(cur);
            cur = next;
        }
        cleanup_node_recursive(first);
    } else {
        if (n->blockCount > 0) {
            free_blocks_array(n->blockPointers, n->blockCount);
        }
        free(n);
    }
}

/* cleanup all memory */
static void cleanup_all(void) {
    FreeBlock *f = freeHead;
    while (f != NULL) {
        FreeBlock *next = f->next;
        free(f);
        f = next;
    }
    freeHead = NULL;
    if (root != NULL) {
        if (root->child != NULL) {
            FileNode *first = root->child;
            FileNode *cur = first->next;
            while (cur != first) {
                FileNode *next = cur->next;
                cleanup_node_recursive(cur);
                cur = next;
            }
            cleanup_node_recursive(first);
        }
        free(root);
        root = NULL;
    }
}

/* initialize VFS */
static void init_vfs(void) {
    init_free_list();
    root = make_node("/", true);
    root->parent = NULL;
    root->child = NULL;
    cwd = root;
}

/* trim leading spaces */
static char* ltrim_inplace(char *s) {
    while (*s != '\0' && (*s == ' ' || *s == '\t')) {
        s++;
    }
    return s;
}

/* main shell loop */
static void run_shell(void) {
    char line[LINE_BUF];
    while (true) {
        printf("%s > ", (cwd == root) ? "/" : cwd->name);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        char lineCopy[LINE_BUF];
        strncpy(lineCopy, line, LINE_BUF - 1);
        lineCopy[LINE_BUF - 1] = '\0';

        char *trimmed = ltrim_inplace(lineCopy);
        if (trimmed[0] == '\0') {
            continue;
        }
        if (trimmed[0] == '#') {
            continue;
        }

        char *cmd = strtok(line, " ");
        if (cmd == NULL) {
            continue;
        }

        if (strcmp(cmd, "mkdir") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg == NULL) {
                printf("Usage: mkdir <name>\n");
            } else {
                command_mkdir(arg);
            }
        } else if (strcmp(cmd, "create") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg == NULL) {
                printf("Usage: create <name>\n");
            } else {
                command_create(arg);
            }
        } else if (strcmp(cmd, "ls") == 0) {
            command_ls();
        } else if (strcmp(cmd, "pwd") == 0) {
            command_pwd();
        } else if (strcmp(cmd, "cd") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg == NULL) {
                printf("Usage: cd <dir>\n");
            } else {
                command_cd(arg);
            }
        } else if (strcmp(cmd, "df") == 0) {
            command_df();
        } else if (strcmp(cmd, "write") == 0) {
            char *name = strtok(NULL, " ");
            if (name == NULL) {
                printf("Usage: write <file> \"data\"\n");
            } else {
                char *quoteStart = strchr(lineCopy, '\"');
                if (quoteStart == NULL) {
                    printf("Usage: write <file> \"data\"\n");
                } else {
                    char *quoteEnd = strrchr(lineCopy, '\"');
                    if (quoteEnd == NULL || quoteEnd == quoteStart) {
                        printf("Usage: write <file> \"data\"\n");
                    } else {
                        int dataLen = (int)(quoteEnd - quoteStart - 1);
                        if (dataLen < 0) {
                            printf("Usage: write <file> \"data\"\n");
                        } else {
                            unsigned char *data = malloc(dataLen);
                            if (data == NULL) {
                                printf("Memory allocation failed.\n");
                            } else {
                                memcpy(data, quoteStart + 1, dataLen);
                                command_write(name, data, dataLen);
                                free(data);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "read") == 0) {
            char *name = strtok(NULL, " ");
            if (name == NULL) {
                printf("Usage: read <file>\n");
            } else {
                command_read(name);
            }
        } else if (strcmp(cmd, "delete") == 0) {
            char *name = strtok(NULL, " ");
            if (name == NULL) {
                printf("Usage: delete <file>\n");
            } else {
                command_delete(name);
            }
        } else if (strcmp(cmd, "rmdir") == 0) {
            char *name = strtok(NULL, " ");
            if (name == NULL) {
                printf("Usage: rmdir <dir>\n");
            } else {
                command_rmdir(name);
            }
        } else if (strcmp(cmd, "exit") == 0) {
            printf("Memory released. Exiting program.\n");
            break;
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}

int main(void) {
    init_vfs();
    run_shell();
    cleanup_all();
    return 0;
}