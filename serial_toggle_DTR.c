/*----------------------- serial_toggle_DTR ----------------------------

PURPOSE - Toggle DTR on a traditional or USB parallel port.

  USAGE - gcc -o z serial_toggle_DTR.c


*/

//------ program parameters.

// Choose either a real parallel port (ttyS0, or a USB
//  plug in ttyUSB0.
//#define SERIAL_PORT "/dev/ttyS0"
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD        B1200


//------ includes.
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <stdlib.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
//#include <termios.h> /* POSIX terminal control definitions */
#include </usr/include/asm-generic/termios.h>


//------ data available to all routines.

struct termios options ; // must be here not in code.

int sfd;      // File descriptor for the port.
int status ; // capture status of library calls.
             //  scratch only, no function calls between setting & using.
int iFlags ; // Flags for TIOCM.	     

#define BUFFER_SIZE 255
char buffer[BUFFER_SIZE];  // Input buffer 


//=================== HW access & support ============================
void DTR_on()
{  iFlags = TIOCM_DTR;
   ioctl(sfd, TIOCMBIS, &iFlags);
}

void DTR_off()
{ iFlags = TIOCM_DTR;
  ioctl(sfd, TIOCMBIC, &iFlags); 
}

void ms_delay(int ms)
{  struct timespec sleep_time ;
   sleep_time.tv_sec = ms/1000 ;
   sleep_time.tv_nsec = (ms % 1000) * 1e6 ;
   nanosleep( &sleep_time, NULL) ;
}


//=================== main ===============================================

int main(int argc, char *argv[])
{//------- open serial port, rw, don't link to kbd, ignore DCD pin state.
   sfd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY ); //| O_NONBLOCK);
   if (sfd == -1)
     { perror("open_port: Unable to open device - ");
       return(-1) ;
       }
   fcntl(sfd, F_SETFL, 0); // ensure don't return from read/write till finished.

 //--- set the serial port options.
   if ( tcgetattr(sfd, &options) < 0)      // get current options.
    {  fprintf( stderr, "Unable to retrieve terminal settings: %s\n", strerror( errno ));
       return( -1 );
    }
   cfsetispeed(&options, BAUD);     // set rx then tx to 1200 baud.
   cfsetospeed(&options, BAUD);
   options.c_cflag |= (CLOCAL | CREAD); // enable receiver, local mode.
   options.c_cflag &= ~PARENB;          // setup no parity and 8 bit data.
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8;          // 8 bit data.
   options.c_cflag &= ~CRTSCTS;     // disable RTS-CTS hardware flow control.
   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // just have raw input, no echo.
   options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off software flow control.
   options.c_oflag &= ~OPOST;       // output sent raw to serial port.

   status = tcsetattr(sfd, TCSANOW, &options);    //set new options into UART.
   if (status != 0)
    printf("tcsetattr failed.\n") ;

   
 //------ play with DTR
   int i ;
   int ms ;
   #define CNT 4
   while (1)
    {//--- stable loop
       //for (i=0; i<CNT; i++)
         { DTR_on()  ;
           ms_delay(500);
           DTR_off() ;
           ms_delay(500) ;
         }
         /*
     //--- variable loop.   
       for (i=0; i<CNT; i++)
         { DTR_on()  ;
           ms_delay(300) ;
           DTR_off() ;
	   ms = (int)((random() / RAND_MAX)*1200) + 100;
           ms_delay(ms) ;
         }
         */
    } 

 //------ finish off and clean up.
   close(sfd);

}
