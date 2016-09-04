/*----------------------- tx_rx_serial ----------------------------

PURPOSE - Show how to tx and rx from the serial port under Linux.
          Pins 2 and 3 should be connected to receive what has
          been transmitted.
          Use this code as the starting point for your own
          serial port program.

USAGE - Copy this file into a working directory such as
         \home/user/coding.
      - Under Linux and KDE double click on the source file to
        view it using KWrite.
      - If the Konqueror file explorer is looking at the directory
        hit F4 to get a terminal window,  or open a terminal
        and do "cd /home/user/coding".
        Compile with the command below toi create a binary file called x
            gcc -o x -g tx_rx_serial.c
      - run by typing-
            ./x
      - debug by typing kdbg x

REFERENCES - see the excellent description by Michael Sweet at
             http://www.easysw.com/~mike/serial/serial.html
             This not only shows you how to program the serial
             port but also has pin outs and the meaning of each
             pin.


NOTE - RS232 outputs RTS and DTR are set active by
       transmission and are not available for general IO use.
     - The inputs CTS, DSR, DCD and RI can have a role in
       flow control but if this is turned off they can be
       used as general purpose inputs.
       This program turns off all flow control.
     - All inputs require +3v for a high and -3v for a low.
       This can be a problem as logic is usually +5v or +3v
       with no swing below 0v.
       You can safely connect a 9v battery between 0v and any
       input or output.  Swap the battery around to get a
       change input level.
     - There is other code available for fast reading and
       writing to serial port IO pins.  Do a web search for-
       "radcliffe linux IO port" to find this.
     - The operating system does all the hard work for you!
       Data is transmitted under interrupt, received under
       interrupt, and buffer in an 8k buffer.
     - Use the gtkterm program as a simple way to check
       if your serial port is working.  Just connect pins
       2 and 3 on the serial port to get a loopback so
       you can see what you send being received.
     - This will work for USB serial ports, see devices
       named /dev/ttyUSB0 in the /dev directory.
*/

//------ program parameters.
#define SERIAL_PORT "/dev/ttyUSB0"
//#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD        B1200
#define TX_CHARS    "1234567890"


//------ includes.
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
//#include <termios.h> /* POSIX terminal control definitions */
#include </usr/include/asm-generic/termios.h>


//------ data available to all routines.

struct termios options ; // must be here not in code.

int sfd;      // File descriptor for the port.
int status ; // capture status of library calls.
             //  scratch only, no function calls between setting & using.

#define BUFFER_SIZE 255
char buffer[BUFFER_SIZE];  // Input buffer






//=================== input testing ======================================

int dtr_hi() // return !=0 if DTR pin is above +3v, 0 if below -3v.
             // can also test using TIOCM_CTS, TIOCM_DCD, TIOCM_RI.
{ int status ;
  ioctl(sfd, TIOCMGET, &status);
  return( status & TIOCM_DSR) ;
}



//=================== main ===============================================

int main(int argc, char *argv[])
{//------- open serial port, rw, don't link to kbd, ignore DCD pin state.
   sfd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
   if (sfd == -1)
     { perror("open_port: Unable to open device - ");
       return(-1) ;
       }
   fcntl(sfd, F_SETFL, 0); // ensure don't return from read/write till finished.

 //--- set the serial port options.
   /* Not sure the purpose of this line. */
   // if ( tcgetattr(sfd, &options) < 0)      // get current options.
   //  {  fprintf( stderr, "Unable to retrieve terminal settings: %s\n", strerror( errno ));
   //     return( -1 );
   //  }

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

 //--- tx a string.
   status = write(sfd, TX_CHARS, strlen(TX_CHARS)) ;
   printf("   Transmitted \"%s\" on serial port.\n", TX_CHARS) ;


 //----- wait for tx to finish,  then receive if pins 2 and 3 connected.
   sleep(1) ;
   status = read(sfd, &buffer, BUFFER_SIZE-1) ;
   printf("   Received %i bytes: %s\n",  status, buffer) ;

 //------ print DTR status
   if ( dtr_hi() )
        printf("   DTR high.\n") ;
   else printf("   DTR low.\n") ;

 //------ finish off and clean up.
   close(sfd);

}
