//
//  RIT's work.CPP
//

#include <stdlib.h>
#include <string.h>
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


    int c,i,j,longest,row=0,k=0;
    unsigned int count=0 , bpack=0;
    char *header, *head[256], *p, ch[3], ptr='\t';
	int max[256],col[256];
	
          header= (char *) malloc(4096);
       for (i=0 ; i < 256 ; i++)  
           head[i]= (char *) malloc(16);

            size_t len = 4096;
            ssize_t read;

           if ((read = getline(&header, &len, stdin)) == -1) {
                 fprintf(stderr, "Can't read header \n");
                 exit(0) ;
            }

           // Parse the header string into substrings 
           // row is the number of rows.
                  p = strtok(header, &ptr);
                  head[0] = p ; 

           row=1;
           while ( (p= strtok(NULL, &ptr)) != NULL) {
                         head[row] = p ; 
                         row++;
                  }

                 putc((unsigned char) 32, stdout);
                 putc((unsigned char) 32, stdout);
                 putc((unsigned char) 32, stdout);
                 putc((unsigned char) 32, stdout);
                 
				 
				 putc((unsigned char) row, stdout);
				 
				 for (i=0 ; i < row; i++)
					putc((unsigned char) 32, stdout);
				 					
                   
				 for (i=0 ; i < row; i++) {
					 max[i]=0; 
					 col[i]=0;	
				 }	 

				 for (i=0 ; i < row ; i++)
                            fprintf(stdout, "%s ", head[i]);

	    while ( ( c = getc( stdin ) ) >=0 )  {
            if ( (c !=  int('\t')  ) && ( c!= '\n') )  {
		 
				
				col[k]=c;

				if (col[k]>max[k])
					max[k]=col[k];
				k++;
			    if (k==row)
				 	k=0;

			  count++ ;
              bpack = (bpack << 4);
              bpack += (c-48);
             if (count%2 == 0) {  
               putc ((unsigned char) bpack, stdout);
               bpack=0;
             }
			}

		}

      // In case there is a last character detected, because of odd number of columns.
             if (count%2 == 1)   
               putc ((unsigned char) bpack, stdout);

         fprintf(stderr, "count=%d \n", count);
         // wtite the  count to the top
               rewind(stdout);
                 putc((  char)  (count>>24) & 0x00ff  , stdout);
                 putc((  char)  (count>>16) & 0x00ff  , stdout);
                 putc(( char)  (count>>8) & 0x00ff  , stdout);
                 putc(( char)  (count & 0x00ff)  , stdout);
				 
				 putc((unsigned char) row, stdout); 	   
				 
				 for (i=0 ; i < row; i++)
					putc((unsigned char) max[i] , stdout);
								
				 	

    return 1;
  //fclose(stdout);
}
