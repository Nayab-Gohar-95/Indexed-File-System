# Indexed File System

## Overview
This project is an **Indexed File System** implementation in C. It simulates a file system with indexed allocation, enabling efficient file storage and retrieval.

## Features
- **File Operations**: Create, delete, read, write, and truncate files.
- **Directory Management**: List, create, and delete directories.
- **Block Allocation**: Efficient indexed allocation strategy.
- **Partition Management**: Initialize and format partitions.

## How It Works
- The file system is initialized with a fixed number of blocks.
- Each file has an **index block** pointing to allocated data blocks.
- A directory keeps track of files and their metadata.
- Operations include reading/writing blocks and managing free space.

## Compilation & Execution
### **Step 1: Compile the Program**
```sh
gcc Indexed-FS.c -o indexed-fs
```

### **Step 2: Run the File System**
```sh
./indexed-fs
```

## Menu Options
- **1. Create a File**
- **2. Delete a File**
- **3. Write to a File**
- **4. Read a File**
- **5. Truncate a File**
- **6. List Directory**
- **7. Create a Directory**
- **8. Delete a Directory**
- **9. Create and Format Partition**
- **0. Exit**

## Example Usage
```
Enter file name: myfile.txt
Enter file size: 5
File 'myfile.txt' created with 5 blocks.
```

## Future Improvements
- Implement persistent storage.
- Add user permissions.
- Support larger file sizes with dynamic allocation.

