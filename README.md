*This project has been created as part of the 42 curriculum by spuschma.*

# Description
The `get_next_line` function takes an file descriptor and gives back the next line from that file descriptor. At the end of the file or on an error `NULL` is returned.

## Algorithm
I had way too much fun trying to condense the whole function into one file. That being said I did not sacrifice performance or readability too much.

In order to handle multiple file descriptors at the same time I have an array of 1024 `char *` pointing to malloced buffers per file. The Buffer size itself is known at compiletime but having all buffers allocated in static storage at all times would waist a lot of space if the buffer size is set high - `BUFFERSIZE=1024` would already be 1MB.

All data necessary for creating the line string is stored in a structure called `t_line`. It stores the pointer to the string, the end of what was already read (since it is not necessary null-terminated), the size of the currently allocated space and the index of the first newline character or `-1` if there is non.

On a call the buffer (if it exists) is used as the new string for the next line. If it does not a new sting is malloced. At the end the leftover part of the sting is stored in a new malloced buffer at the location dictated by `bufs[fd]`.

If there is no `'\n'` character found in the string another `BUFFERSIZE` is read and scanned for `'\n'`. If there is less than `BUFFERSIZE` bytes available in the currenlt malloced string the size is doubled and the old string is copied to the new one, after which the old string gets freed.

The loop ends on finding a linebreak, `read` returning `0`, meaning `EOF` was reached or `read` returning a negative value, indicating a read error. In the case of a read error the `end` field of the line struct is also set to 0 to avoid returning the incomplete string instead of `NULL`.

This algorithm was chosen to avoid quadratic scaling when expanding the malloced strings. The size of the sting is overallocated by a maximum of BUFFERSIZE or the factor of 2, all still being in space of O(n).

The overall time complexity for reasonable BUFFERSIZE should be O(n).

If a file is not read till the end the buffer still exists at the heap. To clean up, all accessed file descriptors have to be drained.

# Instructions
To use this function simply include `get_next_line.h` and compile `get_next_line.c` with the rest of the source files.

For the eval `get_next_line_utils.c`, `get_next_line_bonus.c`, `get_next_line_utils_bonus.c` and `get_next_line_bonus.h` are also present. Compiling with the utils works but doesn't change anything since no helper functions where needed. The bonus version is a copy of the base version which already does both the basic and bonus functionality.

# Resources
For the development I mainly relied on man pages. In addition I talked with a few peers about ideas for a clean structure.

## AI
The structure and code was entirely done without AI. AI was only used for bug hunting at the very end of the project.
