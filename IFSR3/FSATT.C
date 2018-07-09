#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>

#pragma argsused
int main(int argc, char **argv, char **envp) {
   int rc;
   char buf[100];
   FSQBUFFER2 buf2[2];
   char drive;
   char drivename[] = "a:";
   ULONG buflen;

   if (argc == 1 || argc > 2) {
      printf("Usage: FSATT { A | Q }\n");
      return 1;
   }

   if (argv[1][0] == 'a' || argv[1][0] == 'A') {
      rc = DosFSAttach("q:", "EXR0R3", &buf, sizeof(buf), FS_ATTACH);
      printf("ret = 0x%x %d\n", rc, rc);
   } else {
      for (drive = 'c'; drive <= 'z'; drive++) {
         drivename[0] = drive;
         buflen = sizeof(buf2);
         rc = DosQueryFSAttach(drivename, 0, FSAIL_QUERYNAME, &buf2[0], &buflen);
         if (rc != ERROR_INVALID_DRIVE && rc != ERROR_NOT_READY) {
            printf("%s rc: %d type:%d item data:'%s' FSD name:'%s'\n",
                   drivename, rc, buf2[0].iType, buf2[0].szName,
                   buf2[0].szFSDName+buf2[0].cbName);
         }
      }
   }

   return 0;
}
