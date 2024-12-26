# VirtualMemoryManager

A C program demonstrating basic memory management concepts

## Overview
This program simulates a basic memory management system that demonstrates concepts like:

- Virtual Memory
- Paging
- Process Control Blocks (PCB)
- Memory allocation and deallocation

## Features

- Create and manage multiple processes
- Load processes into virtual memory
- View memory status and process list
- Page table implementation
- Interactive menu-driven interface

## How to Run

Compile the program:
```bash
gcc main.c -o memory_manager
```

Run the executable:
```bash
./memory_manager
```

## Program Flow
1. Enter the number of programs (1-25)
2. For each program:
    - Enter program size (1-500000)
3. Use the menu to:
    - Load processes
    - View memory status
    - View process list
    - Exit program

## Menu Options
1. Load Process: Load a program into memory and access its pages
2. View Memory Status: Display current state of virtual memory
3. View Process List: Show all programs and their status
4. Exit: Terminate the program

## Key Components
### Constants
- VM_SIZE: 200 (Virtual Memory size)
- PAGE_SIZE: 3000 (Maximum page size)
- MAX_PROCESSES: 25 (Maximum number of processes)
- Process size limits: 1 to 500000
