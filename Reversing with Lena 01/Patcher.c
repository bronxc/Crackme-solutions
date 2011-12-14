// -------------------------------------------------------------
// Original code - Tests for the existance of a file
//   83F8 FF           CMP EAX,-1
//   75 1D             JNZ SHORT reverseM.0040109A
// -------------------------------------------------------------
// Patched code - Jumps past code that checks for valid license
//   83F8 FF           CMP EAX,-1
//   EB 5B             JMP SHORT reverseM.004010D8
// -------------------------------------------------------------
// Credit
// Source-code is a modified version of int0x80's privoxy-patcher
// (https://github.com/int0x80/privoxy-mods/blob/master/privoxy-patcher.c)

#include <stdio.h>
#include <windows.h>

const unsigned char* original   = "\x83\xf8\xff\x75\x1d";
const unsigned char* patch      = "\x83\xf8\xff\xeb\x5b";
const unsigned int   patchlen   = 5;
const unsigned int   bufsz      = 0x2000;

int main(int argc, char** argv) {
    HANDLE hFile;                      // Handle to reverseMe.exe
    DWORD dwBytesRead = 0;             // Size of reverseMe.exe
    unsigned int patcher = 0;          // Offset for patching
    unsigned int it_buf = 0;           // Buffer iterator
    unsigned char buf[0x2000] = {0};   // Buffer for contents of reverseMe.exe

    // -----------------------------------------------------------
    // Check usage and exit if missing argument
    // -----------------------------------------------------------
    if (argc != 2) {
        fprintf(stderr, "[x] Usage: %s <reverseMe.exe>\n", argv[0]);
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
    // Iterate across file contents
    // -----------------------------------------------------------
    for (it_buf = 0; it_buf < dwBytesRead; it_buf++) {

        // -----------------------------------------------------------
        // Examine current byte vs. original target byte
        // Continue while current bytes match original target bytes
        // -----------------------------------------------------------
        if (*(unsigned char*)(buf + it_buf) == *(unsigned char*)original) {
            for (patcher = 0; patcher < patchlen; patcher++) {
                if (*(unsigned char*)(buf + it_buf + patcher) != *(unsigned char*)(original + patcher)) {
                    break;
                }
            }

            // -----------------------------------------------------------
            // Found the original bytes, apply the patch
            // -----------------------------------------------------------
            if (patcher == patchlen) {
                printf("[+] Found patch target at 0x%08x\n", it_buf);
                for (patcher = 0; patcher < patchlen; patcher++) {
                    *(unsigned char*)(buf + it_buf + patcher) = *(unsigned char*)(patch + patcher);
                }
                printf("[+] Patched in memory, writing to file... ");
                break;
            }
        }
    }

    // -----------------------------------------------------------
    // Write the patched buffer back to the file
    // -----------------------------------------------------------
    if (patcher == patchlen) {
        if (0 != SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
            fprintf(stderr, "\n[x] Could not reset file pointer for patch.\n");
            return 4;
        }

        if (FALSE == WriteFile(hFile, buf, dwBytesRead, &dwBytesRead, NULL)) {
            fprintf(stderr, "\n[x] Could not write patch to file.\n");
            CloseHandle(hFile);
            return 5;
        }

        printf("complete!\n");
    }

    // -----------------------------------------------------------
    // Original bytes to patch were not found
    // -----------------------------------------------------------
    else {
        fprintf(stderr, "[x] Target bytes to patch could not be found.\n");
        return 6;
    }

    CloseHandle(hFile);
    return 0;
}
