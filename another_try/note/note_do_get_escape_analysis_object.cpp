/*
 *
 *	记录Expression 子类的do_get_escape_analysis_object
 *
**/

// Retrieve the escape analysis object corresponding to this variable.
Escape_analysis_object*
Var_expression::do_get_escape_analysis_object(Escape_analysis_info* escape_info)
{
		// We treat all variables as references. In particular local variables
		// act as references to the memory location they name.
		return escape_info->get_ea_object_for_variable(this->named_object(), this);
}


// 返回Variable对应的Esc_object
// 但更重要的是为对象no创建Esc节点，并初始化Esc_level，用在逃逸分析上面.
// named_object_references_map_ 是Named_object与Esc_object的对应关系.
// no是parameter的时候要特殊搞！

// Get the escape analysis object representing a variable.

Escape_analysis_object*
Escape_analysis_info::get_ea_object_for_variable(const Named_object* no,
				Expression* expr)
{
		Named_object_map::iterator p = this->named_object_references_map_.find(no);
		if (p != this->named_object_references_map_.end())
				// We have already seen this variable before.
				return p->second;

		Escape_analysis::Object_type object_type = Escape_analysis::REFVAR;
		Escape_analysis::Escape_level escape_level = Escape_analysis::NO_ESCAPE;

		if (::is_global(no))
		{
				object_type = Escape_analysis::GLOBAL;
		}
		else if (::is_result(no))
		{
				object_type = Escape_analysis::RETURN;
		}

		Escape_analysis_object* variable =
				this->make_object_for_named_object(object_type, no, escape_level);

		this->named_object_references_map_[no] = variable;

		// If we created a parameter variable, we will create a phantom node
		// This new node will be created on the first access though a parameter
		// variable. DO NOT MARK IT AS ARG_ESCAPE. Only its field pointers need
		// to be ARG_ESCAPE, and we will mark it on the propagation step.
		if (::is_parameter(no))
		{
				Escape_analysis_object* phantom_par =
						this->add_phantom(no, Escape_analysis::NO_ESCAPE,
										Escape_analysis::OBJECT);
				variable->add_pointsto_edge(phantom_par);
				// Add a dummy points to field.
				Escape_analysis_object* parabstraction_ref =
						phantom_par->get_field_index_reference(-1);

				Escape_analysis_object* parabstraction =
						this->add_phantom(expr, Escape_analysis::ARG_ESCAPE,
										Escape_analysis::PARAMETER);
				parabstraction_ref->add_pointsto_edge(parabstraction);
		}
		return variable;
}


// MARK Temporary的含义搞不大懂

// Retrieve the escape analysis object corresponding to this temporary.

Escape_analysis_object*
Temporary_reference_expression::do_get_escape_analysis_object(Escape_analysis_info* escape_info)
{
		// We treat all variables as references. In particular local variables
		// act as references to the memory location they name.
		// Temporary_statement::init_ 是个Expression ! 我靠expression竟然取init_这种名字!
		if (this->statement_->init() != NULL)
				return this->statement_->init()->get_escape_analysis_object(escape_info);
		else
				// Create an new reference for this object.
				return escape_info->get_ea_object_for_temporary_statement(this->statement_, this);
}

Escape_analysis_object*
Func_expression::do_get_escape_analysis_object(
				Escape_analysis_info* escape_info)
{
		return escape_info->process_function_reference(this, this->closure_);
}

// function reference 即形如 f := func() { ..closure.. } 的f就是function reference
// 这里连边 defer_edge( f -> closure )
// closure的Esc节点初始化为global，不过还有fixme
// Process a function reference.

Escape_analysis_object*
Escape_analysis_info::process_function_reference(Expression* fn, Expression* closure)
{
		Expression_map::iterator p = this->expression_map_.find(fn);
		if (p != this->expression_map_.end() && p->second != NULL)
				// We have already seen this expression before.
				return p->second;

		Escape_analysis_object* func_ref = this->get_ea_object_for_expression(fn);
		if (closure != NULL)
		{
				Escape_analysis_object* closure_ref =
						closure->get_escape_analysis_object(this);
				// FIXME. Mark closure as escaping globally for now. It should only be
				// marked as globally escaping if the nested function is not safe.
				closure_ref->set_escape_level(Escape_analysis::GLOBAL_ESCAPE);
				func_ref->add_defer_edge(closure_ref);
		}

		return func_ref;
}

// Process a constant literal of a basic type.
// FIXME. Ugly, every constant literal of a basic type is seen as
// as reference to its storage. Only need for implicit conversions
// to an empty interface type.
Escape_analysis_object*
Escape_analysis_info::process_constant(Expression* expr)
{
		return this->get_ea_object_for_expression(expr);
}

// filed node 是什么意思呢？
// Get the escape analysis abstract object for a type conversion
Escape_analysis_object*
Type_conversion_expression::do_get_escape_analysis_object(
				Escape_analysis_info* escape_info)
{
		if (this->type()->interface_type() != NULL
						&& this->expr_->type()->interface_type() == NULL)
		{
				// Conversion from non interface type to interface type.
				// Flat type to interface behaves more like a new and an assignment.
				return escape_info->process_convert_flat_to_interface(this,
								this->expr_);
		}
		else if (this->type()->interface_type() == NULL
						&& this->expr_->type()->interface_type() != NULL)
				// Conversions from an interface to flat types are dereferences.
				return escape_info->process_field_reference(this, this->expr_, -1);
		else

				// If the conversion is interface -> interface or simple type -> simple type
				// just get the inner reference.
				return this->expr_->get_escape_analysis_object(escape_info);
}

// Obtain the escape analysis object for this expression.

Escape_analysis_object*
Unary_expression::do_get_escape_analysis_object(
				Escape_analysis_info* escape_info)
{
		if (this->op_ == OPERATOR_AND)
		{  
				// This is an address-of expression.
				return escape_info->process_address_of(this, this->expr_);
		}  
		else if (this->op_ == OPERATOR_MULT)
		{  
				// Slices are treated exactly as a selector with field -1.
				return escape_info->process_field_reference(this, this->expr_, -1);
		}  
		// MARK 这里直接就返回NULL是不是不好 负号-不算Unary_expression ?
		// All other unary expressions are not relevant.
		return NULL;
}
