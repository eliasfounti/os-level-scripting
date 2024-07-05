#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstring>
#include <cerrno>

void read_prompt(std::string &prompt) {
    int fd = open("prompt.txt", O_RDONLY);
    if (fd < 0) {
        std::cerr << "Error opening prompt file: " << strerror(errno) << std::endl;
        prompt = "Enter command: "; // Default prompt if file fails to open
        return;
    }

    char buffer[256];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        std::cerr << "Error reading prompt file: " << strerror(errno) << std::endl;
        prompt = "Enter command: "; // Default prompt if read fails
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the string
    prompt = buffer;
    close(fd);
}

void new_file() {
    std::string filename;
    std::string content;

    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);
    
    int fd = creat(filename.c_str(), 0644);
    if (fd < 0) {
        std::cerr << "Error creating file: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "Enter content (end with Ctrl+D):" << std::endl;
    std::string line;
    while (std::getline(std::cin, line)) {
        content += line + "\n";
    }

    ssize_t bytes_written = write(fd, content.c_str(), content.size());
    if (bytes_written < 0) {
        std::cerr << "Error writing to file: " << strerror(errno) << std::endl;
    }

    close(fd);
}

void list() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error fork: " << strerror(errno) << std::endl;
        return;
    } else if (pid == 0) {
        execlp("ls", "ls", "-la", (char*)nullptr);
        std::cerr << "Error exec: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else {
        wait(nullptr);
    }
}

void find() {
    std::string search_string;

    std::cout << "Enter search string: ";
    std::getline(std::cin, search_string);

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        std::cerr << "Error creating pipe: " << strerror(errno) << std::endl;
        return;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "Error fork: " << strerror(errno) << std::endl;
        return;
    } else if (pid1 == 0) {
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[1]);

        execlp("find", "find", ".", (char*)nullptr);
        std::cerr << "Error exec: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        std::cerr << "Error fork: " << strerror(errno) << std::endl;
        return;
    } else if (pid2 == 0) {
        close(pipefd[1]); // Close write end
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[0]);

        execlp("grep", "grep", search_string.c_str(), (char*)nullptr);
        std::cerr << "Error exec: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(nullptr);
    wait(nullptr);
}

void seekTest() {
    const size_t FILE_SIZE = 5 * 1024 * 1024; // 5 MB

    int fd_seek = open("seek", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd_seek < 0) {
        std::cerr << "Error opening 'seek' file: " << strerror(errno) << std::endl;
        return;
    }

    if (write(fd_seek, "x", 1) < 0) {
        std::cerr << "Error writing to 'seek' file: " << strerror(errno) << std::endl;
        close(fd_seek);
        return;
    }

    if (lseek(fd_seek, FILE_SIZE, SEEK_SET) < 0) {
        std::cerr << "Error seeking 'seek' file: " << strerror(errno) << std::endl;
        close(fd_seek);
        return;
    }

    if (write(fd_seek, "x", 1) < 0) {
        std::cerr << "Error writing to 'seek' file: " << strerror(errno) << std::endl;
        close(fd_seek);
        return;
    }

    close(fd_seek);

    int fd_loop = open("loop", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd_loop < 0) {
        std::cerr << "Error opening 'loop' file: " << strerror(errno) << std::endl;
        return;
    }

    if (write(fd_loop, "x", 1) < 0) {
        std::cerr << "Error writing to 'loop' file: " << strerror(errno) << std::endl;
        close(fd_loop);
        return;
    }

    char zero = '\0';
    for (size_t i = 0; i < FILE_SIZE; ++i) {
        if (write(fd_loop, &zero, 1) < 0) {
            std::cerr << "Error writing to 'loop' file: " << strerror(errno) << std::endl;
            close(fd_loop);
            return;
        }
    }

    if (write(fd_loop, "x", 1) < 0) {
        std::cerr << "Error writing to 'loop' file: " << strerror(errno) << std::endl;
    }

    close(fd_loop);

    std::cout << "Files created. Check sizes with 'ls -lh' and 'ls -lS'." << std::endl;
}


int main() {
    std::string prompt;
    read_prompt(prompt);

    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (input == "new_file") {
            new_file();
        } else if (input == "ls") {
            list();
        } else if (input == "find") {
            find();
        } else if (input == "seek") {
            seekTest();
        } else if (input == "exit") {
            return 0;
        } else if (input == "quit") {
            return 0;
        } else if (input == "error") {
            return 1;
        } 
        if (std::cin.eof()) {
            return 0;
        }
    }
}
