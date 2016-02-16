#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bcd.h"

//arg1 on/off
//arg2 delay time
int main ( int argc, char *argv[] ) {
    FILE *f;
    char text[100];

    f = fopen( bfile_info, "r" );
    fscanf( f, "%s", text);
    printf( "State:\n%s", text );
    fclose( f );

    /*Initial delay*/
    if(argc > 1 && argv[2][0] > 0) {
        sleep( atoi( argv[2] ) );
    } else sleep( 60 );

    /*Open sys file*/
    f = fopen( bchargefile, "w" );
    if(argv[1][0] == '1') {
        /*Start charging*/
        if(fprintf( f, "%s", bon ) < 0) {
            fprintf( stderr, "Write 1 error\n" );
            goto error;
        }
    } else if(argv[1][0] == '0') {
        /*Stop charging*/
        if(fprintf( f, "%s", bof ) < 0) {
            fprintf( stderr, "Write 0 error\n" );
            goto error;
        }
    }

    fclose( f );

    exit( 0 );

error:
    exit( -1 );
}
