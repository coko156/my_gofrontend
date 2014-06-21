// typedef : Named_object_vector, Caller_map

// 源代码的Escape_analysis::initialize_escape_info有问题。它接收的
// 参数的Named_object其实是个Function类型，这应该是在分析单个函数之// 前对该Named_object对应的函数所作的初始化。但明显这个初始化不够充// 分吧！

class Escape_analysis {
	
	void perform(Gogo *) ;
	void compute_functions_to_process(Gogo *) ;
	void compute_analysis_results() ;
	Escape_analysis_info * initialize_escape_info(Named_object *) ;
}

// 从Gogo里抠除function并排好序以便处理，存到this->sorted_functions_
void 
Escape_analysis :: compute_functions_to_process(Gogo * gogo)
{
	// 看来有必要注释一下这句..=_=
	// traverse会分析当前环境的所有东西并分类搞好给我们，
	// 只要我们根据需求继承了class Traverse并重写相应的函数即可，
	// 如Call_graph_traverse_functions重写了int function()说明"我
	// 只要function，其它都扔掉吧~"
	Call_graph_traverse_functions cgtf(this) ;
	gogo->traverse(&cgtf) ;

	std::queue<Named_object*> current;

	for (Named_object_set::iterator p = this->functions_.begin();
		 p != this->functions_.end();
		 p++) 
	{
		 if (this->caller_map[*p].empty()) 
		 	current.push(*p);
	}

	while (!current.empty()) 
	{
		Named_object * callee = current.front();
		current.pop();
		this->sorted_functions_.push_back(callee);
		
		for (Caller_map::iterator p = this->caller_map.begin();
			 p != this->caller_map.end();
			 p++)
		{
			if (p->second.find(callee) != p->second.end())
			{
				p->second.erase(callee);
				if (p->second.empty())
				{
					Named_object * caller = const_cast<Named_object*>(p->first);
					if (this->functions_.count(caller) != 0) 
					{
						current.push(caller);
					}
				}
			}
		}
	}

	for (Caller_map::iterator p = this->caller_map.begin();
		 p != this->caller_map.end();
		 p ++) 
	{
		Named_object * caller = const_cast<Named_object*>(p->first);
		if (!p->second.empty() && this->functions_.count(caller) != 0)
		{
			this->sorted_functions_.push_back(caller);
		}
	}

	reverse(sorted_functions_.begin(), sorted_functions_.end());
}


void 
Escape_analysis :: compute_analysis_results()
{
	for (Named_object_vector::iterator p = this->sorted_functions_.begin();
		 p != this->sorted_functions_.end();
		 p++)
	{
		Named_object * obj = *p;
		go_assert(no->is_function());
		Function * func = obj->func_value();

		Escape_analysis_info * escape_analysis_info = 
		this->initialize_escape_info(obj);
		
		// Perform the analysis for a single function
		do
		{
			escape_analysis_info->set_updated(false);
			// 照情况看来这里面是建图连边什么的

			// 我只需要该function对应的statements~
			// 我追溯到了Escape_analysis_traverse_statements::statement
			// 接着到Escape_analysis_traverse_expressions::expression
			// 可是这个函数只有这么两句话：
			  //{
			  //(*expr)->get_escape_analysis_object(this->escape_analysis_info_);		为啥要获取obj ??
			  //return TRAVERSE_CONTINUE;
			  //}
			// Expression::do_get_escape_analysis_object是个virtual方法
			// 再继续查看下去就明白了，Escape_analysis_traverse_expressions::expression里这两句好像都是废话，
			// 但其实这是作者偷懒而已。因为有很多expression子类，不一定每个类都要逃逸分析时回调callback他们，
			// 但貌似有些子类是需要的，着我还不清楚，作者就"反正我都回调，谁爱用就用，用不到拉倒"

			// 这里面应该有些expression反过来设置了escape_analysis_info的updated！  我再找找.
			// TODO 应该有些expression反过来设置了escape_analysis_info的updated！  我再找找.
			// 不过这可不好找了，因为set_updated()基本都是在Escape_analysis_object的方法里出现。应该是哪个expression
			// 解析出了Escape_analysis_object，然后建图连边什么的的时候~~~
			// TODO 上述很重要~

			Escape_analysis_traverse_statements eats(escape_analysis_info);
			func->traverse(&eats);
		}
		while (escape_analysis_info->updated() && !escape_analysis_info->aborted()) ;

		// 读到这里~~~~~~~~~~~~~~~~~~~~~下面明天再看~~~~~
		if (escape_analysis_info->aborted())
		{
			// Propagate the escape level through the graph
			// TODO TOREAD 
			escape_analysis_info->propagate_escape_level();
			
			if (escape_analysis_info->is_safe_function())
				this->safe_functions_.insert(no);
		}
	}
}

void 
Escape_analysis :: perform(Gogo * gogo)
{
    Escape_analysis escape_analysis;
	escape_analysis.compute_functions_to_process(gogo);
	escape_analysis.compute_analysis_results();
}


// put it into .cc
class Call_graph_traverse_functions : public Traverse
{
	public:
  	Call_graph_traverse_functions(Escape_analysis* escape_analysis_ctx)
    	: Traverse(traverse_functions), escape_analysis_ctx_(escape_analysis_ctx)
  	{ }

 	protected:
  	int function(Named_object*);

 	private:
  	// The escape analysis information.
  	Escape_analysis* escape_analysis_ctx_;
};

class Escape_analysis_traverse_expressions : public Traverse
class Escape_analysis_traverse_statements : public Traverse
{
	public : 
		Escape_analysis_traverse_statements(
			Escape_analysis_info * escape_analysis_info)
		  : Traverse(traverse_statements),
		    escape_analysis_info_(escape_analysis_info)
		{ }

	protected :
		int statement(Block*, size_t * pindex, Statement *);
	private :
		Escape_analysis_info * escape_analysis_info_;
}

// They will be examined via compute_analysis_results()
Escape_analysis_traverse_statements::statement(Block * block, size_t pindex, Statement * statement)
{
	Escape_analysis_traverse_expressions eate(this->escape_analysis_info_, statement);
	statement->traverse(block, pindex, &eate);

	// assignment 不属于Expression的子类，所以先搞Expr, 再搞Assign
	// TODO 还没写traverse_assign 的实现.
	Escape_analysis_traverse_assignments eata(this->escape_analysis_info_, statement);
	statement->traverse(&eata);

	return TRAVERSE_CONTINUE;
}

class Escape_analysis_traverse_expressions : public Traverse
{
	public :
		Escape_analysis_traverse_expressions(	
			Escape_analysis_info escape_analysis_info, Statement * statement)
		  : Traverse(traverse_block | traverse_expressions),
		    escape_analysis_info_(escape_analysis_info),
			statement_(statement)
		{ }
	
	protected :
		// Only look at top-level expressions: do not descend into blocks.
		// They will be examined via Escape_analysis_traverse_statements.
		int block(Block *)
		{ return TRAVERSE_SKIP_COMPONENTS; }

		int expression(Expression ** ) ;

	private :
		Escape_analysis_info * escape_analysis_info_ ;
		Statement * statement_ ;
} ;

// look for a reference to a variable in an expression
int 
Escape_analysis_traverse_expressions::expression(Expression ** expr)
{
	(*expr)->get_escape_analysis_object(this->escape_analysis_info_);
	return TRAVERSE_CONTINUE;
}
