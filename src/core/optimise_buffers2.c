#include "optimise_buffers2.h"
#include "constants.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ext_mpi_generate_optimise_buffers2(char *buffer_in, char *buffer_out) {
  int nbuffer_out = 0, nbuffer_in = 0, nbuffer_out_start, flag, flag2, flag3,
      o1, o2, size, o1_, bo1, bo2, bo1_, size_, partner, num_comm, nline2,
      round = 0, flag_loop, i;
  char line[1000], line2[1000], *buffer_in_loop, *buffer_out_loop, *buffer_temp,
      *buffer_in_start, *buffer_out_start;
  struct parameters_block *parameters;
  enum eassembler_type estring1, estring2, estring3, estring4, estring5,
      estring1_, estring2_, estring3_, estring1__;
  nbuffer_in += ext_mpi_read_parameters(buffer_in + nbuffer_in, &parameters);
  nbuffer_out += ext_mpi_write_parameters(parameters, buffer_out + nbuffer_out);
  nbuffer_out_start = nbuffer_out;
  buffer_in_start = buffer_in + nbuffer_in;
  buffer_out_start = buffer_out + nbuffer_out;
  buffer_temp = (char *)malloc(MAX_BUFFER_SIZE);
  if (buffer_temp == NULL)
    goto error;
  do {
    flag_loop = 0;
    if (round == 0) {
      buffer_in_loop = buffer_in_start;
      buffer_out_loop = buffer_out_start;
    } else if (round % 2) {
      buffer_in_loop = buffer_out_start;
      buffer_out_loop = buffer_temp;
    } else {
      buffer_in_loop = buffer_temp;
      buffer_out_loop = buffer_out_start;
    }
    nbuffer_in = nbuffer_out = 0;
    nline2 = -1;
    do {
      nbuffer_in += flag3 =
          ext_mpi_read_line(buffer_in_loop + nbuffer_in, line, parameters->ascii_in);
      if (flag3) {
        if (ext_mpi_read_assembler_line_s(line, &estring1, 0) >= 0) {
          flag = 1;
          if (ext_mpi_read_line(buffer_in_loop + nbuffer_in, line2,
                                parameters->ascii_in)) {
            if ((estring1 == eirecv) || (estring1 == eirec_)) {
              i = ext_mpi_read_assembler_line_ssdsdddd(line, &estring1_, &estring2_,
                                                       &bo1_, &estring3_, &o1_, &size_,
                                                       &partner, &num_comm, 0);
              if (i < 0) {
                i = ext_mpi_read_assembler_line_ssdddd(line, &estring1_, &estring2_,
                                                       &o1_, &size_, &partner,
                                                       &num_comm, 0);
                bo1_ = -1;
              }
              if (i >= 0) {
                flag2 = 1;
                nline2 = nbuffer_in;
                while (flag2 && (i = ext_mpi_read_line(buffer_in_loop + nline2, line2,
                                                       parameters->ascii_in))) {
                  nline2 += i;
                  if (ext_mpi_read_assembler_line_s(line2, &estring1__, 0) >= 0) {
                    if (estring1__ == ewaitall) {
                      flag2 = 2;
                    }
                    if ((estring1__ == ereduce) || (estring1__ == ereduc_) ||
                        (estring1__ == ereturn)) {
                      flag2 = 0;
                      nline2 = -1;
                    } else if ((ext_mpi_read_assembler_line_ssdsdd(line2, &estring1, &estring2, &o1, &estring3, &o2, &size, 0) >= 0) && (estring2==erecvbufp)){
                      flag2 = 0;
                      nline2 = -1;
                    } else {
                      if (((estring1__ == ememcpy) ||
                           (estring1__ == ememcp_)) &&
                          (flag2 == 2)) {
                        if (bo1_ >= 0) {
                          if (ext_mpi_read_assembler_line_ssdsdsdsdd(
                                  line2, &estring1, &estring2, &bo1, &estring3,
                                  &o1, &estring4, &bo2, &estring5, &o2, &size,
                                  0) >= 0) {
                            if ((bo2 == bo1_) && (o2 == o1_) &&
                                (size == size_) && (estring2 == eshmempbuffer_offseto)) {
                              nbuffer_out += ext_mpi_write_assembler_line_ssdsdddd(
                                  buffer_out_loop + nbuffer_out, estring1_,
                                  estring2_, bo1, estring3_, o1, size_, partner,
                                  num_comm, parameters->ascii_out);
                              flag = 0;
                              flag_loop = 1;
                            }
                            flag2 = 0;
                          }
                        } else {
                          if (ext_mpi_read_assembler_line_ssdsdd(
                                  line2, &estring1, &estring2, &o1, &estring3,
                                  &o2, &size, 0) >= 0) {
                            if ((o2 == o1_) && (size == size_) &&
                                (estring2 == eshmemp)) {
                              nbuffer_out += ext_mpi_write_assembler_line_ssdddd(
                                  buffer_out_loop + nbuffer_out, estring1_,
                                  estring2_, o1, size_, partner, num_comm,
                                  parameters->ascii_out);
                              flag = 0;
                              flag_loop = 1;
                            }
                            flag2 = 0;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          if (flag && (nbuffer_in != nline2)) {
            nbuffer_out += ext_mpi_write_line(buffer_out_loop + nbuffer_out, line,
                                              parameters->ascii_out);
          }else{
            nline2 = -1;
          }
        }
      }
    } while (flag3);
    nbuffer_out +=
        ext_mpi_write_eof(buffer_out_loop + nbuffer_out, parameters->ascii_out);
    round++;
  } while (flag_loop);
  if (!(round % 2)) {
    buffer_in_loop = buffer_temp;
    buffer_out_loop = buffer_out_start;
    nbuffer_in = nbuffer_out = 0;
    do {
      nbuffer_in += flag =
          ext_mpi_read_line(buffer_in_loop + nbuffer_in, line, parameters->ascii_in);
      if (flag) {
        nbuffer_out += ext_mpi_write_line(buffer_out_loop + nbuffer_out, line,
                                          parameters->ascii_out);
      }
    } while (flag);
    nbuffer_out +=
        ext_mpi_write_eof(buffer_out_loop + nbuffer_out, parameters->ascii_out);
  }
  free(buffer_temp);
  ext_mpi_delete_parameters(parameters);
  return nbuffer_out + nbuffer_out_start;
error:
  free(buffer_temp);
  ext_mpi_delete_parameters(parameters);
  return ERROR_MALLOC;
}
