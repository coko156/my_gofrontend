
class Escape_analysis {
	// FIXME : 感觉很别扭，这个函数
	// 我估计是一个Named_object对应一个Escape_analysis，然后这里的参数no就是当前this对应的Named_object.
	// Initialize the escape information for each function.
	Escape_analysis_info * initialize_escape_info(Named_object* no) {
		Escape_analysis_info* escape_info = new Escape_analysis_info(this);
		this->escape_info_map_[no] = escape_info;
		return escape_info;
	}

	// 拓扑排序function
	void compute_functions_to_process(Gogo* gogo) ; 
	
	// 一个function一个function地建连通图，并分析，相当与逃逸分析的主函数了
	// Perform the analysis in the prescribed function order. 
	void compute_analysis_results() ;
}

class Escape_analysis_info {
	// FIXME : 我觉得这构造函数有问题
	Escape_analysis_info(Escape_analysis* escape_analysis)
	: escape_analysis_(escape_analysis), updated_(false), aborted_(false) { }

	// 连通图分析以及标记是否逃逸的主过程，联系Escape_analysis::compute_analysis_results()来看
	// 因为是Escape_analysis::compute_analysis_results调用的
	propagate_escape_level();
}
