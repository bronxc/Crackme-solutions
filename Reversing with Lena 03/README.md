Crackme Information
===================
The Crackme contains two nag screens. One when the process is first started and the second when the main dialog box is closed.

Download Locations
------------------
Please read the warning provided in the main README before following any of the links.
* http://dl.dropbox.com/u/52247971/Reverse%20engineering%20files/Crackme%20binaries/Reversing%20with%20Lena%2003.7z
* http://tuts4you.com/download.php?view.124
* http://tuts4you.com/download.php?view.2876 (Stage 3)

Solutions
---------

### 1st nag screen
Since the first 36 bytes worth of instructions from the program's Entry Point are dedicated to creating the nag screen I modified the Entry Point in the PE Header from 1000 (RVA) to 1024 (24 hex is 36 decimal).
Note to self: [Great description of VAs and RVAs](http://stackoverflow.com/questions/2170843/va-virutual-adress-rva-relative-virtual-address)

    50 45 00 00>ASCII "PE"   ; PE signature (PE)
    4C01        DW 014C      ; Machine = IMAGE_FILE_MACHINE_I386
    0400        DW 0004      ;  NumberOfSections = 4
    1E29D138    DD 38D1291E  ;  TimeDateStamp = 38D1291E
    00000000    DD 00000000  ;  PointerToSymbolTable = 0
    00000000    DD 00000000  ;  NumberOfSymbols = 0
    E000        DW 00E0      ;  SizeOfOptionalHeader = E0 (224.)
    0F01        DW 010F      ;  Characteristics = EXECUTABLE_IMAGE|32BIT_MACHINE|RELOCS_STRIPPED|LINE_NUMS_STRIPPED|LOCAL_SYMS_STRIPPED
    0B01        DW 010B      ; MagicNumber = PE32
    05          DB 05        ;  MajorLinkerVersion = 5
    0C          DB 0C        ;  MinorLinkerVersion = C (12.)
    00040000    DD 00000400  ;  SizeOfCode = 400 (1024.)
    000A0000    DD 00000A00  ;  SizeOfInitializedData = A00 (2560.)
    00000000    DD 00000000  ;  SizeOfUninitializedData = 0
    00100000    DD 00001000  ;  AddressOfEntryPoint = 1000 ; Replaced with 24100000 (1024). The value is backwards due to [Endianness](https://en.wikipedia.org/wiki/Endianness). 

### 2nd nag screen
The instructions for this message box was simply replaced with NOP instructions.

    6A 00             PUSH 0
    68 7D304000       PUSH Register.0040307D
    68 89304000       PUSH Register.00403089
    6A 00             PUSH 0
    E8 9E010000       CALL <JMP.&USER32.MessageBoxA>

