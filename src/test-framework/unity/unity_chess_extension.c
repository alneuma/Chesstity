#include "unity.h"
#include "unity_internals.h"
#include "unity_chess_extension.h"
#include <stdio.h>

/* start of copied directives from unity.c*/
#ifndef UNITY_PROGMEM
#define UNITY_PROGMEM
#endif

/* If omitted from header, declare overrideable prototypes here so they're ready for use */
#ifdef UNITY_OMIT_OUTPUT_CHAR_HEADER_DECLARATION
void UNITY_OUTPUT_CHAR(int);
#endif

/* Helpful macros for us to use here in Assert functions */
#define UNITY_FAIL_AND_BAIL         do { Unity.CurrentTestFailed  = 1; UNITY_OUTPUT_FLUSH(); TEST_ABORT(); } while (0)
#define UNITY_IGNORE_AND_BAIL       do { Unity.CurrentTestIgnored = 1; UNITY_OUTPUT_FLUSH(); TEST_ABORT(); } while (0)
#define RETURN_IF_FAIL_OR_IGNORE    do { if (Unity.CurrentTestFailed || Unity.CurrentTestIgnored) { TEST_ABORT(); } } while (0)
/* end of copied directives from unity.c*/

static void UnityPrintExpectedAndActualStringsChess(const char* expected, const char* actual);
static void UnityTestResultsFailBegin(const UNITY_LINE_TYPE line);  // copied from unity.c
static void UnityAddMsgIfSpecified(const char* msg); // copied from unity.c

/*-----------------------------------------------*/
void UnityAssertEqualStringChess(const char* expected,
                                 const char* actual,
                                 const char* msg,
                                 const UNITY_LINE_TYPE lineNumber)
{
    UNITY_UINT32 i;

    RETURN_IF_FAIL_OR_IGNORE;

    /* if both pointers not null compare the strings */
    if (expected && actual)
    {
        for (i = 0; expected[i] || actual[i]; i++)
        {
            if (expected[i] != actual[i])
            {
                Unity.CurrentTestFailed = 1;
                break;
            }
        }
    }
    else
    { /* handle case of one pointers being null (if both null, test should pass) */
        if (expected != actual)
        {
            Unity.CurrentTestFailed = 1;
        }
    }

    if (Unity.CurrentTestFailed)
    {
        UnityTestResultsFailBegin(lineNumber);
        UnityPrintExpectedAndActualStringsChess(expected, actual);
        UnityAddMsgIfSpecified(msg);
        UNITY_FAIL_AND_BAIL;
    }
}

/*-----------------------------------------------*/
static void UnityPrintExpectedAndActualStringsChess(const char* expected, const char* actual)
{
    printf("\nExpected:     Was:\n");
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
            putchar(*expected++);

        printf("      ");

        for (j = 0; j < 8; j++)
            putchar(*actual++);
    
        putchar('\n');
    }
}
