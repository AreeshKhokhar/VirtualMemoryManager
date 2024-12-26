#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Constants for memory management
#define VM_SIZE 200
#define PAGE_SIZE 3000
#define MAX_PROCESSES 25
#define MAX_NAME_LENGTH 32
#define MIN_PROCESS_SIZE 1
#define MAX_PROCESS_SIZE 500000

// Memory status constants
#define FREE_SLOT "-1"

// Structure definitions
typedef struct {
    char name[MAX_NAME_LENGTH];
    int size;
    int base;
    int limit;
    bool is_loaded;
    int pageTable[PAGE_SIZE];
} PCB;

// Global state (could be encapsulated in a struct)
typedef struct {
    char VM[VM_SIZE][MAX_NAME_LENGTH];
    PCB PCBlist[MAX_PROCESSES];
    int PCBCount;
    int MM_size;
} MemoryState;

// Global memory state
static MemoryState memState = {0};

// Function prototypes
void initializeMemory(MemoryState* state);
void fillVM(MemoryState* state);
void loadProcess(MemoryState* state);
void loadPage(MemoryState* state, int pcbIndex);
void printMemory(const MemoryState* state);
void printProgramList(const MemoryState* state);
bool isValidProcessSize(int size);
void cleanupMemory(MemoryState* state);
void printMenu();
void printHeader(const char* title);
void printWelcome();

int main() {
    printWelcome();
    initializeMemory(&memState);
    fillVM(&memState);

    printHeader("Memory Manager Setup");
    int nop;
    printf("Enter number of programs (1-%d): ", MAX_PROCESSES);
    scanf("%d", &nop);
    nop = (nop < 1) ? 1 : (nop > MAX_PROCESSES) ? MAX_PROCESSES : nop;

    for (int i = 0; i < nop; i++) {
        printf("\n--- Program %d Configuration ---\n", i);
        int psize;
        do {
            printf("Enter size of P%d (1-%d): ", i, MAX_PROCESS_SIZE);
            scanf("%d", &psize);
            if (!isValidProcessSize(psize)) {
                printf("! Invalid size! Must be between %d and %d.\n", 
                       MIN_PROCESS_SIZE, MAX_PROCESS_SIZE);
            }
        } while (!isValidProcessSize(psize));

        printf("* Program P%d configured with size: %d\n", i, psize);

        snprintf(memState.PCBlist[memState.PCBCount].name, MAX_NAME_LENGTH, "P%d", i);
        memState.PCBlist[memState.PCBCount].size = psize;
        memState.PCBlist[memState.PCBCount].is_loaded = false;
        memState.PCBCount++;
    }

    char choice;
    do {
        printMenu();
        scanf(" %c", &choice);

        switch(choice) {
            case '1':
                printHeader("Load Process");
                printf("Available programs:\n");
                printProgramList(&memState);
                printf("\nEnter program name to load (e.g., P0): ");
                loadProcess(&memState);
                break;
            case '2':
                printHeader("Memory Status");
                printMemory(&memState);
                break;
            case '3':
                printHeader("Process List");
                printProgramList(&memState);
                break;
            case '0':
                printf("\nExiting program...\n");
                break;
            default:
                printf("\n! Invalid option! Please try again.\n");
        }
    } while (choice != '0');

    cleanupMemory(&memState);
    return 0;
}

void initializeMemory(MemoryState* state) {
    for (int i = 0; i < VM_SIZE; i++) {
        strcpy(state->VM[i], FREE_SLOT);
    }
    state->MM_size = 0;
    state->PCBCount = 0;
}

void fillVM(MemoryState* state) {
    int index = 0;

    for (int i = 0; i < state->PCBCount; i++) {
        int psize = state->PCBlist[i].size;
        snprintf(state->VM[index], MAX_NAME_LENGTH, "%s", state->PCBlist[i].name);
        index++;

        int pageNo = 1;
        for (int j = index; pageNo <= psize && j < VM_SIZE; j++) {
            snprintf(state->VM[j], MAX_NAME_LENGTH, "%s_page%d", 
                    state->PCBlist[i].name, pageNo);
            index++;
            pageNo++;
        }

        if (index < VM_SIZE) {
            strcpy(state->VM[index], FREE_SLOT);
            index++;
        }
    }
}

void printMemory(const MemoryState* state) {
    printf("\nVirtual Memory Status:\n");
    printf("+--------------------------------+\n");
    printf("| Index | Content                 |\n");
    printf("+--------------------------------+\n");
    
    for (int i = 0; i < VM_SIZE; i++) {
        if (strcmp(state->VM[i], FREE_SLOT) != 0) {
            printf("| %3d   | %-22s |\n", i, state->VM[i]);
        }
    }
    
    printf("+--------------------------------+\n");
}

void printProgramList(const MemoryState* state) {
    printf("+--------------------------------+\n");
    printf("| Program | Status  | Size        |\n");
    printf("+--------------------------------+\n");
    
    for (int i = 0; i < state->PCBCount; i++) {
        printf("| %-7s | %-7s | %-10d |\n", 
               state->PCBlist[i].name,
               state->PCBlist[i].is_loaded ? "LOADED" : "WAITING",
               state->PCBlist[i].size);
    }
    
    printf("+--------------------------------+\n");
}

bool isValidProcessSize(int size) {
    return size >= MIN_PROCESS_SIZE && size <= MAX_PROCESS_SIZE;
}

void loadProcess(MemoryState* state) {
    char pname[MAX_NAME_LENGTH];
    scanf("%s", pname);

    int pcbIndex = -1;
    for (int i = 0; i < state->PCBCount; i++) {
        if (strcmp(state->PCBlist[i].name, pname) == 0) {
            pcbIndex = i;
            break;
        }
    }

    if (pcbIndex != -1) {
        loadPage(state, pcbIndex);
    } else {
        printf("! Process not found.\n");
    }
}

void loadPage(MemoryState* state, int pcbIndex) {
    PCB* pcb = &state->PCBlist[pcbIndex];
    printHeader("Loading Process");
    printf("Process: %s\n", pcb->name);

    // Check if there's enough space in memory
    if (state->MM_size + pcb->size > VM_SIZE) {
        printf("! Error: Not enough memory to load process.\n");
        return;
    }

    int index = state->MM_size;
    for (int j = 1; j <= pcb->size && index < VM_SIZE; j++) {
        snprintf(state->VM[index], MAX_NAME_LENGTH, "%s_page%d", pcb->name, j);
        pcb->pageTable[j-1] = index;  // Store page location in page table
        index++;
        state->MM_size++;
    }

    char confirm;
    printf("\nConfirm loading pages into memory? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        pcb->is_loaded = true;
        printf("\n* Pages loaded for %s:\n", pcb->name);
        for (int j = 1; j <= pcb->size; j++) {
            printf("  +- %s_page%d\n", pcb->name, j);
        }

        char accessedPage[MAX_NAME_LENGTH];
        printf("Enter Page No. you want to access: ");
        scanf("%s", accessedPage);

        bool pageFound = false;
        char pageName[MAX_NAME_LENGTH];
        for (int j = 1; j <= pcb->size; j++) {
            snprintf(pageName, MAX_NAME_LENGTH, "%s_page%d", pcb->name, j);
            if (strcmp(accessedPage, pageName) == 0) {
                pageFound = true;
                printf("* Page found at frame %d\n", pcb->pageTable[j-1]);
                break;
            }
        }

        if (pageFound) {
            printf("* Page in memory.\n");
        } else {
            printf("! Page not in Main memory, loading it from the secondary drive....\n");
            printf("* Success!!\n");
        }
    }
}

void cleanupMemory(MemoryState* state) {
    // Reset memory state
    state->MM_size = 0;
    state->PCBCount = 0;
    for (int i = 0; i < VM_SIZE; i++) {
        strcpy(state->VM[i], FREE_SLOT);
    }
}

void printMenu() {
    printf("\n+--------------------------------+\n");
    printf("|        Memory Manager Menu      |\n");
    printf("+--------------------------------+\n");
    printf("| 1. Load Process                |\n");
    printf("| 2. View Memory Status          |\n");
    printf("| 3. View Process List           |\n");
    printf("| 0. Exit                        |\n");
    printf("+--------------------------------+\n");
    printf("Choose an option: ");
}

void printHeader(const char* title) {
    printf("\n========== %s ==========\n", title);
}

void printWelcome() {
    printf("\n+--------------------------------+\n");
    printf("|      Memory Manager v1.0        |\n");
    printf("+--------------------------------+\n");
    printf("| Authors:                       |\n");
    printf("| - Ali Shah (14436)             |\n");
    printf("| - Areesha (14673)              |\n");
    printf("+--------------------------------+\n\n");
}
