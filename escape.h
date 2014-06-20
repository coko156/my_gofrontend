// escape.h -- Go frontend escape analysis.     -*- C++ -*-
#ifndef GO_ESCAPE_H
#define GO_ESCAPE_H

#include "gogo.h"

#include <vector>
#include <map>

// Firstly get the minimal set of function to run escape analysis on.
// That is to find strong connected component.
// Run escapes on total IR.
//
// Then build the connection graph for each set of function.
//
// Then Walk the graph to give the variables a tag.

class Escape_analysis 
{
  public:
  enum Function_status {
    // used to mark the status of function been analysised
    ESCAPE_FUNCTION_UNKNOWN = 0,
    ESCAPE_FUNCTION_PLANNED,
    ESCAPE_FUNCTION_STARTED,
    ESCAPE_FUNCTION_FINISHED
  };


    // 总调函数
    static void
    escapes(Gogo*)  { }

    // traverse the Gogo* and get a list of functions
    void
    compute_gogo_to_functions(Gogo*);

  private:

    typedef std::vector<Named_object*> Named_object_vec;

    // map<caller, callee_vector>
    typedef std::map<const Named_object*, Named_object_vec> Caller_map;

    // used to store functions in a package.
    Named_object_vec functions_;
};
