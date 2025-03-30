#include <stdio.h>
#include <time.h>
#include <unistd.h>

char* get_current_time() {
    static char buffer[9];  // "hh:mm:ss\0"
    time_t now = time(NULL);
    strftime(buffer, sizeof(buffer), "%T", localtime(&now));
    return buffer;
}

int main()
{
    while (1)
    {
        char* current_time = get_current_time();
        printf("\r%s x%% (SSID)\033[?25l", current_time);   // \033[?25l hides the cursor
        fflush(stdout); 
        sleep(1);
    }
}