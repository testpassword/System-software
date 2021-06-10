#include <napi.h>
#include <Commander.h>
#include "spo1/src/include/List.h"
#include <string.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#define OUTPUT_BUFFER_SIZE 16384
#define CURRENT_DIR_STRING_LENGTH 255 + 3 + 255
#define COMMAND_MAX_LENGTH CURRENT_DIR_STRING_LENGTH

static ProbeInfo probeInfo;
static FileSystem fs;
static char* currentDir = NULL;

char* napiToStr(const Napi::CallbackInfo &info) { return strdup(info[0].ToString().Utf8Value().c_str()); }

void removeNewlineChars(char* str) {
    int index = 0;
    while (str[index] != '\0') {
        if (str[index] == '\n') str[index] = '\0';
        index++;
    }
}

int parsePath(char* path, char* currentDir) {
    if (path[0] != '.' && path[0] != '/') return -1;
    removeNewlineChars(path);
    const size_t currentDirLength = strlen(currentDir);
    const size_t pathLength = strlen(path);
    if (path[0] == '.') {
        char* currentPathBuffer = (char*) calloc(COMMAND_MAX_LENGTH, 1);
        memcpy(currentPathBuffer, path + 1, pathLength + 1);
        memset(path, 0, COMMAND_MAX_LENGTH);
        if (pathLength > 1 && currentDirLength < 2) {
            memcpy(path, currentDir + 1, currentDirLength - 1);
            memcpy(path + currentDirLength - 1, currentPathBuffer, pathLength);
        }
        else {
            memcpy(path, currentDir, currentDirLength);
            memcpy(path + currentDirLength, currentPathBuffer, pathLength);
        }
        free(currentPathBuffer);
    }
    return 0;
}

Napi::String ConstructOutputString(PathListNode* list, const Napi::Env& env) {
    char* output = (char*) calloc(OUTPUT_BUFFER_SIZE, sizeof(char));
    while (list != NULL) {
        strncat(output, list->token, strlen(list->token));
        list = list->next;
    }
    Napi::String res = Napi::String::New(env, output);
    free(output);
    return res;
}

Napi::Number init(const Napi::CallbackInfo &info) { return Napi::Number::New(info.Env(), Init(&probeInfo)); }

Napi::Number open(const Napi::CallbackInfo &info) { return Napi::Number::New(info.Env(), openByPath(info[0].ToString().Utf8Value().c_str(), &fs)); }

Napi::Number loadFS(const Napi::CallbackInfo &info) {
    currentDir = (char*) calloc(CURRENT_DIR_STRING_LENGTH, sizeof(char));
    char* path = (char*) calloc(COMMAND_MAX_LENGTH, sizeof(char));
    currentDir[0] = '/';
    return Napi::Number::New(info.Env(), 0);
}

void close(const Napi::CallbackInfo &info) { if (currentDir) free(currentDir); }

void printCurDir(const Napi::CallbackInfo &info) {
    removeNewlineChars(currentDir);
    fputs(currentDir, stdout);
    fputs(">", stdout);
    fflush(stdout);
}

Napi::String partitions(const Napi::CallbackInfo &info) { return ConstructOutputString(IterateDevices(&probeInfo), info.Env()); }

Napi::String cd(const Napi::CallbackInfo &info) {
    char* path = napiToStr(info);
    if (parsePath(path, currentDir)) return Napi::String::New(info.Env(), "invalid path\n");
    if (setCurDir(path, &fs)) return Napi::String::New(info.Env(), "required path doesn't exist\n");
    else {
        memset(currentDir, 0, CURRENT_DIR_STRING_LENGTH);
        memcpy(currentDir, path, CURRENT_DIR_STRING_LENGTH);
        return Napi::String::New(info.Env(), "");
    }
}

Napi::String cp(const Napi::CallbackInfo &info) {
    char* path = napiToStr(info);
    if (parsePath(path, currentDir)) return Napi::String::New(info.Env(), "Incorrect path!\n");
    copyItems(path, currentDir, &fs);
    return Napi::String::New(info.Env(), "");
}

Napi::String pwd(const Napi::CallbackInfo &info) { return Napi::String::New(info.Env(), currentDir); }

Napi::String ls(const Napi::CallbackInfo &info) {
    char* path = napiToStr(info);
    if (parsePath(path, currentDir)) return Napi::String::New(info.Env(), "Incorrect path!\n");
    dirContent(path, &fs);
    return ConstructOutputString(fs.output, info.Env());
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(
            Napi::String::New(env, "init"),
            Napi::Function::New(env, init)
            );
    exports.Set(
            Napi::String::New(env, "partitions"),
            Napi::Function::New(env, partitions)
            );
    exports.Set(
            Napi::String::New(env, "open"),
            Napi::Function::New(env, open)
            );
    exports.Set(
            Napi::String::New(env, "loadFS"),
            Napi::Function::New(env, loadFS)
            );
    exports.Set(
            Napi::String::New(env, "cd"),
            Napi::Function::New(env, cd)
            );
    exports.Set(
            Napi::String::New(env, "printCurDir"),
            Napi::Function::New(env, printCurDir)
            );
    exports.Set(
            Napi::String::New(env, "close"),
            Napi::Function::New(env, close)
            );
    exports.Set(
            Napi::String::New(env, "pwd"),
            Napi::Function::New(env, pwd)
            );
    exports.Set(
            Napi::String::New(env, "ls"),
            Napi::Function::New(env, ls)
            );
    exports.Set(
            Napi::String::New(env, "cp"),
            Napi::Function::New(env, cp)
    );
    return exports;
}

NODE_API_MODULE(flexcommander_gui_native, Init)

