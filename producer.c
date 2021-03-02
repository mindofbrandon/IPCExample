/**
 * Simple program demonstrating shared memory in POSIX systems.
 *
 * This is the producer process that writes to the shared memory region.
 *
 * Figure 3.17
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 *
 * modifications by dheller@cse.psu.edu, 31 Jan. 2014
 *
 * Downloaded from: http://www.cse.psu.edu/~deh25/cmpsc473/notes/OSC/Processes/shm.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>

void display(char *prog, char *bytes, int n);

int main(void)
{
    const char *name = "/shm-example";	// file name
    const int SIZE = 4096;		// file size in kilobytes

    const char *message0 = "Studying "; // In CLion, in variables window, do ctrl+enter on message0 to get memory views in hex
    const char *message1 = "Operating Systems ";
    const char *message2 = "Is Fun!";
    const char *msg_end  = "\n";

    int shm_fd;		// file descriptor, from shm_open()
    char *shm_base;	// base address, from mmap()
    char *ptr;		// shm_base is fixed, ptr is movable

    /* create the shared memory segment as if it was a file */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // create region to be shared as file, save file descriptor to shm_fd

    if (shm_fd == -1) { // if creating shared memory fails, exit
        printf("prod: Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }

    /* configure the size of the shared memory segment */
    ftruncate(shm_fd, SIZE); // size is initially 0, resize shared object to 4096

    /* map the shared memory segment to the address space of the process */
    shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);    // (starting addr of mapping -- 0 = let kernel choose, object size,
                                                                                                //  memory protection, determine if sharable, file descriptor, offset into file
    if (shm_base == MAP_FAILED) {
        printf("prod: Map failed: %s\n", strerror(errno));
        // close and shm_unlink?
        exit(1);
    }

    /**
     * Write to the mapped shared memory region.
     *
     * We increment the value of ptr after each write, but we
     * are ignoring the possibility that sprintf() fails.
     */
    display("prod", shm_base, 64);
    ptr = shm_base; // keep base address
    ptr += sprintf(ptr, "%s", message0); // sprintf prints to a string (character array)
    ptr += sprintf(ptr, "%s", message1); // ptr += is moving pointer ahead x characters (0 + message0 = pointer moves 10 places in memory)
                                                // prevents rewriting to same location in memory
    ptr += sprintf(ptr, "%s", message2);
    ptr += sprintf(ptr, "%s", msg_end);
    display("prod", shm_base, 64); // displays the hex memory of each character
                                            // 53 = S, 74 = t, 75 = u, 64 = d ..., 4f = O, 70 = p, 65 = e... and so on

    /* remove the mapped memory segment from the address space of the process */
    if (munmap(shm_base, SIZE) == -1) {
        printf("prod: Unmap failed: %s\n", strerror(errno));
        exit(1);
    }

    /* close the shared memory segment as if it was a file */
    if (close(shm_fd) == -1) {
        printf("prod: Close failed: %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}

void display(char *prog, char *bytes, int n) // print every value at address in hex
{
    printf("display: %s\n", prog);
    for (int i = 0; i < n; i++)
    { printf("%02x%c", bytes[i], ((i+1)%16) ? ' ' : '\n'); }
    printf("\n");
}
