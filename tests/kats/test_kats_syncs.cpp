
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <iomanip>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"

#include "madara/kats/TestFramework.h"
#include "madara/utility/Utility.h"

Madara::KATS::Settings settings;

// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);


int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_INFO | LM_DEBUG, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  // use ACE real time scheduling class
  int prio  = ACE_Sched_Params::next_priority
    (ACE_SCHED_FIFO,
     ACE_Sched_Params::priority_max (ACE_SCHED_FIFO),
     ACE_SCOPE_THREAD);
  ACE_OS::thr_setprio (prio);

  // differentiate our domain from the barrier test. This will allow
  // a urser to run both of the tests at the same time.
  settings.domains = "kats_syncs";

  Madara::KATS::TestFramework testing (settings);

  if (settings.id == 0)
  {
    // go through the three events without joining in the barriers
    // but set our barrier information as done, so that when we sync
    // everything should be okay.
    testing.event ("first", "", "MADARA.BARRIER.first.{.madara.id} = 1",
      "", "", false, true);

    testing.event ("second", "", "MADARA.BARRIER.second.{.madara.id} = 1",
      "", "", false, true);

    // on the third event, send out 
    testing.event ("third", "", "MADARA.BARRIER.third.{.madara.id} = 1",
      "", "", false, true);

    // sleep for 10 seconds without a transport
    madara::utility::sleep (10);

    testing.sync ();

    testing.event ("final", "second.done && third.done", "",
      "", "", false, false);
  }
  else
  {
    testing.event ("first", "", "",
      "", "", true, false);

    testing.event ("second", "", "",
      "", "", true, false);

    // our second barrier is done, which is relevant to id == 0
    if (settings.id == settings.processes - 1)
      testing.event ("second.done", "", "second.done = 1",
        "", "", false, false);

    testing.event ("third", "", "",
      "", "", true, false);

    // our third barrier is done, which is relevant to id == 0
    if (settings.id == settings.processes - 1)
      testing.event ("third.done", "", "third.done = 1",
        "", "", false, false);
  }

  testing.barriered_sync ("finished");

  testing.dump ();

  return 0;
}

int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("n:i:o:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("processes"), 'n', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
//  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 'i':
      // id
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.id;
      }

      ACE_DEBUG ((LM_INFO, "-i = %s; id is now %d\n", 
      cmd_opts.opt_arg (), settings.id ));

      break;
    case 'n':
      // processes
      {
        std::stringstream buffer;
        buffer << cmd_opts.opt_arg ();
        buffer >> settings.processes;
      }

      ACE_DEBUG ((LM_INFO, "-n = %s; processes is now %d\n", 
        cmd_opts.opt_arg (), settings.processes ));

      break;
    case 'o':
      // thread number
      settings.host = cmd_opts.opt_arg ();

      ACE_DEBUG ((LM_INFO, "-o = %s; host is now %s\n", 
        cmd_opts.opt_arg (), settings.host.c_str () ));

      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -n (--processes)   number of testing processes \n\
      -i (--id)          this process id        \n\
      -o (--host)        host identifier        \n\
      -h (--help)        print this menu        \n"
      ));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu\n")), -1); 
      break;
    }
  }

  return 0;
}

