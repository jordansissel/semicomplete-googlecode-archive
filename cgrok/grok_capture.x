
struct grok_capture {
  string name<>;
  string pattern<>;
  int id;
  int pcre_capture_number;

  string predicate_lib<>;
  string predicate_func_name<>;

  /* predicate functions store personal data here. */
  opaque extra<>;
};
