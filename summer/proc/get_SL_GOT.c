#include <stdio.h>
#include <dlfcn.h>
#include <link.h>
#include <string.h>
#include <unistd.h>

int main() {
    void *handle;
    void *funcPtr;
    ElfW(Sym) *sym;
    
    handle = dlopen("./libInject.so", RTLD_LAZY);
    if (handle != NULL) {
        // 获取动态函数的地址
        void *got;
        funcPtr = dlsym(handle, "print_sentence_inject");
        void *funcPtr1 = dlsym(handle, "puts");

        printf("addr of print_sentence:%p\n",funcPtr);
        printf("addr of puts@GLIBC_2.2.5:%p\n",funcPtr1);
        if (funcPtr != NULL) {
            // 获取动态函数对应的符号表项
            sym = (ElfW(Sym) *)funcPtr;
            // 获取GOT表项的地址
            got = (void *)(sym->st_value);
            printf("GOT table entry address: %p\n", got);
        } else {
            printf("Failed to find the dynamic symbol.\n");
        }

         char maps_path[256];
        sprintf(maps_path, "/proc/%d/maps", getpid());
        FILE* maps_file = fopen(maps_path, "r");
        unsigned long dlopen_addr = 0;
        if (maps_file != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), maps_file)) {
                if (strstr(line, "libInject.so") != NULL) {
                    // 找到可执行文件所在的行
                    unsigned long start, end;
                    sscanf(line, "%lx-%lx", &start, &end);
                    printf("start address : %lx\n",start);
                    printf("offset : %lx\n",(unsigned long)got - start);
                    break;
                }
            }
            fclose(maps_file);
        }










        dlclose(handle);
    } else {
        printf("Failed to open the shared library.\n");
    }

    return 0;
}
