Quetion : 这个diff 是和上一次提交版本的diff 还是和原本gccgo的源文件diff ？

// in statements.h
class Statement {
		// Perform statement specific escape analysis.
		// TODO
		void
				perform_escape_analysis(Escape_analysis_info*);


		// Implemented by child class: perform special handling
		// for particular statements. Only implement when it is not sufficient
		// to only traverse assignements and expressions.
		virtual void
				do_perform_escape_analysis(Escape_analysis_info*)
				{ }


		// Return the expression.
		Expression*
				init()
				{ return this->init_; }
}

// in statements.cc
