//
//  RIT's work.CPP
//

#include <stdlib.h>
#include <stdio.h>
#if !defined( unix )
#include <io.h>
#include <fcntl.h>
#endif

main( int argc, char *argv[] )
{
    fprintf( stderr, "PACK char" );
    if ( argc > 1 ) {
        freopen( argv[ 1 ], "rb", stdin );
        fprintf( stderr, "%s", argv[ 1 ] );
    } else
        fprintf( stderr, "stdin" );
    fprintf( stderr, " to " );
    if ( argc > 2 ) {
        freopen( argv[ 2 ], "wb", stdout );
        fprintf( stderr, "%s", argv[ 2 ] );
    } else
        fprintf( stderr, "stdout" );
    fprintf( stderr, "\n" );
#if !defined( unix )
    setmode( fileno( stdin ), O_BINARY );
    setmode( fileno( stdout ), O_BINARY );
#endif


    unsigned int  max[256], count, c1, i, j, row, longest , b1, b2;
    int c;
    char head[256][16];

 
      count=0 ;
      c = getc( stdin ) ; 
       count = (c & 0x00ff)  << 24;
       c = getc( stdin ) ; 
       count = count | ((c & 0x00ff)  << 16);
       c = getc( stdin ) ; 
       count = count | ((c & 0x00ff)  << 8);
       c = getc( stdin ) ; 
       count =  count | (c & 0x00ff)  ;

       fprintf(stderr, "count = %d \n",count) ; 

    if ( ( c = getc( stdin ) ) < 0 ) {
         fprintf(stderr, "Error .. No row information \n");
         exit(1);
          } 
      else 
          row= c; 
          fprintf(stderr, "row = %d \n", row);
  
       j=0;
	   
	   for (i=0 ; i < row; i++) {
		     c = getc( stdin );
			 max[i]=c;
			 putc ((unsigned char) c, stdout);
             fprintf(stderr, "%d \n", (max[i]));
	   }

       for (i=0 ; i < row; i++) { 
                while ( ( c = getc( stdin ) ) != 32 ) head[i][j++]=c; 
                head[i][j]= '\0'; 
               j=0;
           }
       for (i=0 ; i < row; i++)  
             fprintf(stderr, "%s \n", head[i]);

      j= count/2 ;
             fprintf(stderr, "%d \n", j);

     for (i=0 ; i < (count)/2 ;i++){

           if ( (c = getc( stdin )) >= 0 )  {
               b1 = (c & 0x000f) +48;
               b2 = ((c >> 4) & 0x000f) +48;
               putc ((unsigned char) b2, stdout);
               putc ((unsigned char) b1, stdout);
             }
         else {
             fprintf(stderr, "ERROR in count i=%d \n", i);
             exit(0);
            }
      } //for
       // Take care the last element if count is ODD
        if ( count %2 == 1) {
           if ( (c = getc( stdin )) >= 0 )  {
               b1 = (c & 0x000f) +48;
               b2 = ((c >> 4) & 0x000f) +48;
               putc ((unsigned char) b1, stdout);
             }
         }
    return 1;
}
