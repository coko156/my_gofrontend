// escape.h -- Go frontend escape analysis.     -*- C++ -*-
#ifndef GO_ESCAPE_H
#define GO_ESCAPE_H

#include "gogo.h"

#include <vector>
#include <map>

class Named_object;
class Expression;
class Call_expression;


// escapes(Gogo*) is a function to be called in optimize.
// and then we get a set of ordered functions
//
//   build a connection graph on each set of function
// 
//   Initialization: for one set, make it ordered
//   then dig into it and :
//      1. analysis parameters(can build the graph directly)
//      2. analysis on inner expressions
//   for each expression/statement, decide to build what edge.
//
//  mark each reference with level ++
//  and each dereference with level --
//    when a variable's level == 0 means that hit the root variable(or object)
//    and if the loopdepth of current variable is defferent from the object's
//    we can mark current variable and all its upstream ESCAPE_HEAP
//    also all upstreams of a fake node marked ESCAPE_HEAP

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

    // add a edge between caller and callee function
    void
    add_caller_callee(const Named_object* caller, const Named_object* callee);

	void add_function(Named_object * no)
	{ this->functions.insert(no); }

  private:

    typedef std::vector<Named_object*> Named_object_vec;
    typedef std::set<Named_object*>    Named_object_set;

    // map<caller, callee_set>
    typedef std::map<const Named_object*, Named_object_set> Caller_map;

    // used to store functions in a package.
    Named_object_set functions_;

    Caller_map edge;
};
