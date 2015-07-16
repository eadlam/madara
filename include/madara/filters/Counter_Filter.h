

#ifndef _MADARA_FILTERS_COUNTER_FILTER_H_
#define _MADARA_FILTERS_COUNTER_FILTER_H_

/**
 * @file Counter_Filter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for counting filter invocations
 **/

#include <string>
#include <vector>
#include <map>
#include <list>

#include "ace/High_Res_Timer.h"

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/transport/Transport_Context.h"
#include "madara/knowledge_engine/Extern_Function_Variables.h"
#include "madara/knowledge_engine/containers/Integer.h"

#include "Aggregate_Filter.h"

namespace Madara
{
  namespace Filters
  {
    /**
     * Filter for discovering neighboring peers
     **/
    class MADARA_Export Counter_Filter : public Aggregate_Filter
    {
    public:
      /**
      * Constructor
      **/
      Counter_Filter ();

      /**
       * Destructor
       **/
      virtual ~Counter_Filter ();

      /**
       * Adds to the counter
       * @param   records           the aggregate records vector
       * @param   transport_context context for querying transport state
       * @param   vars              context for querying current program state
       **/
      virtual void filter (Knowledge_Map & records,
        const Transport::Transport_Context & transport_context,
        Knowledge_Engine::Variables & vars);

      /**
       * Gets the number of packets filtered
       * @return the number of packets
       **/
      inline Knowledge_Record::Integer get_count ();

      /**
      * Returns the duration of filtering from first to
      * last message in nanoseconds
      * @return test duration in nanoseconds
      **/
      inline Knowledge_Record::Integer get_elapsed ();

      /**
      * Returns the throughput in terms of packets/second
      * @return packet throughput
      **/
      inline double get_throughput ();

    protected:

      /**
       * Tracks if the first timer has been initialized
       **/
      bool initialized_;

      /**
       * A map of discovered peers
       **/
      Knowledge_Record::Integer packets_;

      /**
       * The time to keep record of a peer
       **/
      Knowledge_Record::Integer first_message_;

      /**
      * The time of the last clear of the peer_list
      **/
      Knowledge_Record::Integer last_message_;
    };
  }
}

#include "Counter_Filter.inl"

#endif  // _MADARA_FILTERS_COUNTER_FILTER_H_