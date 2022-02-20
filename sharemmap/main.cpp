#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <libkern/OSAtomic.h>

#define atomic_cmp_set(lock, old_val, new_val) OSAtomicCompareAndSwapInt(old_val, new_val, lock)
#define HAS_ATOMIC 1

struct mutex {
    int lock;
    int value;
};

static void shm_lock(struct mutex *m, int pid) {
#ifdef HAS_ATOMIC
    int i, n;
    for(;;) {
        if (m->lock == 0 && atomic_cmp_set(&m->lock, 0, pid))
            return;
        for (n = 0; n<1024; n++) {
            for (i=0; i<n; i++);
            if (m->lock == 0 && atomic_cmp_set(&m->lock, 0, pid))
                return;
        }
        printf("process %d try to get lock filed\n", pid);
        usleep(1);
    }
#endif
}

static int shm_unlock(struct mutex *m, int pid) {
#ifdef HAS_ATOMIC
    if(atomic_cmp_set(&m->lock, pid, 0))
        return 1;
#endif
    return 0;
}

int main() {
    void *addr = mmap(NULL, sizeof(mutex), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    struct mutex *m = (struct mutex*)addr;
    m->lock = 0;
    m->value = 1;

    int pid = fork();
    if (pid == -1) {
        perror("fork() failed");
        exit(1);
    }

    if (pid > 0) {
        // parent
        int ppid = getpid();
        shm_lock(m, ppid);
        printf("parent start: %d\n", m->value);
        int tmp = m->value;
        sleep(1);
        m->value = tmp+1;
        printf("parent end: %d\n", m->value);
        shm_unlock(m, ppid);
        wait(NULL);
    } else{
        // child
        int ppid = getpid();
        shm_lock(m, ppid);
        printf("child start: %d\n", m->value);
        int tmp = m->value;
        m->value = tmp+1;
        printf("child end: %d\n", m->value);
        shm_unlock(m, ppid);
    }
}
