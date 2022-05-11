#pragma once

void initShell();
void initShellVariables();
int varExpansion(char** tokenList);
int reassignVar(char* var, char* newValue);
int detectVarReassigns(char** tokenList);

//internal commands
int internalCd(char** args);
int internalExit(char** args);
int internalHelp(char** args);
int internalCwd(char** args);
int internalClear(char** args);
int internalEcho(char** args);
int internalShowvar(char** args);
int internalShowenv(char** args);
int internalExport(char** args);
int internalUnset(char** args);
int getSource(char* filePath);

int countInternals();

int startProcesses(char **args);
int executeCommands(char **args, bool hasPipeLine, bool hasAmpersand);
int executeCommandsWithPipeline(char** args);
int executeCommandsWithAmpersand(char**  args);
int executeInternalCommands(char** args);

extern bool screenCleared;