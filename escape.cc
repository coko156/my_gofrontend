#include "escape.h"


class Escape_analysis;
class Call_graph_traverse_functions;
class Call_graph_traverse_expressions;


void 
Escape_analysis::escape(Gogo* gogo)
{
  this->compute_gogo_to_functions(gogo);
}

Escape_analysis::compute_gogo_to_functions(Gogo* gogo)
{
  Call_graph_traverse_functions cgtf(this);
  gogo->traverse(&cgtf);
  // now this->function_ is a list of functions.
}

void
Escape_analysis::add_caller_callee(const Named_object* caller, const Named_object* callee)
{
  this->edge[caller].insert(callee);
}

// used to traverse Gogo to function
class Call_graph_traverse_functions : public Traverse
{
  public:
    Call_graph_traverse_functions(Escape_analysis* ea) :
      Traverse(traverse_functions),
      ea_(ea)
  { }

  protected:
      int
      function(Named_object*);

  private:
      Escape_analysis* ea_;
}

int
Call_graph_traverse_functions::function(Named_object* no)
{
  this->ea_->add_function(no);

  go_assert(no->is_function());
  Function* func = no->func_value();

  Call_graph_traverse_expressions cgte(this->es_, no);
  func->traverse(&cgte);

  return TRAVERSE_CONTINUE;
}

// traverse the function
// This class is used to get the map<caller, callee_vector>
class Call_graph_traverse_expressions : public Traverse
{
  public:
    // Constructor
    Call_graph_traverse_expressions(Escape_analysis *ea,
        Named_object* function) :
      Traverse(traverse_expressions),
      ea_(ea), function_(function)
  { }

  protected:
    int
    expression(Expression**);

  private:
    Escape_analysis* ea_;
    Named_object* function_;
}

int
Call_graph_traverse_expressions::expression(Expression** expr)
{
  Call_expression* call_expression = (*expr)->call_expression();

  if (call_expression == NULL)
    return TRAVERSE_CONTINUE;

  const Named_object* called_function = call_expression->get_function_object();

  if (called_function != NULL) {
    ea->add_caller_callee(this->function_, called_function);
  }
  //TODO now return EXIT
  // return TRAVERSE_CONTINUE;
  return TRAVERSE_EXIT;
}
