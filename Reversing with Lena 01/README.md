Crackme Information
===================
The Crackme checks for a license file and then depending on its contents displays either a "successful" or "failed" message box. 

Download Locations
------------------
Please read the warning provided in the main README before following any of the links.

* http://dl.dropbox.com/u/52247971/Reverse%20engineering%20files/Crackme%20binaries/Reversing%20with%20Lena%2001%20and%2002.7z
* http://tuts4you.com/download.php?view.122
* http://tuts4you.com/download.php?view.2876 (Stage 1 or 2)

Solution
--------
Instead of determining the correct license (as I have done in Reversing with Lena 02) I just modified a jump operation to skip the check made against the license. 

    E8 0B020000       CALL <JMP.&KERNEL32.CreateFileA>
    83F8 FF           CMP EAX,-1
    75 1D             JNZ SHORT reverseM.0040109A      ; Replaced with a JMP...
    6A 00             PUSH 0
    68 00204000       PUSH reverseM.00402000
    68 17204000       PUSH reverseM.00402017
    6A 00             PUSH 0
    E8 D7020000       CALL <JMP.&USER32.MessageBoxA>
    E8 24020000       CALL <JMP.&KERNEL32.ExitProcess>
    E9 83010000       JMP reverseM.0040121D
    6A 00             PUSH 0
    68 73214000       PUSH reverseM.00402173
    6A 46             PUSH 46
    68 1A214000       PUSH reverseM.0040211A
    50                PUSH EAX
    E8 2F020000       CALL <JMP.&KERNEL32.ReadFile>
    85C0              TEST EAX,EAX
    75 02             JNZ SHORT reverseM.004010B4
    EB 43             JMP SHORT reverseM.004010F7
    33DB              XOR EBX,EBX
    33F6              XOR ESI,ESI
    833D 73214000 10  CMP DWORD PTR DS:[402173],10
    7C 36             JL SHORT reverseM.004010F7
    8A83 1A214000     MOV AL,BYTE PTR DS:[EBX+40211A]
    3C 00             CMP AL,0
    74 08             JE SHORT reverseM.004010D3
    3C 47             CMP AL,47
    75 01             JNZ SHORT reverseM.004010D0
    46                INC ESI
    43                INC EBX
    EB EE             JMP SHORT reverseM.004010C1
    83FE 08           CMP ESI,8
    7C 1F             JL SHORT reverseM.004010F7
    E9 28010000       JMP reverseM.00401205            ; ...that points here!