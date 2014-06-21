// the implements of methods is in  escape.cc
// This class is used to handle an assignment statement.

class Escape_analysis_traverse_assignments : public Traverse_assignments
{
		public:
				Escape_analysis_traverse_assignments(
								Escape_analysis_info* escape_analysis_info, Statement* statement)
						: escape_analysis_info_(escape_analysis_info), statement_(statement)
				{ }

		protected:
				void
						initialize_variable(Named_object*);

				void
						assignment(Expression** lhs, Expression** rhs);

				void
						value(Expression**, bool, bool)
						{ }

		private:
				void
						do_assignment(Expression* lhs, Expression* rhs);

				// The escape analysis information.
				Escape_analysis_info* escape_analysis_info_;
				// The Statement in which we are looking.
				Statement* statement_;
};


// in statements.h
// This class is used to traverse assignments made by a statement
// which makes assignments.

class Traverse_assignments
{
		public:
				Traverse_assignments()
				{ }

				virtual ~Traverse_assignments()
				{ }

				// This is called for a variable initialization.
				virtual void
						initialize_variable(Named_object*) = 0;

				// This is called for each assignment made by the statement.  PLHS
				// points to the left hand side, and PRHS points to the right hand
				// side.  PRHS may be NULL if there is no associated expression, as
				// in the bool set by a non-blocking receive.
				virtual void
						assignment(Expression** plhs, Expression** prhs) = 0;

				// This is called for each expression which is not passed to the
				// assignment function.  This is used for some of the statements
				// which assign two values, for which there is no expression which
				// describes the value.  For ++ and -- the value is passed to both
				// the assignment method and the rhs method.  IS_STORED is true if
				// this value is being stored directly.  It is false if the value is
				// computed but not stored.  IS_LOCAL is true if the value is being
				// stored in a local variable or this is being called by a return
				// statement.
				virtual void
						value(Expression**, bool is_stored, bool is_local) = 0;
};

