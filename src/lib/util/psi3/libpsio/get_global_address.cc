/*!
   \file get_global_address.c
   \ingroup (PSIO)
*/

#include <util/psi3/libpsio/psio.h>

namespace psi3 {
namespace libpsio {

/*!
** PSIO_GET_GLOBAL_ADDRESS(): Given the global starting address for a
** TOC entry and a relative offset within the entry, compute the global
** address for the offset.
** 
** \ingroup (PSIO)
*/

psio_address psio_get_global_address(psio_address entry_start,
				     psio_address rel_address)
{
  psio_address address;

  address.page = entry_start.page + rel_address.page;
  address.offset = entry_start.offset + rel_address.offset;
  if((entry_start.offset + rel_address.offset) >= PSIO_PAGELEN) {
     address.offset -= PSIO_PAGELEN;
     address.page++;
     }

  return(address);
}

}
}
