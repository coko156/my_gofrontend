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
//   Initialization: for one set, make it ordered
//   then dig into the functions and find flow(dst, src)
//   For variables assigned to an outer scope variable, or used as return we
//   store a flow(theSink, src). theSink is a abstract of outer scope.
//
//   Walk from theSink, and mark all variables it can reach escape
//   More details needed when special conditions.
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

    // Compute the analysis results for the current package
    void compute_analysis_result();

    // Build CFG and compute escape level
    void compute_escape_level()

      int visit(Escape_analysis_object *, std::stack<Escape_analysis_object*> & );

    void make_tag(Escape_analysis_object *, std::stack<Escape_analysis_object*> & );

    // Initialize the escape information for each function.
    Escape_analysis_info * 
      initialize_escape_info(Named_object *);

    // add a edge between caller and callee function
    void
      add_caller_callee(Named_object* caller, const Named_object* callee);

    void add_function(Named_object * no)
    { this->functions.insert(no); }

  private:

    typedef std::vector<Named_object*> Named_object_vec;
    typedef std::set<Named_object*>    Named_object_set;
    typedef std::set<Escape_analysis_object*> Escape_object_set;
    typedef std::map<Escape_analysis_object*, Escape_analysis_object*> Escape_object_map;

    // map<caller, callee_set>
    typedef std::map<const Named_object*, Named_object_set> Caller_map;

    // used to store functions in a package.
    Named_object_set functions_;

    Caller_map edge;

    // function map to Escape_analysis_info
    std::map<Named_object*, Escape_analysis_info*> escape_info_map_;

    // used to store Escape_analysis_objects in a package
    Escape_object_set esc_objs_;

    // used to label the SCC that a escape_analysis_object refer.
    Escape_object_map SCC;
};


class Escape_analysis_object 
{
  public :
    void set_escape_level(int level)
    { this->escape_level = level; }
    int escape_level()
    { return this->escape_level; }

  private :
    int walkgen;

    int escape_level;

    // FIXME. should separate edge into defer edge and pointsto edge
    std::set<Escape_analysis_object*> edge;
}
