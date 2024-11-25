// Adam Soufan -- Huffman Code

#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define PARENT 999


// Node struct for Huffman tree
typedef struct Node {
  unsigned int ch;
  unsigned int frequency;
  unsigned int parentNum;
  struct Node* leftChild;
  struct Node* rightChild;
} Node; 

// Array for control characters (ASCII 0-32)
const char* controlChars[33] = {
  "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
  "BS", "TAB", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", 
  "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", 
  "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE"
};

// Variables to get codes from Huffman tree
char code[256];
char codes[256][32];
int depth = 0;

// Function to get codes from Huffman tree
void getCodes(Node *node) {
  // If leaf node, set code
  if (node->leftChild == NULL && node->rightChild == NULL) {
    for (int i=0; i<depth; i++) {
    codes[node->ch][i] = code[i];
    }
    depth--;
    return;
  }
  
  // Traverse left side
  if (node->leftChild != NULL) {
    depth++;
    code[depth-1] = '0';
    getCodes(node->leftChild);
  }

  // Traverse right side
  if (node->rightChild != NULL) {
    depth++;
    code[depth-1] = '1';
    getCodes(node->rightChild);
  }

  // Go back up tree
  depth--;
  return;
}

// Function to print Huffman tree
void printTree(Node *node) {
  if (node == NULL) {
    return;
  }

  // Traverse left side
  printTree(node->leftChild);

  // Traverse right side
  printTree(node->rightChild);

   // Print characters 0-32 with their frequencies and codes
  if (node->ch < 33) {
    printf("%s\t%d\t%s\n", controlChars[node->ch], node->frequency, codes[node->ch]);
  } 
  // Print characters 33-255 with their frequencies and codes
  else if (node->ch < 999) {
    printf("%c\t%d\t%s\n", node->ch, node->frequency, codes[node->ch]);
  }
}


// Minimum queue implementation
int size = 0;
void swap(Node *a, Node *b) {
  Node temp = *b;
  *b = *a;
  *a = temp;
}

// Function to heapify the tree
void heapify(Node array[], int size, int i) {
  if (size == 1) {
    printf("Single element in the heap");
  } else {
    // Find the smallest among root, left child and right child
    int lowest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < size && array[l].frequency < array[lowest].frequency)
      lowest = l;
    if (r < size && array[r].frequency < array[lowest].frequency)
      lowest = r;

    // Swap and continue heapifying if root is not smallest
    if (lowest != i) {
      swap(&array[i], &array[lowest]);
      heapify(array, size, lowest);
    }
  }
}

// Function to insert an element into the tree
void insert(Node array[], Node node) {
  if (size == 0) {
    array[0] = node;
    size += 1;
  } else {
    array[size] = node;
    size += 1;
    for (int i = size / 2 - 1; i >= 0; i--) {
      heapify(array, size, i);
    }
  }
}

// Function to remove lowest frequency character
Node removeLeastFrequent(Node array[]) {
  Node temp = array[0];
  swap(&array[0], &array[size - 1]);
  size -= 1;
  for (int i = size / 2 - 1; i >= 0; i--) {
    heapify(array, size, i);
  }
  return temp;
}


// HUFFMAN ENCODING
int main(int argc, char* argv[]) {
  //********************************************************************************************************
  // STEP ZERO: Handle command line options
  //********************************************************************************************************
  int opt;
  char *inputFilename = NULL;
  char *outputFilename = NULL;
  int debug = 0; // Initialize debug flag to off

  while ((opt = getopt(argc, argv, "i:o:d")) != -1) {
    switch (opt) {
      case 'i': // Input file
        inputFilename = optarg; // Get filename
        break;
      case 'o': // Output file
        outputFilename = optarg;
        break;
      case 'd': // Debug
        debug = 1; // Set debug flag to on
        break;
    }
  }

  //********************************************************************************************************
  // STEP ONE: Read input file and collect character frequencies
  //********************************************************************************************************
  
  // Open input file
  if (inputFilename == NULL) inputFilename = "completeShakespeare.txt"; // Default input file
  FILE *inputFile = fopen(inputFilename, "r");

  // Read each character from the file and count frequencies
  unsigned int ch;
  unsigned int uniqueChars = 0;
  unsigned int frequencies[256] = {0}; // Initialize all frequencies to 0

  // Increment frequency for each character read
  while ((ch = fgetc(inputFile)) != EOF) {
    if (frequencies[ch] == 0) uniqueChars++; // If new character, increment num of unique characters
    frequencies[ch] += 1;
  }

  // Close file
  fclose(inputFile);

  //********************************************************************************************************
  // STEP TWO: Build a Huffman tree based on the character frequencies
  //********************************************************************************************************

  // Create forest of nodes
  Node forest[uniqueChars];
  for (int i=0, j=0; i<256; i++) {
    if (frequencies[i] > 0) {
      forest[j].ch = i;
      forest[j].frequency = frequencies[i];
      forest[j].parentNum = -1;
      forest[j].leftChild = NULL;
      forest[j].rightChild = NULL;
      j++;
    }
  }

  // Create a minimum queue and fill it with nodes for each character for use in building Huffman tree.
  Node huffmanQueue[uniqueChars];
  for (int i=0; i<uniqueChars; i++) {
    insert(huffmanQueue, forest[i]);
  }

  // Variables to help build Huffman tree
  Node parents[uniqueChars - 1];
  Node rChild;    
  Node lChild;
  int rN, lN, rFreq, lFreq;
  int numCombinations = 0;
  int isLParent = 0, isRParent = 0;

  // Build Huffman tree by combining two lowest frequency nodes
  while (size >= 2) {
    rChild = removeLeastFrequent(huffmanQueue);
    lChild = removeLeastFrequent(huffmanQueue);

    // Check if right child is a combination of nodes
    // If yes, it will be found in the parents array
    // If no, it will be found in the forest array
    if (rChild.ch == 999) {
        isRParent = 1;
        rN = rChild.parentNum;
      } else {
        isRParent = 0;
        for (int i=0; i<uniqueChars; i++) {
          if (rChild.ch == forest[i].ch) rN = i;
        }
      }

      // Check if left child is a combination of nodes
      // If yes, it will be found in the parents array
      // If no, it will be found in the forest array
      if (lChild.ch == 999) {
        isLParent = 1;
        lN = lChild.parentNum;
      } else {
        isLParent = 0;
        for (int i=0; i<uniqueChars; i++) {
          if (lChild.ch == forest[i].ch) lN = i;
        }
      }

    // Mark node as a parent node
    parents[numCombinations].ch = PARENT;
    parents[numCombinations].parentNum = numCombinations;

    // Set right child and get its frequency
    if (isRParent) {
      rFreq = parents[rN].frequency;
      parents[numCombinations].rightChild = &parents[rN];
    } else {
      rFreq = forest[rN].frequency;
      parents[numCombinations].rightChild = &forest[rN];
    }

    // Set left child and get its frequency
    if (isLParent) {
      lFreq = parents[lN].frequency;
      parents[numCombinations].leftChild = &parents[lN];
    } else {
      lFreq = forest[lN].frequency;
      parents[numCombinations].leftChild = &forest[lN];
    }
    
    // Combine children's frequencies into parent's frequency
    parents[numCombinations].frequency = rFreq + lFreq;
 
    // Put the new combined node into the queue
    insert(huffmanQueue, parents[numCombinations]);
    numCombinations++;
  }

//********************************************************************************************************
// STEP THREE: Parse Huffman tree and generate code for each character
//********************************************************************************************************
  
  // Get the codes for each character
  Node huffmanRoot = removeLeastFrequent(huffmanQueue);
  getCodes(&huffmanRoot);

  // Print characters with their frequencies and codes if debug is on
  if (debug) {
    // Print file size
    int sizeOfFile = huffmanRoot.frequency;
    printf("%s length %d\n", inputFilename, sizeOfFile);

    // Print characters with their frequencies and codes
    printTree(&huffmanRoot);
  }

//********************************************************************************************************
// STEP FOUR: Read input file again, and for each ASCII code, output its Huffman code
//********************************************************************************************************  

  // Reopen input file
  inputFile = fopen(inputFilename, "r");

  // Open the output file
  if (outputFilename == NULL) outputFilename = "huffman.out"; // Default output file
  FILE *outputFile = fopen(outputFilename, "w");

  char buffer = 0;  // Buffer for storing bits
  int bitCount = 0; // Number of bits in the buffer
  char *currentCode; // Code for the scanned character

  // While there are characters to be read
  while ((ch = fgetc(inputFile)) != EOF) {
    // Get the code for the character
    currentCode = codes[ch];

    // Append each bit of the code to the buffer
    for (int i = 0; currentCode[i] != '\0'; i++) {
        buffer <<= 1;  // Shift bits in buffer left
        buffer |= currentCode[i] - '0';  // Add the current bit (0 or 1)
        bitCount++;

      if (bitCount == 8) {
        fwrite(&buffer, 1, 1, outputFile);
        buffer = 0;
        bitCount = 0;
      }
    }
  }

  // If buffer still has bits, write them with padding
  if (bitCount > 0) {
      buffer <<= (8 - bitCount);  // Add padding bits
      fwrite(&buffer, 1, 1, outputFile);
  }

  // Close files
  fclose(inputFile);
  fclose(outputFile);

  return 0;
}
