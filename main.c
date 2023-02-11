#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <wasmedge/wasmedge.h>

static void printVer()
{
    printf("%s\n", WasmEdge_VersionGet());
}

static void printWrongArgs()
{
    printf("Wrong arguments. Please use: tool [version] [run] wasm_path [arguments]\n");
}

static void printWrongInt()
{
    printf("Wrong argument: both arguments must be valid integers. \n");
}

/* Check if all characters in a string are integers. */
static bool intCheck(const char *str)
{
    if (sizeof(str) > __SIZEOF_INT__)
        return false;

    for (char character = *str; character != '\0'; character = *++str)
    {
        if (character < '0' || character > '9')
            return false;
    }

    return true;
}

static void runWasm(const char *path, int add1, int add2)
{
    /* Create the configure context and add the WASI support. */
    /* This step is not necessary unless you need WASI support. */
    WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
    WasmEdge_ConfigureAddHostRegistration(ConfCxt,
                                          WasmEdge_HostRegistration_Wasi);
    /* The configure and store context to the VM creation can be NULL. */
    WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, NULL);

    /* The parameters and returns arrays. */
    WasmEdge_Value Params[2] = {WasmEdge_ValueGenI32(add1),
                                WasmEdge_ValueGenI32(add2)};
    WasmEdge_Value Returns[1];
    /* Function name. */
    WasmEdge_String FuncName = WasmEdge_StringCreateByCString("add");
    /* Run the WASM function from file. */
    WasmEdge_Result Res = WasmEdge_VMRunWasmFromFile(
        VMCxt, path, FuncName, Params, 2, Returns, 1);

    if (WasmEdge_ResultOK(Res))
    {
        printf("%d\n", WasmEdge_ValueGetI32(Returns[0]));
    }
    else
    {
        printf("Error message: %s\n", WasmEdge_ResultGetMessage(Res));
    }

    /* Resources deallocations. */
    WasmEdge_VMDelete(VMCxt);
    WasmEdge_ConfigureDelete(ConfCxt);
    WasmEdge_StringDelete(FuncName);
}

int main(int argc, const char *argv[])
{

    if (argc < 2 || argc > 5)
    {
        printWrongArgs();
        return -1;
    }

    if (strcmp(argv[1], "version") == 0)
    {
        printVer();
        return 0;
    }
    else if (strcmp(argv[1], "run") == 0)
    {
        if (argc < 5)
        {
            printWrongArgs();
            return -1;
        }

        if (intCheck(argv[3]) && intCheck(argv[4]))
            runWasm(argv[2], atoi(argv[3]), atoi(argv[4]));
        else
            printWrongInt();
            return -1;
    }
    else if (argc != 4)
    {
        printWrongArgs();
        return -1;
    }
    else
    {
        if (intCheck(argv[2]) && intCheck(argv[3]))
            runWasm(argv[1], atoi(argv[2]), atoi(argv[3]));
        else
            printWrongInt();
            return -1;
    }

    return 0;
}