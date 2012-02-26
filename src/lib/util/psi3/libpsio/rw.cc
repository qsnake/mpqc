/*!
   \file rw.c
   \ingroup (PSIO)
*/

#include <stdio.h>
#include <unistd.h>
#include <util/psi3/libpsio/psio.h>

namespace psi3 {
namespace libpsio {

/*!
** PSIO_RW()
**
** \ingroup (PSIO)
*/
int psio_rw(unsigned int unit, char *buffer, psio_address address, ULI size, int wrt)
{
  int errcod;
  unsigned int i;
  ULI errcod_uli;
  ULI page, offset;
  ULI buf_offset;
  ULI this_page, this_page_max, this_page_total;
  unsigned int first_vol, this_vol, numvols;
  ULI bytes_left, num_full_pages;
  psio_ud *this_unit;

  this_unit = &(psio_unit[unit]);
  numvols = this_unit->numvols;
  page = address.page;
  offset = address.offset;
  
  /* Seek all volumes to correct starting positions */
  first_vol = page % numvols;
  errcod = psio_volseek(&(this_unit->vol[first_vol]), page, offset, numvols);
  if(errcod == -1) psio_error(unit,PSIO_ERROR_LSEEK);
  for(i=1,this_page=page+1; i < numvols; i++,this_page++) {
      this_vol = this_page % numvols;
      errcod = psio_volseek(&(this_unit->vol[this_vol]), this_page, 
                            (ULI) 0, numvols);
      if(errcod == -1) psio_error(unit,PSIO_ERROR_LSEEK);
    }

  /* Number of bytes left on the first page */
  this_page_max = PSIO_PAGELEN - offset;

  /* If we have enough room on this page, use it */
  if(size <= this_page_max)  this_page_total = size;
  else this_page_total = this_page_max;

  buf_offset = 0;
  if(wrt) {
      errcod_uli = write(this_unit->vol[first_vol].stream, &(buffer[buf_offset]),
		     this_page_total);
      if(errcod_uli != this_page_total) psio_error(unit,PSIO_ERROR_WRITE);
    }
  else {
      errcod_uli = read(this_unit->vol[first_vol].stream, &(buffer[buf_offset]),
		    this_page_total);
      if(errcod_uli != this_page_total) psio_error(unit,PSIO_ERROR_READ);
    }

  /* Total number of bytes remaining to be read/written */
  bytes_left = size - this_page_total;

  /* Read/Write all the full pages */
  num_full_pages = bytes_left/PSIO_PAGELEN;  
  buf_offset += this_page_total;
  for(i=0,this_page=page+1; i < num_full_pages; i++,this_page++) {
      this_vol = this_page % numvols;
      this_page_total = PSIO_PAGELEN;
      if(wrt) {
	  errcod_uli = write(this_unit->vol[this_vol].stream, &(buffer[buf_offset]),
			 this_page_total);
	  if(errcod_uli != this_page_total) psio_error(unit,PSIO_ERROR_WRITE);
	}
      else {
	  errcod_uli = read(this_unit->vol[this_vol].stream, &(buffer[buf_offset]),
			this_page_total);
	  if(errcod_uli != this_page_total) psio_error(unit,PSIO_ERROR_READ);
	}
      buf_offset += this_page_total;
    }

  /* Read/Write the final partial page */
  bytes_left -= num_full_pages * PSIO_PAGELEN;
  this_vol = this_page % numvols;
  if(bytes_left) {
      if(wrt) {
	  errcod_uli = write(this_unit->vol[this_vol].stream, &(buffer[buf_offset]),
			 bytes_left);
	  if(errcod_uli != bytes_left) psio_error(unit,PSIO_ERROR_WRITE);
	}
      else {
	  errcod_uli = read(this_unit->vol[this_vol].stream, &(buffer[buf_offset]),
			 bytes_left);
	  if(errcod_uli != bytes_left) psio_error(unit,PSIO_ERROR_READ);
	}
    }

  return(0);
}

}
}
