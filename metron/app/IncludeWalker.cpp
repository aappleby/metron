#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Utilities.hpp"
#include "matcheroni/Cookbook.hpp"

#include <set>
#include <string>
#include <filesystem>

using namespace matcheroni;
namespace fs = std::filesystem;

//------------------------------------------------------------------------------

struct FileFinder {
  std::vector<std::string> search_paths = {""};

  std::string find_file(const std::string& file_path) {
    bool found = false;
    for (auto &path : search_paths) {
      auto full_path = path + file_path;
      if (!fs::is_regular_file(full_path)) continue;
      return full_path;
    }
    return "";
  }

  /*
  std::string read(const std::string& file_path) {
    bool found = false;
    for (auto &path : search_paths) {
      auto full_path = path + file_path;
      if (!fs::is_regular_file(full_path)) continue;

      printf("Found %s @ %s\n", file_path.c_str(), full_path.c_str());

      std::string buf;
      buf.resize(fs::file_size(full_path));

      FILE* f = fopen(full_path.c_str(), "rb");
      auto size = fread(buf.data(), buf.size(), 1, f);
      (void)size;
      fclose(f);
      return buf;
    }
    return "";
  }
  */
};

//------------------------------------------------------------------------------

struct MyContext : public TextMatchContext {

  FileFinder finder;

  std::set<std::string> visited_files;

  int traverse_depth = 0;

  void print_span(const char* prefix, TextSpan span) {
    printf("%s", prefix);
    for (auto c = span.begin; c < span.end; c++) putc(*c, stdout);
    putc('\n', stdout);
  }

  void indent() {
    for(int i = 0; i < traverse_depth; i++) printf("|  ");
  }

  void traverse(const std::string& path) {
    auto full_path = finder.find_file(path);
    if (full_path.empty()) {
      indent(); printf("Could not find %s\n", path.c_str());
      return;
    }

    if (visited_files.contains(full_path)) return;
    visited_files.insert(full_path);

    indent(); printf("Traversing %s\n", full_path.c_str());

    {
      fs::path p(full_path);
      p.remove_filename();
      finder.search_paths.push_back(p);
      traverse_depth++;

      std::string contents = utils::read(full_path);
      match_lines(utils::to_span(contents));

      traverse_depth--;
      finder.search_paths.pop_back();
    }
  }

  TextSpan handle_include(TextSpan path_span) {
    std::string path(path_span.begin + 1, path_span.end - 1);
    traverse(path);
    return path_span.consume();
  }

  TextSpan match_lines(TextSpan body) {
    //using sink = Ref<&MyContext::match_line>;
    using namespace cookbook;

    using pattern = to_lines<
      Oneof<
        c_include_line<Ref<&MyContext::handle_include>>
      >
    >;

    return pattern::match(*this, body);
  }


  TextSpan print_span(TextSpan span) {
    for (auto c = span.begin; c < span.end; c++) putc(*c, stdout);
    putc('\n', stdout);
    fflush(stdout);
    return span.consume();
  }

};

//------------------------------------------------------------------------------

void test_include_walker(const std::string& path) {

  MyContext ctx;
  ctx.traverse(path);

  //using include_line =
  //Seq<
  //  Lit<"#include">,
  //  Some<cookbook::isspace>,
  //  Dispatch<cookbook::dquote_span, Ref<&MyContext::print_span>>
  //>;
  //std::string blah = "#include \"blarp\"\n\n\n";
  //include_line::match(ctx, utils::to_span(blah));

}

//------------------------------------------------------------------------------
