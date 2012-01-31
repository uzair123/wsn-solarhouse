/**
 * \file
 *         TikiDB shell
 * \author
 *         Pablo Guerrero <guerrero@dvs.tu-darmstadt.de>
 */

#include "contiki.h"
#include "shell.h"
#include "serial-shell.h"
#include "shell-tikidb.h"
//#include "shell-base64.h"
//#include "shell-text.h"

/*---------------------------------------------------------------------------*/
PROCESS(tikidb_shell_process, "TikiDB shell");
AUTOSTART_PROCESSES(&tikidb_shell_process);
PROCESS_THREAD(tikidb_shell_process, ev, data)
{
  PROCESS_BEGIN();

  serial_shell_init();
//  shell_base64_init();
//  shell_text_init();
  shell_tikidb_init();
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
