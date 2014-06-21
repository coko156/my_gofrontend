// 基本上Expression的子类的重载了 do_get_escape_analysis_object() 方法
// Quetion : 貌似这里的Expression是独立于GCCGo的， 跟GCCGo那坨Expressions、Statements是不一样的。

class Expression {
		// Return the node represented by the expression for the escapes
		// analysis graph.
		Escape_analysis_object*
				get_escape_analysis_object(Escape_analysis_info* analysis_context)
				{ return this->do_get_escape_analysis_object(analysis_context); }
		// Return the base and index of an expression for escape analsyis.
		Escape_analysis_object*
				escape_analysis_split_base_index(Escape_analysis_info* analysis_context,
								int* index)
				{ return this->do_escape_analysis_split_base_index(analysis_context, index); }

		// Mark this expression as not escaping the function scope. 
		// Quetion : 话说“逃逸”面向的对象到底是什么呢？
		void 
				unset_escapes_function() 
				{ this->do_unset_escapes_function(); } 

		// Does this expression escape the function scope?
		// Quetion : do_escapes_function() 调用blabla最后是go_unreachable()，grep了一下发现go_unreachable()经常被调用
		bool
				escapes_function()
				{ return this->do_escapes_function(); }

		// Child class implements 
		// how to map the expression into an escape analysis object.
		// Must be implemented.
		//    if expression never evaluates to a reference then
		//        return NULL.
		//    if expression evaulates to the same reference as
		//    some inner expression as in (some) type conversions then
		//        return inner->get_escape_analysis_object
		//    otherwise.
		//        whenever a reference is created or copied it must
		//        be tracked in the escape analysis module.
		virtual Escape_analysis_object*
				do_get_escape_analysis_object(Escape_analysis_info*) = 0;

		// Child class implements how to split the expression into a base and an
		// index. Used only for objects that might point to other objects as
		// in structures, pointer variables, maps, arrays, slices.
		virtual Escape_analysis_object*
				do_escape_analysis_split_base_index(Escape_analysis_info*, int*)
				{ return NULL;}

		// Child class implements whether this expression escapes the functional
		// scope.
		virtual bool
				do_escapes_function()
				{ go_unreachable(); }

		// Child class implements how to set the expression as escaping its functional
		// scope.
		virtual void
				do_unset_escapes_function()
				{ }

		// Child class implements taking the address of an expression.
		virtual void
				do_address_taken(bool)
				{ }
}

class Var_expression : public Expression {
		Escape_analysis_object*
				do_get_escape_analysis_object(Escape_analysis_info*);
}

class Call_expression : public Expression {
		// Return the named object corresponding the function target of a function
		// call. Can return NULL if the call site target is not unique.
		const Named_object*
				get_function_object() const;
}
