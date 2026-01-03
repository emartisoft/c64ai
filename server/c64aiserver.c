// c64aiserver.c
// Coded by emarti, Murat Özdemir
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void set_language(const char* new_language) {
    const char* filename = ".env";
    const char* tmp_filename = ".env.tmp";
    
    char line_buffer[1024];
    int language_found = 0;

    FILE* original_file = fopen(filename, "r");
    FILE* tmp_file = fopen(tmp_filename, "w");

    if (tmp_file == NULL) {
        fprintf(stderr, "Error: Temporary file %s could not be created.\n", tmp_filename);
        if (original_file) fclose(original_file);
        return;
    }

    // Reading original file line by line
    if (original_file != NULL) {
        while (fgets(line_buffer, sizeof(line_buffer), original_file)) {
            // Check if the line starts with "LANGUAGE="
            if (strncmp(line_buffer, "LANGUAGE=", 9) == 0) {
                // Write the new LANGUAGE line to the temporary file
                fprintf(tmp_file, "LANGUAGE=%s\n", new_language);
                language_found = 1;
            } else {
                fputs(line_buffer, tmp_file);
            }
        }
        fclose(original_file);
    }

    // If LANGUAGE variable was not found, add it at the end
    if (!language_found) {
        fprintf(tmp_file, "LANGUAGE=%s\n", new_language);
    }
    
    fclose(tmp_file);

    // Delete the original file
    if (remove(filename) != 0 && original_file != NULL) {
        fprintf(stderr, "Error: Original file %s could not be deleted.\n", filename);
        return;
    }

    // Rename the temporary file to the original file name
    if (rename(tmp_filename, filename) != 0) {
        fprintf(stderr, "Error: File %s could not be renamed to %s.\n", tmp_filename, filename);
        return;
    }
    
    printf("Success: The LANGUAGE variable in the .env file has been set to '%s'.\n", new_language);
}

void clear_history_file() {
    const char* filename = "@history.txt"; 
    FILE* file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Error: The file %s could not be opened or cleared.\n", filename);
    } else {
        fclose(file);
        printf("Success: The contents of the file %s have been cleared.\n", filename);
    }
}

#ifdef _WIN32
#include <windows.h>
#else // POSIX (Linux, macOS)
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>
#endif

// Baud
#ifdef _WIN32
#define BAUD_RATE CBR_2400
#else
#define BAUD_RATE B2400
#endif

#ifdef _WIN32
typedef HANDLE serial_handle_t;
const serial_handle_t INVALID_SERIAL_HANDLE = INVALID_HANDLE_VALUE;
#else
typedef int serial_handle_t;
const serial_handle_t INVALID_SERIAL_HANDLE = -1;
#endif

#ifndef _WIN32
// for POSIX, _stricmp
#define _stricmp strcasecmp
#endif

// Send a line to C64
void send_line_to_c64(serial_handle_t hSerial, const char* line) {
    if (line == NULL) return;
    // The C64 side expects a carriage return (\r) for the end of line.
    char send_buffer[4096];
    snprintf(send_buffer, sizeof(send_buffer), "%s\r", line);
    size_t len = strlen(send_buffer);

#ifdef _WIN32
    DWORD bytes_written;
    if (!WriteFile(hSerial, send_buffer, len, &bytes_written, NULL)) {
        fprintf(stderr, "\nWrite error. Error code: %lu\n", GetLastError());
    }
#else
    ssize_t bytes_written = write(hSerial, send_buffer, len);
    if (bytes_written < 0) {
        fprintf(stderr, "\nWrite error. Error code: %s\n", strerror(errno));
    }
#endif
}

serial_handle_t hSerial;
char serial_buffer[1024];
bool repeat;

void tasks(serial_handle_t hSerial, char serial_buffer[1024])
{
    if(!repeat)
    {
        // 1. @prompttext.txt
        FILE* prompt_file = fopen("@prompttext.txt", "w");
        if (prompt_file) {
            fputs(serial_buffer, prompt_file);
            fclose(prompt_file);
        } else {
            fprintf(stderr, "Error: @prompttext.txt could not be created.\n");
        }

        // 2. ask2ai.exe
        printf("Asking the AI, please wait...\n");
        int ret;
        #ifdef _WIN32
        ret = system("ask2ai.exe");
        #else
        ret = system("./ask2ai");
        #endif
        if (ret != 0) {
            fprintf(stderr, "Error: The ask2ai script failed with exit code %d.\n", ret);
            // The subsequent check for @response.txt will handle sending an error to the C64.
        }
        repeat = false;
    }

    // 3. @response.txt
    FILE* response_file = fopen("@response.txt", "r");
    if (response_file) {
        char line_buffer[256];
        printf("AI response being sent to C64...\n");
        while (fgets(line_buffer, sizeof(line_buffer), response_file)) {
            // Remove trailing newline characters
            line_buffer[strcspn(line_buffer, "\r\n")] = 0;
            
            // If the line is not empty, send it to C64
            if (strlen(line_buffer) > 0) {
                send_line_to_c64(hSerial, line_buffer);
                #ifdef _WIN32
                Sleep(250); // 250ms
                #else
                usleep(250000); // 250ms
                #endif
            }
        }
        fclose(response_file);
        // Send an empty line to indicate the end of the response
        send_line_to_c64(hSerial, "");
        printf("Response sending completed.\n");
    } else {
        const char* error_msg = "Error: Could not receive response from AI ( @response.txt not found ).";
        fprintf(stderr, "%s\n", error_msg);
        send_line_to_c64(hSerial, error_msg);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port_adi>\n", argv[0]);
        fprintf(stderr, "ex. (Windows): %s COM3\n", argv[0]);
        fprintf(stderr, "ex. (Linux)  : %s /dev/ttyUSB0\n", argv[0]);
        return 1;
    }
    char* port_name = argv[1]; 

#ifdef _WIN32
    hSerial = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
#else
    hSerial = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

    if (hSerial == INVALID_SERIAL_HANDLE) {
#ifdef _WIN32
        fprintf(stderr, "Could not open port: %s. Error code: %lu\n", port_name, GetLastError());
#else
        fprintf(stderr, "Could not open port: %s. Error code: %s\n", port_name, strerror(errno));
#endif
        return 1;
    }

#ifdef _WIN32
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Could not retrieve port settings.\n");
        CloseHandle(hSerial); return 1;
    }
    dcbSerialParams.BaudRate = BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Could not configure port settings.\n");
        CloseHandle(hSerial); return 1;
    }
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50; // 50 ms
    timeouts.ReadTotalTimeoutConstant = 50; // 50 ms
    timeouts.ReadTotalTimeoutMultiplier = 10; // 10 ms
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        fprintf(stderr, "Could not configure timeout settings.\n");
        CloseHandle(hSerial); return 1;
    }
    PurgeComm(hSerial, PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
    struct termios tty = {0};
    if (tcgetattr(hSerial, &tty) != 0) {
        fprintf(stderr, "Could not retrieve port settings: %s\n", strerror(errno));
        close(hSerial); return 1;
    }
    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5; // 0.5 saniye timeout
    if (tcsetattr(hSerial, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Could not configure port settings: %s\n", strerror(errno));
        close(hSerial); return 1;
    }
    tcflush(hSerial, TCIOFLUSH);
#endif

    printf("%s port opened (%d baud).\n", port_name, 2400);
    printf("C64AI Server started. Waiting for commands from C64...\n");

    char serial_buffer[1024];
    int buffer_pos = 0;

    while (1) {
        char temp_char;
        int bytesRead = 0;

#ifdef _WIN32
        DWORD dwBytesRead = 0;
        if (ReadFile(hSerial, &temp_char, 1, &dwBytesRead, NULL)) {
            if (dwBytesRead > 0) {
                bytesRead = 1;
            }
        }
#else
        bytesRead = read(hSerial, &temp_char, 1);
#endif

        if (bytesRead > 0) {
            if (temp_char == '\r' || temp_char == '\n') {
                if (buffer_pos > 0) {
                    serial_buffer[buffer_pos] = '\0';
                    printf("Received from C64: %s\n", serial_buffer);

                    // Command processing
                    if (strncmp(serial_buffer, "/", 1) == 0) {
                        printf("Command received: %s\n", serial_buffer);

                        // LANGUAGE
                        if (strncmp(serial_buffer, "/LANG=", 6) == 0) {
                            printf("%s\n", serial_buffer + 6);
                            set_language(serial_buffer + 6);
                            clear_history_file();
                            send_line_to_c64(hSerial, " ");
                            send_line_to_c64(hSerial, "ok.");
                        }

                        // NEW
                        if (strcmp(serial_buffer, "/NEW") == 0) {
                            clear_history_file();
                        }
                    }
                    else
                    {
                        tasks(hSerial, serial_buffer);
                    }
                    printf("\nWaiting for new command from C64...\n");
                    buffer_pos = 0;
                }
            } else if (isprint(temp_char) && (size_t)buffer_pos < sizeof(serial_buffer) - 1) {
                serial_buffer[buffer_pos++] = temp_char;
            }
        }
        
#ifdef _WIN32
        // A short wait reduces CPU usage.
        Sleep(10);
#else
        usleep(10000); // 10ms
#endif
    }

#ifdef _WIN32
    CloseHandle(hSerial);
#else
    close(hSerial);
#endif

    printf("\n\nConnection terminated.\n");
    return 0;
}
