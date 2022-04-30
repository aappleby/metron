#pragma once

#include "Err.h"
#include "MtField.h"
#include "MtUtils.h"

void log_branch_code(uint64_t c);

//------------------------------------------------------------------------------

struct LogEntry {
  uint64_t branch_code;
  ContextState state;
};

//------------------------------------------------------------------------------

struct MtContext {
  MtContext(MtModule* top_mod);
  MtContext(MtContext* parent, MtMethod* _method);
  MtContext(MtContext* parent, MtField* _field);

  static MtContext* param(MtContext* parent, const std::string& name);
  static MtContext* construct_return(MtContext* parent);

  ~MtContext() {
    for (auto c : children) delete c;
    children.clear();
  }

  MtContext* clone();
  std::string get_path() const;
  MtContext* get_child(const std::string& name) const;

  static void instantiate(MtModule* mod, MtContext* inst);

  void assign_state_to_field();

  MtContext* resolve(const std::string& name);
  void dump() const;
  void dump_tree() const;

  MtMethod* current_method() {
    if (method) return method;
    if (parent) return parent->current_method();
    return nullptr;
  }

  void start_branch_a(uint64_t branch_code) {
    action_log.push_back(log_next);
    log_top.branch_code = branch_code;
    log_next = log_top;

    for (auto c : children) c->start_branch_a(branch_code);
  }

  void end_branch_a(uint64_t branch_code) {
    for (auto c : children) c->end_branch_a(branch_code);
  }

  void start_branch_b(uint64_t branch_code) {
    std::swap(log_top, log_next);
    log_top.branch_code = branch_code;
    for (auto c : children) c->start_branch_b(branch_code);
  }

  void end_branch_b(uint64_t branch_code) {
    log_top.state = merge_branch(log_top.state, log_next.state);
    log_top.branch_code = log_top.branch_code >> 1;
    log_next = action_log.back();
    action_log.pop_back();
    for (auto c : children) c->end_branch_b(branch_code);
  }

  CHECK_RETURN Err check_done() {
    Err err;
    if (action_log.size()) {
      err << ERR("Had leftover contexts in action_log\n");
    }

    if (log_next.state != CTX_NONE) {
      err << ERR("Had leftover context in log_next\n");
    }

    if (log_top.state == CTX_INVALID || log_top.state == CTX_PENDING) {
      err << ERR("Had invalid context in log_top\n");
    }

    for (auto c : children) err << c->check_done();
    return err;
  }

  //----------

  MtContext* parent;
  std::string name;
  ContextType type;

  MtField* field;
  MtMethod* method;
  MtModule* mod;

  LogEntry log_top;
  LogEntry log_next;

  std::vector<LogEntry> action_log;
  std::vector<MtContext*> children;

 private:
  MtContext() {}
  MtContext(const MtContext& copy) = delete;
};

//------------------------------------------------------------------------------
