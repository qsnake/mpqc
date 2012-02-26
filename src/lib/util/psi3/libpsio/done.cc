/*!
** \file done.cc
** \ingroup (PSIO)
*/

#include <stdlib.h>
#include <util/psi3/libpsio/psio.h>

#ifdef PSIO_STATS
#include <time.h>
#endif

namespace psi3 {
namespace libpsio {

/*!
** \ingroup (PSIO)
**
** PSIO_DONE(): Frees global data used by the I/O routines.
**
** No arguments.
*/

int psio_done(void)
{
#ifdef PSIO_STATS
  int i;
  ULI total_read=0, total_write=0;
  FILE *io_out;
  time_t my_time;
  my_time = time(NULL);
  io_out = fopen("psio.dat", "a+");
  fprintf(io_out, "\nLIBPSIO Read/Write Statistics\n\n");
  fprintf(io_out, "Run at: %s\n", ctime(&my_time));
  fprintf(io_out, "Unit      Read(kB)    Write(kB)\n");
  fprintf(io_out, "-------------------------------\n");
  for(i=0; i < PSIO_MAXUNIT; i++) {
      total_read += psio_readlen[i];
      total_write += psio_writlen[i];
      
      if(psio_readlen[i] || psio_writlen[i])
	  fprintf(io_out, "%3d   %10.1f   %10.1f\n",i,
		  ((double) psio_readlen[i])/((double) 1024),
		  ((double) psio_writlen[i])/((double) 1024));
    }
  fprintf(io_out, "-------------------------------\n");
  fprintf(io_out, "Total %10.1f   %10.1f\n",
	  ((double) total_read)/((double) 1024),
	  ((double) total_write)/((double) 1024));
  fclose(io_out);
  free(psio_readlen);
  free(psio_writlen);
#endif  

  free(psio_unit);

  _psi3_libpsio_state_ = 0;

  return(0);
}

}
}
