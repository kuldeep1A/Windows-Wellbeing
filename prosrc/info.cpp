#include <iostream>
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")

using namespace std;

void ListProcessesInLast24Hours()
{
    PDH_STATUS status;
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_FMT_COUNTERVALUE value;

    // Open a query object
    status = PdhOpenQuery(nullptr, 0, &query);
    if (status != ERROR_SUCCESS)
    {
        std::cerr << "Failed to open query: " << status << std::endl;
        return;
    }

    // Add the counter for process usage
    status = PdhAddCounter(query, "\\Process(*)\\% Processor Time", 0, &counter);
    if (status != ERROR_SUCCESS)
    {
        std::cerr << "Failed to add counter: " << status << std::endl;
        PdhCloseQuery(query);
        return;
    }

    // Collect the data
    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS)
    {
        std::cerr << "Failed to collect query data: " << status << std::endl;
        PdhCloseQuery(query);
        return;
    }

    // Wait for 1 second to collect some data
    Sleep(1000);

    // Collect the data again
    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS)
    {
        std::cerr << "Failed to collect query data: " << status << std::endl;
        PdhCloseQuery(query);
        return;
    }

    // Retrieve and print the usage of each process
    DWORD bufferSize = 4096;
    CHAR buffer[4096];
    PDH_FMT_COUNTERVALUE_ITEM_A *item = (PDH_FMT_COUNTERVALUE_ITEM_A *)buffer;
    DWORD itemCount = 0;
    while (true)
    {
        status = PdhGetFormattedCounterArrayA(counter, PDH_FMT_LONG, &bufferSize, &itemCount, item);
        if (status != ERROR_SUCCESS)
        {
            break;
        }

        // Print process name and usage
        std::cout << "Process: " << item->szName << ", Usage: " << item->FmtValue.longValue << "%" << std::endl;

        // Move to next item in buffer
        item++;
    }

    // Close the query object
    PdhCloseQuery(query);
}

int main()
{
    ListProcessesInLast24Hours();
    return 0;
}
