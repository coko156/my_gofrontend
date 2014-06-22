#include "escape.h"


class Escape_analysis;
class Call_graph_traverse_functions;
class Call_graph_traverse_expressions;


void 
Escape_analysis::escape(Gogo* gogo)
{
  this->compute_gogo_to_functions(gogo);
  this->compute_analysis_result();
}

Escape_analysis::compute_gogo_to_functions(Gogo* gogo)
{
  Call_graph_traverse_functions cgtf(this);
  gogo->traverse(&cgtf);
  // now this->function_ is a set of functions.
}

// Compute the analysis results for the current package
void compute_analysis_result()
{
	for (Named_object_set::iterator p = this->functions_.begin();
		 p != this->functions_.end();
		 p++)
	{
		Named_object * no = *p;
		go_assert(no->is_function());
		Function * func = no->func_value();

		Escape_analysis_info * esc_info = this->initialize_escape_info(no);

		// Perform the analysis for a single function
		Escape_analysis_traverse_statements cgts(esc_info);
		func->traverse(&cgts);
	}
	
	//TODO. Now it perform as single function, all the function perform together next step
	// I would consult the implementation of escape_analysis in Go compiler.
	// Analysis the connected component graph, which nodes refer Named_object/Var, edges refer
	// the relationship of reference between Named_object/Var/so on.

	this->compute_escape_level();
}

// Initialize the escape information for each function.
Escape_analysis_info *
Escape_analysis::initialize_escape_info(Named_object * no)
{
	Escape_analysis_info * escape_info = new Escape_analysis_info(this);

	this->escape_info_map_[no] = escape_info;

	return escape_info;
}

void 
Escape_analysis::compute_escape_level()
{
	// TODO. didn't create esc_objs_ yet
	// esc_objs_ should contains all the escape_analysis_objects appear in 
	// present environment. class Escape_analysis is just like a environment
	
	for (Esc_object_set::iterator p = this->esc_objs_.begin();
		 p != this->esc_objs_.end();
		 p++)
		(*p)->walkgen = 0;

	std::stack<Escape_analysis_object*> _stack;
	this->visitstamp = 0;
	for (Esc_object_set::iterator p = this->esc_objs_.begin();
		 p != this->esc_objs_.end();
		 p++)
		this->visit(*p, _stack);

	for (Esc_object_set::iterator p = this->esc_objs_.begin();
		 p != this->esc_objs_.end();
		 p++)
		(*p)->walkgen = 0;
}

// Tarjan algorithm
int 
Escape_analysis::visit(Escape_analysis_object * n, std::stack<Escape_analysis_object> & _stack)
{
	if (n->walkgen > 0) 
		return n->walkgen;

	this->timestamp++;
	n->walkgen = timestamp;
	int dfn = n->walkgen;
	_stack.push(n);
	
	// FIXME. Maybe the edges contain 2 types of edge, defer edge and pointsto edge,
	// but now I named them as "edge".
	// I will separate them later.
	for (std::set<Escape_analysis_object*>::iterator p = n->edge.begin();
		 p != n->edge.end();
		 p++)
	{
		Escape_analysis_object * to = *p;
		if (to->walkgen != 0) 
		{
			n->walkgen = std::min(n->walkgen, to->walkgen);
		}
		else 
		{
			this->visit(to, _stack);
			n->walkgen = std::min(n->walkgen, to->walkgen);
		}
	}

	if (dfn == n->walkgen)
	{
		// TODO. n is a root of a strong connected block
		// so make escape level start from n
		this->make_escape_level(n, _stack);
	}

	return n->walkgen;
}

void make_escape_level(Escape_analysis_object * n,
	std::stack<Escape_analysis_object *> & _stack)
{
	Escape_analysis_object * obj;
	int level = n->escape_level();
	std::vector<Escape_analysis_object*> v;
	do 
	{
		obj = _stack.top();
		v.push_back(obj);
		_stack.pop();
		if (level < obj->escape_level())
			level = obj->escape_level();
	}
	while (n != obj);
	for (std::vector<Escape_analysis_object*>::iterator p = v.begin();
		 p != v.end();
		 p++)
	{
		(*p)->set_escape_level(level);
	}
}

void
Escape_analysis::add_caller_callee(Named_object* caller, 
	const Named_object* callee)
{
  this->edge[caller].insert(callee);
}


class Escape_analysis_traverse_statements : public Traverse
{
  public : 
	Escape_analysis_traverse_statements(Escape_analysis_info * esc_info)
	  : Traverse(traverse_statements), esc_info_(esc_info)
	{ }

  protected :
  	int statement(Block*, size_t* pindex, Statement*);

  private :
  	Escape_analysis_info * esc_info_;
}
Escape_analysis_traverse_statements::statement(Block * block,
	size_t * pindex, Statement * statement)
{
	Escape_analysis_traverse_assignments eata(this->esc_info_, statement);
	statement->traverse_assignments(&eata);
	// next add expression traverse

	return TRAVERSE_CONTINUE;
}

class Escape_analysis_traverse_assignment : public Traverse
{
  public :
 	Escape_analysis_traverse_assignment(Escape_analysis_info * esc_info,
		Statement * statement)
	  :	esc_info_(esc_info), statement_(statement)
	{ }

  protected :
   	int assignement(Expression ** lhs, Expression ** rhs) ;

  private :
  	Escape_analysis_info * esc_info_;
	Statement * statement_;
}

// TODO
Escape_analysis_traverse_assignment::assignment(Expression ** lhs, Expression ** rhs)
{
	//..
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
