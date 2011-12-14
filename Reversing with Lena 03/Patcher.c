// -------------------------------------------------------------
// Original PE header
//   50 45 00 00>ASCII "PE"   ; PE signature (PE)
//   4C01        DW 014C      ; Machine = IMAGE_FILE_MACHINE_I386
//   0400        DW 0004      ;  NumberOfSections = 4
//   1E29D138    DD 38D1291E  ;  TimeDateStamp = 38D1291E
//   00000000    DD 00000000  ;  PointerToSymbolTable = 0
//   00000000    DD 00000000  ;  NumberOfSymbols = 0
//   E000        DW 00E0      ;  SizeOfOptionalHeader = E0 (224.)
//   0F01        DW 010F      ;  Characteristics = EXECUTABLE_IMAGE|32BIT_MACHINE|RELOCS_STRIPPED|LINE_NUMS_STRIPPED|LOCAL_SYMS_STRIPPED
//   0B01        DW 010B      ; MagicNumber = PE32
//   05          DB 05        ;  MajorLinkerVersion = 5
//   0C          DB 0C        ;  MinorLinkerVersion = C (12.)
//   00040000    DD 00000400  ;  SizeOfCode = 400 (1024.)
//   000A0000    DD 00000A00  ;  SizeOfInitializedData = A00 (2560.)
//   00000000    DD 00000000  ;  SizeOfUninitializedData = 0
//   00100000    DD 00001000  ;  AddressOfEntryPoint = 1000
// -------------------------------------------------------------
// Patched part of PE header
//   24100000    DD 00001024  ;  AddressOfEntryPoint = 1024

// -------------------------------------------------------------
// Original code - Displays a nag screen
//   6A 00             PUSH 0
//   68 7D304000       PUSH Register.0040307D
//   68 89304000       PUSH Register.00403089
//   6A 00             PUSH 0
//   E8 9E010000       CALL <JMP.&USER32.MessageBoxA>
// -------------------------------------------------------------
// Patched code
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
//   90                NOP
// -------------------------------------------------------------
// Credit
// Source-code is a modified version of int0x80's privoxy-patcher
// (https://github.com/int0x80/privoxy-mods/blob/master/privoxy-patcher.c)

#include <stdio.h>
#include <windows.h>

struct patch {
    unsigned char* original;
    unsigned char* patched;
    unsigned int length;
};

// Changes the entry point
const struct patch  entrypoint = {
    (unsigned char*)"\xD1\x38\x00\x00\x00\x00\x00\x00\x00\x00\xE0\x00\x0F\x01\x0B\x01\x05\x0C\x00\x04\x00\x00\x00\x0A\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00",
    (unsigned char*)"\xD1\x38\x00\x00\x00\x00\x00\x00\x00\x00\xE0\x00\x0F\x01\x0B\x01\x05\x0C\x00\x04\x00\x00\x00\x0A\x00\x00\x00\x00\x00\x00\x24\x10\x00\x00",
    34
};

// NOPs the Message box's creation
const struct patch  dialog_jmp = {
    (unsigned char*)"\x6A\x00\x68\x7D\x30\x40\x00\x68\x89\x30\x40\x00\x6A\x00\xE8\x9E\x01\x00\x00",
    (unsigned char*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
    19
};

const unsigned int   bufsz = 0x45F; // Amount of bytes to read. Used along with buf

int main(int argc, char** argv) {
    HANDLE hFile;                      // Handle to RegisterMe.exe
    DWORD dwBytesRead = 0;             // Size of RegisterMe.exe
    unsigned int patcher = 0;          // Offset for patching
    unsigned int it_buf = 0;           // Buffer iterator
    unsigned int it_patch = 0;         // Patch iterator
    unsigned char buf[0x45F] = {0};    // Buffer for contents of RegisterMe.exe


    struct patch patches[] = {entrypoint, dialog_jmp}; // All patch structures
    unsigned int patches_total = 2;                    // Total patches to apply


    // -----------------------------------------------------------
    // Check usage and exit if missing argument
    // -----------------------------------------------------------
    if (argc != 2) {
        fprintf(stderr, "[x] Usage: %s <RegisterMe.exe>\n", argv[0]);
        return 1;
    }

    // -----------------------------------------------------------
    // Open reverseMe binary for reading
    // -----------------------------------------------------------
    hFile = CreateFile(argv[1], (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[x] Unable to open file: %s\n", argv[1]);
        return 2;
    }

    // -----------------------------------------------------------
    // Read reverseMe binary into buf
    // -----------------------------------------------------------
    if(FALSE == ReadFile(hFile, buf, bufsz-1, &dwBytesRead, NULL)) {
        fprintf(stderr, "[x] Unable to read file: %s\n", argv[1]);
        CloseHandle(hFile);
        return 3;
    }

    // -----------------------------------------------------------
    // Iterate across patches to apply
    // -----------------------------------------------------------
    unsigned int patches_found = 0;  // Counter of patches found
    for (it_patch = 0; it_patch < patches_total; it_patch++) {
        printf("[+] Searching for patch %d\n", it_patch + 1);

        // -----------------------------------------------------------
        // Iterate across file contents
        // -----------------------------------------------------------
        for (it_buf = 0; it_buf < dwBytesRead; it_buf++) {

            // -----------------------------------------------------------
            // Examine current byte vs. original target byte
            // Continue while current bytes match original target bytes
            // -----------------------------------------------------------
            if (*(unsigned char*)(buf + it_buf) == *(unsigned char*)patches[it_patch].original) {
                for (patcher = 0; patcher < patches[it_patch].length; patcher++) {
                    if (*(unsigned char*)(buf + it_buf + patcher) != *(unsigned char*)(patches[it_patch].original + patcher)) {
                        break;
                    }
                }

                // -----------------------------------------------------------
                // Found the original bytes, apply the patch
                // -----------------------------------------------------------
                if (patcher == patches[it_patch].length) {
                    printf("[+] Found patch target at 0x%08x\n", it_buf);
                    patches_found++;

                    for (patcher = 0; patcher < patches[it_patch].length; patcher++) {
                        *(unsigned char*)(buf + it_buf + patcher) = *(unsigned char*)(patches[it_patch].patched + patcher);
                    }
                    printf("[+] Patched in memory\n");

                    break;
                }
            }
        }

    }

    // -----------------------------------------------------------
    // Check all patches were found
    // -----------------------------------------------------------
    if(patches_found == patches_total) {
        printf("[+] Writing patched memory to file\n");

        // -----------------------------------------------------------
        // Write the patched buffer back to the file
        // -----------------------------------------------------------
        if (0 != SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
            fprintf(stderr, "\n[x] Could not reset file pointer for patch.\n");
            return 4;
        }

        if (FALSE == WriteFile(hFile, buf, dwBytesRead, &dwBytesRead, NULL)) {
            fprintf(stderr, "\n[x] Could not write patch to file.\n");
            CloseHandle(hFile);
            return 5;
        }

        printf("Complete!\n");

    } else {
        fprintf(stderr, "[x] Target bytes to patch could not be found.\n");
        return 6;
    }


    CloseHandle(hFile);
    return 0;
}
