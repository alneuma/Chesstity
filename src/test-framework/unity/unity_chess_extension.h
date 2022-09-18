#ifndef UNITY_CHESS_EXTENSION_H
#define UNITY_CHESS_EXTENSION_H

#define UNITY_TEST_ASSERT_EQUAL_STRING_CHESS(expected, actual, line, message)                          UnityAssertEqualStringChess((const char*)(expected), (const char*)(actual), (message), (UNITY_LINE_TYPE)(line))

#define TEST_ASSERT_EQUAL_STRING_CHESS(expected, actual)                                                 UNITY_TEST_ASSERT_EQUAL_STRING_CHESS((expected), (actual), __LINE__, NULL)

void UnityAssertEqualStringChess(const char* expected,
                                 const char* actual,
                                 const char* msg,
                                 const UNITY_LINE_TYPE lineNumber);

void UnityPrintExpectedAndActualStringsChess(const char* expected, const char* actual);

#endif
