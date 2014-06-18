#include "myEscape.h"

void 
Escape_analysis_graph :: analysis_graph()
{
	//TODO add Tarjan() algorithm
	std::map<Named_object,int> :: iterator it = node_set_.begin();
	for ( ; it != node_set_.end(); it++) {
		Named_object * obj = it->first;
		int id = it->second;
		printf("%d %d\n", id, check_object_escape(id));
	}
}
// perform after SCC analysis, so it's not necessary to check circuit.
bool 
Escape_analysis_graph :: check_object_escape(int id) 
{
	Named_object * obj = this->id_to_object[id];
	if (obj->var_value()->is_global()) return true;

	for (std::vector<int,int>::iterator e = edge_set_.begin(); 
		 e != edge_set_.end(); e++) if (e->first == id) {
			int to = e->second;
			if (check_object_escape(to)) return true;
		 }
	return false;
}

void 
Escape_analysis_assignments :: build_escape_analysis_graph(
	Escape_analysis_graph * graph, Expression ** plhs, Expression ** prhs) 
{
	Expression * l = *plhs, * r = *prhs;
	if (l->classification == EXPRESSION_VAR_REFERENCE) {			// FIXME 
			Named_object * na = l->var_expression()->named_object() ;
			Named_object * nb = r->var_expression()->named_object() ;
			// if (!na->is_variable() || !nb->is_variable()) break;
			int sid = graph->get_node_id(nb) ;
			int tid = graph->get_node_id(na) ;
			graph->add_edge(sid, tid);
		}
}

