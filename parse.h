#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Separates a string by a delimiter and by newline, returns the separated pieces in an array; the final entry and delimiters specified becomes NULL
 * 
 * @param string String to be seperated by a delimiter and newline. The string will be manipulated in the process.
 * @param delim Delimiter character to be used.
 * @return char**: Array of strings that are seperated.
 */
char ** parse_args(char *string, char delim);
/**
 * @brief Counts the number of occurrences of a certain delimiter, plus one (for mallocing purposes). This function must be run in order to get the malloc size needed.
 * 
 * @param string String that will be looped through to count the number of delimiters that appear.
 * @param delim Delimiter character to be used 
 * @return int: Number of occurrences of a certain delimiter, plus one.
 */
int countDelimiters(char *string, char delim);
//
/**
 * @brief Calculates the total length of a string that has been separated into an array.
 * 
 * @param array The array to loop through to count the length of the seperated array.
 * @return int: The total length of a string that has been separated into an array.
 */