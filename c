     1                                  section .data
     2                                  ; -----
     3                                  ; Define standard constants.
     4                                  LF	        equ 10 ; line feed
     5                                  SPACE       equ 32 ; space ' '
     6                                  ZERO_CHAR   equ 48 ; zero '0'
     7                                  NULL	    equ 0 ; end of string
     8                                  TRUE	    equ 1
     9                                  FALSE	    equ 0
    10                                  EXIT_SUCCESS equ 0 ; success code
    11                                  STDIN	    equ 0 ; standard input
    12                                  STDOUT	    equ 1 ; standard output
    13                                  STDERR	    equ 2 ; standard error
    14                                  SYS_read	equ 0 ; read
    15                                  SYS_write	equ 1 ; write
    16                                  SYS_open	equ 2 ; file open
    17                                  SYS_close	equ 3 ; file close
    18                                  SYS_fork	equ 57 ; fork
    19                                  SYS_exit	equ 60 ; terminate
    20                                  SYS_creat	equ 85 ; file open/create
    21                                  SYS_time	equ 201 ; get time
    22                                  O_CREAT 	equ 0x40
    23                                  O_TRUNC 	equ 0x200
    24                                  O_APPEND	equ 0x400
    25                                  O_RDONLY	equ 000000q ; read only
    26                                  O_WRONLY	equ 000001q ; write only
    27                                  O_RDWR	    equ 000002q ; read and write
    28                                  S_IRUSR	    equ 00400q
    29                                  S_IWUSR    	equ 00200q
    30                                  S_IXUSR    	equ 00100q
    31                                  
    32                                  ; -----
    33                                  ; Variables/constants for main.
    34                                  BUFF_SIZE	equ 255
    35 00000000 0A00                    newLine     db LF, NULL
    36 00000002 0A46696C6520526561-     header      db LF, "File Read Example."
    36 0000000B 64204578616D706C65-
    36 00000014 2E                 
    37 00000015 0A0A00                              db LF, LF, NULL
    38 00000018 66696C652E74787400      fileName    db "file.txt", NULL
    39 00000021 0000000000000000        fileDesc    dq 0
    40 00000029 4572726F72206F7065-     errMsgOpen  db "Error opening the file.", LF, NULL
    40 00000032 6E696E672074686520-
    40 0000003B 66696C652E0A00     
    41 00000042 4572726F7220726561-     errMsgRead  db "Error reading from the file.", LF, NULL
    41 0000004B 64696E672066726F6D-
    41 00000054 207468652066696C65-
    41 0000005D 2E0A00             
    42                                  ; -------------------------------------------------------
    43                                  section .bss
    44 00000000 <res FFh>               readBuffer  resb BUFF_SIZE
    45 000000FF <res FFh>               resBuffer   resb BUFF_SIZE
    46                                  ; -------------------------------------------------------
    47                                  section .text
    48                                  global _start
    49                                  _start:
    50                                  ; -----
    51                                  ; Display header line...
    52 00000000 48BF-                       mov     rdi, header
    52 00000002 [0200000000000000] 
    53 0000000A E8BD000000                  call    printString
    54                                  
    55                                  ; -----
    56                                  ; Get filename string from first command line argument
    57                                  getArgv1:
    58 0000000F 5B                          pop     rbx ; argc
    59 00000010 5B                          pop     rbx ; argv[0]
    60 00000011 5B                          pop     rbx ; argv[1]
    61 00000012 4889DF                      mov     rdi, rbx
    62                                      
    63                                  ; -----
    64                                  ; Attempt to open file - Use system service for file open
    65                                  ; System Service - Open
    66                                  ; rax = SYS_open
    67                                  ; rdi = address of file name string
    68                                  ; rsi = attributes (i.e., read only, etc.)
    69                                  ; Returns:
    70                                  ; if error -> eax < 0
    71                                  ; if success -> eax = file descriptor number
    72                                  ; The file descriptor points to the File Control
    73                                  ; Block (FCB). The FCB is maintained by the OS.
    74                                  ; The file descriptor is used for all subsequent file
    75                                  ; operations (read, write, close).
    76                                  openInputFile:
    77 00000015 B802000000                  mov     rax, SYS_open ; file open
    78 0000001A BE00000000                  mov     rsi, O_RDONLY ; read only access
    79 0000001F 0F05                        syscall ; call the kernel
    80 00000021 4883F800                    cmp     rax, 0 ; check for success
    81 00000025 7C77                        jl      errorOnOpen
    82 00000027 48890425[21000000]          mov     qword [fileDesc], rax ; save descriptor
    83                                  ; -----
    84                                  ; Read from file.
    85                                  ; For this example, we know that the file has only 1 line.
    86                                  ; System Service - Read
    87                                  ; rax = SYS_read
    88                                  ; rdi = file descriptor
    89                                  ; rsi = address of where to place data
    90                                  ; rdx = count of characters to read
    91                                  ; Returns:
    92                                  ; if error -> rax < 0
    93                                  ; if success -> rax = count of characters actually read
    94 0000002F B800000000                  mov      rax, SYS_read
    95 00000034 488B3C25[21000000]          mov      rdi, qword [fileDesc]
    96 0000003C 48BE-                       mov      rsi, readBuffer
    96 0000003E [0000000000000000] 
    97 00000046 BAFF000000                  mov      rdx, BUFF_SIZE
    98 0000004B 0F05                        syscall
    99 0000004D 4883F800                    cmp      rax, 0
   100 00000051 7C5C                        jl       errorOnRead
   101                                  
   102                                  ; -----
   103                                  ; Handle the buffer
   104                                  ; add the NULL for the print string
   105                                  
   106 00000053 48BE-                       mov     rsi, readBuffer
   106 00000055 [0000000000000000] 
   107 0000005D C6040600                    mov     byte [rsi+rax], NULL
   108 00000061 48BF-                       mov     rdi, readBuffer
   108 00000063 [0000000000000000] 
   109 0000006B E894000000                  call    handleBuffer
   110                                  
   111                                  ; -----
   112                                  ; Print the result buffer
   113                                  ; add the NULL for the print string
   114 00000070 48BE-                       mov     rsi, resBuffer
   114 00000072 [FF00000000000000] 
   115 0000007A C6040600                    mov     byte [rsi+rax], NULL
   116 0000007E 48BF-                       mov     rdi, resBuffer
   116 00000080 [FF00000000000000] 
   117 00000088 E83F000000                  call    printString
   118                                  
   119                                  ; -----
   120                                  ; Close the file.
   121                                  ; System Service - close
   122                                  ; rax = SYS_close
   123                                  ; rdi = file descriptor
   124 0000008D B803000000                  mov     rax, SYS_close
   125 00000092 488B3C25[21000000]          mov     rdi, qword [fileDesc]
   126 0000009A 0F05                        syscall
   127 0000009C EB22                        jmp     exampleDone
   128                                  ; -----
   129                                  ; Error on open.
   130                                  ; note, eax contains an error code which is not used
   131                                  ; for this example.
   132                                  errorOnOpen:
   133 0000009E 48BF-                       mov     rdi, errMsgOpen
   133 000000A0 [2900000000000000] 
   134 000000A8 E81F000000                  call    printString
   135 000000AD EB11                        jmp     exampleDone
   136                                  ; -----
   137                                  ; Error on read.
   138                                  ; note, eax contains an error code which is not used
   139                                  ; for this example.
   140                                  errorOnRead:
   141 000000AF 48BF-                       mov     rdi, errMsgRead
   141 000000B1 [4200000000000000] 
   142 000000B9 E80E000000                  call    printString
   143 000000BE EB00                        jmp     exampleDone
   144                                  ; -----
   145                                  ; Example program done.
   146                                  exampleDone:
   147 000000C0 B83C000000                  mov     rax, SYS_exit
   148 000000C5 BF00000000                  mov     rdi, EXIT_SUCCESS
   149 000000CA 0F05                        syscall
   150                                  ; **********************************************************
   151                                  ; Generic procedure to display a string to the screen.
   152                                  ; String must be NULL terminated.
   153                                  ; Algorithm:
   154                                  ; Count characters in string (excluding NULL)
   155                                  ; Use syscall to output characters
   156                                  ; Arguments:
   157                                  ; 1) address, string
   158                                  ; Returns:
   159                                  ; nothing
   160                                  global printString
   161                                  printString:
   162 000000CC 55                          push    rbp
   163 000000CD 4889E5                      mov     rbp, rsp
   164 000000D0 53                          push    rbx
   165                                  ; -----
   166                                  ; Count characters in string.
   167 000000D1 4889FB                      mov     rbx, rdi
   168 000000D4 BA00000000                  mov     rdx, 0
   169                                  
   170                                  ; -----
   171                                  ; Check is nullptr
   172 000000D9 4883FB00                    cmp     rbx, 0
   173 000000DD 740D                        je      strCountDone
   174                                  strCountLoop:
   175 000000DF 803B00                      cmp     byte [rbx], NULL
   176 000000E2 7408                        je      strCountDone
   177 000000E4 48FFC2                      inc     rdx
   178 000000E7 48FFC3                      inc     rbx
   179 000000EA EBF3                        jmp     strCountLoop
   180                                  strCountDone:
   181 000000EC 4883FA00                    cmp     rdx, 0
   182 000000F0 740F                        je      prtDone
   183                                  ; -----
   184                                  ; Call OS to output string.
   185 000000F2 B801000000                  mov     rax, SYS_write ; code for write()
   186 000000F7 4889FE                      mov     rsi, rdi ; addr of characters
   187 000000FA BF01000000                  mov     rdi, STDOUT ; file descriptor
   188                                  ; count set above
   189 000000FF 0F05                        syscall ; system call
   190                                  ; -----
   191                                  ; String printed, return to calling routine.
   192                                  prtDone:
   193 00000101 5B                          pop     rbx
   194 00000102 5D                          pop     rbp
   195 00000103 C3                          ret
   196                                  
   197                                  ; -----
   198                                  ; Read from readBuffer and write handled string
   199                                  ; into resBuffer
   200                                  ; While handling: 
   201                                  ; 1) Skip extra spaces,
   202                                  ; 2) Pass all the '\n',
   203                                  ; 3) Delete words with only '[1..9]'
   204                                  global handleBuffer
   205                                  handleBuffer:
   206 00000104 4889FB                      mov     rbx, rdi ; rbx = readBuffer + 0
   207 00000107 BA00000000                  mov     rdx, 0
   208 0000010C B100                        mov     cl,  0 ; started = false
   209 0000010E 41B001                      mov     r8b, 1 ; isDigitOnlyWord = true
   210 00000111 41B101                      mov     r9b, 1 ; isResBufferLineEmpty 
   211 00000114 49BA-                       mov     r10, resBuffer
   211 00000116 [FF00000000000000] 
   212                                  
   213                                  strHandlingLoop:
   214 0000011E 80F901                      cmp     cl, 1
   215 00000121 740F                        je      handleIfStarted
   216 00000123 803B20                      cmp     byte [rbx], SPACE
   217 00000126 7457                        je      strHandlingLoopNextIt
   218 00000128 803B0A                      cmp     byte [rbx], LF
   219 0000012B 7452                        je      strHandlingLoopNextIt
   220                                  
   221 0000012D B101                        mov     cl, 1 ; started = true
   222 0000012F 4889DA                      mov     rdx, rbx ; rdx = l
   223                                  
   224                                  handleIfStarted:
   225 00000132 803B00                      cmp     byte [rbx], NULL
   226 00000135 741B                        je      handleIfEOW
   227 00000137 803B20                      cmp     byte [rbx], SPACE
   228 0000013A 7416                        je      handleIfEOW
   229 0000013C 803B0A                      cmp     byte [rbx], LF
   230 0000013F 7411                        je      handleIfEOW
   231                                  
   232 00000141 8A03                        mov     al, byte [rbx]
   233 00000143 2C30                        sub     al, ZERO_CHAR
   234 00000145 3C09                        cmp     al, 9
   235 00000147 B800000000                  mov     rax, 0
   236 0000014C 4C0F47C0                    cmova   r8, rax ; isDigitsOnlyWord = false
   237                                  
   238 00000150 EB2D                        jmp     strHandlingLoopNextIt
   239                                  
   240                                  handleIfEOW:
   241 00000152 B100                        mov     cl, 0 ; started = false
   242                                  
   243 00000154 4180F801                    cmp     r8b, 1
   244 00000158 41B001                      mov     r8b, 1 ; is digit only word = true
   245 0000015B 7422                        je      strHandlingLoopNextIt
   246                                  ;    mov     r8b, 1 ; is digit only word = true
   247                                   
   248 0000015D 4180F901                    cmp     r9b, 1
   249 00000161 41B100                      mov     r9b, 0 ; is res buffer line empty = false
   250                                  
   251 00000164 7407                        je      transferWordLoop
   252                                  
   253 00000166 41C60220                    mov     byte [r10], SPACE
   254 0000016A 49FFC2                      inc     r10
   255                                  
   256                                  transferWordLoop:
   257 0000016D 4839DA                      cmp     rdx, rbx
   258 00000170 740D                        je      strHandlingLoopNextIt
   259 00000172 8A02                        mov     al, byte[rdx]
   260 00000174 418802                      mov     byte [r10], al
   261 00000177 49FFC2                      inc     r10
   262 0000017A 48FFC2                      inc     rdx
   263 0000017D EBEE                        jmp     transferWordLoop
   264                                  
   265                                  strHandlingLoopNextIt:
   266 0000017F 803B00                      cmp     byte [rbx], NULL
   267 00000182 7415                        je      finishHandling
   268                                  
   269 00000184 803B0A                      cmp     byte [rbx], LF
   270 00000187 488D5B01                    lea     rbx, [rbx+1]
   271 0000018B 7591                        jne     strHandlingLoop    
   272                                  
   273 0000018D 41B101                      mov     r9b, 1 ; is res buffer line empty = true
   274 00000190 41C6020A                    mov     byte [r10], LF
   275 00000194 49FFC2                      inc     r10
   276 00000197 EB85                        jmp     strHandlingLoop
   277                                  
   278                                  finishHandling:
   279 00000199 4889D8                      mov     rax, rbx
   280 0000019C 482D[FF000000]              sub     rax, resBuffer
   281 000001A2 C3                          ret
   282                                  
