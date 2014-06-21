class Variable {
 public :
  // Return whether the address is taken but does not escape.
  bool
  is_non_escaping_address_taken() const
  { return this->is_non_escaping_address_taken_; }

  // Note that something takes the address of this variable such that
  // the address does not escape the function.
  void
  set_non_escaping_address_taken()
  { this->is_non_escaping_address_taken_ = true; }

  // Mark this variable as non escaping its function scope.
  void
  unset_escapes_function()
  { this->escapes_function_ = false; }

  // Does this variable escape its function scope.
  bool
  escapes_function()
  { return this->escapes_function_; }

private : 
  // Whether something takes the address of this variable such that
  // the address does not escape the function.
  bool is_non_escaping_address_taken_ : 1;
  // Whether the escape analysis has determined that the variable does
  // not escape a function. It still might need to be allocated with alloca
  // if defined within a loop but escapes it.
  bool escapes_function_ : 1;
}


class Result_variable {
  // Return whether the address is taken but does not escape.
  bool
  is_non_escaping_address_taken() const
  { return this->is_non_escaping_address_taken_; }

  // Note that something takes the address of this variable such that
  // the address does not escape the function.
  void
  set_non_escaping_address_taken()
  { this->is_non_escaping_address_taken_ = true; }
  // Whether something takes the address of this variable such that
  // the address does not escape the function.
  bool is_non_escaping_address_taken_;
}
