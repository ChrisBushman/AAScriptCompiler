#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>
#include <ctype.h>
//#include <bios.h>
#include <string.h>

#define QUOTE_START    0xFF
#define QUOTE_END      0xFE

typedef void T_void ;
typedef char T_sbyte8 ;
typedef unsigned char T_byte8 ;
typedef short int T_sword16 ;
typedef unsigned short int T_word16 ;
typedef signed int T_sword32 ;
typedef unsigned int T_word32 ;
typedef enum T_byte8 {
    FALSE,
    TRUE,
    BOOLEAN_UNKNOWN
} E_Boolean ;

typedef struct {
    T_byte8 name[32] ;
    T_word16 number ;
} T_event ;

typedef struct {
    T_byte8 name[32] ;
    T_word16 number ;
    T_word16 numArgs ;
} T_command ;

typedef struct {
    T_byte8 name[32] ;
    T_word16 number ;
} T_var ;

typedef struct {
    T_byte8 name[32] ;
    T_sword32 number ;
} T_defnum ;

#define MAX_EVENTS  256
T_event *G_events ;
T_word16 G_numEvents = 0 ;

#define MAX_COMMANDS  256
T_command *G_commands ;
T_word16 G_numCommands = 0 ;

#define MAX_VARS   256
T_var *G_vars ;
T_word16 G_numVars = 0 ;

#define MAX_DEFNUMS   1024
T_var *G_defnums ;
T_word16 G_numDefnums = 0 ;

#define MAX_PLACES 256
T_word16 G_places[MAX_PLACES] ;

T_word16 G_place = 0 ;

#define MAX_SIZE_CODE 8192
T_byte8 G_code[MAX_SIZE_CODE] ;

T_word16 G_line = 1 ;

T_word16 G_errors = 0 ;

#define MAX_EVENT_PLACES MAX_EVENTS
T_word16 G_eventPlaces[MAX_EVENT_PLACES] ;

typedef T_byte8 E_scriptDataType ;
#define SCRIPT_DATA_TYPE_8_BIT_NUMBER           0
#define SCRIPT_DATA_TYPE_16_BIT_NUMBER          1
#define SCRIPT_DATA_TYPE_32_BIT_NUMBER          2
#define SCRIPT_DATA_TYPE_STRING                 3
#define SCRIPT_DATA_TYPE_VARIABLE               4
#define SCRIPT_DATA_TYPE_EVENT_PARAMETER        5
#define SCRIPT_DATA_TYPE_FLAG                   6
#define SCRIPT_DATA_TYPE_NONE                   7
#define SCRIPT_DATA_TYPE_UNKNOWN                8

typedef T_byte8 E_scriptFlag ;
#define SCRIPT_FLAG_EQUAL                  0
#define SCRIPT_FLAG_NOT_EQUAL              1
#define SCRIPT_FLAG_LESS_THAN              2
#define SCRIPT_FLAG_NOT_LESS_THAN          3
#define SCRIPT_FLAG_GREATER_THAN           4
#define SCRIPT_FLAG_NOT_GREATER_THAN       5
#define SCRIPT_FLAG_LESS_THAN_OR_EQUAL     6
#define SCRIPT_FLAG_GREATER_THAN_OR_EQUAL  7
#define SCRIPT_FLAG_UNKNOWN                8


T_void CompileInclude(T_byte8 *p_include) ;

void LoadEvents(T_void)
{
    FILE *fp ;
    char buffer[80] ;

    fp = fopen("EVENTS.LST", "r") ;
    if (fp == NULL)  {
        puts("Cannot open EVENTS.LST!\n") ;
        exit(4) ;
    }

    G_events = malloc(sizeof(T_event) * MAX_EVENTS) ;
    if (G_events == NULL)  {
        puts("Not enough memory for event list!\n") ;
        exit(5) ;
    }
    G_numEvents = 0 ;
    fgets(buffer, 80, fp) ;
    while (!feof(fp))  {
        if (isalpha(buffer[0]))  {
            if (G_numEvents >= MAX_EVENTS)  {
                puts("Too many events!") ;
                exit(6) ;
            }
            sscanf(
                buffer,
                "%s%u",
                G_events[G_numEvents].name,
                &G_events[G_numEvents].number) ;
            G_numEvents++ ;
        }
        fgets(buffer, 80, fp) ;
    }
    fclose(fp) ;
    printf("Loaded %d events\n", G_numEvents) ;
}

void LoadCommands(T_void)
{
    FILE *fp ;
    char buffer[80] ;

    fp = fopen("COMMAND.LST", "r") ;
    if (fp == NULL)  {
        puts("Cannot open EVENTS.LST!\n") ;
        exit(4) ;
    }

    G_commands = malloc(sizeof(T_command) * MAX_COMMANDS) ;
    if (G_commands == NULL)  {
        puts("Not enough memory for command list!\n") ;
        exit(7) ;
    }
    G_numCommands = 0 ;
    fgets(buffer, 80, fp) ;
    while (!feof(fp))  {
        if (isalpha(buffer[0]))  {
            if (G_numCommands >= MAX_COMMANDS)  {
                puts("Too many commands!") ;
                exit(8) ;
            }
            sscanf(
                buffer,
                "%s%u%u",
                G_commands[G_numCommands].name,
                &G_commands[G_numCommands].number,
                &G_commands[G_numCommands].numArgs) ;
            G_numCommands++ ;
        }
        fgets(buffer, 80, fp) ;
    }
    fclose(fp) ;
    printf("Loaded %d commands\n", G_numCommands) ;
}

T_sword16 FindEvent(T_byte8 *p_name)
{
    T_sword16 i ;

    for (i=0; i<G_numEvents; i++)
        if (strcmp(G_events[i].name, p_name) == 0)
            return i ;

    return -1 ;
}

T_sword16 FindCommand(T_byte8 *p_name)
{
    T_sword16 i ;

    for (i=0; i<G_numCommands; i++)
        if (strcmp(G_commands[i].name, p_name) == 0)
            return i ;

    return -1 ;
}

T_void AddVar(T_byte8 *p_name, T_word16 number)
{
    if (G_numVars == MAX_VARS)  {
        puts("Too many defvars!") ;
        exit(100) ;
    }

    strcpy(G_vars[G_numVars].name, p_name) ;
    G_vars[G_numVars].number = number ;
    G_numVars++ ;
}

T_sword16 FindVar(T_byte8 *p_name)
{
    T_sword16 i ;

    for (i=0; i<G_numVars; i++)
        if (strcmp(G_vars[i].name, p_name) == 0)
            return i ;

    return -1 ;
}

T_sword16 FindDefnum(T_byte8 *p_name)
{
    T_sword16 i ;

    for (i=0; i<G_numDefnums; i++)
        if (strcmp(G_defnums[i].name, p_name) == 0)
            return i ;

    return -1 ;
}

T_void AddDefnum(T_byte8 *p_name, T_sword32 number)
{
    if (G_numDefnums == MAX_DEFNUMS)  {
        puts("Too many defnums!") ;
        exit(100) ;
    }

//printf("Defining number: %s %ld\n", p_name, number) ;
    strcpy(G_defnums[G_numDefnums].name, p_name) ;
    G_defnums[G_numDefnums].number = (T_word16)number ;
    G_numDefnums++ ;
}

T_void Initialize(T_void)
{
    LoadEvents() ;
    LoadCommands() ;
    G_vars = malloc(MAX_VARS * sizeof(T_var)) ;
    if (G_vars == NULL)  {
        printf("No memory for vars!") ;
        exit(101) ;
    }
    G_defnums = malloc(MAX_DEFNUMS * sizeof(T_defnum)) ;
    if (G_defnums == NULL)   {
        printf("No memory for defnums!") ;
        exit(102) ;
    }
    memset(G_places, 0xFF, sizeof(G_places)) ;
    memset(G_eventPlaces, 0xFF, sizeof(G_eventPlaces)) ;
}

T_void StripComment(T_byte8 *p_string)
{
    T_byte8 *p_where ;
    E_Boolean quoteOn = FALSE ;
    E_Boolean nextChar = FALSE ;

    p_where = p_string ;
    while (*p_string != '\0')  {
        if ((quoteOn == FALSE) && (nextChar == FALSE))  {
            if ((*p_string == ';') ||
                (*p_string == '\r') ||
                (*p_string == '\n') ||
                (*p_string == '/')) {
                *p_string = '\0' ;
                break ;
            }
        }
        if ((*p_string == '\"') && (nextChar == FALSE))
            if (quoteOn == FALSE)  {
                *p_string = QUOTE_START ;
                quoteOn = TRUE ;
            } else {
                *p_string = QUOTE_END ;
                quoteOn = FALSE ;
            }
        *p_where = *p_string ;
        if (*p_string == '\\')  {
            nextChar = TRUE ;
        }  else  {
            p_where++ ;
            nextChar = FALSE ;
        }

        p_string++ ;
    }

    *p_where = '\0' ;
}

T_void OutputByte(T_byte8 b)
{
    if (G_place >= MAX_SIZE_CODE)  {
        printf("Code size too big!") ;
        exit(301) ;
    }
    G_code[G_place++] = b ;
}

T_void Strip(T_byte8 *p_text)
{
    T_byte8 *p_start ;
    T_word16 len ;

    p_start = p_text ;
    while ((*p_start != '\0') &&
           (isspace(*p_start)))
        p_start++ ;
    /* memmove, not strcpy -- p_text and p_start point into the same
       buffer and can overlap (whenever there's leading whitespace to
       strip), which is undefined behavior for strcpy. Harmless in
       practice on this tool's original DOS-era Borland build, but
       modern hardened libc (glibc/Apple libc's _FORTIFY_SOURCE) traps
       it at runtime -- confirmed crashing (SIGILL, __chk_fail_overlap)
       on any script with a leading space in an argument, e.g.
       "Set(DruidLift, NOT_ACTIVATED)" in L1.SRC. */
    memmove(p_text, p_start, strlen(p_start) + 1) ;
    len = strlen(p_text)-1 ;
    while (len)  {
        if (isspace(p_text[len]))
            p_text[len--] = '\0' ;
        else
            break ;
    }
}

T_byte8 *G_systemFlags[] = {
    "Equal",
    "NotEqual",
    "LessThan",
    "NotLessThan",
    "GreaterThan",
    "NotGreaterThan",
    "LessThanOrEqual",
    "GreaterThanOrEqual"
} ;

T_sword16 FindFlag(T_byte8 *p_name)
{
    T_sword16 i ;

    for (i=0; i<8; i++)
        if (strcmp(p_name, G_systemFlags[i]) == 0)
            return i;

    return -1 ;
}

T_void OutputNumber(T_sword32 value)
{
    /* Explicit little-endian shift/mask, not a pointer-cast byte
       extraction -- the latter emits host-native byte order, which is
       wrong (big-endian, backwards) on a big-endian build host (PPC,
       MIPS/IRIX). The game engine always expects little-endian bytecode
       regardless of what platform compiled it. */
    if ((value >= -120L) && (value <= 120L))  {
        OutputByte(SCRIPT_DATA_TYPE_8_BIT_NUMBER) ;
        OutputByte((T_byte8)(value & 0xFF)) ;
    } else if ((value >= -32760L) && (value <= 32760L))  {
        OutputByte(SCRIPT_DATA_TYPE_16_BIT_NUMBER) ;
        OutputByte((T_byte8)(value & 0xFF)) ;
        OutputByte((T_byte8)((value >> 8) & 0xFF)) ;
    } else {
        OutputByte(SCRIPT_DATA_TYPE_32_BIT_NUMBER) ;
        OutputByte((T_byte8)(value & 0xFF)) ;
        OutputByte((T_byte8)((value >> 8) & 0xFF)) ;
        OutputByte((T_byte8)((value >> 16) & 0xFF)) ;
        OutputByte((T_byte8)((value >> 24) & 0xFF)) ;
    }
}

T_void CompileArgs(T_byte8 *p_args, T_word16 numArgs)
{
    char args[10][100] ;
    T_word16 i, j, k ;
    T_byte8 arg[100] ;
    T_sword32 value ;
    T_word16 len ;

    p_args++ ;
    if (numArgs == 0)
        p_args++ ;

    for (i=0; i<numArgs; i++)  {
        j = 0 ;
        while ((*p_args != ',') && (*p_args != ')'))  {
            if (j==100)  {
                printf("Argument %d on line %d too long!\n",
                    i+1,
                    G_line) ;
                G_errors++ ;
                return ;
            }
            args[i][j++] = *(p_args++) ;
        }
        if ((p_args[0] == ')') && ((i+1) != numArgs))  {
            printf("Error!  Wrong number of arguments on line %d\n",
                G_line) ;
            G_errors++ ;
            return ;
        }
        args[i][j] = '\0' ;
        p_args++ ;
    }
    if (p_args[-1] != ')')  {
        printf("Error!  Wrong number of arguments on line %d\n",
             G_line) ;
        G_errors ++ ;
        return ;
    }

    for (i=0; i<numArgs; i++)  {
//printf("arg[%d] = '%s'\n", i, args[i]) ;
        strcpy(arg, args[i]) ;
        Strip(arg) ;
        if ((arg[0] == '-') || (isdigit(arg[0])))  {
            /* Arg is Number. */
            value = atol(arg) ;
            OutputNumber(value) ;
        } else if (arg[0] == '&')  {
            /* Event. */
            value = atol(arg+1) ;
            if (!((value >= 1) && (value <= 3)))  {
                printf("Error!  Event parameter (%d) out of range (1-3) on line %d!\n",
                    value,
                    G_line) ;
                G_errors++ ;
                return ;
            }
            OutputByte(SCRIPT_DATA_TYPE_EVENT_PARAMETER) ;
            OutputByte((T_byte8)value) ;
        } else if (arg[0] == QUOTE_START)  {
            len = strlen(arg)-2 ;
            if (len >= 80)  {
                printf("Error!  String too long on line %d!\n", G_line) ;
                G_errors++ ;
                return ;
            }
            OutputByte(SCRIPT_DATA_TYPE_STRING) ;
            OutputByte((T_byte8)len) ;
            for (k=0; k<len; k++)  {
                OutputByte(arg[1+k]) ;
            }
        } else {
            /* Try doing a flag name. */
            value = FindFlag(arg) ;
            if (value != -1)  {
                OutputByte(SCRIPT_DATA_TYPE_FLAG) ;
                OutputByte((T_byte8)value) ;
            } else {
                value = FindVar(arg) ;
                if (value != -1)  {
                    value = G_vars[value].number ;
                    OutputByte(SCRIPT_DATA_TYPE_VARIABLE) ;
                    /* Explicit little-endian shift/mask -- see
                       OutputNumber()'s comment for why. */
                    OutputByte((T_byte8)(value & 0xFF)) ;
                    OutputByte((T_byte8)((value >> 8) & 0xFF)) ;
                } else {
//printf("Searching for defnum: %s\n", arg) ;
                    value = FindDefnum(arg) ;
                    if (value != -1)  {
                        value = G_defnums[value].number ;
//printf("Output defnum: %s %ld\n", arg, value) ;
                        OutputNumber(value) ;
                    } else {
                        printf("Error!  Unknown argument '%s' on line %d\n",
                            arg,
                            G_line) ;
                        G_errors++ ;
                        return ;
                    }
                }
            }
        }
    }
}

T_void CompileCommand(T_byte8 *p_line)
{
    T_byte8 command[80] ;
    T_byte8 command2[80] ;
    T_byte8 *p_open ;
    T_sword16 commandNum ;

    sscanf(p_line, "%s", command) ;

    p_open = strstr(command, "(") ;
    if (p_open)
        *p_open = '\0' ;
    command2[0] = '\0' ;
    sscanf(command, "%s", command2) ;
    if (!command2[0])
         return ;
    commandNum = FindCommand(command2) ;
    if (commandNum == -1)  {
        printf("Error!  Unknown command '%s' on line %d\n", command2, G_line) ;
        exit(201) ;
    }
//    printf("command: <%s> = %d\n", command2, commandNum) ;
    OutputByte((T_byte8)commandNum) ;

    p_open = strstr(p_line, "(") ;
    if (!p_open)  {
        printf("Error!  Missing '(' on line %d\n", G_line) ;
        exit(202);
    }

    CompileArgs(p_open, G_commands[commandNum].numArgs) ;
}


T_void CompileDefvar(T_byte8 *p_defvar)
{
    T_byte8 var[80] ;
    T_sword32 value ;

    value = -1 ;
    sscanf(p_defvar+6, "%s%d", var, &value) ;

    if ((value < 0) || (value >= 256))  {
        printf("Defvar value %d out of range (0-255) on line %d\n",
            value,
            G_line) ;
        G_errors++ ;
    } else {
        AddVar(var, (T_word16)value) ;
    }
}

T_void CompileDefnum(T_byte8 *p_defnum)
{
    T_byte8 var[80] ;
    T_sword32 value ;

    sscanf(p_defnum+6, "%s%d", var, &value) ;

    AddDefnum(var, value) ;
}

T_void CompileEvent(T_byte8 *p_event)
{
    T_sword16 value ;

    p_event[strlen(p_event)-1] = '\0' ;
    value = FindEvent(p_event) ;
    if (value == -1)  {
        printf("Error!  Event '%s' unknown on line %d",
            p_event,
            G_line) ;
        G_errors++ ;
    } else {
        if (G_eventPlaces[value] == 0xFFFF)  {
            G_eventPlaces[value] = G_place ;
        } else {
            printf("Error!  Event '%s' already defined (line %d)\n",
                p_event,
                G_line) ;
            G_errors++ ;
        }
    }
}

T_void CompileFile(T_byte8 *p_filename)
{
    FILE *fp ;
    T_byte8 buffer[300] ;
    T_word16 place ;
    T_byte8 firstWord[80] ;

    printf("Compiling %s:\n", p_filename) ;

    fp = fopen(p_filename, "r") ;
    if (fp == NULL)  {
        printf("Cannot open file %s\n", p_filename) ;
        exit(2) ;
    }

    fgets(buffer, 300, fp) ;
    while (!feof(fp))  {
        StripComment(buffer) ;
printf("[%s]\n", buffer) ;
        if (buffer[0])  {
            if (isspace(buffer[0]))  {
                  /* Command. */
                  CompileCommand(buffer) ;
            } else if (isdigit(buffer[0])) {
                  /* place. */
                  place = atoi(buffer) ;
printf("place: %d\n", place) ;
                  if (place >= MAX_PLACES)  {
                      printf("Error!  Place %d too big on line %d\n",
                          place,
                          G_line) ;
                      G_errors++ ;
                  } else {
                      if (G_places[place] == 0xFFFF)  {
                          G_places[place] = G_place ;
                      } else {
                          printf("Error!  Place %d already taken (line %d)\n",
                              place,
                              G_line) ;
                          G_errors++ ;
                      }
                  }
            } else {
                  /* event or directive. */
                  firstWord[0] = '\0' ;
                  sscanf(buffer, "%s", firstWord) ;
                  if (strcmp(firstWord, "defvar")==0)  {
                      CompileDefvar(buffer) ;
                  } else if (strcmp(firstWord, "defnum") == 0)  {
                      CompileDefnum(buffer) ;
                  } else if (strcmp(firstWord, "include") == 0)  {
                      CompileInclude(buffer) ;
                  } else {
                      if (firstWord[strlen(firstWord)-1] == ':')  {
                          CompileEvent(firstWord) ;
                      } else {
                          printf("Error!  Unknown directive, line %d\n", G_line) ;
                          G_errors++ ;
                      }
                  }
            }
        }
        fgets(buffer, 300, fp) ;
        G_line++ ;
    }

    fclose(fp) ;
}

T_void CompileInclude(T_byte8 *p_include)
{
    T_byte8 name[80] ;

    sscanf(p_include+8, "%s", name) ;
    CompileFile(name) ;
}

T_word16 FindHighestEvent(T_void)
{
    T_word16 i ;
    T_word16 highest = 0 ;

    for (i=0; i<MAX_EVENT_PLACES; i++)  {
        if (G_eventPlaces[i] != 0xFFFF)
            highest = i+1 ;
    }

    return highest ;
}

T_word16 FindHighestPlace(T_void)
{
    T_word16 i ;
    T_word16 highest = 0 ;

    for (i=0; i<MAX_PLACES; i++)  {
        if (G_places[i] != 0xFFFF)
            highest = i+1 ;
    }

    return highest ;
}

T_void WriteLE16(FILE *fp, T_word16 v)
{
    fputc((int)(v & 0xFF), fp) ;
    fputc((int)((v >> 8) & 0xFF), fp) ;
}

T_void WriteLE32(FILE *fp, T_word32 v)
{
    fputc((int)(v & 0xFF), fp) ;
    fputc((int)((v >> 8) & 0xFF), fp) ;
    fputc((int)((v >> 16) & 0xFF), fp) ;
    fputc((int)((v >> 24) & 0xFF), fp) ;
}

T_void SaveFile(T_byte8 *p_filename)
{
    FILE *fp ;
    T_word16 highestEvent ;
    T_word16 highestPlace ;
    T_word16 i ;

    fp = fopen(p_filename, "wb") ;
    if (fp == NULL)  {
        printf("Cannot open file %s for output!", p_filename) ;
        exit(3) ;
    }

    highestEvent = FindHighestEvent() ;
    highestPlace = FindHighestPlace() ;

    /* The on-disk script header is a fixed 64-byte little-endian layout
       (T_scriptHeaderDisk32 in the game engine's Source/SCRIPT.C), read
       back field-by-field. It used to be produced here by fwriting this
       compiler's own in-memory header struct directly, which embedded
       live pointer fields whose size varies by host word width (4 bytes
       on a 32-bit host, 8 on 64-bit) and would silently change the
       on-disk header's size depending on what machine compiled this
       tool. All the pointer/state fields are always zero on disk; the
       engine fills them in itself when a script is loaded into memory. */
    WriteLE16(fp, highestEvent) ;
    WriteLE16(fp, highestPlace) ;
    WriteLE32(fp, (T_word32)G_place) ;   /* sizeCode */
    for (i = 0 ; i < 6 ; i++)
        WriteLE32(fp, 0) ;               /* reserved[6] */
    WriteLE32(fp, 0) ;                   /* number */
    WriteLE32(fp, 0) ;                   /* tag */
    WriteLE32(fp, 0) ;                   /* p_next */
    WriteLE32(fp, 0) ;                   /* p_prev */
    WriteLE32(fp, 0) ;                   /* lockCount */
    WriteLE32(fp, 0) ;                   /* p_code */
    WriteLE32(fp, 0) ;                   /* p_events */
    WriteLE32(fp, 0) ;                   /* p_places */

    fwrite(G_code, G_place, 1, fp) ;
    /* G_eventPlaces/G_places are in-memory T_word16 arrays -- like the
       header, these must go out little-endian regardless of the build
       host's own byte order, not as a raw host-native array dump. */
    for (i = 0 ; i < highestEvent ; i++)
        WriteLE16(fp, G_eventPlaces[i]) ;
    for (i = 0 ; i < highestPlace ; i++)
        WriteLE16(fp, G_places[i]) ;
    fclose(fp) ;
}

int main(int argc, char *argv[])
{
    puts("\n------- Script Compiler version 0.1 -- LesInk Productions (C) 1996") ;
    if (argc != 3)  {
        printf("USAGE:  SC <script file> <output file>\n") ;
        exit(1) ;
    }
    Initialize() ;
    AddVar("Self", 0x8000) ;
    AddVar("Time", 0x8001) ;
    CompileFile(argv[1]) ;
    if (G_errors == 0)
        SaveFile(argv[2]) ;
    else
        printf("%d errors.\n", G_errors) ;
    puts("Done.") ;
    return (G_errors == 0) ? 0 : 1 ;
}
