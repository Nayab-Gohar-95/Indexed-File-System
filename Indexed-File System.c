#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_SIZE 128
#define MAX_FILE_NAME 64
#define MAX_FILE_SIZE 128
#define BLOCK_SIZE 1024
#define TOTAL_DISK_SIZE (64 * 1024 * 1024)
#define NUM_BLOCKS (TOTAL_DISK_SIZE / BLOCK_SIZE)

typedef struct {
    char name[MAX_FILE_NAME];
    int size;           
    int index_block;       // Block storing the index table
} File;

typedef struct {
    File files[DIR_SIZE];
    int file_count;
} Directory;

char disk[NUM_BLOCKS][BLOCK_SIZE];
int free_blocks[NUM_BLOCKS]; // Tracks free blocks

// Global directory
Directory root_dir;

void initialize_file_system();
int allocate_block();
void free_block(int block_index);
int create_file(const char *name, int size);
void delete_file(const char *name);
void list_directory();
void write_file(const char *name, const char *data);
void read_file(const char *name);
void truncate_file(const char *name);
void create_directory(const char *name);
void delete_directory(const char *name);
void create_partition();

void read_block(int block_index, char *buffer);
void write_block(int block_index, const char *buffer);

void initialize_file_system() {
    memset(free_blocks, 0, sizeof(free_blocks));
    root_dir.file_count = 0;
    printf("File system initialized.\n");
}

int allocate_block() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (!free_blocks[i]) {
            free_blocks[i] = 1;
            return i;
        }
    }
    return -1; // No free blocks
}

void free_block(int block_index) {
    if (block_index >= 0 && block_index < NUM_BLOCKS) {
        free_blocks[block_index] = 0;
    }
}

int create_file(const char *name, int size) {
    if (root_dir.file_count >= DIR_SIZE) {
        printf("Directory full. Cannot create file.\n");
        return -1;
    }

    if (size > MAX_FILE_SIZE) {
        printf("File size exceeds maximum allowed size.\n");
        return -1;
    }

    int index_block = allocate_block();
    if (index_block == -1) {
        printf("No free blocks available.\n");
        return -1;
    }

    File *file = &root_dir.files[root_dir.file_count++];
    strncpy(file->name, name, MAX_FILE_NAME);
    file->size = size;
    file->index_block = index_block;

    int *index_table = (int *)disk[index_block];
    for (int i = 0; i < size; i++) {
        int block = allocate_block();
        if (block == -1) {
            printf("Not enough blocks for file.\n");
            free_block(index_block);
            root_dir.file_count--;
            return -1;
        }
        index_table[i] = block;
    }

    printf("File '%s' created with %d blocks.\n", name, size);
    return 0;
}

void delete_file(const char *name) {
    for (int i = 0; i < root_dir.file_count; i++) {
        if (strcmp(root_dir.files[i].name, name) == 0) {
            int *index_table = (int *)disk[root_dir.files[i].index_block];
            for (int j = 0; j < root_dir.files[i].size; j++) {
                free_block(index_table[j]);
            }
            free_block(root_dir.files[i].index_block);

            // Shift remaining files
            for (int k = i; k < root_dir.file_count - 1; k++) {
                root_dir.files[k] = root_dir.files[k + 1];
            }
            root_dir.file_count--;
            printf("File '%s' deleted.\n", name);
            return;
        }
    }
    printf("File not found.\n");
}

void list_directory() {
    printf("Directory listing:\n");
    for (int i = 0; i < root_dir.file_count; i++) {
        printf("File: %s, Size: %d blocks\n", root_dir.files[i].name, root_dir.files[i].size);
    }
}

void write_file(const char *name, const char *data) {
    for (int i = 0; i < root_dir.file_count; i++) {
        if (strcmp(root_dir.files[i].name, name) == 0) {
            int *index_table = (int *)disk[root_dir.files[i].index_block];
            int blocks_to_write = strlen(data) / BLOCK_SIZE + 1;
            if (blocks_to_write > root_dir.files[i].size) {
                printf("Data exceeds file size.\n");
                return;
            }

            for (int j = 0; j < blocks_to_write; j++) {
                strncpy(disk[index_table[j]], data + j * BLOCK_SIZE, BLOCK_SIZE);
            }
            printf("Data written to file '%s'.\n", name);
            return;
        }
    }
    printf("File not found.\n");
}

void read_file(const char *name) {
    for (int i = 0; i < root_dir.file_count; i++) {
        if (strcmp(root_dir.files[i].name, name) == 0) {
            int *index_table = (int *)disk[root_dir.files[i].index_block];
            printf("Reading file '%s':\n", name);
            for (int j = 0; j < root_dir.files[i].size; j++) {
                printf("%s", disk[index_table[j]]);
            }
            printf("\n");
            return;
        }
    }
    printf("File not found.\n");
}

void truncate_file(const char *name) {
    for (int i = 0; i < root_dir.file_count; i++) {
        if (strcmp(root_dir.files[i].name, name) == 0) {
            int *index_table = (int *)disk[root_dir.files[i].index_block];
            int blocks_to_free = root_dir.files[i].size / 2; // Example: truncate by half
            for (int j = blocks_to_free; j < root_dir.files[i].size; j++) {
                free_block(index_table[j]);
            }
            root_dir.files[i].size /= 2;
            printf("File '%s' truncated to %d blocks.\n", name, root_dir.files[i].size);
            return;
        }
    }
    printf("File not found.\n");
}

void create_directory(const char *name) {
    printf("Directory '%s' created. \n", name);
}

void delete_directory(const char *name) {
    printf("Directory '%s' deleted. \n", name);
}

void create_partition() {
    initialize_file_system();
    printf("Partition created and formatted.\n");
}

void read_block(int block_index, char *buffer) {
    memcpy(buffer, disk[block_index], BLOCK_SIZE);
}

void write_block(int block_index, const char *buffer) {
    memcpy(disk[block_index], buffer, BLOCK_SIZE);
}


void menu() {
    printf("\nIndexed File System Menu\n");
    printf("1. Create a File\n");
    printf("2. Delete a File\n");
    printf("3. Write to a File\n");
    printf("4. Read a File\n");
    printf("5. Truncate a File\n");
    printf("6. List Directory\n");
    printf("7. Create a Directory\n");
    printf("8. Delete a Directory\n");
    printf("9. Create and Format Partition\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    char file_name[MAX_FILE_NAME];
    char data[BLOCK_SIZE * MAX_FILE_SIZE];
    int file_size;

    initialize_file_system();

    do {
        menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: 
                printf("Enter file name: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0'; 
                printf("file size: ");
                scanf("%d", &file_size);
                getchar(); 
                create_file(file_name, file_size);
                break;

            case 2: 
                printf("Enter file name to delete: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                delete_file(file_name);
                break;

            case 3: 
                printf("Enter file name to write to: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                printf("Enter data to write: ");
                fgets(data, sizeof(data), stdin);
                data[strcspn(data, "\n")] = '\0';
                write_file(file_name, data);
                break;

            case 4: 
                printf("Enter file name to read: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                read_file(file_name);
                break;

            case 5:
                printf("Enter file name to truncate: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                truncate_file(file_name);
                break;

            case 6: 
                list_directory();
                break;

            case 7:
                printf("Enter directory name: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                create_directory(file_name);
                break;

            case 8: 
                printf("Enter directory name to delete: ");
                fgets(file_name, MAX_FILE_NAME, stdin);
                file_name[strcspn(file_name, "\n")] = '\0';
                delete_directory(file_name);
                break;

            case 9:
                create_partition();
                break;

            case 0: // Exit
                printf("Exiting the file system.\n");
                break;

            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 0);

    return 0;
}