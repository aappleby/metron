#pragma once
#include "metrolib/core/Err.h"
#include "metron/nodes/MtField.h"
#include "metron/tools/MtUtils.h"

#include <vector>

struct MtModule;
struct MtMethod;
struct MnNode;

//------------------------------------------------------------------------------

struct MtContext {
  MtContext(MtModule* top_mod);
  MtContext(MtContext* parent, MtMethod* _method);
  MtContext(MtContext* parent, MtField* _field);
  ~MtContext();

  static MtContext* param(MtContext* parent, const std::string& name);
  static MtContext* construct_return(MtContext* parent);

  //MtContext* clone();
  std::string get_path() const;
  MtContext* get_child(const std::string& name) const;

  void instantiate();

  void assign_struct_states();
  void assign_state_to_field(MtModule* current_module);

  MtContext* resolve(const std::string& name);
  MtContext* resolve(MnNode node);

  void dump() const;
  void dump_ctx_tree() const;

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

  TraceState state() const {
    return log_top.state;
  }

  CHECK_RETURN Err check_done();
  //----------

  std::string name;
  ContextType context_type;
  MtContext* parent = nullptr;

  MtField* field = nullptr;
  MtMethod* method = nullptr;

  MtModule* parent_mod = nullptr;
  MtStruct* parent_struct = nullptr;

  MtModule* type_mod = nullptr;
  MtStruct* type_struct = nullptr;

  struct LogEntry {
    TraceState state;
  };

  LogEntry log_top;
  LogEntry log_next;

  std::vector<LogEntry> action_log;
  std::vector<MtContext*> children;

 private:
  MtContext() {}
  MtContext(const MtContext& copy) = delete;
};

//------------------------------------------------------------------------------
