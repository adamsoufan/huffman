#include <stdio.h>
#include <getopt.h>

int main(int argc, char* argv[]) {
  int opt;
  char *filename = NULL;
  int debug = 0; // Initialize debug flag to off

  // Handle command line options
  while ((opt = getopt(argc, argv, "i:d")) != -1) {
    switch (opt) {
      case 'i': // Input
        filename = optarg; // Get filename
        break;
      case 'd': // Debug
        debug = 1; // Set debug flag to on
        break;
      case ':': // Missing argument
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        return 1;
      case '?': // Unknown option
        fprintf(stderr, "Unknown option: -%c\n", optopt);
        return 1;
    }
  }

  //********************************************************************************************************
  // STEP ONE: Read input file and collect character frequencies
  //********************************************************************************************************
  
  // Open input file
  if (filename == NULL) {
    fprintf(stderr, "Missing required option -i.\n");
    return 1;
  }
  FILE *file = fopen(filename, "r");

  // Read each character from the file and count frequencies
  unsigned int ch;
  unsigned int frequencies[256];

  // Initialize all frequencies to 0
  for (int i=0; i<256; i++) {
    frequencies[i] = 0;
  }

  // Increment frequency for each character read
  while ((ch = fgetc(file)) != EOF) {
    frequencies[ch] += 1;
  }

  // Create array for control characters (ASCII 0-32)
  const char* controlChars[33] = {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
    "BS", "TAB", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", 
    "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", 
    "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE"
  };

  if (debug) {
    // Print characters 0-32 and their frequencies
    for (int i=0; i<33; i++) {
      if (frequencies[i] > 0) printf("%s\t%d\n", controlChars[i], frequencies[i]);
    }

    // Print characters 33-255 and their frequencies
    for (int i=33; i<256; i++) {
      if (frequencies[i] > 0) printf("%c\t%d\n", i, frequencies[i]);
    }
  }

  // Close file
  fclose(file);
  
  return 0;
}
