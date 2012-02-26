/*!
   \file tocscan.c
   \ingroup (PSIO)
*/

#include <string.h>
#include <util/psi3/libpsio/psio.h>

namespace psi3 {
namespace libpsio {

/*!
** PSIO_TOCSCAN(): Scans the TOC for a particular keyword and returns either
** a pointer to the entry or NULL to the caller.
**
** \ingroup (PSIO)
*/

psio_tocentry *psio_tocscan(unsigned int unit, char *key)
{
  psio_tocentry *this_entry;

  if(key == NULL) return(NULL);

  this_entry = psio_unit[unit].toc;

  while(this_entry != NULL) {
      if(!strcmp(this_entry->key,key)) return(this_entry);
      this_entry = this_entry->next;
    }

  return(NULL);
}

}
}
