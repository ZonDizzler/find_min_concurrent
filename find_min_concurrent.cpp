#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    srand(static_cast<unsigned int>(time(0))); 

    // Length of the array
    int length = 20;
    int array[length];

    for (int i = 0; i < length; i++) { 
        array[i] = rand() % 1000;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) { 
        std::cerr << "Error: Pipe creation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // Fork the process

    if (pid < 0) { // Error during fork
        std::cerr << "Error: Fork failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close read end of pipe

        int current = length / 2;
        int second_half_min = array[current];

        for (current++; current < length; current++) { // Find minimum in second half
            if (array[current] < second_half_min)
                second_half_min = array[current];
        }

        std::cout << "Child process ID: " << getpid() << std::endl;
        std::cout << "Second half min: " << second_half_min << std::endl;

        write(pipefd[1], &second_half_min, sizeof(second_half_min)); // Write min to pipe
        close(pipefd[1]); // Close write end of pipe
        exit(0); // Exit child process
    } 
    else { // Parent process
        close(pipefd[1]); // Close write end of pipe

        int status;
        int current = 0;
        int first_half_min = array[current];

        for (current++; current < length / 2; current++) { // Find minimum in first half
            if (array[current] < first_half_min)
                first_half_min = array[current];
        }

        wait(&status); // Wait for child process to finish

        int second_half_min;
        read(pipefd[0], &second_half_min, sizeof(second_half_min));
        close(pipefd[0]); // Close read end of pipe


        // Determine the global minimum
        int global_min = first_half_min;
        if (second_half_min < global_min) {
            global_min = second_half_min;
        }

        std::cout << "Parent process ID: " << getpid() << std::endl;
        std::cout << "First half min: " << first_half_min << std::endl;
        std::cout << "Global min: " << global_min << std::endl;

    }

    return 0;
}