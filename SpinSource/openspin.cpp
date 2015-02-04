///////////////////////////////////////////////////////////////
//                                                           //
// Propeller Spin/PASM Compiler Command Line Tool 'OpenSpin' //
// (c)2012-2013 Parallax Inc. DBA Parallax Semiconductor.    //
// Adapted from Jeff Martin's Delphi code by Roy Eltham      //
// See end of file for terms of use.                         //
//                                                           //
///////////////////////////////////////////////////////////////
//
// openspin.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../PropellerCompiler/PropellerCompiler.h"
#include "objectheap.h"
#include "pathentry.h"
#include "textconvert.h"
#include "preprocess.h"

#define ObjFileStackLimit   16

#define ListLimit           2000000
#define DocLimit            2000000

#define MAX_FILES           2048    // an object can only reference 32 other objects and only 32 dat files, so the worst case is 32*32*2 files

static struct preprocess s_preprocessor;
static CompilerData* s_pCompilerData = NULL;
static bool s_bUsePreprocessor = false;
static bool s_bAlternatePreprocessorMode  = false;
static int  s_nObjStackPtr = 0;
static int  s_nFilesAccessed = 0;
static char s_filesAccessed[MAX_FILES][PATH_MAX];

static void Banner(void)
{
    fprintf(stdout, "Propeller Spin/PASM Compiler \'OpenSpin\' (c)2012-2014 Parallax Inc. DBA Parallax Semiconductor.\n");
    fprintf(stdout, "Version 1.00.71 Compiled on %s %s\n",__DATE__, __TIME__);
}

/* Usage - display a usage message and exit */
static void Usage(void)
{
    Banner();
    fprintf(stderr, "\
usage: openspin\n\
         [ -h ]                 display this help\n\
         [ -L or -I <path> ]    add a directory to the include path\n\
         [ -o <path> ]          output filename\n\
         [ -b ]                 output binary file format\n\
         [ -e ]                 output eeprom file format\n\
         [ -c ]                 output only DAT sections\n\
         [ -d ]                 dump out doc mode\n\
         [ -t ]                 output just the object file tree\n\
         [ -f ]                 output a list of filenames for use in archiving\n\
         [ -q ]                 quiet mode (suppress banner and non-error text)\n\
         [ -v ]                 verbose output\n\
         [ -p ]                 disable the preprocessor\n\
         [ -a ]                 use alternative preprocessor rules\n\
         [ -D <define> ]        add a define\n\
         [ -M <size> ]          size of eeprom (up to 16777216 bytes)\n\
         [ -s ]                 dump PUB & CON symbol information for top object\n\
         <name.spin>            spin file to compile\n\
\n");
}

FILE* OpenFileInPath(const char *name, const char *mode)
{
    const char* pTryPath = NULL;

    FILE* file = fopen(name, mode);
    if (!file)
    {
        PathEntry* entry = NULL;
        while(!file)
        {
            pTryPath = MakeNextPath(&entry, name);
            if (pTryPath)
            {
                file = fopen(pTryPath, mode);
                if (file != NULL)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (s_nFilesAccessed < MAX_FILES)
    {
        if (!pTryPath)
        {
#ifdef WIN32
            if (_fullpath(s_filesAccessed[s_nFilesAccessed], name, PATH_MAX) == NULL)
#else
            if (realpath(name, s_filesAccessed[s_nFilesAccessed]) == NULL)
#endif
            {
                strcpy(s_filesAccessed[s_nFilesAccessed], name);
            }
            s_nFilesAccessed++;
        }
        else
        {
            strcpy(s_filesAccessed[s_nFilesAccessed++], pTryPath);
        }
    }
    else
    {
        // should never hit this, but just in case
        printf("Too many files!\n");
        exit(-2);
    }

    return file;
}

// returns NULL if the file failed to open or is 0 length
char* LoadFile(char* pFilename, int* pnLength)
{
    char* pBuffer = 0;

    FILE* pFile = OpenFileInPath(pFilename, "rb");
    if (pFile != NULL)
    {
        if (s_bUsePreprocessor)
        {
            pp_push_file_struct(&s_preprocessor, pFile, pFilename);
            pp_run(&s_preprocessor);
            pBuffer = pp_finish(&s_preprocessor);
            *pnLength = strlen(pBuffer);
            if (*pnLength == 0)
            {
                free(pBuffer);
                pBuffer = NULL;
            }
        }
        else
        {
            // get the length of the file by seeking to the end and using ftell
            fseek(pFile, 0, SEEK_END);
            *pnLength = ftell(pFile);

            if (*pnLength > 0)
            {
                pBuffer = (char*)malloc(*pnLength+1); // allocate a buffer that is the size of the file plus one char
                pBuffer[*pnLength] = 0; // set the end of the buffer to 0 (null)

                // seek back to the beginning of the file and read it in
                fseek(pFile, 0, SEEK_SET);
                fread(pBuffer, 1, *pnLength, pFile);
            }
        }
        fclose(pFile);
    }

    return pBuffer;
}

int GetData(unsigned char* pDest, char* pFileName, int nMaxSize)
{
    int nBytesRead = 0;
    FILE* pFile = OpenFileInPath(pFileName, "rb");

    if (pFile)
    {
        if (pDest)
        {
            nBytesRead = (int)fread(pDest, 1, nMaxSize, pFile);
        }
        fclose(pFile);
    }
    else
    {
        printf("Cannot find/open dat file: %s \n", pFileName);
        return -1;
    }

    return nBytesRead;
}

bool GetPASCIISource(char* pFilename)
{
    // read in file to temp buffer, convert to PASCII, and assign to s_pCompilerData->source
    int nLength = 0;
    char* pBuffer = LoadFile(pFilename, &nLength);
    if (pBuffer)
    {
        char* pPASCIIBuffer = new char[nLength+1];
        if (!UnicodeToPASCII(pBuffer, nLength, pPASCIIBuffer, s_bUsePreprocessor))
        {
            printf("Unrecognized text encoding format!\n");
            delete [] pPASCIIBuffer;
            free(pBuffer);
            return false;
        }

        // clean up any previous buffer
        if (s_pCompilerData->source)
        {
            delete [] s_pCompilerData->source;
        }

        s_pCompilerData->source = pPASCIIBuffer;

        free(pBuffer);
    }
    else
    {
        s_pCompilerData->source = NULL;
        return false;
    }

    return true;
}

void PrintError(const char* pFilename, const char* pErrorString)
{
    int lineNumber = 1;
    int column = 1;
    int offsetToStartOfLine = -1;
    int offsetToEndOfLine = -1;
    int offendingItemStart = 0;
    int offendingItemEnd = 0;
    GetErrorInfo(lineNumber, column, offsetToStartOfLine, offsetToEndOfLine, offendingItemStart, offendingItemEnd);

    printf("%s(%d:%d) : error : %s\n", pFilename, lineNumber, column, pErrorString);

    char errorItem[512];
    char errorLine[512];
    if ( offendingItemStart == offendingItemEnd && s_pCompilerData->source[offendingItemStart] == 0 )
    {
        strcpy(errorLine, "End Of File");
        strcpy(errorItem, "N/A");
    }
    else
    {
        strncpy(errorLine, &s_pCompilerData->source[offsetToStartOfLine], offsetToEndOfLine - offsetToStartOfLine);
        errorLine[offsetToEndOfLine - offsetToStartOfLine] = 0;

        strncpy(errorItem, &s_pCompilerData->source[offendingItemStart], offendingItemEnd - offendingItemStart);
        errorItem[offendingItemEnd - offendingItemStart] = 0;
    }

    printf("Line:\n%s\nOffending Item: %s\n", errorLine, errorItem);
}

bool CompileRecursively(char* pFilename, bool bQuiet, bool bFileTreeOutputOnly)
{
    if (s_nObjStackPtr > 0 && (!bQuiet || bFileTreeOutputOnly))
    {
        char spaces[] = "                              \0";
        printf("%s|-%s\n", &spaces[32-(s_nObjStackPtr<<1)], pFilename);
    }
    s_nObjStackPtr++;
    if (s_nObjStackPtr > ObjFileStackLimit)
    {
        printf("%s : error : Object nesting exceeds limit of %d levels.\n", pFilename, ObjFileStackLimit);
        return false;
    }

    void *definestate = 0;
    if (s_bUsePreprocessor)
    {
        definestate = pp_get_define_state(&s_preprocessor);
    }
    if (!GetPASCIISource(pFilename))
    {
        printf("%s : error : Can not find/open file.\n", pFilename);
        return false;
    }

    // first pass on object
    const char* pErrorString = Compile1();
    if (pErrorString != 0)
    {
        PrintError(pFilename, pErrorString);
        return false;
    }

    if (s_pCompilerData->obj_files > 0)
    {
        char filenames[file_limit*256];

        int numObjects = s_pCompilerData->obj_files;
        for (int i = 0; i < numObjects; i++)
        {
            // copy the obj filename appending .spin if it doesn't have it.
            strcpy(&filenames[i<<8], &(s_pCompilerData->obj_filenames[i<<8]));
            if (strstr(&filenames[i<<8], ".spin") == NULL)
            {
                strcat(&filenames[i<<8], ".spin");
            }
        }

        for (int i = 0; i < numObjects; i++)
        {
            if (!CompileRecursively(&filenames[i<<8], bQuiet, bFileTreeOutputOnly))
            {
                return false;
            }
        }

        // redo first pass on parent object
        if (s_bUsePreprocessor)
        {
            // undo any defines in sub-objects
            pp_restore_define_state(&s_preprocessor, definestate);
        }
        if (!GetPASCIISource(pFilename))
        {
            printf("%s : error : Can not find/open file.\n", pFilename);
            return false;
        }

        pErrorString = Compile1();
        if (pErrorString != 0)
        {
            PrintError(pFilename, pErrorString);
            return false;
        }

        if (!CopyObjectsFromHeap(s_pCompilerData, filenames))
        {
            printf("%s : error : Object files exceed 128k.\n", pFilename);
            return false;
        }
    }

    // load all DAT files
    if (s_pCompilerData->dat_files > 0)
    {
        int p = 0;
        for (int i = 0; i < s_pCompilerData->dat_files; i++)
        {
            // Get DAT's Files

            // Get name information
            char filename[256];
            strcpy(&filename[0], &(s_pCompilerData->dat_filenames[i<<8]));

            // Load file and add to dat_data buffer
            s_pCompilerData->dat_lengths[i] = GetData(&(s_pCompilerData->dat_data[p]), &filename[0], data_limit - p);
            if (s_pCompilerData->dat_lengths[i] == -1)
            {
                s_pCompilerData->dat_lengths[i] = 0;
                return false;
            }
            if (p + s_pCompilerData->dat_lengths[i] > data_limit)
            {
                printf("%s : error : Object files exceed 128k.\n", pFilename);
                return false;
            }
            s_pCompilerData->dat_offsets[i] = p;
            p += s_pCompilerData->dat_lengths[i];
        }
    }

    // second pass of object
    pErrorString = Compile2();
    if (pErrorString != 0)
    {
        PrintError(pFilename, pErrorString);
        return false;
    }

    // Check to make sure object fits into 32k (or eeprom size if specified as larger than 32k)
    unsigned int i = 0x10 + s_pCompilerData->psize + s_pCompilerData->vsize + (s_pCompilerData->stack_requirement << 2);
    if ((s_pCompilerData->compile_mode == 0) && (i > s_pCompilerData->eeprom_size))
    {
        printf("%s : error : Object exceeds runtime memory limit by %d longs.\n", pFilename, (i - s_pCompilerData->eeprom_size) >> 2);
        return false;
    }

    // save this object in the heap
    if (!AddObjectToHeap(pFilename, s_pCompilerData))
    {
        printf("%s : error : Object Heap Overflow.\n", pFilename);
        return false;
    }
    s_nObjStackPtr--;

    return true;
}

bool ComposeRAM(unsigned char** ppBuffer, int& bufferSize, bool bDATonly, bool bBinary, unsigned int eeprom_size)
{
    if (!bDATonly)
    {
        unsigned int varsize = s_pCompilerData->vsize;                                                // variable size (in bytes)
        unsigned int codsize = s_pCompilerData->psize;                                                // code size (in bytes)
        unsigned int pubaddr = *((unsigned short*)&(s_pCompilerData->obj[8]));                        // address of first public method
        unsigned int publocs = *((unsigned short*)&(s_pCompilerData->obj[10]));                       // number of stack variables (locals), in bytes, for the first public method
        unsigned int pbase = 0x0010;                                                                  // base of object code
        unsigned int vbase = pbase + codsize;                                                         // variable base = object base + code size
        unsigned int dbase = vbase + varsize + 8;                                                     // data base = variable base + variable size + 8
        unsigned int pcurr = pbase + pubaddr;                                                         // Current program start = object base + public address (first public method)
        unsigned int dcurr = dbase + 4 + (s_pCompilerData->first_pub_parameters << 2) + publocs;      // current data stack pointer = data base + 4 + FirstParams*4 + publocs

        if (bBinary)
        {
           // reset ram
           *ppBuffer = new unsigned char[vbase];
           memset(*ppBuffer, 0, vbase);
           bufferSize = vbase;
        }
        else
        {
           if (vbase + 8 > eeprom_size)
           {
              printf("ERROR: eeprom size exceeded by %d longs.\n", (vbase + 8 - eeprom_size) >> 2);
              return false;
           }
           // reset ram
           *ppBuffer = new unsigned char[eeprom_size];
           memset(*ppBuffer, 0, eeprom_size);
           bufferSize = eeprom_size;
           (*ppBuffer)[dbase-8] = 0xFF;
           (*ppBuffer)[dbase-7] = 0xFF;
           (*ppBuffer)[dbase-6] = 0xF9;
           (*ppBuffer)[dbase-5] = 0xFF;
           (*ppBuffer)[dbase-4] = 0xFF;
           (*ppBuffer)[dbase-3] = 0xFF;
           (*ppBuffer)[dbase-2] = 0xF9;
           (*ppBuffer)[dbase-1] = 0xFF;
        }

        // set clock frequency and clock mode
        *((int*)&((*ppBuffer)[0])) = s_pCompilerData->clkfreq;
        (*ppBuffer)[4] = s_pCompilerData->clkmode;

        // set interpreter parameters
        ((unsigned short*)&((*ppBuffer)[4]))[1] = (unsigned short)pbase;         // always 0x0010
        ((unsigned short*)&((*ppBuffer)[4]))[2] = (unsigned short)vbase;
        ((unsigned short*)&((*ppBuffer)[4]))[3] = (unsigned short)dbase;
        ((unsigned short*)&((*ppBuffer)[4]))[4] = (unsigned short)pcurr;
        ((unsigned short*)&((*ppBuffer)[4]))[5] = (unsigned short)dcurr;

        // set code
        memcpy(&((*ppBuffer)[pbase]), &(s_pCompilerData->obj[4]), codsize);

        // install ram checksum byte
        unsigned char sum = 0;
        for (unsigned int i = 0; i < vbase; i++)
        {
          sum = sum + (*ppBuffer)[i];
        }
        (*ppBuffer)[5] = (unsigned char)((-(sum+2028)) );
    }
    else
    {
        unsigned int objsize = *((unsigned short*)&(s_pCompilerData->obj[4]));
        if (s_pCompilerData->psize > 65535)
        {
            objsize = s_pCompilerData->psize;
        }
        unsigned int size = objsize - 4 - (s_pCompilerData->obj[7] * 4);
        *ppBuffer = new unsigned char[size];
        bufferSize = size;
        memcpy(&((*ppBuffer)[0]), &(s_pCompilerData->obj[8 + (s_pCompilerData->obj[7] * 4)]), size);
    }

    return true;
}

void CleanupMemory()
{
    // cleanup
    if ( s_pCompilerData )
    {
        delete [] s_pCompilerData->list;
        delete [] s_pCompilerData->doc;
        delete [] s_pCompilerData->obj;
        delete [] s_pCompilerData->source;
    }
    CleanObjectHeap();
    CleanupPathEntries();
    Cleanup();
}

int main(int argc, char* argv[])
{
    char* infile = NULL;
    char* outfile = NULL;
    char* p = NULL;
    bool bVerbose = false;
    bool bQuiet = false;
    bool bDocMode = false;
    bool bDATonly = false;
    bool bBinary  = true;
    unsigned int  eeprom_size = 32768;
    s_bUsePreprocessor = true;
    s_bAlternatePreprocessorMode = false;
    s_nObjStackPtr = 0;
    s_nFilesAccessed = 0;
    s_pCompilerData = NULL;
    bool bFileTreeOutputOnly = false;
    bool bFileListOutputOnly = false;
    bool bDumpSymbols = false;

    // go through the command line arguments, skipping over any -D
    for(int i = 1; i < argc; i++)
    {
        // handle switches
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
            case 'I':
            case 'L':
                if(argv[i][2])
                {
                    p = &argv[i][2];
                }
                else if(++i < argc)
                {
                    p = argv[i];
                }
                else
                {
                    Usage();
                    CleanupMemory();
                    return 1;
                }
                AddPath(p);
                break;

            case 'M':
                if (argv[i][2])
                {
                    p = &argv[i][2];
                }
                else if(++i < argc)
                {
                    p = argv[i];
                }
                else
                {
                    Usage();
                    CleanupMemory();
                    return 1;
                }
                sscanf(p, "%d", &eeprom_size);
                if (eeprom_size > 16777216)
                {
                    Usage();
                    CleanupMemory();
                    return 1;
                }
                break;

            case 'o':
                if(argv[i][2])
                {
                    outfile = &argv[i][2];
                }
                else if(++i < argc)
                {
                    outfile = argv[i];
                }
                else
                {
                    Usage();
                    CleanupMemory();
                    return 1;
                }
                break;

            case 'p':
                s_bUsePreprocessor = false;
                break;

            case 'a':
                s_bAlternatePreprocessorMode = true;
                break;

            case 'D':
                if (s_bUsePreprocessor)
                {
                    if (argv[i][2])
                    {
                        p = &argv[i][2];
                    }
                    else if(++i < argc)
                    {
                        p = argv[i];
                    }
                    else
                    {
                        Usage();
                        CleanupMemory();
                        return 1;
                    }
                    // just skipping these for now
                }
                else
                {
                    Usage();
                    CleanupMemory();
                    return 1;
                }
                break;

            case 't':
                bFileTreeOutputOnly = true;
                break;

            case 'f':
                bFileListOutputOnly = true;
                break;

            case 'b':
                bBinary = true;
                break;

            case 'c':
                bDATonly = true;
                break;

            case 'd':
                bDocMode = true;
                break;

            case 'e':
                bBinary = false;
                break;

            case 'q':
                bQuiet = true;
                break;

            case 'v':
                bVerbose = true;
                break;

            case 's':
                bDumpSymbols = true;
                break;

            case 'h':
            default:
                Usage();
                CleanupMemory();
                return 1;
                break;
            }
        }
        else // handle the input filename
        {
            if (infile)
            {
                Usage();
                CleanupMemory();
                return 1;
            }
            infile = argv[i];
        }
    }

    // must have input file
    if (!infile)
    {
        Usage();
        CleanupMemory();
        return 1;
    }

    if (bFileTreeOutputOnly || bFileListOutputOnly || bDumpSymbols)
    {
        bQuiet = true;
    }

    if (s_bUsePreprocessor)
    {
        pp_init(&s_preprocessor, s_bAlternatePreprocessorMode);

        // go through the command line arguments again, this time only processing -D
        for(int i = 1; i < argc; i++)
        {
            // handle switches
            if(argv[i][0] == '-')
            {
                if (argv[i][1] == 'D')
                {
                    if (s_bUsePreprocessor)
                    {
                        if (argv[i][2])
                        {
                            p = &argv[i][2];
                        }
                        else if(++i < argc)
                        {
                            p = argv[i];
                        }
                        else
                        {
                            Usage();
                            CleanupMemory();
                            return 1;
                        }
                        // add any predefined symbols here - note that when using the 
                        // "alternate" rules, these symbols have a null value - i.e.
                        // they are just "defined", but are not used in macro substitution
                        pp_define(&s_preprocessor, p, (s_bAlternatePreprocessorMode ? "" : "1"));
                    }
                    else
                    {
                        Usage();
                        CleanupMemory();
                        return 1;
                    }
                }
            }
        }

        // add symbols with predefined values here
        pp_define(&s_preprocessor, "__SPIN__", "1");
        pp_define(&s_preprocessor, "__TARGET__", "P1");

        pp_setcomments(&s_preprocessor, "\'", "{", "}");
    }

    // finish the include path
    AddFilePath(infile);

    char outputFilename[256];
    if (!outfile)
    {
        // create *.binary filename from user passed in spin filename
        strcpy(&outputFilename[0], infile);
        const char* pTemp = strstr(&outputFilename[0], ".spin");
        if (pTemp == 0)
        {
            printf("ERROR: spinfile must have .spin extension. You passed in: %s\n", infile);
            Usage();
            CleanupMemory();
            return 1;
        }
        else
        {
            int offset = pTemp - &outputFilename[0];
            outputFilename[offset+1] = 0;
            if (bDATonly)
            {
                strcat(&outputFilename[0], "dat");
            }
            else if (bBinary)
            {
                strcat(&outputFilename[0], "binary");
            }
            else 
            {
                strcat(&outputFilename[0], "eeprom");
            }
        }
    }
    else // use filename specified with -o
    {
        strcpy(outputFilename, outfile);
    }

    if (!bQuiet)
    {
        Banner();
        printf("Compiling...\n%s\n", infile);
    }

    if (bFileTreeOutputOnly)
    {
        printf("%s\n", infile);
    }

    s_pCompilerData = InitStruct();

    s_pCompilerData->list = new char[ListLimit];
    s_pCompilerData->list_limit = ListLimit;
    memset(s_pCompilerData->list, 0, ListLimit);

    if (bDocMode && !bQuiet && !bDATonly)
    {
        s_pCompilerData->doc = new char[DocLimit];
        s_pCompilerData->doc_limit = DocLimit;
        memset(s_pCompilerData->doc, 0, DocLimit);
    }
    else
    {
        s_pCompilerData->doc = 0;
        s_pCompilerData->doc_limit = 0;
    }
    s_pCompilerData->bDATonly = bDATonly;
    s_pCompilerData->bBinary = bBinary;
    s_pCompilerData->eeprom_size = eeprom_size;

    // allocate space for obj based on eeprom size command line option
    s_pCompilerData->obj_limit = eeprom_size > min_obj_limit ? eeprom_size : min_obj_limit;
    s_pCompilerData->obj = new unsigned char[s_pCompilerData->obj_limit];

    // copy filename into obj_title, and chop off the .spin
    strcpy(s_pCompilerData->obj_title, infile);
    char* pExtension = strstr(&s_pCompilerData->obj_title[0], ".spin");
    if (pExtension != 0)
    {
        *pExtension = 0;
    }

    if (!CompileRecursively(infile, bQuiet, bFileTreeOutputOnly))
    {
        CleanupMemory();
        return 1;
    }

    if (!bQuiet)
    {
        printf("Done.\n");
    }

    if (!bFileTreeOutputOnly && !bFileListOutputOnly && !bDumpSymbols)
    {
        unsigned char* pBuffer = NULL;
        int bufferSize = 0;
        if (ComposeRAM(&pBuffer, bufferSize, bDATonly, bBinary, eeprom_size))
        {
            FILE* pFile = fopen(outputFilename, "wb");
            if (pFile)
            {
                fwrite(pBuffer, bufferSize, 1, pFile);
                fclose(pFile);
            }
        }
        else
        {
            CleanupMemory();
            return 1;
        }

        if (!bQuiet)
        {
           printf("Program size is %d bytes\n", bufferSize);
        }

        delete [] pBuffer;
    }

    if (bDumpSymbols)
    {
        for (int i = 0; i < s_pCompilerData->info_count; i++)
        {
            char szTemp[256];
            szTemp[0] = '*';
            szTemp[1] = 0;
            int length = 0;
            int start = 0;
            if (s_pCompilerData->info_type[i] == info_pub || s_pCompilerData->info_type[i] == info_pri)
            {
                length = s_pCompilerData->info_data3[i] - s_pCompilerData->info_data2[i];
                start = s_pCompilerData->info_data2[i];
            }
            else if (s_pCompilerData->info_type[i] != info_dat && s_pCompilerData->info_type[i] != info_dat_symbol)
            {
                length = s_pCompilerData->info_finish[i] - s_pCompilerData->info_start[i];
                start = s_pCompilerData->info_start[i];
            }

            if (length > 0 && length < 256)
            {
                strncpy(szTemp, &s_pCompilerData->source[start], length);
                szTemp[length] = 0;
            }

            switch(s_pCompilerData->info_type[i])
            {
                case info_con:
                    printf("CON, %s, %d\n", szTemp, s_pCompilerData->info_data0[i]);
                    break;
                case info_con_float:
                    printf("CONF, %s, %f\n", szTemp, *((float*)&(s_pCompilerData->info_data0[i])));
                    break;
                case info_pub_param:
                    {
                        char szTemp2[256];
                        szTemp2[0] = '*';
                        szTemp2[1] = 0;
                        length = s_pCompilerData->info_data3[i] - s_pCompilerData->info_data2[i];
                        start = s_pCompilerData->info_data2[i];
                        if (length > 0 && length < 256)
                        {
                            strncpy(szTemp2, &s_pCompilerData->source[start], length);
                            szTemp2[length] = 0;
                        }
                        printf("PARAM, %s, %s, %d, %d\n", szTemp2, szTemp, s_pCompilerData->info_data0[i], s_pCompilerData->info_data1[i]);
                    }
                    break;
                case info_pub:
                    printf("PUB, %s, %d, %d\n", szTemp, s_pCompilerData->info_data4[i] & 0xFFFF, s_pCompilerData->info_data4[i] >> 16);
                    break;
            }
        }
    }

    if (bFileListOutputOnly)
    {
        for (int i = 0; i < s_nFilesAccessed; i++)
        {
            for (int j = i+1; j < s_nFilesAccessed; j++)
            {
                if (strcmp(s_filesAccessed[i], s_filesAccessed[j]) == 0)
                {
                    s_filesAccessed[j][0] = 0;
                }
            }
        }

        for (int i = 0; i < s_nFilesAccessed; i++)
        {
            if (s_filesAccessed[i][0] != 0)
            {
                printf("%s\n", s_filesAccessed[i]);
            }
        }
    }

    if (bVerbose && !bQuiet && !bDATonly)
    {
        // do stuff with list and/or doc here
        int listOffset = 0;
        while (listOffset < s_pCompilerData->list_length)
        {
            char* pTemp = strstr(&(s_pCompilerData->list[listOffset]), "\r");
            if (pTemp)
            {
                *pTemp = 0;
            }
            printf("%s\n", &(s_pCompilerData->list[listOffset]));
            if (pTemp)
            {
                *pTemp = 0x0D;
                listOffset += (pTemp - &(s_pCompilerData->list[listOffset])) + 1;
            }
            else
            {
                listOffset += strlen(&(s_pCompilerData->list[listOffset]));
            }
        }
    }

    if (bDocMode && !bQuiet && !bDATonly)
    {
        // do stuff with list and/or doc here
        int docOffset = 0;
        while (docOffset < s_pCompilerData->doc_length)
        {
            char* pTemp = strstr(&(s_pCompilerData->doc[docOffset]), "\r");
            if (pTemp)
            {
                *pTemp = 0;
            }
            printf("%s\n", &(s_pCompilerData->doc[docOffset]));
            if (pTemp)
            {
                *pTemp = 0x0D;
                docOffset += (pTemp - &(s_pCompilerData->doc[docOffset])) + 1;
            }
            else
            {
                docOffset += strlen(&(s_pCompilerData->doc[docOffset]));
            }
        }
    }

    CleanupMemory();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//                           TERMS OF USE: MIT License                                   //
///////////////////////////////////////////////////////////////////////////////////////////
// Permission is hereby granted, free of charge, to any person obtaining a copy of this  //
// software and associated documentation files (the "Software"), to deal in the Software //
// without restriction, including without limitation the rights to use, copy, modify,    //
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    //
// permit persons to whom the Software is furnished to do so, subject to the following   //
// conditions:                                                                           //
//                                                                                       //
// The above copyright notice and this permission notice shall be included in all copies //
// or substantial portions of the Software.                                              //
//                                                                                       //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   //
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         //
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    //
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     //
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                //
///////////////////////////////////////////////////////////////////////////////////////////
