ABS CODING STYLE
================================================================================

1. Introduction
--------------------------------------------------------------------------------
Following a consistent coding style makes the job of reading and interpreting
software between developers easier, increasing the possibilities of
collaboration between developers. A coding style also helps in the debugging
because it clearly lays out the functionality of the program: the code is
self-documented.

These rules should be followed from the very beginning, and not introduced
later. Anything that is left for later is prone to not being done.


2. Line width
--------------------------------------------------------------------------------
Line width should be reasonably short to allow embedding the sources in DIN-A4
documents, GitHub web-based code viewers, and to open it in a 13" full-screen 
editor. Maintaining a readable, homogeneous width is more than recommended, 
although an exact number of characters is not defined. As a general guideline, 
lines should not be longer than 120 characters and splitting should not occur at 
less than 80 characters. 


3. Whitespace
--------------------------------------------------------------------------------
 1. Indents are four spaces. Tabs are forbidden for identation purposes unless 
    they are a language or syntax request (e.g. Makefiles).
 2. Separate parameters in functions with one space:
 
        function(param1, param2, param3);
 
 3. Opeands and operators must be separated with (at least) one space at each 
    side of the operation:
 
        b = a + 1;
 
 4. Extra spaces can be optionally added to improve readability only when
    several operations can be aligned together. In this situation, the alignment
    should be ideally done with the equal (=) symbol:
 
        b    = a + 1;
        c_2  = b * 4;
        c_2 *= a;
 
 5. Extra spaces are not allowed after '*' in C pointers. The 
    purpose of this rule is to clearly distinguish pointers from scalar 
    products. Example:
 
        int  *a;         /* Ok.    */
        int*  a;         /* Wrong. */ 
        int * a;         /* Wrong. */
        char *a, *b, *c; /* Ok    */
        b = *a;          /* Ok.    */
        b = * a;         /* Wrong. */
 
    Exception: function headers can have whitespace before and after the '*'.
 6. Do not leave whitespace dangling off the ends of lines.
 7. Leave one empty line between logical groups of instructions.
 8. Leave one empty line between functions.
 9. Leave (at least) one empty line at the end of a file.


4. Block structure
--------------------------------------------------------------------------------
 1. Braced blocks of code must be properly indented.
 2. Every indented statement is braced; even if the block contains just one
    statement. 
 3. Flow control statements must not contain spaces between the name and 
    conditions/arguments.
 
        if(condition)   /* Correct. */
        if (condition)  /* Wrong.   */
 4. Blocks containing a single statement can skip the braces if and only if they 
    fit in a single line, e.g.: 
 
        if(condition) op = 42;
 
    The purpose of this rule is only to shrink the SLOC number and should 
    *never* worsen readability. Nesting is not allowed without braces:
 
        for(int i = 0; i < N; i++)          /*          */
            for(int j = 0; j < N; j++)      /* Wrong.   */
                for(int k = 0; k < N; k++)  /*          */
                    single_line();          /*          */

        for(int i = 0; i < N; i++) if(condition) single_line(); /* Wrong.*/
         
        for(int i = 0; i < N; i++)
        {
            for(int i = 0; i < N; i++)
            {
                for(int i = 0; i < N; i++) single_line();   /* Correct. */
            }
        }
        
 5. For flow control statements (`if`, `while`, `for`, `switch`):
   1. The opening brace must be in the same line that contains the flow 
      control statement and must be preceeded by a space; 
   2. the closing brace is in a new line; 
   3. the `} else ... {` statement is in a single new line;
   4. braces are separated one space from the rest of the line (if any).
      Example:
        
            if(a == 5) {                /* One space before "{".                */
                printf("a was 5.\n");   /* Identation is applied.               */
            } else if(a == 6) {         /* Padding spaces next to braces.       */
                printf("a was 6.\n");
            } else {                    /* All in one new line.                 */
                printf("a was something else entirely.\n");
            }
            for(i = 0; i < 3; i++) {    /* Braces are optional here because the */
                printf("i is %d\n", i); /* inner part of the block is short     */
            }                           /* enough to write it on a single line. */
        
 6. Functions, classes and methods:
   1. Both opening and closing braces must be written in a new line.
   2. No blank lines can be included right after or before the opening and 
      closing braces, respectively.
      Example:

            int a_function(void)
            {                   /* New line.                                    */
                do_something(); /* No space between prev. line and this one.    */
                                /* Blank lines here ARE allowed.                */
                return 0;       /* No space after the return line.              */
            }                   /* New line.                                    */

 7. Switch control blocks must be indented setting the "case ...:" and 
    "default:" keywords at a different level than the body of the condition. 
    Example:

        switch(var) {                           /* Same rules.              */
            case 1:                             /* 1 indent. step applied.  */
            case 2:
            case 3:
                printf("Var is 1, 2 or 3\n");   /* 2 indent. steps applied. */
                break;                          /* 2 indent. steps applied. */
            case 4:
                printf("Var is 4\n");
                break;
            default:
                printf("Var is not 1, 2, 3 or 4\n");
                break;
        }

 8. Java/C++ classes must be defined in single files (i.e. defining more than 
    one class in a file is forbidden).


5. Naming
--------------------------------------------------------------------------------
 1. Names have to be lexically sufficient and must never induce to a wrong 
    understanding of the value/functionality. 
 2. The use of common words such as "aux", "error", "device", "my_function"... 
    is discouraged in general, though not forbidden. However, their usage is
    indeed restricted to local scopes (i.e. global variables/functions must 
    never be named using those common words)
 3. Variables are `lower_case_with_underscores`; easy to type and read (for all 
    languages: Java, C, Python...)
 4. In C/C++ and similar languages: functions are `lower_case_with_underscores`.
 5. In Java: methods and class names are in `CamelCase`.
 6. Structured and enum type names are in `CamelCase`; harder to type but standing
    out.
 7. Constants are `UPPER_CASE_WITH_UNDERSCORES`; to stand out.
 8. Global symbols include the domain of which they are part:
 
        syscore_init_start
        syscore_init_clean
        syscore_power_flag
 
 9. When wrapping standard library functions, use the prefix abs_ to alert
    readers that they are seeing a wrapped version; otherwise avoid this prefix:
 
        abs_malloc

 10. Files have to be named using the following structure: `<codename>[_<extra>].<ext>`
    Where:
   1. `<codename>` is the project's abbreviation for the program name (e.g. "eps").
   2. `<extra>` is an optional sub-name for the file (e.g. "driver").
   3. `<ext>` is the file extension.
   
    For C/C++ programs, two files are always compulsory: `<codename>.c` and 
    `<codename>.h`. Additional files in that case would be, e.g.: `<codename>.conf`,
    `<codename>_<extra1>.c`, `<codename>_<extra2>.c`, and so on. 
    Although OO programs are excluded from this rule, the must always include,
    at least, one source file (.java or .cpp) which name matches the codename of
    the program. 


6. Declarations
--------------------------------------------------------------------------------
 1. Mixed declarations (i.e. interleaving statements and declarations within 
    blocks) are not allowed; declarations should be at the beginning of blocks. 
    In other words, the code should not generate warnings if using GCC's
    -Wdeclaration-after-statement option.
 2. Global variables initialization should be ideally performed within the body 
    of the entry point (main) or an initialization handler. Avoid initializing a
    global variable outside a function scope and at several source files. Try to 
    use constants instead of hardcoded expressions when possible.
 3. In C, new types must always be defined in the *global* namespace. Moreover, 
    for coherency and uniformity, structs must be written using the name at the 
    beginning AND at the end. Example:
    
        typedef struct S { int x; } S;  /* Correct. Preferrable option.       */
        typedef struct S { int x; };    /* Wrong.                             */
        typedef struct { int x; } S;    /* Wrong.                             */
        struct S { int x; };            /* Wrong, unless used with the next 
                                           line.                              */
        typedef struct S S;             /* Requires `struct S` to be defined. */
        /* The use of the symbol 'S' is therefore restricted anywhere else in 
           this program. */
         
        

7. Static and Extern
--------------------------------------------------------------------------------
 1. All data global for one file, and all auxiliar functions to that file must
    have the static keyword. Do not make global what it is not necessary.
 2. Global functions and variables must be declared in the header file using the 
    "extern" modificatior. Their definition must be coded in the source files 
    where they are used (except functions). Example:
    
        /* ---- example.h: -----*/
        extern int example_global;
        extern char state_global;
        extern void my_function();
        
        /* ---- example_file1.c: ----*/
        int example_global;
        static private_function()
        {
            /* Function that uses example_global varaible. */
        }
        
        /* ---- example_file2.c: ----*/
        char state_global;
        void my_function()
        {
            /* Function that uses state_global variable. */
        }
        
        
8. Headers
--------------------------------------------------------------------------------
 1. Each source file must have one single header file. Their names must match 
    (except in the extension) and the header file should include all libraries
    and external headers. The source files should contain only one #include 
    directive (see 5.10 for more information).
 2. Header files must contain **all** constants, macros, type definitions, and
    global function prototypes and variables. The preferred header structure is
    as follows:
    
        /* File header (generic comment). */
        
        #ifndef __FILENAME_H
        #define __FILENAME_H
        
        /* INCLUDES SECTION ***************************************************/
        /* -- Standard C libraries: ----------------------------------------- */
        #include <stdlib.h>     /* Standard C library.                        */
        #include <stdio.h>      /* For general purpose I/O operations.        */
        #include <stdarg.h>     /* For variable length arguments.             */
        #include <string.h>     /* For str* functions.                        */
        #include <....h>        /* Description ...                            */         
        /* -- ABS libraries: ------------------------------------------------ */
        #include <abs/abs.h>    /* Meta header-file for configuration.        */
        #include <abs/log.h>    /* Log and Fileslog libraries.                */
        #include <abs/....h>    /* Description ...                            */         
        /* -- Other libraries: ---------------------------------------------- */
        #include "....h"        /* Description ...                            */
        #include <....h>        /* Description ...                            */
                
        /* CONSTANTS and MACROS SECTION ***************************************/
        #define MAX_LINE_SIZE   30 
        #define MAX(a,b)        (a > b ? a : b)
        #define ...             ...
        
        /* TYPES SECTION ******************************************************/
        typedef enum EnumName { A, B, C, ... };
        typedef struct StructName1 {
            ...
        } StructName1;
        
        /* GLOBAL VARIABLES SECTION *******************************************/
        extern int         variable1;
        extern int         variable2;
        extern char        *variable3;
        extern StructName1 data;
        extern EnumName    value;
        
        /* GLOBAL FUNCTION PROTOTYPES *****************************************/
        extern void my_function1();
        extern void my_function2();
        extern void my_function3();
        extern int *my_function4();
        extern ... 
        
        #endif        
        
 3. Header files must have a conditional compilation with a descriptive name 
    given its domain and file name, and finish with a `_H`:
    
        #ifndef APPMOD_H
        #define APPMOD_H
        ...
        #endif
        
 4. Only include what is needed. Avoid including what it is not needed (i.e. 
    unused libraries, old headers...)
 5. C/C++ macros and complex constants (i.e. those which are calculated at 
    compile-time) are there to save time, reduce lines of code, improve 
    readability and mitigate typo-like errors. Use them with care, though, and 
    don't forget to wrap complex expressions with parentheses. Example:
    
        #define A   3
        #define B   4
        /* ---- case 1: */
        #define C   A + B 
        #define D   A * C   /* The result will be 13. */
        /* ---- case 2: */
        #define C   (A + B) 
        #define D   (A * C) /* The result will be 21. */
        

9. Comments and in-code documentation
--------------------------------------------------------------------------------
 1. Use in-code comments rationally. There's no need to comment each and every 
    line but it is very important to comment on decisions made, obscure hacks or 
    other parts which might be difficult to understand.
 2. Always surround the comments with `/* */`. Do not use other methods like `//`.
 3. Comments are English sentences: although short and clear, they must be 
    grammatically correct. Do use punctuation (commas, full stops, parentheses 
    and hyphens) and a proper language (e.g. avoid slang words, use technical 
    expressions).
 4. Functions and files must be documented according to the JavaDoc (for Java 
    code), PlDoc (for Prolog code) and Doxygen (for the rest of the languages)
    specifications.


11. Reliability, debug easiness and flexibility
--------------------------------------------------------------------------------
Based on Holzmann's **"The Power of Ten: Rules for Developing Safety Critical 
Code"**, NASA/JPL Laboratory for Reliable Software, available 
[here](http://spinroot.com/gerard/pdf/Power_of_Ten.pdf), the following rules
should be applied to the software developed for ABS. Their rationale is
detailed in the aforementioned paper. 

1.  Restrict all code to very simple control flow constructs –do not use 
    goto statements, setjmp or longjmp constructs, and direct or 
    indirect recursion. 
    Exception: "goto" statements can be used to implement cleaning 
    sections before the return of a function. The purpose of this rule 
    is only to provide an easy way to clean/free multiple resources 
    which could have not been gained due to partial errors. Example:
    
        int function()
        {
            int error;
            if((error = do_something(1)) < 0) goto clean3;
            if((error = do_something(2)) < 0) goto clean2;
            if((error = do_something(3)) < 0) goto clean1;
        clean1:
            undo_something(3);
        clean2:
            undo_something(2);
        clean3:
            undo_something(1);
            return error;
        }
        
2.  All loops must have a fixed upper-bound. It must be trivially
    possible for a checking tool to prove statically that a preset 
    upper-bound on the number of iterations of a loop cannot be 
    exceeded. If the loop-bound cannot be proven statically, the rule is 
    considered violated.
3.  Reduce the number of dynamic allocations to the minimum possible. If
    the size of memory blocks can be predicted (at compile time), it is
    compulsory to declare the memory region size statically.
    Pointers are allowed only when there's a clear justification for 
    them. If a section initializes a memory block (i.e. calls some of 
    the malloc() family functions) it is strictly mandatory to include 
    complementary free calls. 
4.  No function should be longer than what can be printed on a single 
    sheet of paper in a standard reference format with one line per 
    statement and one line per declaration. Typically, this means no 
    more than about 100 lines of code per function.
5.  The assertion density of the code should average to a minimum of two 
    assertions per function. Assertions are used to check for anomalous 
    conditions that should never happen in real-life executions. 
    Assertions must always be side-effect free and should be defined as 
    Boolean tests. When an assertion fails, an explicit recovery action 
    must be taken, e.g., by returning an error condition to the caller 
    of the function that executes the failing assertion. Any assertion 
    for which a static checking tool can prove that it can never fail or 
    never hold violates this rule. (i.e., it is not possible to satisfy 
    the rule by adding unhelpful `assert(true)` statements.)
    A typical use of an assertion would be as follows:
    
        if(!c_assert(p >= 0) == true) 
        {
            return ERROR;
        }
    
    with the assertion defined as follows:
    
        #define c_assert(e) ((e) ? (true) : \
            tst_debugging("%s,%d: assertion '%s' failed\n", \
            __FILE__, __LINE__, #e), false)
    
    In this definition, `__FILE__` and `__LINE__` are predefined by the 
    macro preprocessor to produce the filename and line-number of the 
    failing assertion. The syntax #e turns the assertion condition e 
    into a string that is printed as part of the error message. In code 
    destined for an embedded processor there is of course no place to 
    print the error message itself – in that case, the call to 
    tst_debugging is turned into a no-op, and  the assertion turns into 
    a pure Boolean test that enables error recovery from anomolous 
    behavior.
6.  The return value of non-void functions must be checked by each 
    calling function, and the validity of parameters must be checked 
    inside each function.
7.  The use of pointers should be restricted. Specifically, no more than 
    one level of dereferencing is allowed. Pointer dereference 
    operations may not be hidden in macro definitions or inside typedef 
    declarations. Function pointers are only permitted when the have a
    direct and clear justification.
8.  All code must be compiled, from the first day of development, with 
    all compiler warnings enabled at the compiler’s most pedantic 
    setting. All code must compile with these setting without any 
    warnings. All code must be checked daily with at least one, but 
    preferably more than one, state-of-the-art static source code 
    analyzer and should pass the analyses with zero warnings. 
    (see [this link](http://spinroot.com/static/index.html))


12. Final remarks
--------------------------------------------------------------------------------
 1. Any code modifications accepted into the master tree must be compliant with 
    these sets of rules and must compile without warnings using the -Wall 
    option.
 2. New code must be provided together with a Makefile. Including *README.md* 
    temporary files within certain folders is allowed (and recommended) to save 
    time (and headaches) to the rest of the team.
   


