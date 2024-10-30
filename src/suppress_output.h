#ifndef SUPPRESS_OUTPUT_H
#define SUPPRESS_OUTPUT_H

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include "global_vars.h"

class OutputSuppressor {
public:
    OutputSuppressor() {
        if (SHOW_OUTPUT) {
            return;
        }

        // Save the original file descriptors
        saved_stdout = dup(STDOUT_FILENO);
        saved_stderr = dup(STDERR_FILENO);

        // Open /dev/null
        int dev_null = open("/dev/null", O_WRONLY);

        // Redirect stdout and stderr to /dev/null
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);

        // Close the /dev/null file descriptor
        close(dev_null);
    }

    ~OutputSuppressor() {
        if (SHOW_OUTPUT) {
            return;
        }

        // Restore the original file descriptors
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stderr, STDERR_FILENO);

        // Close the saved file descriptors
        close(saved_stdout);
        close(saved_stderr);
    }

private:
    int saved_stdout;
    int saved_stderr;
};

#endif // SUPPRESS_OUTPUT_H