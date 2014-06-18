// escape.h -- Go frontend escape analysis.     -*- C++ -*-
#ifndef GO_ESCAPE_H
#define GO_ESCAPE_H




// Used for a connection graph
// A connection graph is for a set of mutually recursive functions.
// Record a status of the whole set
class Escape_analysis
{
  public:
    enum Object_type
    {

    };

    enum Escape_level
    {

    };

    // escapes -> a list of functions -> vector<vector<Named_object*> >
    // <vector<vector<int> >::iterator pp;
    // for (vector<vector<int> > :: iterator :: i1 ;...)
    //   for (vector<int> :: iterator i2 = i1 -> begin(); ...)
    //     if (i2.count() == 1) {
    //     Escape_analysis_func();
    //     ...
    //     } else {
    //     Escape_analysis_func();
    //     ...
    //     }
    // same as escapes
    // used as traverse

    // Travers the whole IR to get a set of functions.
    // Use Tarjan to get the connection components.
    // We build a connection graph for every connection graph.
    static void
    escapes(Gogo* IR);

    // Variables needed.
    // phathom node for escaped node to leak to.
    // a bool variable to record if it is mutually recursived.

    // Record for depth of the environment
    int depth;

    // A list of functions to be analysised.
    vector<<vector<Named_object*> > function_list;
};


class Escape_node
{
  // Record the depth of a 
  int loopdepth;






}
