#pragma once

#include "Err.h"
#include "MtField.h"
#include "MtUtils.h"

//------------------------------------------------------------------------------

struct LogEntry {
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

  void start_branch_a() {
    action_log.push_back(log_next);
    log_next = log_top;

    for (auto c : children) c->start_branch_a();
  }

  void end_branch_a() {
    for (auto c : children) c->end_branch_a();
  }

  void start_branch_b() {
    std::swap(log_top, log_next);
    for (auto c : children) c->start_branch_b();
  }

  void end_branch_b() {
    log_top.state = merge_branch(log_top.state, log_next.state);
    log_next = action_log.back();
    action_log.pop_back();
    for (auto c : children) c->end_branch_b();
  }

  void start_switch() {
    action_log.push_back(log_next);
    action_log.push_back(log_top);
    log_next.state = CTX_PENDING;
    for (auto c : children) c->start_switch();
  }

  void start_case() {
    log_top = action_log.back();
    for (auto c : children) c->start_case();
  }

  void end_case() {
    log_next.state = merge_branch(log_top.state, log_next.state);
    for (auto c : children) c->end_case();
  }

  void end_switch() {
    log_top = log_next;
    action_log.pop_back();
    log_next = action_log.back();
    action_log.pop_back();
    for (auto c : children) c->end_switch();
  }

  CHECK_RETURN Err check_done();
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
