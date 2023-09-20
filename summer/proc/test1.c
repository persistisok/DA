#include <stdio.h>
#include <dlfcn.h>
#include <link.h>

int main() {
    // 打开共享库
    void* handle = dlopen("libexample.so", RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "Failed to open shared library: %s\n", dlerror());
        return 1;
    }

    // 获取共享库的 dl_phdr_info 结构体
    struct dl_phdr_info info;
    dladdr(handle, &info);

    // 遍历共享库的节
    for (int i = 0; i < info.dlpi_phnum; i++) {
        ElfW(Phdr)* phdr = &(info.dlpi_phdr[i]);

        // 输出节的信息
        printf("Section %d:\n", i);
        printf("  Type: %d\n", phdr->p_type);
        printf("  Virtual Address: %p\n", (void*)phdr->p_vaddr);
        printf("  Physical Address: %p\n", (void*)phdr->p_paddr);
        printf("  Size in Memory: %lu\n", (unsigned long)phdr->p_memsz);
        printf("  Size in File: %lu\n", (unsigned long)phdr->p_filesz);
    }

    // 关闭共享库
    dlclose(handle);

    return 0;
}

