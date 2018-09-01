#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define _CRT_SECURE_NO_WARNINGS

const int DEF_MAX_SIZE = 300;
const int DEF_MIN_SIZE = 10;
const int DEF_BUF_SIZE = 128;

class TProcessor {
private:
    HANDLE file_handle;
    char* path_to_file;
    int buf_size;
    bool file_opened;
    int min_file_size;
    int max_file_size;

    void PrintHelp();
    bool SetNewFile(char* path);
    int CountLines();
    int CountChar();
    char* ReadLine(int numb);

    typedef struct {
        int line;
        int pos;
    } TPair;

    typedef struct {
        TPair* body;
        int size;
    } TSubstr;

    TSubstr* SearchSubstring(char* substr, bool registr);
    bool Replace(char* old_w, char* new_w);
    int ReplaceAll(char* old_w, char* new_w);

public:
    TProcessor();
    ~TProcessor();
    void StartProcessor(int argc, char** argv);
    void InteractiveMode();
    void CommandMode(int argc, char** argv);

};

TProcessor::TProcessor() {
    file_handle = NULL;
    path_to_file = NULL;
    file_opened = false;
    buf_size = DEF_BUF_SIZE;
    min_file_size = DEF_MIN_SIZE;
    max_file_size = DEF_MAX_SIZE;
}

TProcessor::~TProcessor() {
    CloseHandle(file_handle);
    if (path_to_file != NULL)
        free(path_to_file);
}

void TProcessor::StartProcessor(int argc, char** argv) {
    argc == 1 ? InteractiveMode() : CommandMode(argc, argv);
}

void TProcessor::CommandMode(int argc, char** argv) {
    int n = 1;
    while (n < argc) {
        if (strcmp(argv[n], "SetNewFile") == 0 && (argc - n) >= 1) {
            printf(">> %s:\t\t", argv[n]);
            SetNewFile(argv[n + 1]) ? printf("Ok\n") : printf("");
            n += 2;
        }
        else if (strcmp(argv[n], "CountLines") == 0) {
            printf(">> %s:\t\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                ++n;
                continue;
            }
            printf("Summary lines %d\n", CountLines());
            ++n;
        }
        else if (strcmp(argv[n], "CountChar") == 0) {
            printf(">> %s:\t\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                ++n;
                continue;
            }
            printf("Summary char: %d\n", CountChar());
            ++n;
        }
        else if (strcmp(argv[n], "ReadLine") == 0 && (argc - n) >= 1) {
            printf(">> %s:\t\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                n += 2;
                continue;
            }
            printf("<---- Read %d line ---->\n", atoi(argv[n + 1]));
            printf("\t\t\t%s\n", ReadLine(atoi(argv[n + 1])));
            n += 2;
        }
        else if (strcmp(argv[n], "SearchSubstring") == 0 && (argc - n) >= 2) {
            printf(">> %s:\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                n += 3;
                continue;
            }
            if (strlen(argv[n + 1]) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n\t\t\t");
                buf_size = DEF_BUF_SIZE;
            }
            TSubstr* res = SearchSubstring(argv[n + 1], atoi(argv[n + 2]));
            if (res->size == 0) {
                printf("There are no substring that you request\n");
            }
            else {
                printf("<---- Match substring ---->\n");
                for (int i = 0; i < res->size; ++i)
                    printf("\t\t\tLine %d position %d\n", res->body[i].line, res->body[i].pos);
            }
            free(res);
            n += 3;
        }
        else if (strcmp(argv[n], "Replace") == 0 && (argc - n) >= 2) {
            printf(">> %s:\t\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                n += 3;
                continue;
            }
            if (strlen(argv[n + 1]) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n\t\t\t");
                buf_size = DEF_BUF_SIZE;
            }
            Replace(argv[n + 1], argv[n + 2]) ? printf("Ok\n") : printf("There are no words to replace\n");
            n += 3;
        }
        else if (strcmp(argv[n], "ReplaceAll") == 0 && (argc - n) >= 2) {
            printf(">> %s:\t\t", argv[n]);
            if (!file_opened) {
                printf("I can't do \"%s\". First you have to open file\n", argv[n]);
                n += 3;
                continue;
            }
            if (strlen(argv[n + 1]) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n\t\t\t");
                buf_size = DEF_BUF_SIZE;
            }
            int k = ReplaceAll(argv[n + 1], argv[n + 2]);
            k ? printf("Replased %d words\n", k) : printf("There are no words to replace\n");
            n += 3;
        }
        else if (strcmp(argv[n], "SetMinFileSize") == 0 && (argc - n) >= 1) {
            printf(">> %s:\t", argv[n]);
            min_file_size = atoi(argv[n + 1]);
            printf("Ok\n");
            n += 2;
        }
        else if (strcmp(argv[n], "SetMaxFileSize") == 0 && (argc - n) >= 1) {
            printf(">> %s:\t", argv[n]);
            max_file_size = atoi(argv[n + 1]);
            printf("Ok\n");
            n += 2;
        }
        else if (strcmp(argv[n], "SetBufSize") == 0 && (argc - n) >= 1) {
            printf(">> %s:\t\t", argv[n]);
            buf_size = atoi(argv[n + 1]);
            printf("Ok\n");
            n += 2;
        }
        else if (strcmp(argv[n], "help") == 0 || strcmp(argv[n], "--help") == 0 || strcmp(argv[n], "?") == 0 || strcmp(argv[n], "wtf") == 0) {
            PrintHelp();
            ++n;
        }
        else {
            printf("\nSomething went wrong...Syntaxis error or not enough arguments: %s\n", argv[n]);
            return;
        }
    }
}

void TProcessor::InteractiveMode() {
    printf("-------------------------This is interactive mode, let's start work!-------------------------\n");
    while (true) {
        char command[100];
        printf("\n>> ");
        scanf("%s", command);
        if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0 || strcmp(command, "wtf") == 0) {
            PrintHelp();
        }
        else if (strcmp(command, "SetNewFile") == 0) {
            char* path = (char*)malloc(buf_size * sizeof(char));
            scanf("%s", path);
            SetNewFile(path) ? printf("Ok\n") : printf("");
            free(path);
        }
        else if (strcmp(command, "CountLines") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            printf("Summary lines %d", CountLines());
        }
        else if (strcmp(command, "CountChar") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            printf("Summary chars %d", CountChar());
        }
        else if (strcmp(command, "ReadLine") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            int numb;
            scanf("%d", &numb);
            char* str = ReadLine(numb);
            str == NULL ? printf("File has no so much strings\n") : printf("%s\n", str);
            free(str);
        }
        else if (strcmp(command, "SearchSubstring") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            char substr[100];
            scanf("%s", substr);
            if (strlen(substr) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n");
                buf_size = DEF_BUF_SIZE;
            }
            int reg;
            scanf("%d", &reg);
            TSubstr* res = SearchSubstring(substr, (reg == 1 ? true : false));
            if (res->size == 0) {
                printf("There are no substring that you request\n");
            }
            else {
                printf("\n<---- Match substring ---->\n\n");
                for (int i = 0; i < res->size; ++i)
                    printf("Line %d position %d\n", res->body[i].line, res->body[i].pos);
            }
            free(res);
        }
        else if (strcmp(command, "Replace") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            char old_w[100];
            char new_w[100];
            scanf("%s", old_w);
            scanf("%s", new_w);
            if (strlen(old_w) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n");
                buf_size = DEF_BUF_SIZE;
            }
            Replace(old_w, new_w) ? printf("Ok\n") : printf("There are no words to replace\n");
        }
        else if (strcmp(command, "ReplaceAll") == 0) {
            if (!file_opened) {
                printf("First you have to open the file\n");
                continue;
            }
            char old_w[100];
            char new_w[100];
            scanf("%s", old_w);
            scanf("%s", new_w);
            if (strlen(old_w) >= (buf_size * 2)) {
                printf("Your request has changed the buffer size to the default size\n");
                buf_size = DEF_BUF_SIZE;
            }
            int k = ReplaceAll(old_w, new_w);
            k ? printf("Replased %d words\n", k) : printf("There are no words to replace\n");
        }
        else if (strcmp(command, "SetMinFileSize") == 0) {
            scanf("%d", &min_file_size);
            printf("Ok\n");
        }
        else if (strcmp(command, "SetMaxFileSize") == 0) {
            scanf("%d", &max_file_size);
            printf("Ok\n");
        }
        else if (strcmp(command, "SetBufSize") == 0) {
            scanf("%d", &buf_size);
            printf("Ok\n");
        }
        else if (strcmp(command, "Exit") == 0) {
            break;
        }
        else {
            printf("Hmm...I think it's not a command...\n");
        }
    }
}

void TProcessor::PrintHelp() {
    printf("\n==================================================< Command list >==============================================\n\n");
    printf("|> SetNewFile <path>\n Sets a new file\n");
    printf("|> CountLines\nCount number of strings in file\n");
    printf("|> CountChar\nCount number of characters in file\n");
    printf("|> ReadLine <number>\nRead line from file by number\n");
    printf("|> SearchSubstring <substring> <register>\nSearch substring in file. The <register> specifies the ability \n");
    printf("   \t        to search is case-sensitive(1) and case-insensitive(0).\n");
    printf("|> Replace <old word> <new word>\nReplace substring by another string\n");
    printf("|> ReplaceAll <old word> <new word>\nReplace all \"old word\" by \"new word\"\n");
    printf("|> SetMinFileSize <size>\nSet minimal file size (%d by default)\n", DEF_MIN_SIZE);
    printf("|> SetMaxFileSize <size>\nSet maximal file size (%d by default)\n", DEF_MAX_SIZE);
    printf("|> SetBufSize <size>\nSet the maximum amount of memory (%d by default)\n", DEF_BUF_SIZE);
    printf("|> Exit\nTo finish work (Interactive mod only)\n\n");
    printf("=================================================================================================================\n\n");
}

bool TProcessor::SetNewFile(char* path) {
    if (file_opened) {
        if (strcmp(path, path_to_file) == 0) {
            printf("%s %s  You already opened this file LoL\n", path, path_to_file);
            return false;
        }
        CloseHandle(file_handle);
    }
    SECURITY_ATTRIBUTES secAtrib;
    secAtrib.nLength = sizeof(secAtrib);
    secAtrib.lpSecurityDescriptor = NULL;
    secAtrib.bInheritHandle = TRUE;
    file_handle = CreateFile(path,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (!file_handle) {
        printf("Error !!! Can't create file: error code %d\n", GetLastError());
        if (file_opened)
            file_handle = CreateFile(path_to_file,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            &secAtrib,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        return false;
    }
    int size;
    size = GetFileSize(file_handle, NULL);
    if (size > max_file_size) {
        printf("File size is too big\n");
        CloseHandle(file_handle);
        if (file_opened)
            file_handle = CreateFile(path_to_file,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            &secAtrib,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        return false;
    }
    if (size < min_file_size) {
        printf("File size is too small\n");
        CloseHandle(file_handle);
        if (file_opened)
            file_handle = CreateFile(path_to_file,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            &secAtrib,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        return false;
    }
    free(path_to_file);
    path_to_file = _strdup(path);
    file_opened = true;
    return true;
}

int TProcessor::CountLines() {
    char* buf = (char*)malloc(buf_size * sizeof(char));
    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    int lines = 1, i;
    while (true) {
        DWORD readed_bytes;
        ReadFile(file_handle, buf, buf_size, &readed_bytes, NULL);
        if (readed_bytes < buf_size)
            buf[readed_bytes] = '\0';
        for (i = 0; buf[i] != '\0' && i < buf_size; ++i) {
            if (buf[i] == '\n')
                ++lines;
        }
        if (buf[i] == '\0')
            break;
    }
    free(buf);
    return lines;
}

int TProcessor::CountChar() {
    char* buf = (char*)malloc(buf_size * sizeof(char));
    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    int chars = 0, i;
    while (true) {
        DWORD readed_bytes;
        ReadFile(file_handle, buf, buf_size, &readed_bytes, NULL);
        if (readed_bytes < buf_size)
            buf[readed_bytes] = '\0';
        for (i = 0; buf[i] != '\0' && i < buf_size; ++i) {
            if (buf[i] != '\n' && buf[i] != '\r')
                ++chars;
        }
        if (buf[i] == '\0')
            break;
    }
    free(buf);
    return chars;
}

char* TProcessor::ReadLine(int numb) {
    char* buf = (char*)malloc(buf_size * sizeof(char));
    DWORD bytes_readed;
    int count = 1, i = 0;
    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    ReadFile(file_handle, buf, buf_size, &bytes_readed, NULL);
    if (bytes_readed < buf_size)
        buf[bytes_readed] = '\0';

    while (count != numb) {
        if (buf[i] == '\n')
            ++count;
        if (buf[i] == '\0')
            return NULL;
        ++i;
        if (i == buf_size) {
            ReadFile(file_handle, buf, buf_size, &bytes_readed, NULL);
            if (bytes_readed < buf_size)
                buf[bytes_readed] = '\0';
            i = 0;
        }
    }
    char* res = (char*)malloc(1024 * sizeof(char));
    int j = 0;
    while (buf[i] != '\n' && buf[i] != '\0') {
        res[j++] = buf[i++];
        if (i == buf_size) {
            ReadFile(file_handle, buf, buf_size, &bytes_readed, NULL);
            if (bytes_readed < buf_size)
                buf[bytes_readed] = '\0';
            i = 0;
        }
    }
    res[j] = '\0';
    return res;
}

TProcessor::TSubstr* TProcessor::SearchSubstring(char* substr, bool registr) {
    TSubstr* res = (TSubstr*)malloc(1 * sizeof(TSubstr));
    int length = strlen(substr);

    res->body = (TPair*)malloc(100 * sizeof(TPair));
    res->size = 100;
    int iter = 0;
    int lines = 1;
    int line_pos = 1;

    char* buf1 = (char*)malloc(buf_size * sizeof(char));
    char* buf2 = (char*)malloc(buf_size * sizeof(char));

    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    DWORD readed_bytes;
    ReadFile(file_handle, buf1, buf_size, &readed_bytes, NULL);
    ReadFile(file_handle, buf2, buf_size, &readed_bytes, NULL);
    if (readed_bytes < buf_size)
        buf2[readed_bytes] = '\0';
    int i = 0;

    while (true) {
        int j = 0, cur_it = i;
        char* cur = buf1;
        while (j < length && (registr ? (substr[j] == cur[cur_it]) : (tolower(substr[j]) == tolower(cur[cur_it]))) && cur[cur_it] != '\0') {
            ++j;
            ++cur_it;
            if (cur_it == buf_size) {
                cur = buf2;
                cur_it = 0;
            }
        }
        if (j == length) {
            res->body[iter].line = lines;
            res->body[iter].pos = line_pos;
            ++iter;
            if (iter == res->size) {
                res->body = (TPair*)realloc(res->body, res->size * 2 * sizeof(TPair));
                res->size *= 2;
            }
        }
        ++i;
        ++line_pos;
        if (buf1[i] == '\0')
            break;
        if (buf1[i] == '\n') {
            ++lines;
            line_pos = 1;
            ++i;
        }
        if (i == buf_size) {
            free(buf1);
            buf1 = _strdup(buf2);
            ReadFile(file_handle, buf2, buf_size, &readed_bytes, NULL);
            if (readed_bytes < buf_size)
                buf2[readed_bytes] = '\0';
            i = 0;
        }
    }
    res->body = (TPair*)realloc(res->body, iter * sizeof(TPair));
    res->size = iter;
    free(buf1);
    free(buf2);
    return res;
}

bool TProcessor::Replace(char* old_w, char* new_w) {
    SECURITY_ATTRIBUTES secAtrib;
    secAtrib.nLength = sizeof(secAtrib);
    secAtrib.lpSecurityDescriptor = NULL;
    secAtrib.bInheritHandle = TRUE;
    HANDLE tmp = CreateFile("./tmp",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    char* buf1 = (char*)malloc(buf_size * sizeof(char));
    char* buf2 = (char*)malloc(buf_size * sizeof(char));

    DWORD bytes_readed;
    DWORD prev_bytes_readed;
    DWORD bytes_written;
    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    ReadFile(file_handle, buf1, buf_size, &bytes_readed, NULL);
    prev_bytes_readed = bytes_readed;
    ReadFile(file_handle, buf2, buf_size, &bytes_readed, NULL);

    int i = 0, j = 0, length = strlen(old_w);

    while (true) {
        j = 0;
        int cur_it = i;
        char* cur = buf1;
        while (old_w[j] == cur[cur_it] && j < length && cur[cur_it] != '\0') {
            ++j;
            ++cur_it;
            if (cur_it == buf_size) {
                cur = buf2;
                cur_it = 0;
            }
        }
        if (j == length) {
            break;
        }
        WriteFile(tmp, &buf1[i], 1, &bytes_written, NULL);
        ++i;
        if (buf1[i] == '\0')
            break;
        if (i == buf_size) {
            free(buf1);
            buf1 = _strdup(buf2);
            prev_bytes_readed = bytes_readed;
            if (prev_bytes_readed == 0) {
                buf1[i] = '\0';
                break;
            }
            ReadFile(file_handle, buf2, buf_size, &bytes_readed, NULL);
            i = 0;
        }
    }
    if (buf1[i] == '\0' && j != length) {
        CloseHandle(tmp);
        DeleteFile("./tmp");
        return false;
    }

    WriteFile(tmp, new_w, strlen(new_w), &bytes_written, NULL);
    if (i + length >= buf_size) {
        if (i + length - buf_size < bytes_readed)
            WriteFile(tmp, (buf2 + ((i + length) % buf_size)), bytes_readed - ((i + length) % buf_size), &bytes_written, NULL);
    }
    else {
        WriteFile(tmp, (buf1 + i + length), (prev_bytes_readed - i - length), &bytes_written, NULL);
        WriteFile(tmp, buf2, bytes_readed, &bytes_written, NULL);
    }
    while (bytes_readed == buf_size) {
        ReadFile(file_handle, buf1, buf_size, &bytes_readed, NULL);
        WriteFile(tmp, buf1, bytes_readed, &bytes_written, NULL);
    }
    CloseHandle(file_handle);
    file_handle = CreateFile(path_to_file,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        TRUNCATE_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    SetFilePointer(tmp, 0, NULL, FILE_BEGIN);
    while (true) {
        ReadFile(tmp, buf1, buf_size, &bytes_readed, NULL);
        WriteFile(file_handle, buf1, bytes_readed, &bytes_written, NULL);
        if (bytes_readed != buf_size)
            break;
    }
    CloseHandle(tmp);
    DeleteFile("./tmp");
    free(buf1);
    free(buf2);
    return true;
}

int TProcessor::ReplaceAll(char* old_w, char* new_w) {
    SECURITY_ATTRIBUTES secAtrib;
    secAtrib.nLength = sizeof(secAtrib);
    secAtrib.lpSecurityDescriptor = NULL;
    secAtrib.bInheritHandle = TRUE;
    HANDLE tmp = CreateFile("./tmp",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    char* buf1 = (char*)malloc(buf_size * sizeof(char));
    char* buf2 = (char*)malloc(buf_size * sizeof(char));

    DWORD bytes_readed;
    DWORD prev_bytes_readed;
    DWORD bytes_written;
    SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
    ReadFile(file_handle, buf1, buf_size, &bytes_readed, NULL);
    prev_bytes_readed = bytes_readed;
    ReadFile(file_handle, buf2, buf_size, &bytes_readed, NULL);

    int i = 0, j = 0, k = 0, length_old = strlen(old_w), length_new = strlen(new_w);

    while (true) {
        j = 0;
        int cur_it = i;
        char* cur = buf1;
        while (old_w[j] == cur[cur_it] && j < length_old && cur[cur_it] != '\0') {
            ++j;
            ++cur_it;
            if (cur_it == buf_size) {
                cur = buf2;
                cur_it = 0;
            }
        }
        if (j == length_old) {
            WriteFile(tmp, new_w, length_new, &bytes_written, NULL);
            ++k;
            i += length_old;
        }
        else {
            WriteFile(tmp, &buf1[i], 1, &bytes_written, NULL);
            ++i;
        }
        if (i >= buf_size) {
            free(buf1);
            if (bytes_readed == 0)
                break;
            buf1 = _strdup(buf2);
            prev_bytes_readed = bytes_readed;
            ReadFile(file_handle, buf2, buf_size, &bytes_readed, NULL);
            i = 0;
        }
        if (i == prev_bytes_readed)
            break;
    }
    CloseHandle(file_handle);
    file_handle = CreateFile(path_to_file,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &secAtrib,
        TRUNCATE_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    SetFilePointer(tmp, 0, NULL, FILE_BEGIN);
    while (true) {
        ReadFile(tmp, buf1, buf_size, &bytes_readed, NULL);
        WriteFile(file_handle, buf1, bytes_readed, &bytes_written, NULL);
        if (bytes_readed != buf_size)
            break;
    }
    CloseHandle(tmp);
    DeleteFile("./tmp");
    free(buf1);
    free(buf2);
    return k;
}

int _tmain(int argc, char* argv[]) {
    TProcessor* processor = new TProcessor();
    processor->StartProcessor(argc, argv);
    delete processor;
    return 0;
}
