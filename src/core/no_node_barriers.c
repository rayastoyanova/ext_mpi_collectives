#include "no_node_barriers.h"
#include "constants.h"
#include "read.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ext_mpi_generate_no_node_barriers(char *buffer_in, char *buffer_out) {
  int nbuffer_out = 0, nbuffer_in = 0, i, flag;
  char line[1000];
  enum eassembler_type estring1;
  struct parameters_block *parameters;
  nbuffer_in += i = ext_mpi_read_parameters(buffer_in + nbuffer_in, &parameters);
  if (i < 0)
    goto error;
  if (parameters->node_row_size*parameters->node_column_size != 1){
    printf("ext_mpi_generate_no_node_barriers only for 1 task per node\n");
    exit(2);
  }
  nbuffer_out += ext_mpi_write_parameters(parameters, buffer_out + nbuffer_out);
  do {
    nbuffer_in += flag =
        ext_mpi_read_line(buffer_in + nbuffer_in, line, parameters->ascii_in);
    if (flag) {
      if (ext_mpi_read_assembler_line_s(line, &estring1, 0) >= 0) {
        if (estring1 != enode_barrier) {
          nbuffer_out += ext_mpi_write_line(buffer_out + nbuffer_out, line,
                                            parameters->ascii_out);
        }
      }
    }
  } while (flag);
  ext_mpi_write_eof(buffer_out + nbuffer_out, parameters->ascii_out);
  ext_mpi_delete_parameters(parameters);
  return nbuffer_out;
error:
  ext_mpi_delete_parameters(parameters);
  return ERROR_MALLOC;
}
