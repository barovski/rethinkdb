#ifndef RDB_PROTOCOL_VAR_TYPES_HPP_
#define RDB_PROTOCOL_VAR_TYPES_HPP_

#include "containers/counted.hpp"
#include "containers/archive/archive.hpp"
#include "rdb_protocol/sym.hpp"

namespace ql {

class datum_t;

// RSI: The construction and passing down of var_visibility_t and var_scope_t in
// various constructors and eval_impl functions needs to be keep in sync.  Maybe we can
// avoid that requirement.

// RSI: Make this efficient.
class var_visibility_t {
public:
    var_visibility_t();

    // Updates the implicit variable visibility according to the rules.
    var_visibility_t with_func_arg_name_list(const std::vector<sym_t> &arg_names) const;

    bool contains_var(sym_t varname) const;
    bool implicit_is_accessible() const;

    uint32_t get_implicit_depth() const { return implicit_depth; }

private:
    friend class var_scope_t;
    friend void debug_print(printf_buffer_t *buf, const var_visibility_t &var_visibility);


    std::set<sym_t> visibles;
    uint32_t implicit_depth;
};

void debug_print(printf_buffer_t *buf, const var_visibility_t &var_visibility);

// RSI: Make this efficient.
class var_scope_t {
public:
    var_scope_t();

    var_scope_t with_func_arg_list(const std::vector<std::pair<sym_t, counted_t<const datum_t> > > &new_vars) const;

    counted_t<const datum_t> lookup_var(sym_t varname) const;
    counted_t<const datum_t> lookup_implicit() const;

    // Dumps a complete "human readable" description of the var_scope_t.  Is used by
    // info_term_t via reql_func_t::print_source().
    std::string print() const;

    var_visibility_t compute_visibility() const;

    void rdb_serialize(write_message_t &msg) const;  // NOLINT(runtime/references)
    archive_result_t rdb_deserialize(read_stream_t *s);

private:
    std::map<sym_t, counted_t<const datum_t> > vars;

    uint32_t implicit_depth;
    // Is non-empty IFF implicits_depth == 1.
    counted_t<const datum_t> maybe_implicit;
};



}  // namespace ql



#endif  // RDB_PROTOCOL_VAR_TYPES_HPP_