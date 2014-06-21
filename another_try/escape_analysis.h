#ifndef ESCAPE_ANALYSIS_H__
#define	ESCAPE_ANALYSIS_H__


class Escape_analysis
{
  public :
	void compute_functions_to_process(Gogo * gogo);
	void compute_analysis_results();
	void perform(Gogo * gogo);
	Escape_analysis_info * initialize_escape_info(Named_object *);

  private :
  	Named_object_set functions_;
	Named_object_vector sorted_functions_;
	Named_object_set safe_functions_;
	std::map<Named_object,Escape_analysis_info*> escape_info_map_;
}


#endif
