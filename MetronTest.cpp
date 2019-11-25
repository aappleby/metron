
namespace Clorp {

  struct Blah {

    int a = 0;
    int b = 0;

    void tick() {
      a = b + 2;
    }

    void tock() {
      b = a + derp();
    }

    int derp() {
      return 3;
    }

  };
};