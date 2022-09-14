# OpenMP_ImageHistogramCalculation

A program that calculates the histogram of a grayscale image. The program supports gray images (PNM P5 option).

Arguments are passed to the program through the command line:
<name_input_file> <name_output_file> <number_of_threads>

Input data format:
Image in Netpbm P5 format (grayscale in binary), maximum element value is 255.

Output format:
A binary array of 32-bit unsigned integers in little endian format.
