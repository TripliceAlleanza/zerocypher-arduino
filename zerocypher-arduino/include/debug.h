#define DEBUG
#define DEBUG_OUTPUT Serial

#ifdef DEBUG
#define DEBUG_PRINTLN(...) DEBUG_OUTPUT.println(__VA_ARGS__)
#define DEBUG_PRINT(...) DEBUG_OUTPUT.print(__VA_ARGS__)
#define MATRIX_DEBUG(matrix, rows, columns) printMatrix(matrix, rows, columns)
#else
#define DEBUG_PRINTLN(...) {}
#define DEBUG_PRINT(...) {}
#define MATRIX_DEBUG(matrix, rows, columns) {}
#endif