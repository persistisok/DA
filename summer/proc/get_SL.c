#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

void print_dependent_libraries(const char* executable_path) {
    FILE* file = fopen(executable_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "无法打开可执行文件\n");
        exit(1);
    }

    Elf64_Ehdr elf_header;
    if (fread(&elf_header, 1, sizeof(elf_header), file) != sizeof(elf_header)) {
        fprintf(stderr, "无法读取 ELF 头部\n");
        exit(1);
    }

    if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "不是有效的 ELF 文件\n");
        exit(1);
    }

    Elf64_Shdr dynamic_section;
    fseek(file, elf_header.e_shoff + elf_header.e_shentsize * elf_header.e_shstrndx, SEEK_SET);
    Elf64_Shdr shstrtab_section;
    if (fread(&shstrtab_section, 1, sizeof(shstrtab_section), file) != sizeof(shstrtab_section)) {
        fprintf(stderr, "无法读取节头表\n");
        exit(1);
    }
    char* shstrtab = malloc(shstrtab_section.sh_size);
    fseek(file, shstrtab_section.sh_offset, SEEK_SET);
    if (fread(shstrtab, 1, shstrtab_section.sh_size, file) != shstrtab_section.sh_size) {
        fprintf(stderr, "无法读取节名字符串表\n");
        exit(1);
    }

    fseek(file, elf_header.e_shoff, SEEK_SET);
    for (int i = 0; i < elf_header.e_shnum; i++) {
        Elf64_Shdr section_header;
        if (fread(&section_header, 1, sizeof(section_header), file) != sizeof(section_header)) {
            fprintf(stderr, "无法读取节头表\n");
            exit(1);
        }

        char* section_name = shstrtab + section_header.sh_name;
        // printf("section_name: %s\n",section_name);
        if (strcmp(section_name, ".dynamic") == 0) {
            dynamic_section = section_header;
            break;
        }
    }

    if (dynamic_section.sh_offset == 0 || dynamic_section.sh_size == 0) {
        fprintf(stderr, "未找到动态链接信息\n");
        exit(1);
    }

    fseek(file, dynamic_section.sh_offset, SEEK_SET);

    Elf64_Dyn dynamic_entry;
    while (ftell(file) < dynamic_section.sh_offset + dynamic_section.sh_size) {
        // printf("0x%lx\n",ftell(file));
        // printf("0x%x\n",sizeof(dynamic_entry));
        if (fread(&dynamic_entry, 1, sizeof(dynamic_entry), file) != sizeof(dynamic_entry)) {
            fprintf(stderr, "无法读取动态链接信息\n");
            exit(1);
        }
        // printf("0x%x\n",dynamic_entry.d_tag);
        if (dynamic_entry.d_tag == DT_NEEDED) {
            Elf64_Off current_offset = ftell(file);
            Elf64_Sxword strtab_offset = dynamic_entry.d_un.d_val; // 获取字符串表偏移量
            fseek(file, elf_header.e_shoff + elf_header.e_shentsize * dynamic_section.sh_link, SEEK_SET); // 定位到字符串表节
            Elf64_Shdr strtab_section;
            if (fread(&strtab_section, 1, sizeof(strtab_section), file) != sizeof(strtab_section)) {
                fprintf(stderr, "无法读取字符串表节\n");
                exit(1);
            }
            char* strtab = malloc(strtab_section.sh_size);
            fseek(file, strtab_section.sh_offset, SEEK_SET);
            if (fread(strtab, 1, strtab_section.sh_size, file) != strtab_section.sh_size) {
                fprintf(stderr, "无法读取字符串表\n");
                exit(1);
            }
            char* library_name = strtab + strtab_offset; // 使用偏移量和字符串表起始地址获取共享库名称
            printf("%s\n", library_name); // 打印共享库名称
            free(strtab);
            fseek(file, current_offset, SEEK_SET);
        }
    }

    fclose(file);
    free(shstrtab);
}

int main() {
    const char* executable_path = "./victim";
    print_dependent_libraries(executable_path);

    return 0;
}
