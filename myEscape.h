
class Escape_analysis_graph {
	public :
		int get_node_id(Named_object * obj)
		{
			std::map<Named_object*,int> &_map = this->node_set();
			if (_map.find(obj) == _map.end()) {
				_map.insert( pair<Named_object,int>(obj, _map.size()) );
				id_to_object[_map[obj]] = obj;
			}

			return _map[obj];
		}
		void add_edge(int sid, int tid) 
		{
			this->edge_set_.push_back(std::pair<int,int> (sid,tid)) ;
		}
		void analysis_graph() ;

	private : 
		std::map<Named_object,int> node_set_ ;
		std::map<int,Named_object> id_to_object ;
		std::vector< pair<int,int> >  edge_set_ ;
		bool check_object_escape(int id) ;
}


class Escape_analysis_object 
{
	public :
		Escape_analysis_object(const Named_object * obj, bool is_escape_ = false)
			: obj(obj) , is_escape_(is_esacpe_), 
		{ }
		
		bool is_escape() { return is_escape_ ; }
		void set_escape(bool esc) { is_escape_ = esc; }

	private :
		Named_object * obj ;
		bool is_escape_ ;
}

class Escape_analysis_assignments 
{
	public :
		// This is called for each assignment made by the statement.  PLHS
		// points to the left hand side, and PRHS points to the right hand
		// side.  PRHS may be NULL if there is no associated expression, as
		// in the bool set by a non-blocking receive.
		Escape_analysis_assignments( Escape_analysis_graph * graph,
			Expression ** plhs, Expression ** prhs )
		{
			build_escape_analysis_graph(graph, plhs, prhs) ;
		}

	private :
		void build_escape_analysis_graph(graph, plhs, prhs) ;
}



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
