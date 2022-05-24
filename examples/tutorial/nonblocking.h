class NonBlockingAssignment {
public:
  int count;
  int update_bad() {
    count = count + 1;
    return count;
  }
  /*
  int update_good() {
    int new_count = count + 1;
    count = new_count;
    return new_count;
  }
  */
};
