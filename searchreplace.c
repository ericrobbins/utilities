#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int
searchreplace(char *buf, int len, char *findstring, char *replacewith)
{
        int changed = 0;
        char *tmp;
        char *here;

        if (!buf || !findstring || !replacewith)
                return -1;

        here = strstr(buf, findstring);
        if (here)
        {
                tmp = malloc(len);
                changed = 1;
                *here = '\0';
                snprintf(tmp, len, "%s%s%s", buf, replacewith,
                        here + strlen(findstring));
                here += strlen(replacewith);
                strncpy(buf, tmp, len);
                buf[len - 1] = '\0';
                free(tmp);
        }
        if (changed)
                searchreplace(here, (len - (here - buf)), findstring, 
			replacewith);

        return 0;
}
