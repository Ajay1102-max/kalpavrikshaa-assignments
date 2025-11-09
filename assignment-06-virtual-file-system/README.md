# Week 4 – Virtual File System (C)

This project simulates a minimal virtual file system (VFS) in C using dynamic memory and linked lists.  
The system mimics basic Unix-like commands for directories and files, all managed in RAM.

## Features
- Create / remove directories (`mkdir`, `rmdir`)
- Create / write / read / delete files (`create`, `write`, `read`, `delete`)
- Navigate directories (`cd`, `pwd`)
- List contents (`ls`)
- Check disk usage (`df`)
- Graceful exit (`exit`)

## Implementation
- **Free blocks** managed by doubly linked list.
- **Directory contents** managed using circular linked list.
- **Each file node** stores allocated block pointers.
- **Disk simulation** with fixed number of blocks (1024).

## Build & Run
```bash
gcc vfs.c -o vfs
./vfs