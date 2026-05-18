# Standard C Library Headers

| Header            | Purpose                         | Docs                                                                                                     |
| ----------------- | ------------------------------- | -------------------------------------------------------------------------------------------------------- |
| `<assert.h>`      | Runtime assertions              | [assert.h documentation](https://en.cppreference.com/w/c/header/assert)           |
| `<complex.h>`     | Complex numbers                 | [complex.h documentation](https://en.cppreference.com/w/c/header/complex)         |
| `<ctype.h>`       | Character classification        | [ctype.h documentation](https://en.cppreference.com/w/c/header/ctype)             |
| `<errno.h>`       | Error codes                     | [errno.h documentation](https://en.cppreference.com/w/c/header/errno)             |
| `<fenv.h>`        | Floating-point environment      | [fenv.h documentation](https://en.cppreference.com/w/c/header/fenv)               |
| `<float.h>`       | Floating-point limits           | [float.h documentation](https://en.cppreference.com/w/c/header/float)             |
| `<inttypes.h>`    | Integer format macros           | [inttypes.h documentation](https://en.cppreference.com/w/c/header/inttypes)       |
| `<iso646.h>`      | Alternative operators           | [iso646.h documentation](https://en.cppreference.com/w/c/header/iso646)           |
| `<limits.h>`      | Integer limits                  | [limits.h documentation](https://en.cppreference.com/w/c/header/limits)           |
| `<locale.h>`      | Localization                    | [locale.h documentation](https://en.cppreference.com/w/c/header/locale)           |
| `<math.h>`        | Math functions                  | [math.h documentation](https://en.cppreference.com/w/c/header/math)               |
| `<setjmp.h>`      | Non-local jumps                 | [setjmp.h documentation](https://en.cppreference.com/w/c/header/setjmp)           |
| `<signal.h>`      | Signal handling                 | [signal.h documentation](https://en.cppreference.com/w/c/header/signal)           |
| `<stdalign.h>`    | Alignment utilities             | [stdalign.h documentation](https://en.cppreference.com/w/c/header/stdalign)       |
| `<stdarg.h>`      | Variable arguments              | [stdarg.h documentation](https://en.cppreference.com/w/c/header/stdarg)           |
| `<stdatomic.h>`   | Atomic operations               | [stdatomic.h documentation](https://en.cppreference.com/w/c/header/stdatomic)     |
| `<stdbit.h>`      | Bit operations (C23)            | [stdbit.h documentation](https://en.cppreference.com/w/c/header/stdbit)           |
| `<stdbool.h>`     | Boolean type                    | [stdbool.h documentation](https://en.cppreference.com/w/c/header/stdbool)         |
| `<stdckdint.h>`   | Checked integers (C23)          | [stdckdint.h documentation](https://en.cppreference.com/w/c/header/stdckdint)     |
| `<stddef.h>`      | Common definitions              | [stddef.h documentation](https://en.cppreference.com/w/c/header/stddef)           |
| `<stdint.h>`      | Fixed-width integers            | [stdint.h documentation](https://en.cppreference.com/w/c/header/stdint)           |
| `<stdio.h>`       | Standard I/O                    | [stdio.h documentation](https://en.cppreference.com/w/c/header/stdio)             |
| `<stdlib.h>`      | Memory allocation and utilities | [stdlib.h documentation](https://en.cppreference.com/w/c/header/stdlib)           |
| `<stdnoreturn.h>` | `noreturn` support              | [stdnoreturn.h documentation](https://en.cppreference.com/w/c/header/stdnoreturn) |
| `<string.h>`      | String and memory operations    | [string.h documentation](https://en.cppreference.com/w/c/header/string)           |
| `<tgmath.h>`      | Type-generic math               | [tgmath.h documentation](https://en.cppreference.com/w/c/header/tgmath)           |
| `<threads.h>`     | Thread support                  | [threads.h documentation](https://en.cppreference.com/w/c/header/threads)         |
| `<time.h>`        | Time/date utilities             | [time.h documentation](https://en.cppreference.com/w/c/header/time)               |
| `<uchar.h>`       | Unicode characters              | [uchar.h documentation](https://en.cppreference.com/w/c/header/uchar)             |
| `<wchar.h>`       | Wide characters                 | [wchar.h documentation](https://en.cppreference.com/w/c/header/wchar)             |
| `<wctype.h>`      | Wide character classification   | [wctype.h documentation](https://en.cppreference.com/w/c/header/wctype)           |

---

# POSIX / Linux Headers

| Header           | Purpose                  | Docs                                                                                                                             |
| ---------------- | ------------------------ | -------------------------------------------------------------------------------------------------------------------------------- |
| `<unistd.h>`     | POSIX API                | [unistd.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/unistd.h.html)         |
| `<sys/types.h>`  | System data types        | [sys/types.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html)   |
| `<sys/stat.h>`   | File information/status  | [sys/stat.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_stat.h.html)     |
| `<fcntl.h>`      | File control options     | [fcntl.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/fcntl.h.html)           |
| `<dirent.h>`     | Directory access         | [dirent.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dirent.h.html)         |
| `<pthread.h>`    | POSIX threads            | [pthread.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)       |
| `<sys/socket.h>` | Socket API               | [sys/socket.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html) |
| `<netinet/in.h>` | Internet protocol family | [netinet/in.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/netinet_in.h.html) |
| `<arpa/inet.h>`  | IP address conversion    | [arpa/inet.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/functions/inet_ntop.html)    |
| `<sys/wait.h>`   | Process waiting          | [sys/wait.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_wait.h.html)     |
| `<sys/mman.h>`   | Memory mapping           | [sys/mman.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_mman.h.html)     |
| `<poll.h>`       | Event polling            | [poll.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/poll.h.html)             |
| `<sys/epoll.h>`  | Epoll event API (Linux)  | [epoll documentation](https://man7.org/linux/man-pages/man7/epoll.7.html)                                 |
| `<termios.h>`    | Terminal I/O control     | [termios.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/termios.h.html)       |
| `<pwd.h>`        | User account database    | [pwd.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pwd.h.html)               |
| `<grp.h>`        | Group database           | [grp.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/grp.h.html)               |
| `<dlfcn.h>`      | Dynamic linking/loading  | [dlfcn.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dlfcn.h.html)           |
| `<semaphore.h>`  | POSIX semaphores         | [semaphore.h documentation](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/semaphore.h.html)   |

`<bits.h>`
`<features.h`
